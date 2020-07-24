/**
  ******************************************************************************
    @file
    @author
    @version
    @date
    @brief
  ******************************************************************************
    @history

  ******************************************************************************
*/

#include "include.h"

NMEA_MSG_T *ptGPSMsg;

/*******************************************************
  * @brief  :提取GPS的NMEA-0183信息
  * @note   :
  * @param  :ptGPSMessage：nmea信息结构体
  * @param  :pucBuf:接收到的GPS/北斗数据缓冲区首地址
  * @param  :
  * @retval :
*******************************************************/
void gpsNmeaAnalysis(NMEA_MSG_T *ptGPSMessage, uint8_t *pucBuf)
{
    if ((strstr((const char *)pucBuf, "GNRMC")) != NULL)
    {
        gpsNmeaGNRMCAnalysis(ptGPSMessage, pucBuf);	//GNRMC解析
    }
    else if ((strstr((const char *)pucBuf, "GPRMC")) != NULL)
    {
        gpsNmeaGPRMCAnalysis(ptGPSMessage, pucBuf);	//GPRMC解析
    }
    else if((strstr((const char *)pucBuf, "GPGSV")) != NULL)
    {
        gpsNmeaGPGSVAnalysis(ptGPSMessage, pucBuf);	//GPGSV解析
    }
    else if ((strstr((const char *)pucBuf, "BDGSV")) != NULL)
    {
        gpsNmeaBDGSVAnalysis(ptGPSMessage, pucBuf);	//BDGSV解析
    }
    else if ((strstr((const char *)pucBuf, "GNGGA")) != NULL)
    {
        gpsNmeaGNGGAAnalysis(ptGPSMessage, pucBuf);	//GNGGA解析
    }
    else if ((strstr((const char *)pucBuf, "GNGSA")) != NULL)
    {
        gpsNmeaGNGSAAnalysis(ptGPSMessage, pucBuf);	//GNGSA解析
    }
    else if ((strstr((const char *)pucBuf, "GNVTG")) != NULL)
    {
        gpsNmeaGNVTGAnalysis(ptGPSMessage, pucBuf);	//GNVTG解析
    }
}

/*******************************************************
  * @brief  :从pucBuf里面得到第ucNumber个逗号所在的位置
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @retval :0~0XFE:代表逗号所在位置的偏移.0xFF:代表不存在第ucNumber个逗号
*******************************************************/
static uint8_t gpsGetNmeaCommaPosition(uint8_t *pucBuf, uint8_t ucNumber)
{
    uint8_t *pucStr = pucBuf;

    while (ucNumber)
    {
        if (*pucBuf == '*' || *pucBuf < ' ' || *pucBuf > 'z')
        {
            return 0xFF;    //遇到'*'或者非法字符,则不存在第ucNumber个逗号
        }

        if (*pucBuf == ',')
        {
            ucNumber--;
        }

        pucBuf++;
    }

    return (pucBuf - pucStr);
}

/*******************************************************
  * @brief  :ucBottom的ucPower次方
  * @note   :库函数里的参数是double类型的这里用的是uint8_t
  * @param  :
  * @param  :
  * @param  :
  * @retval :m^n次方.
*******************************************************/
uint32_t gpsNmeaPow(uint8_t ucBottom, uint8_t ucPower)
{
    uint32_t ulResult = 1;

    while (ucPower--)
    {
        ulResult *= ucBottom;
    }

    return ulResult;
}

