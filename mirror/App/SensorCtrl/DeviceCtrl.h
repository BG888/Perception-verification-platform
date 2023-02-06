/*
 * DeviceCtrl.h
 *
 *  Created on: Sep 1, 2022
 *      Author: tct
 */

#ifndef APP_SENSORCTRL_DEVICECTRL_H_
#define APP_SENSORCTRL_DEVICECTRL_H_

#include "commontypes.h"
#include  <time.h>
#include "Platform.h"
#include <sys/time.h>

#define MAX_OUTPUT_ELEMENT_COUNT	20
#define MAX_INPUT_ELEMENT_COUNT	20
#define MAX_INPUT_GROUP_COUNT	20

enum ProtocolType{
	udp = 1,
	tcp,
	modbustcp,
	tcp4G,

};

struct EleCtrlType{
	UINT16 inputCmd;			/*平台输入参数*/
	UINT16 outputCmd;			/*板卡输出参数*/
};

struct DeviceCtrlType{
	UINT16 ID;					/*设备操作ID*/
	char name[20];				/*名称*/
	enum ProtocolType pro;		/*协议*/
	void* pdev;					/*操作句柄*/
	UINT16 holdtime;			/*持续时间 单位s*/
	void* proConf;				/*指向不同协议的配置参数*/
	softTimer_st timer;
	UINT16 devGroup;

	int (*init)(void* ptr, void** pHandler);
	STATUS_T (*open)(void* pHandler, UINT32 para);
	void (*close)(UINT16 argc, void* argv);
	void (*getinfo)(void* pHandler);
	void (*confCpy)(void*, void*);
};

/*输入型场景元素采样参数配置*/
struct RawDataConfType{
	enum ProtocolType pro;

};

struct InputDataGroupType{
	UINT16 groupID;				/*场景元素ID  xml文件读取*/
	char name[20];				/*名称 xml文件读取*/
	enum ProtocolType pro;		/*通信协议 xml文件读取*/
	void* initConf;				/*初始化配置 xml文件读取*/
	char* pBuf;					/*buffer地址*/
	struct timeval stampus;		/*采样时间戳us*/
	UINT32 datasize;			/*数据长度 xml文件读取*/
	pthread_t pthread;			/*线程ID*/

	int (*init)(void* ptr);
	void* (*resvDamon)(void* arg);

};

extern struct DeviceCtrlType* g_deviceCtrlTab[MAX_OUTPUT_ELEMENT_COUNT];

/**
 * @brief      	查找操作句柄
 * @details		根据操作ID找到对应的操作句柄
 * @param		UINT16 id	查找ID
 * @return     struct ElementCtrlType* 操作句柄
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
struct DeviceCtrlType* GetDeviceHandlerFromID(UINT16 id);

/**
 * @brief      设备层控制
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T DeviceCtrl(struct DeviceCtrlType* pdevice, UINT32 para);

/**
 * @brief      初始化场景元素
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void* DeviceCtrlRunDamon(void* arg);

/**
 * @brief     设备层初始化服务
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
int DeviceCtrlIint(void);

/**
 * @brief     设备层采样服务
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void* DeviceCtrlSampleDamon(void* arg);

/**
 * @brief     设备层采样服务初始化
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T DeviceCtrlSampleInit(void);

/**
 * @brief     获取采样组对应的操作句柄
 * @details
 * @param		UINT16 id	组别ID
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
struct InputDataGroupType* GetGroupHandler(UINT16 id);

#endif /* APP_SENSORCTRL_DEVICECTRL_H_ */
