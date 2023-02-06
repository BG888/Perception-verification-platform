/*
 * udp.h
 *
 *  Created on: Sep 9, 2022
 *      Author: tct
 */

#ifndef PLATFORM_PROTOCOL_UDP_UDP_H_
#define PLATFORM_PROTOCOL_UDP_UDP_H_

#include <commontypes.h>

struct UDPSampleGroupType{
	char ip[20];
	UINT16 port;
	int server_fd;
	UINT8 rsvCnt;
};

/**
 * @brief     UDP数据组初始化
 * @details
 * @param
 * @return     void* pHandler	操作句柄
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
int UDPInit(void* pHandler);

/**
 * @brief     UDP数据组数据接受
 * @details
 * @param
 * @return     void* arg	参数表
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void* UDPGroupSampleDamon(void* arg);

#endif /* PLATFORM_PROTOCOL_UDP_UDP_H_ */
