/*
 * Transaction.c
 *
 *  Created on: Sep 14, 2022
 *      Author: tct
 */
#include "Transaction.h"
#include "Platform.h"
#include "EndianChange.h"
#include "crc32.h"
#include <pthread.h>

#include "SensorCtrl/SceneElement.h"

#include "CommandResolve/CommandResolve.h"

#define FRAME_HEAD_DEFAULT	  	0x55aa
#define FRAME_TAIL_DEFAULT		0xaa55

#define FRAME_HEAD		0

struct CommonType{
	struct TCPCommType* tcpComm;
	UINT32 recvSerial;
	UINT32 sendSerial;
	UINT16 loaclID;
	void* plist;
};

struct RecvDataHeadFormatType{
	UINT16 head;
	UINT32 seral;
	UINT16 len;
	UINT16 type;
}ALIGN(1);

struct RecvDataTailFormatType{
	UINT32 crc32;
	UINT16 tail;
}ALIGN(1);

struct SendDataHeadFormatType{
	UINT16 head;
	UINT32 serial;
	UINT16 payloadSize;
	UINT16 localID;
	UINT16 type;
}ALIGN(1);

struct SendDataTailFormatType{
	UINT32 crc32;
	UINT16 tail;
}ALIGN(1);

struct CommonType g_CommwithPlat;

pthread_t g_commRecv;
pthread_t g_commSend;

/**
 * @brief     数据包发送
 * @details	 按预设规则发送数据包
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T MessageSend(char* ip, UINT8* data, UINT16 len, enum SendDataType type)
{
	STATUS_T ret = RET_UNKNOWN_ERR;
	UINT8 sendbuf[1000] = {0};
	struct SendDataHeadFormatType* pdataHead = (struct SendDataHeadFormatType*)sendbuf;
	struct SendDataTailFormatType* pdataTail;
	/*报头拼装*/
	pdataHead->head = FRAME_HEAD_DEFAULT;
	pdataHead->serial = g_CommwithPlat.sendSerial++;
	pdataHead->localID = g_CommwithPlat.loaclID;
	pdataHead->type = type;
	pdataHead->payloadSize = len;
#ifdef LITTLE_ENDIAN_SQUENCE
	pdataHead->head = ChangeEndian16(pdataHead->head);
	pdataHead->serial = ChangeEndian32(pdataHead->serial);
	pdataHead->localID = ChangeEndian16(pdataHead->localID);
	pdataHead->type = ChangeEndian16(pdataHead->type);
	pdataHead->payloadSize = ChangeEndian16(pdataHead->payloadSize);;
#endif
	/*数据载荷拷贝*/
	memcpy(pdataHead + 1, data, len);
	pdataTail = (struct SendDataTailFormatType* )((UINT8*)(pdataHead + 1) + len);

	/*报文尾部拼装*/
	UINT16 crcsize = sizeof(struct SendDataHeadFormatType) + len;
	pdataTail->crc32 = Crc32(sendbuf, crcsize);
	pdataTail->tail = FRAME_TAIL_DEFAULT;
#ifdef LITTLE_ENDIAN_SQUENCE
	pdataTail->crc32 = ChangeEndian32(pdataTail->crc32);
	pdataTail->tail = ChangeEndian16(pdataTail->tail);
#endif
	/**/

	if(NULL == g_CommwithPlat.tcpComm)
	{
		return ret;
	}

	ret = TCPClientSend(g_CommwithPlat.tcpComm, (char*)sendbuf, crcsize + sizeof(struct SendDataTailFormatType));
	if(RET_NO_ERR == ret)
	{
//		printf("send completed\r\n");
	}
	else
	{
		printf("send error\r\n");
	}

	return ret;
}

/**
 * @brief     数据报接收
 * @details	 按预设规则发送数据包
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void* MessageResv(void* arg)
{
	STATUS_T ret = RET_UNKNOWN_ERR;
	static char recv[1000] = {0};
	UINT16 len = 1000;
	struct RecvDataHeadFormatType* head;
	struct RecvDataTailFormatType* tail;
	static UINT32 cnt = 0;

	if(NULL == g_CommwithPlat.tcpComm)
	{
		return NULL;
	}

	while(1)
	{
		/*数据接收*/
		ret = TCPClientRecv(g_CommwithPlat.tcpComm, recv, &len);
		if(RET_NO_ERR != ret)
		{
			printf("Receive from platform error\r\n");
			return NULL;
		}
		head = (struct RecvDataHeadFormatType*)recv;

