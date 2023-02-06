/*
 * TCPClient.h
 *
 *  Created on: Sep 14, 2022
 *      Author: tct
 */

#ifndef PLATFORM_PROTOCOL_TCP_TCPCLIENT_H_
#define PLATFORM_PROTOCOL_TCP_TCPCLIENT_H_

#include <commontypes.h>

struct TCPCommType{
	char ip[20];
	UINT16 port;
	int fd;
	UINT8 rsvCnt;
};

/**
 * @brief     tcp客户端接收功能-阻塞
 * @details
 * @param	  struct TCPSampleGroupType* ptr	操作句柄
 * 			  char* pbuf						接收数据
 * 			  UINT16* len					    数据长度
 * @return    int 返回初始化结果
 *     - 0  		成功
 *     - ohter      失败
 */
STATUS_T TCPClientRecv(struct TCPCommType* ptr, char* pbuf, UINT16* len);

/**
 * @brief     tcp客户端发送功能-非阻塞
 * @details
 * @param	  struct TCPSampleGroupType* ptr	操作句柄
 * 			  char* pbuf						发送数据
 * 			  UINT16 len					    数据长度
 * @return    int 返回初始化结果
 *     - 0  		成功
 *     - ohter      失败
 */
STATUS_T TCPClientSend(struct TCPCommType* ptr, char* pbuf, UINT16 len);

/**
 * @brief     tcp客户端初始化
 * @details
 * @param	  struct TCPSampleGroupType* ptr
 * @return    int 返回初始化结果
 *     - 0  		成功
 *     - ohter      失败
 */
int TcpClientInit(struct TCPCommType* ptr);

#endif /* PLATFORM_PROTOCOL_TCP_TCPCLIENT_H_ */
