/*
 * MobusTCPOps.c
 *
 *  Created on: Sep 1, 2022
 *      Author: tct
 */
#include "ModbusTCPOps.h"
#include <unistd.h>
#include <pthread.h>

pthread_t g_tcpAccept;
pthread_t g_tcpRecv;

struct TCPThreadType{
	void** phander;
	void* pCtx;
	struct ModbusTCPConfType* conf;
};
/**
 * @brief      Modbustcp从站连接
 * @details    初始化MODBUS tcp  设置从站地址
 * @param      struct ModbusTCPConfType* ptr 输入 操作结构体
 * 			   void* pHandler	输出操作句柄
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
int ModbusTCPMasterInit(void* ptr, void** pHandler)
{
	struct ModbusTCPConfType* pConf = ptr;
	modbus_t* pCtx;
	int ret = -1;
//	uint16_t tab_reg[100] = {0};
	if(NULL != ptr)
	{
		/*初始化modbusTCP配置*/
		pCtx = modbus_new_tcp(pConf->ip, pConf->port);

		/*设置从机地址*/
		ret = modbus_set_slave(pCtx, pConf->slaveID);
		if (-1 != ret)
		{
			/*连接modbus设备*/
			ret = modbus_connect(pCtx);
			if(-1 != ret)
			{
				printf("Connect to ip = %s port = %d device success!!!\r\n",pConf->ip, pConf->port);

				*pHandler = pCtx;
			}
			else
			{
				printf("Connect to ip = %s port = %d device failed!\r\n",pConf->ip, pConf->port);
				modbus_free(pCtx);
			}
		}
		else
		{
			fprintf(stderr, "Invalid slave ID\n");
			modbus_free(pCtx);
		}
	}
	else
	{
		fprintf(stderr, "Invalid input ID\n");
	}

	return ret;
}

/**
 * @brief      ModbustcpTCPreceive
 * @details    接收到数据后处理数据，当接收到返回值为0时认为对端离线，清理操作句柄后，立即返回
 * @param      void* argv 参数
 * @return     void*
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void* ModbusTCPRecv(void* argv)
{
	struct TCPThreadType* pcenter = argv;
	while(1)
	{
		UINT8 query[MODBUS_TCP_MAX_ADU_LENGTH];
		int rc;

		rc = modbus_receive(pcenter->pCtx, query);
		if(0 < rc)
		{

		}
		else
		{
//			*pcenter->phander = NULL;
			break;
		}
	}
	return NULL;
}

/**
 * @brief      ModbustcpTCPaccept
 * @details    接收到连接后初始化句柄
 * @param      void* argv 参数
 * @return     void*
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void* ModbusTCPAccept(void* argv)
{
	struct TCPThreadType center;
	memcpy(&center, argv, sizeof(struct TCPThreadType));

	while(1)
	{
		center.conf->acceptfd = modbus_tcp_accept(center.pCtx, &center.conf->listenfd);
		*center.phander = center.pCtx;

		ModbusTCPRecv(&center);
	}
	return NULL;
}

/**
 * @brief      Modbustcp从站连接-tcp服务端
 * @details    初始化MODBUS tcp  设置从站地址
 * @param      struct ModbusTCPConfType* ptr 输入 操作结构体
 * 			   void* pHandler	输出操作句柄
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
int ModbusTCPServerMasterInit(void* ptr, void** pHandler)
{
	struct ModbusTCPConfType* pConf = ptr;
	modbus_t* pCtx = NULL;
	int ret = -1;
//	uint16_t tab_reg[100] = {0};
	if(NULL != ptr)
	{
		/*初始化modbusTCP配置*/
		pCtx = modbus_new_tcp(NULL, pConf->port);
//		pCtx = modbus_new_tcp(NULL, 8081);
//		modbus_tcp_debug(pCtx, TRUE);
		pConf->listenfd = modbus_tcp_listen(pCtx, 1);
		if(-1 == pConf->listenfd)
		{
			printf("MODBUS TCP SERVER ERROR to LISTEN \r\n");
		}

		/*设置从机地址*/
		ret = modbus_set_slave(pCtx, pConf->slaveID);
		if (-1 != ret)
		{
			/*这里的局部变量传值有问题！！！！！！！！！！！！！！！！！！！*/
			struct TCPThreadType center;
			center.pCtx = pCtx;
			center.phander = pHandler;
			center.conf = pConf;
			pthread_create(&g_tcpAccept, NULL, ModbusTCPAccept, &center);
			pthread_detach(g_tcpAccept);

			sleep(2);
		}
		else
		{
			fprintf(stderr, "Invalid slave ID\n");
			modbus_free(pCtx);
		}
	}
	else
	{
		fprintf(stderr, "Invalid input ID\n");
	}

	return ret;
}

/**
 * @brief      modbusTCP线圈写入
 * @details    写线圈指
 * @param      void* pHandler	输出操作句柄
 * 			   UINT16 addr 	线圈地址
 * 			   UINT16 state 控制开关
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
int ModbusTCPSetBit(void* pHandler, UINT16 addr, UINT16 state)
{
	modbus_t* pCtx = pHandler;
	int ret = -1;

	if(NULL != pCtx)
	{
		/*控制线圈*/
		while(1)
		{
			ret = modbus_write_bit(pCtx, addr, state?1:0);
			if(-1 != ret)
			{
				fprintf(stderr, "modbus operation ok\r\n");
				break;
			}
			else
			{
				fprintf(stderr, "modbus operation error\r\n");
			}
		}
	}
	else
	{
		fprintf(stderr, "Invalid input handler\n");
	}

	return ret;
}
