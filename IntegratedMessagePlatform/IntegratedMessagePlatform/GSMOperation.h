#ifndef _GSM_OPERATION_H_
#define _GSM_OPERATION_H_

#include <cstring>
#include <string>
#include <vector>
#include <afx.h>
#include <iostream>
#include "Message.h"
#include "DBInterface.h"
#include <time.h> 
using namespace std;

// 用户信息编码方式
#define GSM_7BIT        0
#define GSM_8BIT        4
#define GSM_UCS2        8

// 短消息参数结构，编码/解码共用
// 其中，字符串以0结尾
typedef struct {
	char SCA[16];       // 短消息服务中心号码(SMSC地址)
	char TPA[16];       // 目标号码或回复号码(TP-DA或TP-RA)
	char TP_PID;        // 用户信息协议标识(TP-PID)
	char TP_DCS;        // 用户信息编码方式(TP-DCS)
	char TP_SCTS[16];   // 服务时间戳字符串(TP_SCTS), 接收时用到
	char TP_UD[161];    // 原始用户信息(编码前或解码后的TP-UD)
	char index;         // 短消息序号，在读取时用到
} SM_PARAM;

BOOL OpenComm1(const char* pPort, int nBaudRate, int nParity, int nByteSize, int nStopBits);
BOOL OpenComm2(const char* pPort, int nBaudRate, int nParity, int nByteSize, int nStopBits);

BOOL CloseComm1();
BOOL CloseComm2();

void WriteComm1(void* pData, int nLength);
void WriteComm2(void* pData, int nLength);

int ReadComm1(void* pData, int nLength);
int ReadComm2(void* pData, int nLength);

BOOL gsmInit();

int gsmEncode7bit(const char* pSrc, unsigned char* pDst, int nSrcLength);
int gsmDecode7bit(const unsigned char* pSrc, char* pDst, int nSrcLength);

int gsmEncode8bit(const char* pSrc,unsigned char* pDst,int nSrcLength);
int gsmDecode8bit(const unsigned char* pSrc, char* pDst, int nSrcLength);

int gsmEncodeUcs2(const char* pSrc, unsigned char* pDst, int nSrcLength);
int gsmDecodeUcs2(const unsigned char* pSrc, char* pDst, int nSrcLength);

int gsmString2Bytes(const char* pSrc, unsigned char* pDst, int nSrcLength);
int gsmBytes2String(const unsigned char* pSrc, char* pDst, int nSrcLength);

int gsmInvertNumbers(const char* pSrc, char* pDst, int nSrcLength);
int gsmSerializeNumbers(const char* pSrc, char* pDst, int nSrcLength);
int gsmEncodePdu(const SM_PARAM* pSrc, char* pDst);
int gsmDecodePdu(const char* pSrc, SM_PARAM* pDst);

BOOL gsmSendMessage(const SM_PARAM* pSrc);
int gsmReadMessage(SM_PARAM* pMsg);
BOOL gsmDeleteMessage(const int index);
BOOL gsmGetCSCA();
BOOL gsmGetMessage(SM_PARAM* pMsg, int index);

void convertToSmparam(Message msg, SM_PARAM* pMsg);
void sendMessage(Message msg);

#endif