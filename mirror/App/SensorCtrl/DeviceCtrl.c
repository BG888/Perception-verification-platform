/*
 * DeviceCtrl.c
 *
 *  Created on: Sep 1, 2022
 *      Author: tct
 */

#include  <stdio.h>
#include  <stdlib.h>
#include  <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

#include "Locker4G.h"
#include "DeviceCtrl.h"
#include "Platform.h"

#define MAX_GROUP_CNT		10
#define MAX_SAME_GROUP_CNT	20

struct DeviceCtrlType* g_deviceCtrlTab[MAX_OUTPUT_ELEMENT_COUNT];
UINT16 g_acuCtrlCnt = 0;

struct DeviceSampleType* g_deviceSampleTab[MAX_INPUT_ELEMENT_COUNT];
UINT16 g_acuSampleCnt = 0;

struct InputDataGroupType* g_inputDataGroup[MAX_INPUT_GROUP_COUNT];
UINT16 g_acuGroupCnt = 1;

/*组别分析*/
UINT16 g_devGroupTab[MAX_GROUP_CNT][MAX_SAME_GROUP_CNT];
/**
 * @brief      获取系统时间ms
 * @details
 * @param
 * @return     long long 获取到的系统时间
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
long long GetSysTimeMS(void)
{
	long long timeMS = 0;
	struct timeval sysCurrentTime;

	gettimeofday(&sysCurrentTime, NULL);
	timeMS = ((long long)sysCurrentTime.tv_sec*1000000 + sysCurrentTime.tv_usec)/1000;

	return timeMS;
}

/**
 * @brief      	查找操作句柄
 * @details		根据操作ID找到对应的操作句柄
 * @param		UINT16 id	查找ID
 * @return     struct ElementCtrlType* 操作句柄
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
struct DeviceCtrlType* GetDeviceHandlerFromID(UINT16 id)
{
	UINT16 i = 0;
	struct DeviceCtrlType* ptr = NULL;

	for(i = 0; i < MAX_OUTPUT_ELEMENT_COUNT; i++)
	{
		if(0 != g_deviceCtrlTab[i])
		{
			if(id == g_deviceCtrlTab[i]->ID)
			{
				ptr = g_deviceCtrlTab[i];
				break;
			}
			else
			{

			}
		}
		else
		{

		}
	}
	return ptr;
}

/**
 * @brief      关闭Modbus设备
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void CloseModbusDevice(uint16_t argc, void* argv)
{
	struct DeviceCtrlType* pHandler = (struct DeviceCtrlType*)argv;

	printf("Closing device %s\r\n", pHandler->name);

    if(NULL != pHandler->pdev)
    {
    	ModbusTCPSetBit(pHandler->pdev, ((struct ModbusTCPConfType*)pHandler->proConf)->addr , 0);
    }
    else
    {
    	printf("Device handler is not initialed\r\n");
    }
}

/**
 * @brief      打开Modbus设备
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T OpenModbusDevice(void* ptr, UINT32 para)
{
	STATUS_T ret = RET_UNKNOWN_ERR;
	struct DeviceCtrlType* pHandler = ptr;
    /*打开modbus设备*/
	printf("Opening device %s\r\n", pHandler->name);
    if(NULL != pHandler->pdev)
    {
    	int result = ModbusTCPSetBit(pHandler->pdev, ((struct ModbusTCPConfType*)pHandler->proConf)->addr , 1);
    	if(-1 != result)
    		ret = RET_NO_ERR;
    	else
    		ret = RET_NO_ERR;

    	ModbusTCPSetBit(pHandler->pdev, ((struct ModbusTCPConfType*)pHandler->proConf)->addrAux , 0);
    }
    else
    {
    	printf("Device handler is not initialed\r\n");
    }

    return ret;
}

