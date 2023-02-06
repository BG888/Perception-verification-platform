/*
 * SceneElement.h
 *
 *  Created on: Sep 6, 2022
 *      Author: tct
 */

#ifndef APP_SENSORCTRL_SCENEELEMENT_H_
#define APP_SENSORCTRL_SCENEELEMENT_H_

#include <commontypes.h>

enum RESET_MODE_TYPE{
	RESET_ALL,
	RESET_ANYONE,
};
/**
 * @brief     设备层后台服务
 * @details
 * @param     void* arg		操作句柄
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohterSceneCmdElement
 */
void* TempCtrl(void* arg);

/**
 * @brief     场景元素初始化
 * @details	  通过解析控制命令决定
 * @param     void* arg		操作句柄
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T SceneElementCtrlRun(void);

/**
 * @brief     场景元素导向安全操作
 * @details	  通过解析预设命令控制场景元素导向安全
 * @param     UINT16 id   场景元素ID
 * 			  enum RESET_MODE_TYPE mode   可选择全部初始化或某个初始化，若选择全部初始化id无意义
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T SceneElementReset(enum RESET_MODE_TYPE mode, UINT16 id);

/**
 * @brief     场景元素控制 为命令模块提供的控制接口
 * @details	  通过场景元素ID及命令控制绑定设备
 * @param     UINT16 id
 * 			  UINT16 cmd
 * @return     STATUS_T ret执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T SceneElementCMDCtrl(void* pdata);

/**
 * @brief     获取采样类场景元素状态
 * @details	  获取场景元素状态 数据排列顺序为自定义顺序
 * @param     UINT8* pbuf, 		数据存放空间
 * 			  UINT16 areasize	空间大小
 * 			   UINT16* pSCcnt	场景元素个数
 * @return     占用空间大小
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
UINT16 GetSCSamplerSta(UINT8* pbuf, UINT16 areasize, UINT16* pSCcnt);

/**
 * @brief     获取控制类场景元素状态
 * @details	  获取场景元素状态 数据排列顺序为自定义顺序
 * @param     UINT8* pbuf, 		数据存放空间
 * 			  UINT16 areasize	空间大小
 * 			   UINT16* pSCcnt	场景元素个数
 * @return     占用空间大小
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
UINT16 GetSCCtrllerSta(UINT8* pbuf, UINT16 areasize, UINT16* pSCcnt);

/**
 * @brief     场景元素初始化
 * @details	  通过解析控制命令决定
 * @param     void* arg		操作句柄
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T SceneElementSamplerInit(void);

#endif /* APP_SENSORCTRL_SCENEELEMENT_H_ */
