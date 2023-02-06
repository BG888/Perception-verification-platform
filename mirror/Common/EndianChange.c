/*******************************************************************************
* 文件名      ：  EndianChange.c
* 版权说明  ：交控科技股份有限公司
* 版本号      ：  1.0
* 创建时间  ：  2020年11月9日
* 作者         ：  交控研究院
* 功能描述  ：  
* 使用注意  ：无
* 修改记录  ：无
*******************************************************************************/

#include "EndianChange.h"

/*  函数功能：16位换端函数
 *  输入参数：换端src
 *  输出参数：
 *  返回值    ：转换后的数据
 *  使用注意：无。
 */
UINT16 ChangeEndian16(UINT16 src)
{
    UINT8* ptr;
    UINT8 temp;

    ptr = (UINT8*)&src;
    temp = *ptr;
    *ptr = *(ptr+1);
    *(ptr+1) = temp;

    return src;
}

/*  函数功能：32位换端函数
 *  输入参数：换端src
 *  输出参数：
 *  返回值    ：转换后的数据
 *  使用注意：无。
 */
UINT32 ChangeEndian32(UINT32 src)
{
    UINT32 res = 0;
    UINT16* ptr = (UINT16*)&src;
    UINT16 high = *ptr;
    UINT16 low  = *(ptr+1);

    ptr = (UINT16*)&res;
    *(ptr+1) = ChangeEndian16(high);
    *ptr = ChangeEndian16(low);

    return res;
}

/*  函数功能：64位换端函数
 *  输入参数：换端src
 *  输出参数：
 *  返回值    ：转换后的数据
 *  使用注意：无。
 */
UINT64 ChangeEndian64(UINT64 src)
{
    UINT64 res = 0;
    UINT32* ptr = (UINT32*)&src;
    UINT32 high = *ptr;
    UINT32 low  = *(ptr+1);

    ptr = (UINT32*)&res;
    *(ptr+1) = ChangeEndian32(high);
    *ptr = ChangeEndian32(low);

    return res;
}
