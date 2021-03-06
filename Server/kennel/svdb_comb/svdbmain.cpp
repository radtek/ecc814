#include "svdbmain.h"
#include "ThreadContrl.h"
#include "DB.h"
#include "WatchThread.h"
#include "SocketWatchThread.h"
#include <set>

SvdbMain::SvdbMain(void)
{
	m_RootPath="D:\\v70\\data\\main";
	m_pMonitor=NULL;
	m_pGroup=NULL;
	m_pEntity=NULL;
	m_pMT=NULL;
	m_pSVSE=NULL;
	m_pResource=NULL;
	m_pOption=NULL;
	m_pWT=NULL;
	m_pSWT=NULL;
	m_pTC=NULL;

	m_pMT_orig=NULL;
    m_pEntityGroup_orig=NULL;
	m_pEntityTemplet_orig=NULL;
	m_pLanguage=NULL;
	m_pWholeConfig=NULL;

	m_pEntityEx=NULL;
	m_pTopologyChart=NULL;
	m_pVirtualGroup=NULL;

	m_pNnmSave=NULL;
	m_pBackupRecordThread=NULL;
	m_pBackupRecordDamonThread=NULL;
	m_pBackupConfigThread=NULL;
}

SvdbMain::~SvdbMain(void)
{
	INIFILEPOOL::iterator it;
	while(m_IniPool.findnext(it))
		if((*it).getvalue()!=NULL)
			delete (*it).getvalue();

	if(m_pOption!=NULL)
		delete m_pOption;

	if(m_pWT!=NULL)
		delete m_pWT;
	if(m_pSWT!=NULL)
		delete m_pSWT;
	if(m_pTC!=NULL)
		delete m_pTC;

	if(m_pEntityEx!=NULL)
		delete m_pEntityEx;
	if(m_pTopologyChart!=NULL)
		delete m_pTopologyChart;
	if(m_pVirtualGroup!=NULL)
		delete m_pVirtualGroup;
	if(m_pNnmSave!=NULL)
		delete m_pNnmSave;
	if(m_pBackupRecordThread!=NULL)
		delete m_pBackupRecordThread;
	if(m_pBackupRecordDamonThread!=NULL)
		delete m_pBackupRecordDamonThread;
	if(m_pBackupConfigThread!=NULL)
		delete m_pBackupConfigThread;

	if(m_pWholeConfig!=NULL)
		delete m_pWholeConfig;
}

void SvdbMain::toExitListen(void)
{
	IdcUser::BoolToExit=true;

	if(m_pTC!=NULL)
		m_pTC->toExit();

	if(m_pWT!=NULL)
		m_pWT->toExit();
	if(m_pSWT!=NULL)
		m_pSWT->toExit();

	if(m_pNnmSave!=NULL)
		m_pNnmSave->toExit();
	if(m_pBackupRecordThread!=NULL)
		m_pBackupRecordThread->toExit();
	if(m_pBackupRecordDamonThread!=NULL)
		m_pBackupRecordDamonThread->toExit();
	if(m_pBackupConfigThread!=NULL)
		m_pBackupConfigThread->toExit();
}

