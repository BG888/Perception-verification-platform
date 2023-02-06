/*
 * DeviceConf.c
 *
 *  Created on: Aug 31, 2022
 *      Author: tct
 */

#include "DeviceConf.h"
#include "GP_Xml.h"

#define CFG_FILE "Device.xml" 	/* 配置文件名称 */

/**
 * @brief      配置文件初始化函数
 * @details    读取配置信息，对相关变量进行初始化
 * @param[in]  pfile     配置文件名
 * @return     STATUS_T  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
static STATUS_T Xml_Init(const CHAR *pfile)
{
	STATUS_T ret = RET_UNKNOWN_ERR;

	XML_HANDLE_T xmlDoc;
	P_XML_HANDLE_T pxml = &xmlDoc;
	INT32 intret = -1;
	XML_TOKEN_T xmlret = TOK_EOF;

	CHAR tag[MAX_TAG_LEN] = {0};
	CHAR value[MAX_TOK_LEN] = {0};
	CHAR attribute[MAX_TOK_LEN] = {0};
	UINT32 valueInt = 0;

	/** 文件名参数有效 */
	if (NULL != pfile)
	{
		/** 打开xml文件 */
		ret = trdp_XMLOpen(pxml, pfile);

		/** 打开文件成功 */
		if (RET_NO_ERR == ret)
		{
			/* 返回文件初始位置 */
			trdp_XMLRewind(pxml);
			/* 进入下一层 */
			trdp_XMLEnter(pxml);
			/* 获取父元素 */
			intret = trdp_XMLSeekStartTag(pxml, "device");
			/* 获取父元素成功 */
			if (0 == intret)
			{
				/* 进入下一层 */
				trdp_XMLEnter(pxml);
				/* 遍历当前层元素 */
				while (0 == trdp_XMLSeekStartTagAny(pxml, tag, MAX_TAG_LEN))
				{
					/* riom配置元素 */
					if (0 == strcmp(tag, "sensor-configuration"))
					{
						/* 获取当前元素属性 */
						xmlret = trdp_XMLGetAttribute(pxml, attribute, &valueInt, value);
						/* 获取元素属性成功 */
						if (TOK_ATTRIBUTE == xmlret)
						{
							/* 属性名称匹配 */
							if (0 == strcmp(attribute, "addr"))
							{
								/* 获取属性值 */
								//g_RiomInfo.riomaddr = (UINT8)valueInt;
								printf("value = %d\r\n", valueInt);
							}
							/* 属性名称不匹配 */
							else
							{
								/* 不做处理 */
							}
						}
						/* 获取元素属性失败 */
						else
						{
							/* 不做处理 */
						}
					}
				}
			}
		}
	}
	return 0;
}

/**
 * @brief      配置初始化函数
 * @details    读取配置文件和本地网络信息，对相关变量进行初始化
 * @param      无
 * @return     STATUS_T  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T Config_Init(void)
{
	STATUS_T ret = RET_UNKNOWN_ERR;
//	UINT8 InitFlag = 0;     /* 初始化标志变量 */

	ret = Xml_Init(CFG_FILE);

	return ret;
}



