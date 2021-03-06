// GsmOperate.cpp: implementation of the CGsmOperate class.
//
//////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "GsmOperate.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
CGsmOperate::CGsmOperate()
{

}

CGsmOperate::~CGsmOperate()
{

}
/////////////////////////////////////////////////////////////////////////////////
// 函数：gsmDecode8bit                                                         //
// 说明：8-bit解码                                                             //
// 参数：                                                                      //
//      pSrc: 源编码串指针                                                     //
//      pDst: 目标字符串指针                                                   //
//      nSrcLength: 源编码串长度                                               //
// 返回值: 目标字符串长度                                                      //
/////////////////////////////////////////////////////////////////////////////////
int CGsmOperate::gsmDecode8bit(const unsigned char *pSrc, char *pDst, int nSrcLength)
{
    // 简单复制
	memcpy(pDst, pSrc, nSrcLength);

	// 输出字符串加个结束符
	*pDst = '\0';

	return nSrcLength;
}
/////////////////////////////////////////////////////////////////////////////////
// 函数：gsmEncode8bit                                                         //
// 说明：8-bit编码                                                             //
// 参数：                                                                      //
//      pSrc: 源编码串指针                                                     //
//      pDst: 目标字符串指针                                                   //
//      nSrcLength: 源编码串长度                                               //
// 返回: 目标字符串长度                                                        //
/////////////////////////////////////////////////////////////////////////////////
int CGsmOperate::gsmEncode8bit(const char *pSrc, unsigned char *pDst, int nSrcLength)
{
    // 简单复制
	memcpy(pDst, pSrc, nSrcLength);
	return nSrcLength;
}

