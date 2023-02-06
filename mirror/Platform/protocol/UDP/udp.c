/*
 * udp.c
 *
 *  Created on: Sep 9, 2022
 *      Author: tct
 */

#include<stdio.h>
#include<stdlib.h>
#include<assert.h>
#include<string.h>
#include<unistd.h>

#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include <time.h>
#include <sys/time.h>

#include "udp.h"

#define BUFF_LEN 1024

/**
 * @brief     UDP数据组数据接收数据
 * @details
 * @param
 * @return     void* arg	参数表
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void* UDPGroupSampleDamon(void* arg)
{
    struct UDPSampleGroupType* ptr = *(struct UDPSampleGroupType**)arg;
	void* pBuf = *(void**)(arg + sizeof(void*));
	struct timeval* pstampUS = (struct timeval*)(arg + sizeof(void*) + sizeof(void*));
    int count;
    struct sockaddr_in clent_addr;
    socklen_t len;

    while(1)
    {
        len = sizeof(clent_addr);
        count = recvfrom(ptr->server_fd, pBuf, BUFF_LEN, 0, (struct sockaddr*)&clent_addr, &len);
        if(count == -1)
        {
            printf("recieve data fail!\n");
            return NULL;
        }
        ptr->rsvCnt++;
        gettimeofday(pstampUS, NULL);
		printf("%ld:%ld\r\n", pstampUS->tv_sec, pstampUS->tv_usec);
        printf("recieve data!\n");
    }

    return NULL;
}

/**
 * @brief     UDP数据组初始化
 * @details
 * @param
 * @return     void* pHandler	操作句柄
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
int UDPInit(void* pHandler)
{
    int ret;
    struct sockaddr_in ser_addr;
    struct UDPSampleGroupType* ptr = pHandler;

    ptr->server_fd = socket(AF_INET, SOCK_DGRAM, 0); //AF_INET:IPV4;SOCK_DGRAM:UDP
    if(ptr->server_fd < 0)
    {
        printf("create socket fail!\n");
        return -1;
    }

    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);//inet_addr(ptr->ip); //IP地址，需要进行网络序转换，INADDR_ANY：本地地址
    ser_addr.sin_port = htons(ptr->port);  //端口号，需要网络序转换

    ret = bind(ptr->server_fd, (struct sockaddr*)&ser_addr, sizeof(ser_addr));
    if(ret < 0)
    {
        printf("socket bind fail!\n");
        return -1;
    }

    return 0;
}
