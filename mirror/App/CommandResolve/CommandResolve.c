/*******************************************************************************
* 文件名      ：  CommandResolve.c
* 版权说明  ：交控科技股份有限公司
* 版本号      ：  1.0
* 创建时间  ：  2021年10月9日
* 作者         ：  交控研究院
* 功能描述  ：  
* 使用注意  ：无
* 修改记录  ：无
*******************************************************************************/

#include "CommandResolve.h"

#include "LinkList.h"
#include "CommonMemory.h"
#include "CommandCB.h"
#include "EndianChange.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define MAX_NSYN_COMMAND_COUNT      10

/* 回复数据帧类型 */
enum ACKStaType{
    COMMAND_RECEIVE_OK = 1, 	/* 接收成功 */
    COMMAND_RUN_OK = 2, 		/* 执行成功 */
    COMMADN_EXCEED = 3, 		/* 超出同时响应的能力 */
    COMMADN_ILLEGAL = 4, 	 	/* 不合法的命令 */
	COMMADN_PARA_ERR = 5,		/* 参数错误*/
	COMMAND_ERROR = 6,			/* 执行时出错*/
};

/* 命令解析数据结构 */
struct CommandResolveType{
    struct CommandType *commandTab; /* 命令参数配置 */
    UINT8 cbNum;                    /* 命令个数 */
    void* nsyncCommd;               /* 异步命令链表 */
};

/* 命令发送端到命令执行端通信协议 */
struct CommandFrameType{
    UINT16 commdSerial;    /* 命令串号 */
    UINT16 commdCode;      /* 命令码 */
    UINT64 para;          /* 命令参数 */
}ALIGN(1);
/* 命令执行端到命令发送端 */
struct ACKFrameType{
    UINT16 commdSerial;      /* 命令串号 */
    UINT16 commdCode;        /* 命令码 */
    enum ACKStaType sta;	 /* 回复信息枚举 */
}ALIGN(1);

struct CommandNodeType{
    void* next;             /* 后向数据地址 */
    struct CommandFrameType node;   /* 命令关键信息 */
}ALIGN(1);

struct CommandResolveType g_commandResolve = {0};   /* 命令模块操作句柄 */
pthread_t g_nsyncCommandRun;

/*  函数功能：异步命令执行线程
 *  输入参数：
 *  输出参数：无。
 *  返回值    ：
 *  使用注意：
 */
void NSyncCommandRun(void)
{
    struct CommandNodeType* pNSyncCommand = NULL;
    struct ACKFrameType ackFrame;

    /*检查命令池是否有命令*/
    pNSyncCommand = FindNode(g_commandResolve.nsyncCommd, 1);
    if(NULL != pNSyncCommand)
    {
        if(pNSyncCommand->node.commdCode < g_commandResolve.cbNum)
        {
            /*命令执行*/
            g_commandResolve.commandTab[pNSyncCommand->node.commdCode].CommandCB(&pNSyncCommand->node.para);
            /*执行确认返回发送*/
            /*memset(&ackFrame, 0, sizeof(ackFrame));*/
            CommonMemSet(&ackFrame, sizeof(ackFrame), 0, sizeof(ackFrame));
            ackFrame.commdSerial = pNSyncCommand->node.commdSerial;
            ackFrame.sta = COMMAND_RUN_OK;
            ackFrame.commdCode = pNSyncCommand->node.commdCode;

            /*-通过用户提供的方法发送数据*/
            CmdUserOperation((UINT8*)&ackFrame, sizeof(ackFrame));

            /*删除执行完毕的节点*/
            DeleteLink(g_commandResolve.nsyncCommd, 1);
        }
        else
        {
            /*报告异常*/
        }
    }
    else
    {
        /* 暂无新指令直接退出 */
    }
}

/*  函数功能：命令解析回调函数
 *  输入参数：
 *  UINT8* pdata 数据起始地址
 *  UINT8* pLen   数据长度
 *  UINT16* pNodeNO  发送节点位置号
 *  UINT16* pdev    外设
 *  输出参数：无。
 *  返回值    ：
 *  使用注意：
 */