#ifdef LITTLE_ENDIAN_SQUENCE
		head->head = ChangeEndian16(head->head);
		head->seral = ChangeEndian32(head->seral);
		head->len = ChangeEndian16(head->len);
		head->type = ChangeEndian16(head->type);
#endif

		tail = (struct RecvDataTailFormatType*)((char*)(head + 1) + head->len);
#ifdef LITTLE_ENDIAN_SQUENCE
		tail->crc32 = ChangeEndian32(tail->crc32);
		tail->tail = ChangeEndian16(tail->tail);
#endif

		cnt++;
		printf("receive head = %x cnt = %d\r\n", head->head, cnt);
		/*数据包验证*/
		if((head->head == FRAME_HEAD_DEFAULT) && (tail->tail == FRAME_TAIL_DEFAULT))
		{
			printf("good frame head\r\n");
			/*CRC验证*/

			/*数据序列号验证*/
			if(g_CommwithPlat.recvSerial == head->seral)
			{
				g_CommwithPlat.recvSerial++;
				printf("good frame serial\r\n");
				UINT8 dataLen = (UINT8)head->len;
				/*命令数据解析*/
				CommandResolveCB((UINT8*)(head + 1), &dataLen, NULL, NULL);

			}
			else
			{
				g_CommwithPlat.recvSerial = head->seral + 1;
			}

		}

	}

	close(g_CommwithPlat.tcpComm->fd);
	return NULL;
}


/**
 * @brief     数据报发送
 * @details	 按预设规则发送数据包
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void* MessageSendSCState(void* arg)
{
	STATUS_T ret = RET_UNKNOWN_ERR;
	UINT8 payload[1000] = {0};
	enum SendDataType type = SCState;
	UINT16 ctrlsize, samplersize, payloadsize = 0;

	while(1)
	{
		/*拼装发送信息 (UINT16*)payload存储控制类场景元素长度 (UINT16*)payload + 1存储反馈类场景元素长度*/
		ctrlsize = GetSCCtrllerSta(&payload[4], 996, (UINT16*)payload);
		samplersize = GetSCSamplerSta(&payload[4+ctrlsize], 996 - ctrlsize, (UINT16*)payload + 1);
#ifdef LITTLE_ENDIAN_SQUENCE
		*(UINT16*)payload = ChangeEndian16(*(UINT16*)payload);
		*((UINT16*)payload + 1) = ChangeEndian16(*((UINT16*)payload + 1));
#endif
		payloadsize = ctrlsize + samplersize + sizeof(UINT16) + sizeof(UINT16);
		ret = MessageSend(NULL, payload, payloadsize, type);
		if(0 == ret)
		{
			*(UINT16*)payload = 0;
		}
//		usleep(200000);
		sleep(1);
	}

	return NULL;
}

/**
 * @brief     通信初始化
 * @details	  初始化与平台之间的通信
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void* TransactionRun(void* argv)
{
	g_CommwithPlat.tcpComm = malloc(sizeof(struct TCPCommType));
	/*配置文件读取*/
	strcpy(g_CommwithPlat.tcpComm->ip, "172.16.224.169");
	g_CommwithPlat.tcpComm->port = 8550;
	g_CommwithPlat.recvSerial = 0;
	g_CommwithPlat.sendSerial = 0;
	g_CommwithPlat.loaclID = 0x1234;

	/*建立TCP连接*/
	TcpClientInit(g_CommwithPlat.tcpComm);

	pthread_create(&g_commRecv, NULL, MessageResv, NULL);
	pthread_detach(g_commRecv);
	pthread_create(&g_commSend, NULL, MessageSendSCState, NULL);
	pthread_detach(g_commSend);

	return NULL;
}

/**
 * @brief     平台通信守护进程
 * @details	  平台通信进程启动服务
 * @param
 * @return
 */
void PlatCommDamon(void)
{
	pthread_t transPid;
	pthread_create(&transPid, NULL, TransactionRun, NULL);
	pthread_detach(transPid);
}