/*******************************************************
  * @brief  :str转换为数字,以','或者'*'结束
  * @note   :
  * @param  :pucBuf:数字存储区
  * @param  :Decimals:小数点位数,返回给调用函数
  * @param  :
  * @retval :转换后的数值
*******************************************************/
static int32_t gpsNmeaStr2Num(uint8_t *pucBuf, uint8_t *pucDecimals)
{
    uint8_t *pucTemp = pucBuf;
    uint32_t ulInteger = 0, ulDecimal = 0;
    uint8_t ucIntegerLen = 0, ucDecimalLen = 0, i;
    uint8_t ucMask = 0;
    int32_t iRes;

    for (;;) //得到整数和小数的长度
    {
        if (*pucTemp == '-')
        {
            ucMask |= 0x02;    //是负数
            pucTemp++;
        }

        if (*pucTemp == ',' || (*pucTemp == '*'))
        {
            break;    //遇到结束了
        }

        if (*pucTemp == '.')
        {
            ucMask |= 0x01;    //遇到小数点了
            pucTemp++;
        }
        else if (*pucTemp > '9' || (*pucTemp < '0'))	//有非法字符
        {
            ucIntegerLen = 0;
            ucDecimalLen = 0;
            break;
        }

        if (ucMask & 0x01)
        {
            ucDecimalLen++;
        }
        else
        {
            ucIntegerLen++;
        }

        pucTemp++;
    }

    if (ucMask & 0x02)
    {
        pucBuf++;    //去掉负号
    }

    for (i = 0; i < ucIntegerLen; i++)	//得到整数部分数据
    {
        ulInteger += gpsNmeaPow(10, ucIntegerLen - 1 - i) * (pucBuf[i] - '0');
    }

    if (ucDecimalLen > 5)
    {
        ucDecimalLen = 5;    //最多取5位小数
    }

    *pucDecimals = ucDecimalLen;	 		//小数点位数

    for (i = 0; i < ucDecimalLen; i++)	//得到小数部分数据
    {
        ulDecimal += gpsNmeaPow(10, ucDecimalLen - 1 - i) * (pucBuf[ucIntegerLen + 1 + i] - '0');
    }

    iRes = ulInteger * gpsNmeaPow(10, ucDecimalLen) + ulDecimal;

    if (ucMask & 0x02)
    {
        iRes = -iRes;
    }

    return iRes;
}

/*******************************************************
  * @brief  :分析GNRMC信息
  * @note   :
  * @param  :ptGPSMessage:nmea信息结构体
  * @param  :pucBuf:接收到的GPS/北斗数据缓冲区首地址
  * @param  :
  * @retval :
*******************************************************/
void gpsNmeaGNRMCAnalysis(NMEA_MSG_T *ptGPSMessage, uint8_t *pucBuf)
{
    uint8_t *pucStr, Decimals;
    uint8_t ucCommaPosition;
    int32_t ulValue;  
    float_t fValue;

    pucStr = (uint8_t *)strstr((const char *)pucBuf, "GNRMC"); //"$GNRMC",有&和GNRMC分开的情况,故只判断GNRMC.
    
    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 1);      //得到UTC时间
    if (ucCommaPosition != 0xFF)
    {
        ulValue = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals) / gpsNmeaPow(10, Decimals);     //得到UTC时间,去掉ms
        ptGPSMessage->tUtc.ucHour = ulValue / 10000;
        ptGPSMessage->tUtc.ucMinute = (ulValue / 100) % 100;
        ptGPSMessage->tUtc.ucSecond = ulValue % 100;
    }
    
    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 2); //得到定位状态，A有效，V无效
    if (ucCommaPosition != 0xFF)
    {
        ptGPSMessage->ucGPSStatus = *(pucStr + ucCommaPosition);
    }
    
    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 3);      //得到纬度
    if (ucCommaPosition != 0xFF)
    {
        ulValue = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);
        ptGPSMessage->ulLatitude = ulValue / gpsNmeaPow(10, Decimals + 2);	//得到°
        fValue = ulValue % gpsNmeaPow(10, Decimals + 2);	        //得到'
        ptGPSMessage->ulLatitude = ptGPSMessage->ulLatitude * gpsNmeaPow(10, 5) + (fValue * gpsNmeaPow(10, 5 - Decimals)) / 60; //转换为°
    }

    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 4);      //南纬还是北纬
    if (ucCommaPosition != 0xFF)
    {
        ptGPSMessage->ucSorN_Latitude = *(pucStr + ucCommaPosition);
    }

    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 5);      //得到经度
    if (ucCommaPosition != 0xFF)
    {
        ulValue = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);
        ptGPSMessage->ulLongitude = ulValue / gpsNmeaPow(10, Decimals + 2);	//得到°
        fValue = ulValue % gpsNmeaPow(10, Decimals + 2);	        //得到'
        ptGPSMessage->ulLongitude = ptGPSMessage->ulLongitude * gpsNmeaPow(10, 5) + (fValue * gpsNmeaPow(10, 5 - Decimals)) / 60; //转换为°
    }

    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 6);      //东经还是西经
    if (ucCommaPosition != 0xFF)
    {
        ptGPSMessage->ucEorW_Longitude = *(pucStr + ucCommaPosition);
    }

    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 9);      //得到UTC日期
    if (ucCommaPosition != 0xFF)
    {
        ulValue = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);	        //得到UTC日期
        ptGPSMessage->tUtc.ucDate = ulValue / 10000;
        ptGPSMessage->tUtc.ucMonth = (ulValue / 100) % 100;
        ptGPSMessage->tUtc.ucYear = ulValue % 100;
    }
}

