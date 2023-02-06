/*
 * TCPClient.c
 *
 *  Created on: Sep 14, 2022
 *      Author: tct
 */


#include "TCPClient.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#define MAXLINE 1000

/**
 * @brief     tcp客户端初始化
 * @details
 * @param	  struct TCPSampleGroupType* ptr
 * @return    int 返回初始化结果
 *     - 0  		成功
 *     - ohter      失败
 */
int TcpClientInit(struct TCPCommType* ptr)
{
    struct sockaddr_in  servaddr;

	ptr->fd = socket(AF_INET, SOCK_STREAM, 0);
    if(0 > ptr->fd)
    {
        printf("create socket error: %s(errno: %d)\n", strerror(errno),errno);
        return 0;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(ptr->port);
    if( inet_pton(AF_INET, ptr->ip, &servaddr.sin_addr) <= 0){
        printf("inet_pton error for %s\n",ptr->ip);
        return 0;
    }

    while( connect(ptr->fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        printf("connect error: %s(errno: %d)\n",strerror(errno),errno);
        sleep(1);
    }

    return 0;
}

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
STATUS_T TCPClientSend(struct TCPCommType* ptr, char* pbuf, UINT16 len)
{
	ssize_t size;
	STATUS_T ret = RET_UNKNOWN_ERR;

	size = send(ptr->fd, pbuf, len, MSG_NOSIGNAL);
	if(size != len)
	{
		ret = RET_IO_ERR;
	}
	else
	{
		ret = RET_NO_ERR;
	}

	return ret;
}

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
STATUS_T TCPClientRecv(struct TCPCommType* ptr, char* pbuf, UINT16* len)
{
	STATUS_T ret = RET_UNKNOWN_ERR;

	memset(pbuf, 0, *len);
	*len = recv(ptr->fd, pbuf, MAXLINE, 0);
	if(*len <= 0)
	{
		ret = RET_DATA_ERR;
	}
	else
	{
		ret = RET_NO_ERR;
	}

	return ret;
}

