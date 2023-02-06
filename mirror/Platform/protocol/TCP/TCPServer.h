/* TCPServer.h
 *
 *
 *  Created on: Oct 20, 2022
 *      Author: tct
 */

#ifndef PLATFORM_PROTOCOL_TCP_TCPSERVER_H_
#define PLATFORM_PROTOCOL_TCP_TCPSERVER_H_

#include "commontypes.h"

struct clientInfoType;
struct NetParaType;

typedef void (*RecvCBType)(struct NetParaType* pnet, struct clientInfoType* psrc, UINT8* pdata, UINT16 len);
typedef void (*commonTCPCBType)(struct NetParaType* pnet, struct clientInfoType* psrc);
struct cbFuncType {
	RecvCBType tcpRecvDisposal;			/*收到数据后的回调*/
	commonTCPCBType tcpConnected;		/*连接成功回调*/
	commonTCPCBType	tcpDisconnected;	/*设备掉线回调*/
};

struct clientInfoType{
	void* next;				/*指针指向下一个位置*/
	char ip[16];			/*对端地址*/
	UINT16 port;			/*对端端口号*/
	int accFd;				/*描述符*/

};

struct NetParaType{
	UINT16 localPort;			/*本地端口号*/
	struct cbFuncType* iCBFunc;
	void* ClientInfo;		/*client链表*/
};

/**
 * @brief     TCP服务器初始化函数
 * @details
 * @param	  struct NetParaType* ptr操作句柄
 * @return    成功后返回handler,失败返回0
 *     - 0  		成功
 *     - ohter      失败
 */
struct NetParaType* TCPServerInit(struct NetParaType* ptr);

/**
 * @brief     TCP服务器发送数据
 * @details
 * @param	  struct NetParaType* pNet	操作句柄
 * 			  char* pdestIP, 			目的IP
 * 			  UINT8* pdata, 			发送数据
 * 			  UINT16 len				发送数据长度
 * @return    int 返回初始化结果
 *     - 0  		成功
 *     - ohter      失败
 */
int TCPServerSend(struct NetParaType* pNet, char* pdestIP, UINT8* pdata, UINT16 len);

#endif /* PLATFORM_PROTOCOL_TCP_TCPSERVER_H_ */