/////////////////////////////////////////////////////////////////////////////////
// 函数：gsmDecodeUcs2                                                         //
// 说明：UCS2解码                                                              //
// 参数：                                                                      //
//      pSrc: 源编码串指针                                                     //
//      pDst: 目标字符串指针                                                   //
//      nSrcLength: 源编码串长度                                               //
// 返回: 目标字符串长度                                                        //
/////////////////////////////////////////////////////////////////////////////////
int CGsmOperate::gsmDecodeUcs2(const unsigned char* pSrc, char* pDst, int nSrcLength)
{
	int nDstLength;        // UNICODE宽字符数目
    WCHAR wchar[128];      // UNICODE串缓冲区
    
    // 高低字节对调，拼成UNICODE
    for(int i=0; i<nSrcLength/2; i++)
    {
        // 先高位字节
        wchar[i] = *pSrc++ << 8;
        // 后低位字节
        wchar[i] |= *pSrc++;
    }
    // UNICODE串-->字符串
    nDstLength = ::WideCharToMultiByte(CP_ACP, 0, wchar, nSrcLength/2, pDst, 160, NULL, NULL);
    // 输出字符串加个结束符    
    pDst[nDstLength] = '\0';    
    // 返回目标字符串长度
    return nDstLength;
}
/////////////////////////////////////////////////////////////////////////////////
// 函数：gsmEncodeUcs2                                                         //
// 说明：UCS2编码                                                              //
// 参数：                                                                      //
//      pSrc: 源字符串指针                                                     //
//      pDst: 目标编码串指针                                                   //
//      nSrcLength: 源字符串长度                                               //
// 返回: 目标编码串长度                                                        //
/////////////////////////////////////////////////////////////////////////////////
int CGsmOperate::gsmEncodeUcs2(const char* pSrc, unsigned char* pDst, int nSrcLength)
{
    int nDstLength;        // UNICODE宽字符数目
    WCHAR wchar[128] = {0};      // UNICODE串缓冲区
    
    // 字符串-->UNICODE串
    nDstLength = ::MultiByteToWideChar(CP_ACP, 0, pSrc, nSrcLength, wchar, 128);

    // 高低字节对调，输出
    for(int i=0; i<nDstLength; i++)
    {
    // 先输出高位字节
        *pDst++ = wchar[i] >> 8;
        // 后输出低位字节
        *pDst++ = wchar[i] & 0xff;
    }
    
    // 返回目标编码串长度
    return nDstLength * 2;
}
/////////////////////////////////////////////////////////////////////////////////
// 函数：gsmDecode7bit                                                         //
// 说明：7-bit解码                                                             //
// 参数：                                                                      //
//      pSrc: 源编码串指针                                                     //
//      pDst: 目标字符串指针                                                   //
//      nSrcLength: 源编码串长度                                               //
// 返回值: 目标字符串长度                                                      //
/////////////////////////////////////////////////////////////////////////////////
int CGsmOperate::gsmDecode7bit(const unsigned char* pSrc, char* pDst, int nSrcLength)
{
    int nSrc;        // 源字符串的计数值
    int nDst;        // 目标解码串的计数值
    int nByte;       // 当前正在处理的组内字节的序号，范围是0-6
    unsigned char nLeft;    // 上一字节残余的数据    
    // 计数值初始化
    nSrc = 0;
    nDst = 0;    
    // 组内字节序号和残余数据初始化
    nByte = 0;
    nLeft = 0;    
    // 将源数据每7个字节分为一组，解压缩成8个字节
    // 循环该处理过程，直至源数据被处理完
    // 如果分组不到7字节，也能正确处理
    while(nSrc<nSrcLength)
    {
        // 将源字节右边部分与残余数据相加，去掉最高位，得到一个目标解码字节
        *pDst = ((*pSrc << nByte) | nLeft) & 0x7f;
        // 将该字节剩下的左边部分，作为残余数据保存起来
        nLeft = *pSrc >> (7-nByte);    
        // 修改目标串的指针和计数值
        pDst++;
        nDst++;
        // 修改字节计数值
        nByte++;    
        // 到了一组的最后一个字节
        if(nByte == 7)
        {
            // 额外得到一个目标解码字节
            *pDst = nLeft;    
            // 修改目标串的指针和计数值
            pDst++;
            nDst++;    
            // 组内字节序号和残余数据初始化
            nByte = 0;
            nLeft = 0;
        }    
        // 修改源串的指针和计数值
        pSrc++;
        nSrc++;
	}
    
    *pDst = 0;    
    // 返回目标串长度
    return nDst;
}
/////////////////////////////////////////////////////////////////////////////////
// 函数：gsmEncode7bit                                                         //
// 说明：7bit编码                                                              //
// 参数：                                                                      //
//      pSrc: 源字符串指针                                                     //
//      pDst: 目标编码串指针                                                   //
//      nSrcLength: 源字符串长度                                               //
// 返回: 目标编码串长度                                                        //
/////////////////////////////////////////////////////////////////////////////////
int CGsmOperate::gsmEncode7bit(const char* pSrc, unsigned char* pDst, int nSrcLength)
{
    int nSrc;        // 源字符串的计数值
    int nDst;        // 目标编码串的计数值
    int nChar;       // 当前正在处理的组内字符字节的序号，范围是0-7
    unsigned char nLeft;    // 上一字节残余的数据
    
    // 计数值初始化
    nSrc = 0;
    nDst = 0;
    
    // 将源串每8个字节分为一组，压缩成7个字节
    // 循环该处理过程，直至源串被处理完
    // 如果分组不到8字节，也能正确处理
    while(nSrc<nSrcLength)
    {
        // 取源字符串的计数值的最低3位
        nChar = nSrc & 7;
		  // 处理源串的每个字节
        if(nChar == 0)
        {
            // 组内第一个字节，只是保存起来，待处理下一个字节时使用
            nLeft = *pSrc;
        }
        else
        {
            // 组内其它字节，将其右边部分与残余数据相加，得到一个目标编码字节
            *pDst = (*pSrc << (8-nChar)) | nLeft;
      // 将该字节剩下的左边部分，作为残余数据保存起来
            nLeft = *pSrc >> nChar;
            // 修改目标串的指针和计数值 pDst++;
            nDst++; 
        } 
        
        // 修改源串的指针和计数值
        pSrc++; nSrc++;
    }
    
    // 返回目标串长度
    return nDst; 
}
/////////////////////////////////////////////////////////////////////////////////
// 函数：gsmString2Bytes                                                       //
// 说明：可打印字符串转换为字节数据                                            //
//       如："C8329BFD0E01" --> {0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01}           //
// 参数：                                                                      //
//      pSrc: 源字符串指针                                                     //
//      pDst: 目标数据指针                                                     //
//      nSrcLength: 源字符串长度                                               //
// 返回: 目标数据长度                                                          //
/////////////////////////////////////////////////////////////////////////////////
int CGsmOperate::gsmString2Bytes(const char* pSrc, unsigned char* pDst, int nSrcLength)
{
    for(int i=0; i<nSrcLength; i+=2)
    {
        // 输出高4位
        if(*pSrc>='0' && *pSrc<='9')
        {
            *pDst = (*pSrc - '0') << 4;
		}
        else
        {
            *pDst = (*pSrc - 'A' + 10) << 4;
        }
    
        pSrc++;
        // 输出低4位
        if(*pSrc>='0' && *pSrc<='9')
        {
            *pDst |= *pSrc - '0';
        }
        else
		{
            *pDst |= *pSrc - 'A' + 10;
        }
        pSrc++;
        pDst++;
    }
    
    // 返回目标数据长度
    return nSrcLength / 2;
}
/////////////////////////////////////////////////////////////////////////////////
// 函数：gsmBytes2String                                                       //
// 说明：字节数据转换为可打印字符串                                            //
//      如：{0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01} --> "C8329BFD0E01"            //
// 参数：                                                                      //
//      pSrc: 源数据指针                                                       //
//      pDst: 目标字符串指针                                                   //
//      nSrcLength: 源数据长度                                                 //
// 返回值: 目标字符串长度                                                      //
/////////////////////////////////////////////////////////////////////////////////
int CGsmOperate::gsmBytes2String(const unsigned char* pSrc, char* pDst, int nSrcLength)
{
    const char tab[]="0123456789ABCDEF";    // 0x0-0xf的字符查找表    
    for(int i=0; i<nSrcLength; i++)
    {
		// 输出低4位
        *pDst++ = tab[*pSrc >> 4];                                        
        // 输出高4位
        *pDst++ = tab[*pSrc & 0x0f];    
        pSrc++;
	}    
    // 输出字符串加个结束符
    *pDst = '\0';    
    // 返回目标字符串长度
    return nSrcLength * 2;
}