/*******************************************************
  * @brief  :分析GPRMC信息
  * @note   :
  * @param  :ptGPSMessage:nmea信息结构体
  * @param  :pucBuf:接收到的GPS/北斗数据缓冲区首地址
  * @param  :
  * @retval :
*******************************************************/
void gpsNmeaGPRMCAnalysis(NMEA_MSG_T *ptGPSMessage, uint8_t *pucBuf)
{
    uint8_t *pucStr, Decimals;
    uint8_t ucCommaPosition;
    int32_t ulValue;
    float_t fValue;

    pucStr = (uint8_t *)strstr((const char *)pucBuf, "GPRMC"); //"$GPRMC",有&和GPRMC分开的情况,故只判断GPRMC.
    
    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 1);      //得到UTC时间
    if (ucCommaPosition != 0xFF)
    {
        ulValue = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals) / gpsNmeaPow(10, Decimals);     //得到UTC时间,去掉ms
        ptGPSMessage->tUtc.ucHour = ulValue / 10000;
        ptGPSMessage->tUtc.ucMinute = (ulValue / 100) % 100;
        ptGPSMessage->tUtc.ucSecond = ulValue % 100;
    }

    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 2);       //得到定位状态，A有效，V无效
    if (ucCommaPosition != 0xFF)
    {
        ptGPSMessage->ucGPSStatus = *(pucStr + ucCommaPosition);
    }  
        
    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 3);      //得到纬度
    if (ucCommaPosition != 0xFF)
    {
        ulValue = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);
        ptGPSMessage->ulLatitude = ulValue / gpsNmeaPow(10, Decimals + 2);	//得到°
        fValue = ulValue % gpsNmeaPow(10, Decimals + 2);	        //得到'
        ptGPSMessage->ulLatitude = ptGPSMessage->ulLatitude * gpsNmeaPow(10, 5) + (fValue * gpsNmeaPow(10, 5 - Decimals)) / 60; //转换为°
    }

    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 4);      //南纬还是北纬
    if (ucCommaPosition != 0xFF)
    {
        ptGPSMessage->ucSorN_Latitude = *(pucStr + ucCommaPosition);
    }

    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 5);      //得到经度
    if (ucCommaPosition != 0xFF)
    {
        ulValue = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);
        ptGPSMessage->ulLongitude = ulValue / gpsNmeaPow(10, Decimals + 2);	//得到°
        fValue = ulValue % gpsNmeaPow(10, Decimals + 2);	        //得到'
        ptGPSMessage->ulLongitude = ptGPSMessage->ulLongitude * gpsNmeaPow(10, 5) + (fValue * gpsNmeaPow(10, 5 - Decimals)) / 60; //转换为°
    }

    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 6);      //东经还是西经
    if (ucCommaPosition != 0xFF)
    {
        ptGPSMessage->ucEorW_Longitude = *(pucStr + ucCommaPosition);
    }

    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 9);      //得到UTC日期
    if (ucCommaPosition != 0xFF)
    {
        ulValue = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);      //得到UTC日期
        ptGPSMessage->tUtc.ucDate = ulValue / 10000;
        ptGPSMessage->tUtc.ucMonth = (ulValue / 100) % 100;
        ptGPSMessage->tUtc.ucYear = ulValue % 100;
    }
}

