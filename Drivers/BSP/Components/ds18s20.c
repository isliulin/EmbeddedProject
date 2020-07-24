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
 * @brief : 初始化DS18B20的IO口 DQ 同时检测DS的存在
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:返回1:不存在,返回0:存在
 */
uint8_t DS18S20_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    ONE_WIRE_GPIO_CLK_ENABLE();			//使能GPIO时钟

    GPIO_Initure.Pin   = ONE_WIRE_PIN;
    GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_OD;      //开漏模式输出
    GPIO_Initure.Pull  = GPIO_PULLUP;              //
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;       //
    HAL_GPIO_Init(ONE_WIRE_PORT, &GPIO_Initure); //初始化

    DS18S20_Rst();
    return DS18S20_WaitAck();
}

/**
 * @brief : 复位DS18B20
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
void DS18S20_Rst(void)
{
    ONE_WIRE_OUT(0);    //拉低DQ
    Delay_us(500);      //拉低750us 该时间的时间范围可以从480到960微秒
    ONE_WIRE_OUT(1);    //DQ=1 
    Delay_us(15);       //15us 从机接收到主机的复位信号后，会在15~60us后给主机发一个存在脉冲
}

/**
 * @brief : 等待DS18B20的回应
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:返回1:未检测到DS18B20的存在,返回0:存在
 */
static uint8_t DS18S20_WaitAck(void)
{
    uint8_t ucRetryTime = 0;

    //等待存在脉冲的到来，存在脉冲为一个60~240us的低电平信号,如果存在脉冲没有来则做超时处理
    while(ONE_WIRE_READ() && ucRetryTime < 200)
    {
        ucRetryTime++;
        Delay_us(1);
    }

    if(ucRetryTime >= 200)
    {
        return 1;
    }
    else
    {
        ucRetryTime = 0;
    }

    while(!ONE_WIRE_READ() && ucRetryTime < 240)//存在脉冲到来，且存在的时间不能超过240us
    {
        ucRetryTime++;
        Delay_us(1);
    }

    if(ucRetryTime >= 240)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief : 从DS18B20读取一个位
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:返回值：ucData:1/0
 */
static uint8_t DS18S20_ReadBit(void)
{
    uint8_t i, ucData;
    
    ONE_WIRE_OUT(0);//至少保持1us
    Delay_us(2);
    ONE_WIRE_OUT(1);
    Delay_us(12);
    if(ONE_WIRE_READ())//从拉低这个引脚到读取引脚状态，不能超过 15us
    {
        ucData = 1;
    }
    else
    {
        ucData = 0;
    }
    Delay_us(50);

    return ucData;
}

/**
 * @brief : 从DS18B20读取一个字节
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:返回值：读到的数据
 */
uint8_t DS18S20_ReadByte(void)
{
    uint8_t i, ucRead, ucData;

    /*ucData = 0;
    for(i = 1; i <= 8; i++)
    {
        ucRead = DS18S20_ReadBit();
        ucData = (ucRead << 7) | (ucData >> 1);
    }
    return ucData;*/
    
    for(i = 8; i > 0; i--)
    {
        ucData >>= 1;
        ONE_WIRE_OUT(0);     //从读时序开始到采样信号线必须在15u内，且采样尽量安排在15u的最后  
        Delay_us(5);   //5us
        ONE_WIRE_OUT(1);
        Delay_us(5);   //5us
        if(ONE_WIRE_READ())
        {
            ucData |= 0x80;
        }
        else
        {
            ucData &= 0x7f;
        }
        Delay_us(50);   //65us
        ONE_WIRE_OUT(1);
    }
    return ucData;
}

/**
 * @brief : 写一个字节到DS18B20
 * @note  :
 * @param :ucData：要写入的字节
 * @param :
 * @param :
 * @return:返回值：读到的数据
 */
void DS18S20_WriteByte(uint8_t ucData)
{
    uint8_t i;
    uint8_t ucTemp;

    for(i = 1; i <= 8; i++)
    {
        ucTemp = ucData & 0x01;
        ucData = ucData >> 1;
        if(ucTemp)       // 写1
        {
            ONE_WIRE_OUT(0);
            Delay_us(2);
            ONE_WIRE_OUT(1);
            Delay_us(60);
        }
        else            //写0
        {
            ONE_WIRE_OUT(0);
            Delay_us(60);
            ONE_WIRE_OUT(1);
            Delay_us(2);
        }
    }

    /*for(i = 8; i > 0; i--)
    {
        ONE_WIRE_OUT(0);     //在15u内送数到数据线上，DS18B20在15-60u读数 
        Delay_us(5);    //5us
        if(ucData & 0x01)
        {
            ONE_WIRE_OUT(1);
        }
        else
        {
            ONE_WIRE_OUT(0);
        }
        Delay_us(65);    //65us
        ONE_WIRE_OUT(1);
        Delay_us(2);    //连续两位间应大于1us
        ucData >>= 1;
    }*/
}

/**
 * @brief : 执行匹配 DS18B20 ROM
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
void DS18S20_MatchRom(void)
{
    DS18S20_Rst();
    DS18S20_WaitAck();
    DS18S20_WriteByte(0X55);                // 匹配 ROM
}

/**
 * @brief : 读取DS18B20序列号 
 * @note  :
 * @param :ds18b20_id用于存放DS18B20序列号的数组的首地址
 * @param :
 * @param :
 * @return:
 */
void DS18B20_ReadId(uint8_t *pucId)
{
    uint8_t i;

    DS18S20_WriteByte(ReadROM);       //读取序列号

    for(i = 0; i < 8; i++)
    {
        pucId[i] = DS18S20_ReadByte();
    }
}

/** 
 * @brief : 开始温度转换
 * @note  :
 * @param : 
 * @param : 
 * @param : 
 * @return:
 */
void DS18S20_StartConvert(void)
{
    DS18S20_Rst();
    DS18S20_WaitAck();
    DS18S20_WriteByte(SkipROM);   //跳过 ROM
    DS18S20_WriteByte(StartConvert);//开始温度转换
}

/**
 * @brief : 读暂存器的9个字节
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
void DS18S20_ReadScratchpad(void)
{
    DS18S20_Rst();
    DS18S20_WaitAck();
    DS18S20_WriteByte(SkipROM);    //跳过 ROM
    DS18S20_WriteByte(ReadScratchpad);//读暂存器的9个字节
}

//----------------------------------------------------------------------