/////////////////////////////////////////////////////////////////////////////////
// 函数：gsmDecodePdu                                                          //
// 说明：PDU解码，用于接收、阅读短消息                                         //
// 参数：                                                                      //
//      pSrc: 源PDU串指针                                                      //
//      pDst: 目标PDU参数指针                                                  //
// 返回: 用户信息串长度                                                        //
/////////////////////////////////////////////////////////////////////////////////
int CGsmOperate::gsmDecodePdu(const char* pSrc, SM_PARAM* pDst)
{
    int nDstLength;          // 目标PDU串长度
    unsigned char tmp;       // 内部用的临时字节变量
    unsigned char buf[256];  // 内部用的缓冲区

    // SMSC地址信息段
    gsmString2Bytes(pSrc, &tmp, 2);    // 取长度
    tmp = (tmp - 1) * 2;    // SMSC号码串长度
    pSrc += 4;              // 指针后移
    gsmSerializeNumbers(pSrc, pDst->SCA, tmp);    // 转换SMSC号码到目标PDU串
    pSrc += tmp;        // 指针后移
    
    // TPDU段基本参数、回复地址等
    gsmString2Bytes(pSrc, &tmp, 2);    // 取基本参数
    pSrc += 2;        // 指针后移
    //if(tmp & 0x80)
    {
        // 包含回复地址，取回复地址信息
        gsmString2Bytes(pSrc, &tmp, 2);    // 取长度
        if(tmp & 1) tmp += 1;    // 调整奇偶性
        pSrc += 4;          // 指针后移
        gsmSerializeNumbers(pSrc, pDst->TPA, tmp);    // 取TP-RA号码
		m_strRecvPhone = CString(pDst->TPA);
        pSrc += tmp;        // 指针后移
    }
    
    // TPDU段协议标识、编码方式、用户信息等
    gsmString2Bytes(pSrc, (unsigned char*)&pDst->TP_PID, 2);    // 取协议标识(TP-PID)
    pSrc += 2;        // 指针后移
    gsmString2Bytes(pSrc, (unsigned char*)&pDst->TP_DCS, 2);    // 取编码方式(TP-DCS)
    pSrc += 2;        // 指针后移
    gsmSerializeNumbers(pSrc, pDst->TP_SCTS, 14);        // 服务时间戳字符串(TP_SCTS) 
    pSrc += 14;       // 指针后移
    gsmString2Bytes(pSrc, &tmp, 2);    // 用户信息长度(TP-UDL)
    pSrc += 2;        // 指针后移
    pDst->TP_DCS=8;
    if(pDst->TP_DCS == GSM_7BIT)    
    {
        // 7-bit解码
        nDstLength = gsmString2Bytes(pSrc, buf, tmp & 7 ? (int)tmp * 7 / 4 + 2 : (int)tmp * 7 / 4);  // 格式转换
        gsmDecode7bit(buf, pDst->TP_UD, nDstLength);    // 转换到TP-DU
        nDstLength = tmp;
    }
    else if(pDst->TP_DCS == GSM_UCS2)
    {
        // UCS2解码
        nDstLength = gsmString2Bytes(pSrc, buf, tmp * 2);        // 格式转换
        nDstLength = gsmDecodeUcs2(buf, pDst->TP_UD, nDstLength);    // 转换到TP-DU
    }
    else
    {
        // 8-bit解码
        nDstLength = gsmString2Bytes(pSrc, buf, tmp * 2);        // 格式转换
        nDstLength = gsmDecode8bit(buf, pDst->TP_UD, nDstLength);    // 转换到TP-DU
		// nDstLength = gsmDecodeUcs2(buf, pDst->TP_UD, nDstLength);    // 转换到TP-DU		
    }
    
    // 返回目标字符串长度
    return nDstLength;

}

