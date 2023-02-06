/*******************************************************************************
* 文件名      ：  CommandResolve.h
* 版权说明  ：交控科技股份有限公司
* 版本号      ：  1.0
* 创建时间  ：2021年10月9日
* 作者         ：  交控研究院
* 功能描述  ：  
* 使用注意  ：无
* 修改记录  ：无
*******************************************************************************/

/*
 * CommandResolve.h
 *
 *  Created on: 2021年10月9日
 *      Author: ding
 */

#ifndef USER_COMMANDRESOLVE_COMMANDRESOLVE_H_
#define USER_COMMANDRESOLVE_COMMANDRESOLVE_H_

#include "commontypes.h"

/*  函数功能：异步命令执行线程
 *  输入参数：
 *  输出参数：无。
 *  返回值    ：
 *  使用注意：
 */
void NSyncCommandRun(void);

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
void CommandResolveCB(UINT8* pdata, UINT8* pLen, UINT16* pNodeNO, UINT16* pdev);

/*  函数功能：初始化函数
 *  输入参数：
 *  输出参数：无。
 *  返回值    ：
 *  使用注意：
 */
void CommandResolveInit(void);

#endif /* USER_COMMANDRESOLVE_COMMANDRESOLVE_H_ */
