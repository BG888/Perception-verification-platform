/*
 * main.c
 *
 *  Created on: Sep 1, 2022
 *      Author: tct
 */
#include "App.h"
#include "Platform.h"
#include <pthread.h>
#include "crc32.h"

struct SoftVersionType{
	UINT32 s4	:	8;
	UINT32 s3   :	6;
	UINT32 s2	:	10;
	UINT32 s1	:	8;
};

union SoftVersionUnionType{
	struct SoftVersionType bit;
	UINT32 all;
};

union SoftVersionUnionType g_softversion = {{3,2,3,4}};

/**
 * @brief      主函数入口
 * @details
 * @param
 * @return     long long 获取到的系统时间
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
int main(void)
{
	printf("--------------ScenenController software version %d.%d.%d.%d------------\r\n", g_softversion.bit.s1, g_softversion.bit.s2, g_softversion.bit.s3, g_softversion.bit.s4); /* prints !!!Hello World!!! */
	pthread_t cmdRun;
	CommandResolveInit();
	/*初始化场景元素*/
	SceneElementCtrlRun();
	SceneElementSamplerInit();

	/*与平台建立连接*/
	PlatCommDamon();

	/*启动自检线程 场景元素报备 场景元素命令解析*/

	printf("Initial Scene Element\r\n");

	pthread_create(&cmdRun, NULL, TempCtrl, NULL);
	pthread_join(cmdRun, NULL);
	while(1);
}
