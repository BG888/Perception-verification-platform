/*
 * ModbusTCPOps.h
 *
 *  Created on: Sep 1, 2022
 *      Author: tct
 */

#ifndef PLATFORM_PROTOCOL_MODBUSTCP_MODBUSTCPOPS_H_
#define PLATFORM_PROTOCOL_MODBUSTCP_MODBUSTCPOPS_H_

#include <commontypes.h>
#include "modbus.h"

struct ModbusTCPConfType{
	char ip[20];
	UINT16 port;
	UINT16 slaveID;
	UINT16 addr;
	UINT16 addrAux;
	UINT16 data;
	int listenfd;
	int	acceptfd;
};

//struct ModbusTCPConfType{
//	char ip[20];
//	UINT16 port;
//	UINT16 slaveID;
//	UINT16
//};

/**
 * @brief      modbusTCP线圈写入
 * @details    写线圈指
 * @param      void* pHandler	输出操作句柄
 * 			   UINT16 addr 	线圈地址
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
int ModbusTCPSetBit(void* pHandler, UINT16 addr, UINT16 state);

/**
 * @brief      Modbustcp从站连接
 * @details    初始化MODBUS tcp  设置从站地址
 * @param      struct ModbusTCPConfType* ptr 输入 操作结构体
 * 			   void* pHandler	输出操作句柄
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */

int ModbusTCPMasterInit(void* ptr, void** pHandler);

/**
 * @brief      Modbustcp从站连接-tcp服务端
 * @details    初始化MODBUS tcp  设置从站地址
 * @param      struct ModbusTCPConfType* ptr 输入 操作结构体
 * 			   void* pHandler	输出操作句柄
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
int ModbusTCPServerMasterInit(void* ptr, void** pHandler);

#endif /* PLATFORM_PROTOCOL_MODBUSTCP_MODBUSTCPOPS_H_ */
