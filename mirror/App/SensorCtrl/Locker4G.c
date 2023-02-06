/*
 * Locker4G.c
 *
 *  Created on: Oct 20, 2022
 *      Author: tct
 */
#include "Locker4G.h"
#include "DeviceCtrl.h"
#include "stddef.h"

/**
 * @brief     抑或校验算法
 * @details
 * @param	  uint8_t *data,	数据
 * 			  uint16_t length	长度
 * @return    函数执行结果
 */
uint8_t getBcc(uint8_t *data, uint16_t length)
{
    uint8_t bcc = 0;        // Initial value
#if 1
	while(length--)
	{
		bcc ^= *data++;
	}
#else
	for (uint8_t i = 0; i < length; i++ )
	{
		bcc ^= data[i];        // crc ^= *data;
	}
#endif
    return bcc;
}

void LockerRecvFunc(struct NetParaType* pnet, struct clientInfoType* psrc, UINT8* pdata, UINT16 len)
{
	/*利用container_of方法查询*/
	struct Locker4GConfType* plocker = container_of((void*)pnet, struct Locker4GConfType, pNetCtrl);
	struct LockerProtocolHeadType* pBuf = (struct LockerProtocolHeadType*)pdata;
	plocker->deviceID = pBuf->address;

	if(7 >= pBuf->length)
	{
		UINT8 sendBuf[sizeof(struct LockerProtocolHeadType) + 1] = {0};
		struct LockerProtocolHeadType* ptr = (struct LockerProtocolHeadType*)sendBuf;
		ptr->length = sizeof(struct LockerProtocolHeadType) + 1;
		ptr->address = pBuf->address;
		ptr->cmd = heartBeat | 0x80;
		ptr->para = 0;
		sendBuf[sizeof(struct LockerProtocolHeadType)] = getBcc(sendBuf, sizeof(struct LockerProtocolHeadType));
		TCPServerSend(pnet, NULL, sendBuf, ptr->length);
	}
	else
	{
		printf("4G locker state:");
		for(int i = 0; i < pBuf->length; i++)
		{
			printf("%x", (*(pdata+i)));
		}
		printf("\r\n");
	}

	printf("Receive data, client ip %s\r\n", psrc->ip);
}

void LockerOnline(struct NetParaType* pnet, struct clientInfoType* psrc)
{
	/*利用container_of方法查询*/
	struct Locker4GConfType* plocker = container_of((void*)pnet, struct Locker4GConfType, pNetCtrl);

	*plocker->phandler = pnet;
	printf("Device online, client ip %s\r\n", psrc->ip);
}

void LockerOffline(struct NetParaType* pnet, struct clientInfoType* psrc)
{
	/*利用container_of方法查询*/
	struct DeviceCtrlType* pSCAddr = container_of((void*)pnet, struct DeviceCtrlType, pdev);

	pSCAddr->pdev = NULL;
	printf("Device offline, client ip %s\r\n", psrc->ip);
}



/**
 * @brief     打开地锁设备
 * @details
 * @param	  void* ptr,
 * 			  UINT32 para
 * @return    函数执行结果
 */
STATUS_T Open4GDevice(void* ptr, UINT32 para)
{
	STATUS_T ret = RET_UNKNOWN_ERR;
	struct DeviceCtrlType* pHandler = ptr;
	printf("Opening device %s\r\n", pHandler->name);
	if(NULL != (struct Locker4GConfType*)pHandler->proConf)
	{
		if(NULL != (struct Locker4GConfType*)pHandler->pdev)
		{
			UINT8 sendBuf[sizeof(struct LockerProtocolSendHeadType) + 1] = {0};
			struct LockerProtocolSendHeadType* plocker = (struct LockerProtocolSendHeadType*)sendBuf;
			plocker->length = sizeof(struct LockerProtocolSendHeadType) + 1;
			plocker->address = ((struct Locker4GConfType*)pHandler->proConf)->deviceID;
			plocker->cmd = rising;
			sendBuf[sizeof(struct LockerProtocolSendHeadType)] = getBcc(sendBuf, sizeof(struct LockerProtocolSendHeadType));
			TCPServerSend((struct NetParaType*)pHandler->pdev, NULL, sendBuf, plocker->length);
			ret = RET_NO_ERR;
		}
		else
		{
			printf("device %s not online!\r\n", pHandler->name);
		}
	}
	else
	{
		printf("device %s not initied!\r\n", pHandler->name);
	}

	return ret;
}

