#ifndef _SOFTWARE_H__
#define _SOFTWARE_H__

#include <errno.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* 定时的工作模式定时 */
typedef enum _timerMode_Struct
{
    SOFTTIMER_MODE_ONE_SHOT = 0x55, /* 单次运行模式 */
    SOFTTIMER_MODE_PERIODIC = 0xAA, /* 周期运行模式 */
    SOFTTIMER_MODE_DEFAULT = 0xFF,  /* 默认模式，无效 */
} timerMode;

/* 定时器的状态定义 */
typedef enum _timerState_struct
{
    SOFTTIMER_STATE_STOPPED = 0xAA, /* 停止 */
    SOFTTIMER_STATE_RUNNING = 0x55, /* 运行 */
    SOFTTIMER_STATE_TIMEOUT = 0xCC  /* 超时 */
} timerState;

/* 软件定时器的结构定义 */
typedef struct _softTimer_struct
{
    uint8_t state;                              /* 状态 */
    uint8_t mode;                               /* 模式 */
    uint32_t timeout;                           /* 到期时间 */
    uint32_t interval;                          /* 定时周期 */
    void (*handler)(uint16_t argc, void *argv); /* 回调函数指针 */
    uint16_t argc;                              /* 参数的个数 */
    void *argv;                                 /* 参数指针 */
} softTimer_st;

/**************************************************************
* 函数名称：softTimer_Update
* 函数功能：定时的状态更新
* 函数参数：
*      timer   ：定时器的指针，不能为NULL
*      timernum：定时器的数量，不能超过定义的数量
* 函数返回： 无
* 函数错误：
*      如果参数无效（timer指针为NULL，或者num大于实际定义的定时
*      的数量，本函数将导致进程奔溃）
**************************************************************/
void softTimer_Update(softTimer_st *timer, uint16_t timernum);

/**************************************************************
* 函数名称：softTimer_Init
* 函数功能：定时器初始化，设置默认项
* 函数参数：
*      timer   ：定时器的指针，不能为NULL
*      timernum：定时器的数量，不能超过定义的数量
* 函数返回： 无
* 函数错误：
*      如果参数无效（timer指针为NULL，或者num大于实际定义的定时
*      的数量，本函数将导致进程奔溃）
**************************************************************/
void softTimer_Init(softTimer_st *timer, uint16_t timernum);

/************************************************************************
* 函数名称：softTimer_Start
* 函数功能：启动定时，需要设置定时的相关属性
* 函数参数：
*      timer    ：定时器的指针，不能为NULL
*      id       ：定时器的ID
*      mode     ：定时器的模式
*      interval ：定时器的定时时间
*      callback ：定时器到达后的服务函数指针
*      argc     ：服务函数的参数个数
*      argv     ：服务函数的参数指针
* 函数返回： 
*      启动成功：0
*      启动失败：非0
* 函数错误：
*      如果参数无效（主要指的是id过大或者interval过大），则可能会导致进程奔溃
************************************************************************/
int softTimer_Start(softTimer_st *timer, timerMode mode,
                    uint32_t interval, void *callback, uint16_t argc, void *argv);
/************************************************************************
* 函数名称：softTimer_Stop
* 函数功能：停止定时
* 函数参数：
*      timer ：定时器的指针，不能为NULL
*      id    ：要停止的定时器的ID
* 函数返回：无
* 函数错误：
*      如果参数无效（主要指的是id过大），则可能会导致进程奔溃
************************************************************************/
void softTimer_Stop(softTimer_st *timer);

/************************************************************************
* 函数名称：softTimer_GetState
* 函数功能：获取定时的状态
* 函数参数：
*      timer ：定时器的指针，不能为NULL
*      id    ：要停止的定时器的ID
* 函数返回： 
*      定时器的状态，在参数有效的情况下此函数永远返回正确
* 函数错误：
*      如果参数无效（主要指的是id过大），则可能会导致进程奔溃
************************************************************************/
uint8_t softTimer_GetState(softTimer_st *timer);

#endif