/**
 * @brief      同步同组别操作句柄
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void* SyncDevHandler(void* argv)
{
	while(1)
	{
		for(int i = 0; i < MAX_GROUP_CNT; i++)
		{
			/*是否有组别信息*/
			if(0xffff != g_devGroupTab[i][0])
			{
				/*第一个组的操作句柄是否被初始化*/
				if(NULL != g_deviceCtrlTab[g_devGroupTab[i][0]]->pdev)
				{
					for(int j = 1; j < MAX_SAME_GROUP_CNT; j++)
					{
						/**/
						if(0xFFFF != g_devGroupTab[i][j])
						{
							g_deviceCtrlTab[g_devGroupTab[i][j]]->pdev = g_deviceCtrlTab[g_devGroupTab[i][0]]->pdev;
							if(NULL != g_deviceCtrlTab[g_devGroupTab[i][0]]->confCpy)
							{
								g_deviceCtrlTab[g_devGroupTab[i][0]]->confCpy(g_deviceCtrlTab[g_devGroupTab[i][0]]->proConf, g_deviceCtrlTab[g_devGroupTab[i][j]]->proConf);
							}
						}
						else
						{

						}
					}
				}
				else
				{

				}
			}
			else
			{

			}
		}

		sleep(1);
	}
}

/**
 * @brief      设备层控制
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T DeviceCtrl(struct DeviceCtrlType* pdevice, UINT32 para)
{
	STATUS_T ret = RET_UNKNOWN_ERR;

	if(NULL != pdevice)
	{
		ret = pdevice->open(pdevice, para);
		if(0 != pdevice->holdtime)
		{
			/*启动定时器*/
			softTimer_Start(&pdevice->timer, SOFTTIMER_MODE_ONE_SHOT, pdevice->holdtime, pdevice->close, 1, pdevice);
		}
	}
	else
	{
		ret = RET_PARAM_ERR;
	}

	return ret;
}