bool SvdbMain::Init(void)
{
	m_pOption=new Option();
	if(!m_pOption)
		return false;
	if(!m_pOption->LoadOption())
		return false;

	m_RootPath=m_pOption->m_rootpath;
	IdcUser::RootPath= m_RootPath.getword();
	printf("\nm_RootPath=%s\n", m_RootPath.getword());
	string sename="localhost";

	char buf[1024]={0};

	sprintf(buf,"%s/data/%s/EntityEx.data",m_RootPath.getword(),sename.c_str());
	m_pEntityEx=new EntityPoolEx(buf);
	m_pEntityEx->Load();

	sprintf(buf,"%s/data/%s/TopologyChart.data",m_RootPath.getword(),sename.c_str());
	m_pTopologyChart=new TopologyChartPool(buf);
	m_pTopologyChart->Load();

	sprintf(buf,"%s/data/%s/VirtualGroup.data",m_RootPath.getword(),sename.c_str());
	m_pVirtualGroup=new VirtualGroupPool(buf);
	m_pVirtualGroup->Load();


	if( IdcUser::EnableConfigDB )
	{
		string cdbpath=		IdcUser::RootPath + "/data/configdb";
		string historypath=	IdcUser::RootPath + "/data/configdb_history";
		IdcUser::CreatDirNonThreadSafe(cdbpath);
		IdcUser::CreatDirNonThreadSafe(historypath);
		m_pWholeConfig= new CWholeConfig(IdcUser::RootPath + "/data/configdbIndex.data" , cdbpath+"/", historypath+"/" );
		string estr;
		m_pWholeConfig->LoadData(estr);
		if(!estr.empty())
			cout<<estr.c_str()<<endl;
	}

	if( IdcUser::EnableIdc )
	{
		printf("\n\nLoad data for IDC\n");
		string fname= IdcUser::RootPath; fname += "/data/idc_data";
		if( !IdcUser::CreatDirNonThreadSafe(fname.c_str()) )
			;//cout<<"----- 创建目录失败："<<fname.c_str()<<endl;
	}
	else
		printf("\n\nLoad data\n");

	sprintf(buf,"%s/data/%s/Resource.data",m_RootPath.getword(),sename.c_str());
	m_pResource=new ResourcePool(buf);
	m_pResource->Load(); //takes very long to load the resource, can be optimized
	if(IdcUser::AutoResolveIDS)
	{
		string strlang= m_pOption->m_defaultlanguage;
		strlang+="-ForTpl";
		m_pLanguage= m_pResource->GetResource(strlang.c_str());
		if(m_pLanguage==NULL)
		{
			cout<<"Failed to load resource:"<<strlang<<endl;
			cout<<" ********************************************************************************\n"<<endl;
			cout<<"    严重错误  Fatal error of Template file: "<<buf<<endl;
			cout<<"\n ********************************************************************************"<<endl;
		}

		IdcUser::m_pResource= (void *)m_pResource;
	}

	StringMap & defaultlang= m_pLanguage->GetResourceDict();
	word *tpv=NULL;
	if( ( tpv=defaultlang.find("IDS_MonitorDisabled") )!=NULL )
		IdcUser::StrDisable= *tpv;
	tpv=NULL;
	if( ( tpv=defaultlang.find("IDS_TempDisableMonitorTime") )!=NULL )
		IdcUser::StrTempDisable= *tpv;


	sprintf(buf,"%s/data/%s/MonitorTemplet.data",m_RootPath.getword(),sename.c_str());
	m_pMT=new MonitorTempletPool(buf);
	m_pMT->Load();
	if(IdcUser::AutoResolveIDS)
	{
		m_pMT_orig=new MonitorTempletPool(buf,true);
		m_pMT_orig->Load();
		if(m_pLanguage!=NULL)
		{
			m_pMT->ResolveIds(m_pLanguage);
			IdcUser::m_pLanguage= (void*)m_pLanguage;
		}
	}

	sprintf(buf,"%s/data/%s/Task.data",m_RootPath.getword(),sename.c_str());
	m_pTask=new GeneralPool(buf);
	m_pTask->Load();

	sprintf(buf,"%s/data/%s/EntityGroup.data",m_RootPath.getword(),sename.c_str());
	m_pEntityGroup=new EntityGroupPool(buf);
	m_pEntityGroup->Load();
	if(IdcUser::AutoResolveIDS)
	{
		m_pEntityGroup_orig=new EntityGroupPool(buf,true);
		m_pEntityGroup_orig->Load();
		if(m_pLanguage!=NULL)
			m_pEntityGroup->ResolveIds(m_pLanguage);
	}

	sprintf(buf,"%s/data/%s/EntityTemplet.data",m_RootPath.getword(),sename.c_str());
	m_pEntityTemplet=new EntityTempletPool(buf);
	m_pEntityTemplet->Load();
	if(IdcUser::AutoResolveIDS)
	{
		m_pEntityTemplet_orig=new EntityTempletPool(buf,true);
		m_pEntityTemplet_orig->Load();
		if(m_pLanguage!=NULL)
			m_pEntityTemplet->ResolveIds(m_pLanguage);
	}

	clock_t time1=clock();  
	m_pDB = new DB();
	if(m_pDB==NULL)
		return false;

	int nPFS=(m_pOption->m_dbPerFileSize > 0) ? m_pOption->m_dbPerFileSize : PERFILESIZE;
	int nPS=(m_pOption->m_dbPageSize > 0) ? m_pOption->m_dbPageSize : PAGESIZE;

	printf("SingleFileSize:%d,PageSize:%d\n",nPFS,nPS);

	if(!m_pDB->Init("SiteViewLog",this,nPFS,nPS))
	{
		AddToErrorLog("Failed to Init \".db\" ! ");
		cout<<"Failed to Init \".db\" ! "<<endl;
		return false;
	}
	clock_t time2=clock(); 
	clock_t time3,time4(0);

	if( IdcUser::EnableIdc )
	{   // for IDC
		string fname= IdcUser::RootPath; fname += "/data/idc_data";

		sprintf(buf,"%s/data/idc_data/SVSE.data",m_RootPath.getword());
		m_pSVSE=new SVSEPool(buf);
		if(!m_pSVSE->Load())
			m_pSVSE->InitSVSEPool();
		m_pSVSE->InitLockedSVSEPool();
			
		IdcUser::Users.clear();
		for(S_UINT seId= 1; seId<=m_pSVSE->maxsize; seId++) 
		{   //得到所有的 user (即所有顶层组的id)
			if( !m_pSVSE->Find(seId) )
				continue;
			SVSE * m_Svse= m_pSVSE->GetSVSE( seId );
			WORDLIST subGroupList= m_Svse->GetSubGroups();
			WORDLIST::iterator lit;
			
			for(lit=subGroupList.begin(); lit!=subGroupList.end();  ++lit)
				IdcUser::Users.insert( lit->getword() );
		}

		m_pGroup=new GroupPool();

		sprintf(buf,"%s/data/idc_data/Entity.data",m_RootPath.getword());
		m_pEntity=new EntityPool();
		m_pEntity->LoadFile( buf );
		
		sprintf(buf,"%s/data/%s/Monitor.data",m_RootPath.getword(),sename.c_str());
		m_pMonitor=new MonitorPool(buf);
		sprintf(buf,"%s/data/idc_data/Monitor.data",m_RootPath.getword());
		m_pMonitor->LoadFile( buf );

		std::set<string>::iterator uit;
		for(uit=IdcUser::Users.begin(); uit!=IdcUser::Users.end(); ++uit)
		{
			fname= IdcUser::RootPath; fname += "/data/idc_data/"; fname+= *uit; 
			if( !IdcUser::CreatDirNonThreadSafe(fname.c_str()) )
				;
			string openfile;
			openfile = fname + "/Group.data"; 
			m_pGroup->LoadFile( openfile.c_str() );

			openfile = fname + "/Entity.data"; 
			m_pEntity->LoadFile( openfile.c_str() );

			openfile = fname + "/Monitor.data"; 
			m_pMonitor->LoadFile( openfile.c_str() );

			time3=clock(); 
			DB* tempDB = new DB();
			if(tempDB==NULL)
				continue;

			if(!tempDB->Init(*uit,"SiteViewLog",this,nPFS,nPS))
			{
				AddToErrorLog("Failed to start sub db.");
				delete tempDB;
				continue;
			}

			m_pDB_map.insert(make_pair(*uit, tempDB));
			time4 += (clock()-time3); 
		}
	}
	else
	{
		// for non-IDC
		sprintf(buf,"%s/data/%s/SVSE.data",m_RootPath.getword(),sename.c_str());
		m_pSVSE=new SVSEPool(buf);
		if(!m_pSVSE->Load())
			m_pSVSE->InitSVSEPool();
		m_pSVSE->InitLockedSVSEPool();

		sprintf(buf,"%s/data/%s/Group.data",m_RootPath.getword(),sename.c_str());
		m_pGroup=new GroupPool(buf);
		m_pGroup->Load();

		sprintf(buf,"%s/data/%s/Entity.data",m_RootPath.getword(),sename.c_str());
		m_pEntity=new EntityPool(buf);
		m_pEntity->Load();

		sprintf(buf,"%s/data/%s/Monitor.data",m_RootPath.getword(),sename.c_str());
		m_pMonitor=new MonitorPool(buf);
		m_pMonitor->Load();
	}
	printf("Start all %d DB heads:  %.2f seconds\n\n",IdcUser::Users.size()+1,(double)(time2-time1+time4)/CLK_TCK);
	initialBackupConfigIni();

	m_pMonitor->ReSetBackupId();	
	if( IdcUser::RecordsAutoBackup )
	{
		m_pBackupRecordDamonThread= new CBackupRecordDamon();
		if(m_pBackupRecordDamonThread==NULL)
		{
			AddToErrorLog("Failed to new thread of BackupRecordDamon ! ");
			printf("Failed to new thread of BackupRecordDamon ! ");
			return false;
		}
		m_pBackupRecordDamonThread->Start();

		m_pBackupRecordThread=new CBackupRecord(m_pDB, m_pBackupRecordDamonThread);
		if(m_pBackupRecordThread==NULL)
		{
			AddToErrorLog("Failed to new thread of BackupRecord ! ");
			printf("Failed to new thread of BackupRecord ! ");
			return false;
		}
		m_pBackupRecordThread->Start();

		m_pBackupRecordDamonThread->setBkpRcdThread(m_pBackupRecordThread);
	}

	m_pBackupConfigThread=new CBackupConfig(this);
	if(m_pBackupConfigThread==NULL)
	{
		AddToErrorLog("Failed to new thread of BackupConfig ! ");
		printf("Failed to new thread of BackupConfig ! ");
		return false;
	}
	m_pBackupConfigThread->Start();


	if(m_pGroup!=NULL)
		m_pGroup->UpdateNnmEntityParentGid();
	
	if( !IdcUser::DisableEntityCombine && IdcUser::PreCreateNnmEntityParent && !IdcUser::nnmEntityParentOk)
	{
		m_pEntityEx->CreateNnmEntityParentGroup(this);
		printf("\n\n");
	}
	if(m_pOption->m_waitMode==Option::WAITMODE_VAR)
	{
		m_pWT=new WatchThread;
		if(m_pWT==NULL)
			return false;
		m_pWT->m_pMain=this;

		if(!m_pWT->Init())
		{
			AddToErrorLog("Init watch thread failed");
			return false;
		}
		m_pWT->Start();
		AddToErrorLog("Wait mode VAR");

	}else{
		m_pTC=new ThreadContrl();
		if(!m_pTC->Init(this))
			return false;

		AddToErrorLog("Wait mode FIX");
	}

	m_pSWT=new SocketWatchThread;
	if(m_pSWT==NULL)
		return false;
	m_pSWT->m_pMain=this;
	if(!m_pSWT->Init())
	{
		AddToErrorLog("Init socket watch thread failed");
		return false;

	}
	m_pSWT->Start();
	
	AddToErrorLog("Socket mode VAR");

	m_pNnmSave=new NnmSaveThread;
	if(m_pNnmSave==NULL)
		return false;
	m_pNnmSave->m_pMain= this;
	if(!m_pNnmSave->Init())
	{
		AddToErrorLog("Init NnmSaveThread failed! ");
		printf("Init NnmSaveThread failed! ");
		return false;
	}
	m_pNnmSave->Start();

	printf("\n\nSVDB server starts working... \n");
	return true;

}

