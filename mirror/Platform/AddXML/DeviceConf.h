/*
 * DeviceConf.h
 *
 *  Created on: Aug 31, 2022
 *      Author: tct
 */

#ifndef APP_ADDXML_DEVICECONF_H_
#define APP_ADDXML_DEVICECONF_H_

#include <commontypes.h>

/**
 * @brief      配置初始化函数
 * @details    读取配置文件和本地网络信息，对相关变量进行初始化
 * @param      无
 * @return     STATUS_T  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T Config_Init(void);


#endif /* APP_ADDXML_DEVICECONF_H_ */
