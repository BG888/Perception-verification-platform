#include "softtime.h"

static uint32_t g_systemCounter = 0;

static uint32_t timerTicks_Get()
{
    return g_systemCounter;
}

struct itimerval g_Running_Timeticks;

static void softTime_Running_Timeticks(int signo)
{
    /* 每次自加1，增加一个ticks */
    g_systemCounter++;
}

/**************************************************************
* 函数名称：softTimer_Timeticks_Start
* 函数功能：定时的时钟启动
* 函数参数：
*      tv   ： 时钟的粒度
* 函数返回： 
*      启动成功，返回true
*      启动失败，返回false
* 函数错误：无
**************************************************************/
static bool softTimer_Timeticks_Start(uint32_t tv)
{
    bool return_val = true;

    /* 设置信号的处理方式 */
    signal(SIGALRM, softTime_Running_Timeticks);
    memset(&g_Running_Timeticks, 0, sizeof(g_Running_Timeticks));

    /* Timeout to run first time */
    g_Running_Timeticks.it_value.tv_sec = 0;
    g_Running_Timeticks.it_value.tv_usec = tv;

    /* After first, the Interval time for clock */
    g_Running_Timeticks.it_interval.tv_sec = 0;
    g_Running_Timeticks.it_interval.tv_usec = tv;

    /* 以系统真实的时间来计算，送出SIGALRM信号 */
    if (setitimer(ITIMER_REAL, &g_Running_Timeticks, NULL) < 0)
    {
        printf("Fatal error %s\n", strerror(errno));
        return_val = false;
    }

    return return_val;
}

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
void softTimer_Update(softTimer_st *timer, uint16_t timernum)
{
    uint16_t i = 0;

    for (i = 0; i < timernum; i++)
    {
        switch (timer[i].state)
        {
        case SOFTTIMER_STATE_STOPPED:
            break;

        case SOFTTIMER_STATE_RUNNING:
            /* 比较超时值与当前值的关系 */
            if (timer[i].timeout <= timerTicks_Get())
            {
                timer[i].state = SOFTTIMER_STATE_TIMEOUT;
                /* 执行回调函数 */
                timer[i].handler(timer[i].argc, timer[i].argv);
            }
            break;

        case SOFTTIMER_STATE_TIMEOUT:
            /* 如果为单次触发，则超时之后设置为停止状态 */
            if (timer[i].mode == SOFTTIMER_MODE_ONE_SHOT)
            {
                timer[i].state = SOFTTIMER_STATE_STOPPED;
            }
            else
            {
                /* 比较超时值与当前值的关系 */
                timer[i].timeout = timerTicks_Get() + timer[i].interval;
                timer[i].state = SOFTTIMER_STATE_RUNNING;
            }
            break;

        default:
            break;
        }
    }
}

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
void softTimer_Init(softTimer_st *timer, uint16_t timernum)
{
    uint16_t i;

    (void)softTimer_Timeticks_Start(10000);

    for (i = 0; i < timernum; i++)
    {
        timer[i].state = SOFTTIMER_STATE_STOPPED;
        timer[i].mode = SOFTTIMER_MODE_DEFAULT;
        timer[i].timeout = 0;
        timer[i].interval = 0;
        timer[i].handler = NULL;
        timer[i].argv = NULL;
        timer[i].argc = 0;
    }
}

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
                    uint32_t interval, void *callback, uint16_t argc, void *argv)
{
    /* 指针类型判断，不得为NULL */
    if (timer == NULL || callback == NULL)
    {
        return -1;
    }
    /* 模式必须为有效的模式 */
    if (mode != SOFTTIMER_MODE_ONE_SHOT && mode != SOFTTIMER_MODE_PERIODIC)
    {
        return -2;
    }

    timer[0].state = SOFTTIMER_STATE_RUNNING;       /* 状态为运行状态 */
    timer[0].mode = mode;                           /* 模式 */
    timer[0].interval = interval;                   /* 运行的时间间隔 */
    timer[0].timeout = timerTicks_Get() + interval; /* 运行到期的时间 */
    timer[0].handler = callback;                    /* 服务函数 */
    timer[0].argc = argc;                           /* 服务函数参数的个数 */
    timer[0].argv = argv;                           /* 服务函数参数逇指针 */

    /* 返回成功 */
    return 0;
}

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
void softTimer_Stop(softTimer_st *timer)
{
    timer[0].state = SOFTTIMER_STATE_STOPPED;
}

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
uint8_t softTimer_GetState(softTimer_st *timer)
{
    return timer[0].state;
}
