/*******************************************************************************
* 文件名      ：  EndianChange.h
* 版权说明  ：交控科技股份有限公司
* 版本号      ：  1.0
* 创建时间  ：2020年11月9日
* 作者         ：  交控研究院
* 功能描述  ：  
* 使用注意  ：无
* 修改记录  ：无
*******************************************************************************/

/*
 * EndianChange.h
 *
 *  Created on: 2020年11月9日
 *      Author: Steven
 */

#ifndef COMMON_ENDIANCHANGE_H_
#define COMMON_ENDIANCHANGE_H_
#include "commontypes.h"

/*  函数功能：16位换端函数
 *  输入参数：换端src
 *  输出参数：
 *  返回值    ：转换后的数据
 *  使用注意：无。
 */
UINT16 ChangeEndian16(UINT16 src);

/*  函数功能：32位换端函数
 *  输入参数：换端src
 *  输出参数：
 *  返回值    ：转换后的数据
 *  使用注意：无。
 */
UINT32 ChangeEndian32(UINT32 src);

/*  函数功能：64位换端函数
 *  输入参数：换端src
 *  输出参数：
 *  返回值    ：转换后的数据
 *  使用注意：无。
 */
UINT64 ChangeEndian64(UINT64 src);


#endif /* COMMON_ENDIANCHANGE_H_ */