//PDU全串的编解码模块。为简化编程，有些字段用了固定值。
/////////////////////////////////////////////////////////////////////////////////
// 函数：gsmEncodePdu                                                          //
// 说明：PDU编码，用于编制、发送短消息                                         //
// 参数：                                                                      //
//      pSrc: 源PDU参数指针                                                    //
//      pDst: 目标PDU串指针                                                    //
// 返回: 目标PDU串长度                                                         //
/////////////////////////////////////////////////////////////////////////////////
int CGsmOperate::gsmEncodePdu(const SM_PARAM* pSrc, char* pDst)
{
    int nLength;             // 内部用的串长度
    int nDstLength;          // 目标PDU串长度
    unsigned char buf[256];  // 内部用的缓冲区
    
    // SMSC地址信息段
    nLength = strlen(pSrc->SCA);    // SMSC地址字符串的长度    
    buf[0] = (char)((nLength & 1) == 0 ? nLength : nLength + 1) / 2 + 1;    // SMSC地址信息长度
    buf[1] = 0x91;        // 固定: 用国际格式号码
    nDstLength = gsmBytes2String(buf, pDst, 2);        // 转换2个字节到目标PDU串
    nDstLength += gsmInvertNumbers(pSrc->SCA, &pDst[nDstLength], nLength);    // 转换SMSC到目标PDU串
     // TPDU段基本参数、目标地址等
    nLength = strlen(pSrc->TPA);    // TP-DA地址字符串的长度
	m_strRecvPhone=CString(pSrc->TPA);
    buf[0] = 0x11;            // 是发送短信(TP-MTI=01)，TP-VP用相对格式(TP-VPF=10)
    buf[1] = 0;               // TP-MR=0
    buf[2] = (char)nLength;   // 目标地址数字个数(TP-DA地址字符串真实长度)
    buf[3] = 0x91;            // 固定: 用国际格式号码
    nDstLength += gsmBytes2String(buf, &pDst[nDstLength], 4);  // 转换4个字节到目标PDU串
    nDstLength += gsmInvertNumbers(pSrc->TPA, &pDst[nDstLength], nLength); // 转换TP-DA到目标PDU串
    
    // TPDU段协议标识、编码方式、用户信息等
    nLength = strlen(pSrc->TP_UD);    // 用户信息字符串的长度
    buf[0] = pSrc->TP_PID;        // 协议标识(TP-PID)
    buf[1] = pSrc->TP_DCS;        // 用户信息编码方式(TP-DCS)
    buf[2] = 0;            // 有效期(TP-VP)为5分钟
    if(pSrc->TP_DCS == GSM_7BIT)    
    {
        // 7-bit编码方式
//        buf[3] = nLength;            // 编码前长度
//        nLength = gsmEncode7bit(pSrc->TP_UD, &buf[4], nLength+1) + 4; 
		// 转换		TP-DA到目标PDU串
        buf[3] = gsmEncode8bit(pSrc->TP_UD, &buf[4], nLength);    // 转换TP-DA到目标PDU串
        nLength = buf[3] + 4;        // nLength等于该段数据长度
    }
    else if(pSrc->TP_DCS == GSM_UCS2)
    {
        // UCS2编码方式
        buf[3] = gsmEncodeUcs2(pSrc->TP_UD, &buf[4], nLength);    // 转换TP-DA到目标PDU串
        nLength = buf[3] + 4;        // nLength等于该段数据长度
    }
    else
    {
        // 8-bit编码方式
        buf[3] = gsmEncode8bit(pSrc->TP_UD, &buf[4], nLength);    // 转换TP-DA到目标PDU串
        nLength = buf[3] + 4;        // nLength等于该段数据长度
    }
    nDstLength += gsmBytes2String(buf, &pDst[nDstLength], nLength);        // 转换该段数据到目标PDU串
    
    // 返回目标字符串长度
    return nDstLength;
}
/////////////////////////////////////////////////////////////////////////////////
// 函数：gsmSerializeNumbers                                                   //
// 说明：两两颠倒的字符串转换为正常顺序的字符串                                //
//       如："8613910199192" --> "683119109991F2"                              //
// 参数：                                                                      //
//      pSrc: 源字符串指针                                                     //
//      pDst: 目标字符串指针                                                   //
//      nSrcLength: 源字符串长度                                               //
// 返回: 目标字符串长度                                                        //
/////////////////////////////////////////////////////////////////////////////////
int CGsmOperate::gsmSerializeNumbers(const char* pSrc, char* pDst, int nSrcLength)
{
    int nDstLength;   // 目标字符串长度
    char ch;          // 用于保存一个字符
    
    // 复制串长度
    nDstLength = nSrcLength;
	  // 两两颠倒
    for(int i=0; i<nSrcLength;i+=2)
    {
        ch = *pSrc++;        // 保存先出现的字符
        *pDst++ = *pSrc++;   // 复制后出现的字符
        *pDst++ = ch;        // 复制先出现的字符
    }
    
    // 最后的字符是'F'吗？
    if(*(pDst-1) == 'F')
    {
        pDst--;
        nDstLength--;        // 目标字符串长度减1
    }
    
    // 输出字符串加个结束符
    *pDst = '\0';
    
    // 返回目标字符串长度
    return nDstLength;
}
/////////////////////////////////////////////////////////////////////////////////
// 函数：gsmInvertNumbers                                                      //
// 说明：PDU串中的号码和时间，都是两两颠倒的字符串。利用下面两个函数可进行正反 //
//      变换：正常顺序的字符串转换为两两颠倒的字符串，若长度为奇数，补'F'凑成偶//
//      数，如："8613910199192" --> "683119109991F2"                           //
// 参数：                                                                      //
//      pSrc: 源字符串指针                                                     //
//      pDst: 目标字符串指针                                                   //
//      nSrcLength: 源字符串长度                                               //
// 返回: 目标字符串长度                                                        //
/////////////////////////////////////////////////////////////////////////////////
int CGsmOperate::gsmInvertNumbers(const char* pSrc, char* pDst, int nSrcLength)
{
    int nDstLength;   // 目标字符串长度
    char ch;          // 用于保存一个字符
    
    // 复制串长度
    nDstLength = nSrcLength;
    
    // 两两颠倒
    for(int i=0; i<nSrcLength;i+=2)
    {
        ch = *pSrc++;        // 保存先出现的字符
        *pDst++ = *pSrc++;   // 复制后出现的字符
        *pDst++ = ch;        // 复制先出现的字符
    }
    
    // 源串长度是奇数吗？
    if(nSrcLength & 1)
   {
        *(pDst-2) = 'F';     // 补'F'
        nDstLength++;        // 目标串长度加1
    }
    
    // 输出字符串加个结束符
    *pDst = '\0';
    
    // 返回目标字符串长度
    return nDstLength;
}