void CommandResolveCB(UINT8* pdata, UINT8* pLen, UINT16* pNodeNO, UINT16* pdev)
{
    struct ACKFrameType ackFrame;
    struct CommandFrameType* cmdFrame = (struct CommandFrameType*)pdata;
    UINT8 nodeCount;
    struct CommandNodeType* cmdNode = NULL;
    STATUS_T ret = RET_UNKNOWN_ERR;

#ifdef LITTLE_ENDIAN_SQUENCE
    cmdFrame->commdCode = ChangeEndian16(cmdFrame->commdCode);
    cmdFrame->commdSerial = ChangeEndian16(cmdFrame->commdSerial);
#endif

    /*接收确认返回发送*/
    /*memset(&ackFrame, 0, sizeof(ackFrame));*/
    CommonMemSet(&ackFrame, sizeof(ackFrame), 0, sizeof(ackFrame));
    ackFrame.commdSerial = cmdFrame->commdSerial;
    ackFrame.commdCode = cmdFrame->commdCode;
    ackFrame.sta = COMMAND_RECEIVE_OK;

    /*-通过用户提供的方法发送数据*/
    CmdUserOperation((UINT8*)&ackFrame, sizeof(ackFrame));

    /* 检查命令是否合法 */
    if((*pLen == sizeof(struct CommandFrameType)) && (cmdFrame->commdCode < g_commandResolve.cbNum))
    {
        /* 判断是否为同步命令 */
        if(COMMAND_SYNC == g_commandResolve.commandTab[cmdFrame->commdCode].sync)
        {
            /*命令执行*/
            ret = g_commandResolve.commandTab[cmdFrame->commdCode].CommandCB(&cmdFrame->para);

            /*执行确认返回发送*/
            /*memset(&ackFrame, 0, sizeof(ackFrame));*/
            CommonMemSet(&ackFrame, sizeof(ackFrame), 0, sizeof(ackFrame));
            ackFrame.commdSerial = cmdFrame->commdSerial;
            ackFrame.commdCode = cmdFrame->commdCode;

            if(RET_NO_ERR == ret)
            	ackFrame.sta = COMMAND_RUN_OK;
            else if(RET_PARAM_ERR == ret)
            	ackFrame.sta = COMMADN_PARA_ERR;
            else
            	ackFrame.sta = COMMAND_ERROR;

            /*-通过用户提供的方法发送数据*/
            CmdUserOperation((UINT8*)&ackFrame, sizeof(ackFrame));
        }
        else
        {
            nodeCount = GetLinkNodeCount(g_commandResolve.nsyncCommd);
            if(MAX_NSYN_COMMAND_COUNT >= nodeCount)
            {
                InsertNode(g_commandResolve.nsyncCommd, nodeCount - 1, sizeof(struct CommandFrameType));
                cmdNode = FindNode(g_commandResolve.nsyncCommd, nodeCount);
                if(NULL != cmdNode)
                {
                    /*memcpy(&cmdNode->node, cmdFrame, sizeof(struct CommandFrameType));*/
                    CommonMemCpy(&cmdNode->node, sizeof(struct CommandFrameType), cmdFrame, sizeof(struct CommandFrameType));
                }
                else
                {

                }
            }
            else
            {
                /*超出最大响应命令数 不予响应 反馈超出最大承受条目*/
                /*memset(&ackFrame, 0, sizeof(ackFrame));*/
                CommonMemSet(&ackFrame, sizeof(ackFrame), 0, sizeof(ackFrame));
                ackFrame.commdSerial = cmdFrame->commdSerial;
                ackFrame.sta = COMMADN_EXCEED;
                ackFrame.commdCode = cmdFrame->commdCode;

                /*-通过用户提供的方法发送数据*/
                CmdUserOperation((UINT8*)&ackFrame, sizeof(ackFrame));
            }
        }
    }
    else
    {
        /* 命令号异常 */
        /*memset(&ackFrame, 0, sizeof(ackFrame));*/
        CommonMemSet(&ackFrame, sizeof(ackFrame), 0, sizeof(ackFrame));
        ackFrame.commdSerial = cmdFrame->commdSerial;
        ackFrame.sta = COMMADN_ILLEGAL;
        ackFrame.commdCode = cmdFrame->commdCode;

        /*-通过用户提供的方法发送数据*/
        CmdUserOperation((UINT8*)&ackFrame, sizeof(ackFrame));
    }
}

/*  函数功能：异步命令执行线程-阻塞
 *  输入参数：
 *  输出参数：无。
 *  返回值    ：
 *  使用注意：
 */
void* NSyncCommandRunBlock(void* argv)
{
	while(1)
	{
		NSyncCommandRun();
//		usleep(100000);
		sleep(1);
	}

	return NULL;
}

/*  函数功能：初始化命令解析模块
 *  输入参数：
 *  输出参数：无。
 *  返回值    ：
 *  使用注意：
 */
void CommandResolveInit(void)
{
    g_commandResolve.commandTab = g_commandTab;
    g_commandResolve.cbNum = sizeof(g_commandTab)/sizeof(g_commandTab[0]);
    g_commandResolve.nsyncCommd = CreatLink(0, sizeof(struct CommandFrameType));
    if(NULL != g_commandResolve.nsyncCommd)
    {
        printf("Command init success!\r\n");
    }
    else
    {
        printf("Command init failed!\r\n");
    }
	pthread_create(&g_nsyncCommandRun, NULL, NSyncCommandRunBlock, NULL);
	pthread_detach(g_nsyncCommandRun);
}