void SvdbMain::initialBackupConfigIni(void)
{
	GeneralPool *pGp=NULL;
	std::string fname= "svdbbackupconfig.ini";
	char filepath[512]={0};
	sprintf(filepath,"%s/data/IniFile/%s", m_pOption->m_rootpath.c_str(),fname.c_str());
    
	if(m_IniPool.find(filepath)!=NULL )
		pGp=m_IniPool[filepath];
	else
	{
		pGp=new GeneralPool(filepath);
		if(pGp==NULL)
			return;
		pGp->Load();
		m_IniPool[filepath]=pGp;
	}
	if(pGp==NULL)
		return;

	char strvalue[512]={0};
	sprintf(strvalue,"%s", IdcUser::CenterAdress.c_str());
	pGp->WriteString("backupCenter", "CenterAdress", strvalue);

	memset(strvalue,0,512);
	sprintf(strvalue,"%s", IdcUser::GetLocalSEIdStr().c_str());
	pGp->WriteString("backupCenter", "LocalSEId", strvalue);

	memset(strvalue,0,512);
	sprintf(strvalue,"%d", IdcUser::SELocked);
	pGp->WriteString("backupCenter", "SELocked", strvalue);

	memset(strvalue,0,512);
	sprintf(strvalue,"%s", IdcUser::CacheQueueName.c_str());
	pGp->WriteString("backupCenter", "CacheQueueName", strvalue);

	memset(strvalue,0,512);
	sprintf(strvalue,"%d", IdcUser::msBackup);
	pGp->WriteString("backupCenter", "MilliSecondBetweenBackup", strvalue);

	memset(strvalue,0,512);
	sprintf(strvalue,"%d", IdcUser::RecordsAutoBackup);
	pGp->WriteString("backupCenter", "RecordsAutoBackup", strvalue);

	memset(strvalue,0,512);
	sprintf(strvalue,"%d", IdcUser::ConfigAutoBackup);
	pGp->WriteString("backupCenter", "ConfigAutoBackup", strvalue);

	memset(strvalue,0,512);
	sprintf(strvalue,"%d", IdcUser::AcceptConfigIncoming);
	pGp->WriteString("backupCenter", "AcceptConfigIncoming", strvalue);

	memset(strvalue,0,512);
	sprintf(strvalue,"%d", IdcUser::FullTeleBackup);
	pGp->WriteString("backupCenter", "FullTeleBackup", strvalue);

	
	pGp->Submit();
	pGp->m_LastAccessTime=svutil::TTime::GetCurrentTimeEx();
}

bool SvdbMain::Start(void)
{
	return true;
}
bool  SvdbMain::Quit(void)
{
	return m_pDB->Quit();
}


