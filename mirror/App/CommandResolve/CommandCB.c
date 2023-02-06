/**
* @file CommandCB.c
* @brief 
* @author 交控研究院
* @date 2021年11月21日
* @version V1.1
* @copyright TCT
* @par 修改日志
* <table>
* <tr><th>Date        <th>Version  <th>Author     <th>Description
* </table>
*/
#include "CommandCB.h"
#include "SensorCtrl/SceneElement.h"
#include "SensorCtrl/Transaction.h"

/*命令配置参数*/
struct CommandType g_commandTab[COMMAND_CNT] = {
													{  SceneElementCMDCtrl   ,   COMMAND_NSYNC   ,   0   },
};

/**
* @brief 参数写入操作
* @param UINT8* pdata,      发送数据
* @param UINT8 len,         数据长度
*/
void CmdUserOperation(UINT8* pdata, UINT8 len)
{
#ifdef REMOTE_DI_BOARD
    CCUUserDataSend(0, DI_COMMAND_PORT, pdata, len);
#endif
#ifdef REMOTE_DO_BOARD
    CCUUserDataSend(0, DO_COMMAND_PORT, pdata, len);
#endif
#ifdef REMOTE_AX_BOARD
    CCUUserDataSend(0, AX_COMMAND_PORT, pdata, len);
#endif
    enum SendDataType type = Command;
    MessageSend(NULL, pdata, len, type);
}