/*******************************************************
  * @brief  :分析GPGSV信息
  * @note   :
  * @param  :ptGPSMessage:nmea信息结构体
  * @param  :pucBuf:GPS数据缓冲区首地址
  * @param  :
  * @retval :
*******************************************************/
void gpsNmeaGPGSVAnalysis(NMEA_MSG_T *ptGPSMessage, uint8_t *pucBuf)
{
    uint8_t *pucTempPoint, *pucStr, Decimals;
    uint8_t ucLen, i, j, ucSatelliteIndex = 0;
    uint8_t ucCommaPosition;

    pucTempPoint = pucBuf;
    pucStr = (uint8_t *)strstr((const char *)pucTempPoint, "$GPGSV");
    
    ucLen = pucStr[7] - '0';      //得到GPGSV的条数
    
    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 3);      //得到可见卫星总数
    if (ucCommaPosition != 0xFF)
    {
        ptGPSMessage->ucVisibleGPSNumber = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);
    }

    for (i = 0; i < ucLen; i++)
    {
        pucStr = (uint8_t *)strstr((const char *)pucTempPoint, "$GPGSV");

        for (j = 0; j < 4; j++)
        {
            ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 4 + j * 4);
            if (ucCommaPosition != 0xFF)
            {
                ptGPSMessage->tGPSMsg[ucSatelliteIndex].ucNumber = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);    //得到卫星编号
            }
            else
            {
                break;
            }

            ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 5 + j * 4);
            if (ucCommaPosition != 0xFF)
            {
                ptGPSMessage->tGPSMsg[ucSatelliteIndex].ucElevation = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);    //得到卫星仰角
            }
            else
            {
                break;
            }

            ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 6 + j * 4);
            if (ucCommaPosition != 0xFF)
            {
                ptGPSMessage->tGPSMsg[ucSatelliteIndex].usAzimuth = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);    //得到卫星方位角
            }
            else
            {
                break;
            }

            ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 7 + j * 4);
            if (ucCommaPosition != 0xFF)
            {
                ptGPSMessage->tGPSMsg[ucSatelliteIndex].ucSNR = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);    //得到卫星信噪比
            }
            else
            {
                break;
            }

            ucSatelliteIndex++;
        }

        pucTempPoint = pucStr + 1; //切换到下一个GPGSV信息
    }
}

/*******************************************************
  * @brief  :分析BDGSV信息
  * @note   :
  * @param  :ptGPSMessage:nmea信息结构体
  * @param  :pucBuf:GPS数据缓冲区首地址
  * @param  :
  * @retval :
*******************************************************/
void gpsNmeaBDGSVAnalysis(NMEA_MSG_T *ptGPSMessage, uint8_t *pucBuf)
{
    uint8_t *pucTempPoint, *pucStr, Decimals;
    uint8_t ucLen, i, j, ucSatelliteIndex = 0;
    uint8_t ucCommaPosition;

    pucTempPoint = pucBuf;
    pucStr = (uint8_t *)strstr((const char *)pucTempPoint, "$BDGSV");
    
    ucLen = pucStr[7] - '0';      //得到BDGSV的条数
    
    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 3);      //得到可见北斗卫星总数
    if (ucCommaPosition != 0xFF)
    {
        ptGPSMessage->ucVisibleBEIDOUNumber = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);
    }

    for (i = 0; i < ucLen; i++)
    {
        pucStr = (uint8_t *)strstr((const char *)pucTempPoint, "$BDGSV");

        for (j = 0; j < 4; j++)
        {
            ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 4 + j * 4);
            if (ucCommaPosition != 0xFF)
            {
                ptGPSMessage->tBeidouMsg[ucSatelliteIndex].ucBEIDOUNumber = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);    //得到卫星编号
            }
            else
            {
                break;
            }

            ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 5 + j * 4);
            if (ucCommaPosition != 0xFF)
            {
                ptGPSMessage->tBeidouMsg[ucSatelliteIndex].ucBEIDOUElevation = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);    //得到卫星仰角
            }
            else
            {
                break;
            }

            ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 6 + j * 4);
            if (ucCommaPosition != 0xFF)
            {
                ptGPSMessage->tBeidouMsg[ucSatelliteIndex].usBEIDOUAzimuth = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);    //得到卫星方位角
            }
            else
            {
                break;
            }

            ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 7 + j * 4);
            if (ucCommaPosition != 0xFF)
            {
                ptGPSMessage->tBeidouMsg[ucSatelliteIndex].ucBEIDOUSNR = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);    //得到卫星信噪比
            }
            else
            {
                break;
            }

            ucSatelliteIndex++;
        }

        pucTempPoint = pucStr + 1; //切换到下一个BDGSV信息
    }
}