/**
 * @brief     打开地锁设备
 * @details
 * @param	  void* ptr,
 * 			  UINT32 para
 * @return    函数执行结果
 */
STATUS_T Close4GDevice(void* ptr, UINT32 para)
{
	STATUS_T ret = RET_UNKNOWN_ERR;
	struct DeviceCtrlType* pHandler = ptr;
	/**/

	printf("Opening device %s\r\n", pHandler->name);
	if(NULL != (struct Locker4GConfType*)pHandler->proConf)
	{
		if(NULL != (struct Locker4GConfType*)pHandler->pdev)
		{
			UINT8 sendBuf[sizeof(struct LockerProtocolSendHeadType) + 1] = {0};
			struct LockerProtocolSendHeadType* plocker = (struct LockerProtocolSendHeadType*)sendBuf;
			plocker->length = sizeof(struct LockerProtocolSendHeadType) + 1;
			plocker->address = ((struct Locker4GConfType*)pHandler->proConf)->deviceID;
			plocker->cmd = falling;
			sendBuf[sizeof(struct LockerProtocolSendHeadType)] = getBcc(sendBuf, sizeof(struct LockerProtocolSendHeadType));
			TCPServerSend((struct NetParaType*)pHandler->pdev, NULL, sendBuf, plocker->length);
			ret = RET_NO_ERR;
		}
		else
		{
			printf("device %s not online!\r\n", pHandler->name);
		}
	}
	else
	{
		printf("device %s not initied!\r\n", pHandler->name);
	}

	return ret;
}

/**
 * @brief     参数拷贝
 * @details
 * @param	  void* psrc, 	源参数
 * 			  void* pdst,	目的参数
 * @return    函数执行结果
 */
void LockerConfCpy(void* psrc, void* pdst)
{
	memcpy((UINT8*)pdst, (UINT8*)psrc, sizeof(struct Locker4GConfType));
}

/**
 * @brief     获取关键信息
 * @details
 * @param	  void* ptr,
 * @return    函数执行结果
 */
void Get4GLockerInfo(void* ptr)
{
	struct DeviceCtrlType* pHandler = ptr;
	/**/

	printf("Opening device %s\r\n", pHandler->name);
	if(NULL != (struct Locker4GConfType*)pHandler->proConf)
	{
		UINT8 sendBuf[sizeof(struct LockerProtocolSendHeadType) + 1] = {0};
		struct LockerProtocolSendHeadType* plocker = (struct LockerProtocolSendHeadType*)sendBuf;
		plocker->length = sizeof(struct LockerProtocolSendHeadType) + 1;
		plocker->address = ((struct Locker4GConfType*)pHandler->proConf)->deviceID;
		plocker->cmd = getstate;
		sendBuf[sizeof(struct LockerProtocolSendHeadType)] = getBcc(sendBuf, sizeof(struct LockerProtocolSendHeadType));
		TCPServerSend((struct NetParaType*)pHandler->pdev, NULL, sendBuf, plocker->length);
	}
	else
	{
		printf("device %s not initied!\r\n", pHandler->name);
	}
}

/**
 * @brief     初始化地锁设备
 * @details
 * @param	  void* ptr,
 * 			  UINT32 para
 * @return    函数执行结果
 */
int Device4GInit(void* ptr, void** pHandler)
{
	struct Locker4GConfType* pNet = ptr;
	pNet->pNetCtrl.iCBFunc = malloc(sizeof(struct cbFuncType));
	pNet->pNetCtrl.localPort = pNet->port;
	pNet->phandler = pHandler;
	pNet->pNetCtrl.iCBFunc->tcpConnected = LockerOnline;
	pNet->pNetCtrl.iCBFunc->tcpDisconnected = LockerOffline;
	pNet->pNetCtrl.iCBFunc->tcpRecvDisposal = LockerRecvFunc;

	(void*)TCPServerInit(&pNet->pNetCtrl);

	return 0;
}
