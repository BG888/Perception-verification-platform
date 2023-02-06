/*
 * Locker4G.h
 *
 *  Created on: Oct 20, 2022
 *      Author: tct
 */

#ifndef APP_SENSORCTRL_LOCKER4G_H_
#define APP_SENSORCTRL_LOCKER4G_H_

#include "commontypes.h"
#include "Platform.h"

enum LockerErrorType{
	OPER_OK = 0,
	OPER_FAILED,
	ERR_FRAME_LENTH,
	ERR_ADDRESS,
	ERR_OPER_CODE,
	ERR_XOR_TEST,
	ERR_BUSY,
	ERR_PARA_SETTING,
	ERR_REACH_LIMIT,
	ERR_TIME_OUT,
	ERR_UPDATEFILE,
	ERR_LOWPOWER,
	ERR_LOCKED,
	ERR_COMM_TIMEOUT = 255,
};

enum LockerCMDType{
	heartBeat = 0,
	paraSet,
	rising,
	falling,
	stop,
	getstate,
};

struct LockerProtocolHeadType{
	UINT8 length;
	UINT32 address;
	UINT8 cmd;
	UINT8 para;
}ALIGN(1);

struct LockerProtocolSendHeadType{
	UINT8 length;
	UINT32 address;
	UINT8 cmd;
}ALIGN(1);

struct Locker4GConfType{
	UINT16 port;
	void** phandler;
	struct NetParaType pNetCtrl;
	UINT32 deviceID;

};

/**
 * @brief     打开地锁设备
 * @details
 * @param	  void* ptr,
 * 			  UINT32 para
 * @return    函数执行结果
 */
STATUS_T Open4GDevice(void* ptr, UINT32 para);

/**
 * @brief     打开地锁设备
 * @details
 * @param	  void* ptr,
 * 			  UINT32 para
 * @return    函数执行结果
 */
STATUS_T Close4GDevice(void* ptr, UINT32 para);

/**
 * @brief     参数拷贝
 * @details
 * @param	  void* psrc, 	源参数
 * 			  void* pdst,	目的参数
 * @return    函数执行结果
 */
void LockerConfCpy(void* psrc, void* pdst);

/**
 * @brief     初始化地锁设备
 * @details
 * @param	  void* ptr,
 * 			  UINT32 para
 * @return    函数执行结果
 */
void Get4GLockerInfo(void* ptr);

/**
 * @brief     初始化地锁设备
 * @details
 * @param	  void* ptr,
 * 			  UINT32 para
 * @return    函数执行结果
 */
int Device4GInit(void* ptr, void** pHandler);

#endif /* APP_SENSORCTRL_LOCKER4G_H_ */