/**
 * @brief      组别检查
 * @details	   读入设备操作层配置后 将同组元素归入一类 预设10个组别0-9 每个设备操作都有自己的组别号
 * 				同组设备初始化一次 同组设备操作句柄同步
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void CheckGroup(void)
{
	UINT16 row, index[MAX_GROUP_CNT] = {0};

	memset(g_devGroupTab, 0xff, sizeof(g_devGroupTab));

	for(int i = 0; i < g_acuCtrlCnt; i++)
	{
		if(0 != g_deviceCtrlTab[i]->devGroup)
		{
			row = g_deviceCtrlTab[i]->devGroup - 1;
			g_devGroupTab[row][index[row]] = i;
			index[row]++;
		}
	}
}

/**
 * @brief     设备层控制初始化服务
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
int DeviceCtrlIint(void)
{
	int ret = -1;

	/*读取配置文件*/
	puts("Read XML"); /* prints !!!Hello World!!! */

	Config_Init();

	/*与场景元素建立连接并初始化设备*/
	puts("Init device");

	{
		struct ModbusTCPConfType* pDevConf = malloc(sizeof(struct ModbusTCPConfType)); /*配置后期由xml文件给出*/
		g_deviceCtrlTab[0] = malloc(sizeof(struct DeviceCtrlType));					   /*数据保存点 部分信息由xml文件给出*/
		/*地锁设备上升模拟参数配置*/
		memcpy(pDevConf->ip, "172.16.224.190", sizeof("172.16.224.190"));
		pDevConf->port = 8551;
		pDevConf->slaveID = 1;
		pDevConf->addr = 2;
		pDevConf->addrAux = 3;
		strcpy(g_deviceCtrlTab[0]->name, "DO3");
		g_deviceCtrlTab[0]->ID = 0;
		g_deviceCtrlTab[0]->holdtime = 500;
		g_deviceCtrlTab[0]->init = ModbusTCPServerMasterInit;
		g_deviceCtrlTab[0]->open = OpenModbusDevice;
		g_deviceCtrlTab[0]->close = CloseModbusDevice;
		g_deviceCtrlTab[0]->pro = modbustcp;
		g_deviceCtrlTab[0]->proConf = pDevConf;
		g_deviceCtrlTab[0]->devGroup = 1;
	}

	{
		struct ModbusTCPConfType* pDevConf = malloc(sizeof(struct ModbusTCPConfType)); 							/*配置后期由xml文件给出*/
		g_deviceCtrlTab[1] = malloc(sizeof(struct DeviceCtrlType));					   /*数据保存点 部分信息由xml文件给出*/
		memcpy(pDevConf->ip, "172.16.224.190", sizeof("172.16.224.190"));
		pDevConf->port = 8551;
		pDevConf->slaveID = 1;
		pDevConf->addr = 3;
		pDevConf->addrAux = 2;
		strcpy(g_deviceCtrlTab[1]->name, "DO4");
		g_deviceCtrlTab[1]->ID = 1;
		g_deviceCtrlTab[1]->holdtime = 500;
		g_deviceCtrlTab[1]->init = ModbusTCPServerMasterInit;
		g_deviceCtrlTab[1]->open = OpenModbusDevice;
		g_deviceCtrlTab[1]->close = CloseModbusDevice;
		g_deviceCtrlTab[1]->pro = modbustcp;
		g_deviceCtrlTab[1]->proConf = pDevConf;
		g_deviceCtrlTab[1]->devGroup = 1;
	}

	{
		/*伸缩杆伸出控制参数配置*/
		struct ModbusTCPConfType* pDevConf = malloc(sizeof(struct ModbusTCPConfType)); 							/*配置后期由xml文件给出*/
		g_deviceCtrlTab[2] = malloc(sizeof(struct DeviceCtrlType));					   /*数据保存点 部分信息由xml文件给出*/
		memcpy(pDevConf->ip, "172.16.224.190", sizeof("172.16.224.190"));
		pDevConf->port = 8551;
		pDevConf->slaveID = 1;
		pDevConf->addr = 0;
		pDevConf->addrAux = 1;
		strcpy(g_deviceCtrlTab[2]->name, "DO0");
		g_deviceCtrlTab[2]->ID = 2;
		g_deviceCtrlTab[2]->holdtime = 700;
		g_deviceCtrlTab[2]->init = ModbusTCPServerMasterInit;
		g_deviceCtrlTab[2]->open = OpenModbusDevice;
		g_deviceCtrlTab[2]->close = CloseModbusDevice;
		g_deviceCtrlTab[2]->pro = modbustcp;
		g_deviceCtrlTab[2]->proConf = pDevConf;
		g_deviceCtrlTab[2]->devGroup = 1;
	}

	{
		/*伸缩杆缩回控制参数配置*/
		struct ModbusTCPConfType* pDevConf = malloc(sizeof(struct ModbusTCPConfType)); 							/*配置后期由xml文件给出*/
		g_deviceCtrlTab[3] = malloc(sizeof(struct DeviceCtrlType));					   /*数据保存点 部分信息由xml文件给出*/
		memcpy(pDevConf->ip, "172.16.224.190", sizeof("172.16.224.190"));
		pDevConf->port = 8551;
		pDevConf->slaveID = 1;
		pDevConf->addr = 1;
		pDevConf->addrAux = 0;
		strcpy(g_deviceCtrlTab[3]->name, "DO1");
		g_deviceCtrlTab[3]->ID = 3;
		g_deviceCtrlTab[3]->holdtime = 700;
		g_deviceCtrlTab[3]->init = ModbusTCPServerMasterInit;
		g_deviceCtrlTab[3]->open = OpenModbusDevice;
		g_deviceCtrlTab[3]->close = CloseModbusDevice;
		g_deviceCtrlTab[3]->pro = modbustcp;
		g_deviceCtrlTab[3]->proConf = pDevConf;
		g_deviceCtrlTab[3]->devGroup = 1;
	}

	{
		/*灯光打开控制参数配置*/
		struct ModbusTCPConfType* pDevConf = malloc(sizeof(struct ModbusTCPConfType)); 							/*配置后期由xml文件给出*/
		g_deviceCtrlTab[4] = malloc(sizeof(struct DeviceCtrlType));					   /*数据保存点 部分信息由xml文件给出*/
		memcpy(pDevConf->ip, "172.16.224.190", sizeof("172.16.224.190"));
		pDevConf->port = 8552;
		pDevConf->slaveID = 1;
		pDevConf->addr = 0;
		pDevConf->addrAux = 1;
		strcpy(g_deviceCtrlTab[4]->name, "LIGHT-DO0");
		g_deviceCtrlTab[4]->ID = 4;
		g_deviceCtrlTab[4]->holdtime = 100;
		g_deviceCtrlTab[4]->init = ModbusTCPServerMasterInit;
		g_deviceCtrlTab[4]->open = OpenModbusDevice;
		g_deviceCtrlTab[4]->close = CloseModbusDevice;
		g_deviceCtrlTab[4]->pro = modbustcp;
		g_deviceCtrlTab[4]->proConf = pDevConf;
		g_deviceCtrlTab[4]->devGroup = 2;
	}

	{
		/*灯光关闭控制参数配置*/
		struct ModbusTCPConfType* pDevConf = malloc(sizeof(struct ModbusTCPConfType)); 							/*配置后期由xml文件给出*/
		g_deviceCtrlTab[5] = malloc(sizeof(struct DeviceCtrlType));					   /*数据保存点 部分信息由xml文件给出*/
	//	memcpy(pDevConf->ip, "172.16.224.190", sizeof("172.16.224.190"));
		pDevConf->port = 8552;
		pDevConf->slaveID = 1;
		pDevConf->addr = 1;
		pDevConf->addrAux = 0;
		strcpy(g_deviceCtrlTab[5]->name, "LIGHT-DO1");
		g_deviceCtrlTab[5]->ID = 5;
		g_deviceCtrlTab[5]->holdtime = 100;
		g_deviceCtrlTab[5]->init = ModbusTCPServerMasterInit;
		g_deviceCtrlTab[5]->open = OpenModbusDevice;
		g_deviceCtrlTab[5]->close = CloseModbusDevice;
		g_deviceCtrlTab[5]->pro = modbustcp;
		g_deviceCtrlTab[5]->proConf = pDevConf;
		g_deviceCtrlTab[5]->devGroup = 2;
	}

	{
		/*灯箱牌控制抬起参数配置*/
		struct Locker4GConfType* pDevConf = malloc(sizeof(struct Locker4GConfType)); 							/*配置后期由xml文件给出*/
		g_deviceCtrlTab[6] = malloc(sizeof(struct DeviceCtrlType));					  							 /*数据保存点 部分信息由xml文件给出*/
		pDevConf->port = 8500;
		strcpy(g_deviceCtrlTab[6]->name, "4G-Locker-up");
		g_deviceCtrlTab[6]->ID = 6;
		g_deviceCtrlTab[6]->holdtime = 0;
		g_deviceCtrlTab[6]->init = Device4GInit;
		g_deviceCtrlTab[6]->open = Open4GDevice;
		g_deviceCtrlTab[6]->confCpy = LockerConfCpy;
		g_deviceCtrlTab[6]->getinfo = Get4GLockerInfo;
		g_deviceCtrlTab[6]->pro = tcp4G;
		g_deviceCtrlTab[6]->proConf = pDevConf;
		g_deviceCtrlTab[6]->devGroup = 3;
	}

	{
		/*灯箱牌控制降下参数配置*/
		struct Locker4GConfType* pDevConf = malloc(sizeof(struct Locker4GConfType)); 							/*配置后期由xml文件给出*/
		g_deviceCtrlTab[7] = malloc(sizeof(struct DeviceCtrlType));					   /*数据保存点 部分信息由xml文件给出*/
		pDevConf->port = 8500;
		strcpy(g_deviceCtrlTab[7]->name, "4G-Locker-down");
		g_deviceCtrlTab[7]->ID = 7;
		g_deviceCtrlTab[7]->holdtime = 0;
		g_deviceCtrlTab[7]->init = Device4GInit;
		g_deviceCtrlTab[7]->open = Close4GDevice;
		g_deviceCtrlTab[7]->confCpy = LockerConfCpy;
		g_deviceCtrlTab[7]->pro = tcp4G;
		g_deviceCtrlTab[7]->proConf = pDevConf;
		g_deviceCtrlTab[7]->devGroup = 3;
	}

	g_acuCtrlCnt = 8;

	CheckGroup();

	/*初始化协议*/
	for(int i = 0; i < MAX_GROUP_CNT; i++)
	{
		if(0xffff != g_devGroupTab[i][0])
		{
			ret = g_deviceCtrlTab[g_devGroupTab[i][0]]->init((void*)g_deviceCtrlTab[g_devGroupTab[i][0]]->proConf, &g_deviceCtrlTab[g_devGroupTab[i][0]]->pdev);
			if(-1 == ret)
			{
				puts("Init error");
			}
		}
	}

	for(int i = 0; i < g_acuCtrlCnt; i++)
	{
		/*为每个设备定义一个软timer*/
		softTimer_Init(&g_deviceCtrlTab[i]->timer, 1);
	}

	pthread_t pid;
	pthread_create(&pid, NULL, SyncDevHandler, NULL);
	pthread_detach(pid);

	sleep(5);
	return ret;
}

