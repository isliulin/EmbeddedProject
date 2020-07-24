/**
  ******************************************************************************
  * @file       :
  * @author     :
  * @version    :
  * @date       :
  * @brief      : 
  ******************************************************************************
    @history

  ******************************************************************************
*/

#include "include.h"

/** 
 * @brief : 
 * @note  :
 * @param : 
 * @param : 
 * @param : 
 * @return:
 */
void BSP_TEMP_Init(void)
{
    DS18S20_Init();

    /*while(DS18S20_Init());
           
    DS18S20_WriteByte(SkipROM);
    DS18S20_WriteByte(WriteScratchpad);
    DS18S20_WriteByte(DS_AlarmTL);
    DS18S20_WriteByte(DS_AlarmTH);
    DS18S20_WriteByte(DS_PRECISION);

    DS18S20_WriteByte(SkipROM);
    DS18S20_WriteByte(CopyScratchpad);
  
    while(!ONE_WIRE_READ());  //等待复制完成
    */
}

/** 
 * @brief : 在跳过匹配 ROM 情况下获取 DS18B20 温度值
 * @note  :温度 = 符号位 + 整数 + 小数*0.0625  温度是16 位的带符号扩展的二进制补码形式 当工作在12位分辨率时，其中5个符号位，7个整数位，4个小数位
 * @param : 
 * @param : 
 * @param : 
 * @return:
 */
float tempGetTemperatureSkipRom(void)
{
    uint8_t ucTempHigh, ucTempLow;
    int16_t sTemperature;
    float fTemp;

    DS18S20_StartConvert();           //开始转换

    DS18S20_ReadScratchpad();

    ucTempLow = DS18S20_ReadByte();
    ucTempHigh = DS18S20_ReadByte();

    sTemperature = ucTempHigh << 8;
    sTemperature = sTemperature | ucTempLow;
 
    if(sTemperature < 0)                // 负温度
    {
        fTemp = (~sTemperature + 1) * 0.0625;
    }
    else
    {
        fTemp = sTemperature * 0.0625;
    }

    return fTemp;
}

/** 
 * @brief : 在匹配 ROM 情况下获取 DS18B20 温度值
 * @note  :
 * @param : ds18b20_id：存放 DS18B20 序列号的数组的首地址
 * @param : 
 * @param : 
 * @return:
 */
float tempGetTemperatureMatchRom(uint8_t *pucId)
{
    uint8_t ucTempHigh, ucTempLow, i;
    int16_t sTemperature;
    float fTemp;

    DS18S20_MatchRom();            //匹配ROM

    for(i = 0; i < 8; i++)
    {
        DS18S20_WriteByte(pucId[i]);
    }

    DS18S20_WriteByte(0X44);                                /* 开始转换 */

    DS18S20_MatchRom();            //匹配ROM

    for(i = 0; i < 8; i++)
    {
        DS18S20_WriteByte(pucId[i]);
    }

    DS18S20_WriteByte(0XBE);                                /* 读温度值 */

    ucTempLow = DS18S20_ReadByte();
    ucTempHigh = DS18S20_ReadByte();

    sTemperature = ucTempHigh << 8;
    sTemperature = sTemperature | ucTempLow;

    if(sTemperature < 0)                /* 负温度 */
    {
        fTemp = (~sTemperature + 1) * 0.0625;
    }
    else
    {
        fTemp = sTemperature * 0.0625;
    }
    
    return fTemp;
}

/**
 * @brief :从ds18b20得到温度值
 * @note  :精度：0.1C  当工作在12位分辨率时，其中5个符号位，7个整数位，4个小数位 温度 = 符号位 + 整数 + 小数*0.0625
 * @param :
 * @param :
 * @param :
 * @return:返回值：温度值 （-550~1250）
 */
float tempGetTemperature(void)
{
    uint8_t ucTmp;
    uint8_t ucTempLow, ucTempHigh;
    int16_t sTemperature;
    float fTemp;

    DS18S20_StartConvert();           //开始转换 \

    DS18S20_ReadScratchpad();

    ucTempLow = DS18S20_ReadByte();     // LSB   
    ucTempHigh = DS18S20_ReadByte();     // MSB  

    if(ucTempHigh & 0x80)//温度为负  
    {
        ucTempHigh = ~ucTempHigh;
        ucTempLow = ~ucTempLow + 1;        
        ucTmp = 0;//温度为负 
    }
    else//温度为正
    {
        ucTmp = 1;//温度为正
    }

    sTemperature = ucTempHigh; //获得高八位
    sTemperature <<= 8;
    sTemperature += ucTempLow;//获得底八位
    fTemp = (float)sTemperature * 0.5;//转换     

    if(ucTmp)
    {
        return fTemp;//返回温度值
    }
    else
    {
        return -fTemp;
    }
}

/**
 * @brief :
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:返回值：温度值 （-550~1250）
 *
float tempGetTemperature(void)
{
    float fTemp;
  
    fTemp = tempGetTemperature_f();
    g_fTemperature = fTemp;
        
    return fTemp;
}
*/
//-----------------------------------------------------