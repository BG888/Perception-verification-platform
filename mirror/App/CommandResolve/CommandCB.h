/**
@file  CommandCB.h
@brief  
@author 交控研究院
@version 1.0.0.0
@date 2021年11月21日
*/
/*
 * CommandCB.h
 *
 *  Created on: 2021年11月21日
 *      Author: TCT_XPS
 */

#ifndef USER_COMMANDRESOLVEUSER_COMMANDCB_H_
#define USER_COMMANDRESOLVEUSER_COMMANDCB_H_

#include "commontypes.h"

#define COMMAND_CNT        10

/* 命令帧类型 同步/非同步 */
enum CMDSyncType{
    COMMAND_SYNC = 0,
    COMMAND_NSYNC = 1,
};

/* 每条命令对应的参数 */
struct CommandType{
    STATUS_T (*CommandCB)(void* pData); /*当前命令号对应的执行函数*/
    enum CMDSyncType sync : 1;      /*设置命令同步执行或异步执行*/
    UINT8 rsv : 7;                  /*保留*/
};

extern struct CommandType g_commandTab[COMMAND_CNT];

/**
* @brief 参数写入操作
* @param UINT8* pdata,      发送数据
* @param UINT8 len,         数据长度
*/
void CmdUserOperation(UINT8* pdata, UINT8 len);


#endif /* USER_COMMANDRESOLVEUSER_COMMANDCB_H_ */