/*******************************************************
  * @brief  :分析GNGGA信息
  * @note   :
  * @param  :ptGPSMessage:nmea信息结构体
  * @param  :pucBuf:GPS数据缓冲区首地址
  * @param  :
  * @retval :
*******************************************************/
void gpsNmeaGNGGAAnalysis(NMEA_MSG_T *ptGPSMessage, uint8_t *pucBuf)
{
    uint8_t *pucStr, Decimals;
    uint8_t ucCommaPosition;

    pucStr = (uint8_t *)strstr((const char *)pucBuf, "$GNGGA");
    
    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 6);      //得到GPS状态
    if (ucCommaPosition != 0xFF)
    {
        ptGPSMessage->ucGPSStatus = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);
    }

    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 7);      //得到用于定位的卫星数
    if (ucCommaPosition != 0xFF)
    {
        ptGPSMessage->ucLocationSatelliteNum = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);
    }

    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 9);      //得到海拔高度
    if (ucCommaPosition != 0xFF)
    {
        ptGPSMessage->lAltitude = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);
    }
}

/*******************************************************
  * @brief  :分析GNGSA信息
  * @note   :
  * @param  :ptGPSMessage:nmea信息结构体
  * @param  :pucBuf:GPS数据缓冲区首地址
  * @param  :
  * @retval :
*******************************************************/
void gpsNmeaGNGSAAnalysis(NMEA_MSG_T *ptGPSMessage, uint8_t *pucBuf)
{
    uint8_t *pucStr, Decimals;
    uint8_t ucCommaPosition;
    uint8_t i;

    pucStr = (uint8_t *)strstr((const char *)pucBuf, "$GNGSA");
    
    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 2);      //得到定位类型
    if (ucCommaPosition != 0xFF)
    {
        ptGPSMessage->ucFixMode = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);
    }

    for (i = 0; i < 12; i++)      //得到定位卫星编号
    {
        ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 3 + i);
        if (ucCommaPosition != 0xFF)
        {
            ptGPSMessage->ucLocationSatellite[i] = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);
        }
        else
        {
            break;
        }
    }

    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 15);      //得到PDOP位置精度因子
    if (ucCommaPosition != 0xFF)
    {
        ptGPSMessage->usPDOP = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);
    }

    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 16);      //得到HDOP位置精度因子
    if (ucCommaPosition != 0xFF)
    {
        ptGPSMessage->usHDOP = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);
    }

    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 17);      //得到VDOP位置精度因子
    if (ucCommaPosition != 0xFF)
    {
        ptGPSMessage->usVDOP = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);
    }
}

/*******************************************************
  * @brief  :分析GNVTG信息
  * @note   :
  * @param  :ptGPSMessage:nmea信息结构体
  * @param  :pucBuf:GPS数据缓冲区首地址
  * @param  :
  * @retval :
*******************************************************/
void gpsNmeaGNVTGAnalysis(NMEA_MSG_T *ptGPSMessage, uint8_t *pucBuf)
{
    uint8_t *pucStr, Decimals;
    uint8_t ucCommaPosition;

    pucStr = (uint8_t *)strstr((const char *)pucBuf, "$GNVTG");
    
    ucCommaPosition = gpsGetNmeaCommaPosition(pucStr, 7);      //得到地面速率
    if (ucCommaPosition != 0xFF)
    {
        ptGPSMessage->ulSpeed = gpsNmeaStr2Num(pucStr + ucCommaPosition, &Decimals);
        if (Decimals < 3)
        {
            ptGPSMessage->ulSpeed *= gpsNmeaPow(10, 3 - Decimals);    //确保扩大1000倍
        }
    }
}