/**
 * @brief     获取采样组对应的操作句柄
 * @details
 * @param		UINT16 id	组别ID
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
struct InputDataGroupType* GetGroupHandler(UINT16 id)
{
	struct InputDataGroupType* pHandler;
	UINT16 i;

	for(i = 0; i < g_acuGroupCnt; i++)
	{
		if(id == g_inputDataGroup[i]->groupID)
		{
			pHandler = g_inputDataGroup[i];
		}
	}

	return pHandler;
}

/**
 * @brief     设备层采样服务初始化
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T DeviceCtrlSampleInit(void)
{
	STATUS_T ret = RET_UNKNOWN_ERR;
	/*从配置文件中读取配置*/
	g_inputDataGroup[0] = malloc(sizeof(struct InputDataGroupType));
	/*xml读取*/
	g_inputDataGroup[0]->groupID = 0;
	g_inputDataGroup[0]->datasize = 1024;
	strcpy(g_inputDataGroup[0]->name, "TIDS采样");
	g_inputDataGroup[0]->pro = udp;

	/*数据存放地址*/
	g_inputDataGroup[0]->pBuf = malloc(g_inputDataGroup[0]->datasize);
	switch(g_inputDataGroup[0]->pro)
	{
	case udp:
		g_inputDataGroup[0]->initConf = malloc(sizeof(struct InputDataGroupType));
		strcpy(((struct UDPSampleGroupType*)g_inputDataGroup[0]->initConf)->ip, "192.168.2.102");
		((struct UDPSampleGroupType*)g_inputDataGroup[0]->initConf)->port = 8800;
		g_inputDataGroup[0]->init = UDPInit;
		g_inputDataGroup[0]->resvDamon = UDPGroupSampleDamon;
		break;
	default:
		break;
	}

	/*UDP采样 Group建立连接*/
	int result = g_inputDataGroup[0]->init((void*)g_inputDataGroup[0]->initConf);
	if(0 == result)
	{
		ret = RET_NO_ERR;
	}

	return ret;
}

/**
 * @brief     设备层采样服务
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void* DeviceCtrlSampleDamon(void* arg)
{
	/*数据采样到Group*/
	pthread_create(&g_inputDataGroup[0]->pthread, NULL, g_inputDataGroup[0]->resvDamon, &g_inputDataGroup[0]->initConf);

	return NULL;
}

/**
 * @brief     设备层后台服务
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void* DeviceCtrlRunDamon(void* arg)
{
	while(1)
	{

		for(int i = 0; i < g_acuCtrlCnt; i++)
		{
			softTimer_Update(&g_deviceCtrlTab[i]->timer, 1);
		}
		sleep(1);
	}

	return NULL;
}


