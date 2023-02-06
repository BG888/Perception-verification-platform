/*
 * Transaction.h
 *
 *  Created on: Sep 14, 2022
 *      Author: tct
 */

#ifndef APP_SENSORCTRL_TRANSACTION_H_
#define APP_SENSORCTRL_TRANSACTION_H_

#include <commontypes.h>


enum SendDataType{
	SCState = 0,
	PowerOnTest = 1,
	RunTest = 2,
	Command = 3,
};

/**
 * @brief     通信初始化
 * @details	  初始化与平台之间的通信
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void* TransactionRun(void* argv);

/**
 * @brief     平台通信守护进程
 * @details	  平台通信进程启动服务
 * @param
 * @return
 */
void PlatCommDamon(void);

/**
 * @brief     数据报接收
 * @details	 按预设规则发送数据包
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void* MessageResv(void* arg);

/**
 * @brief     数据报发送
 * @details	 按预设规则发送数据包
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void* MessageSendSCState(void* arg);

/**
 * @brief     数据报发送
 * @details	 按预设规则发送数据包
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T MessageSend(char* ip, UINT8* data, UINT16 len, enum SendDataType type);

#endif /* APP_SENSORCTRL_TRANSACTION_H_ */
