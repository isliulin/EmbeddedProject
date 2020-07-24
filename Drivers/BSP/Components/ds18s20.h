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


#ifndef __DS18S20_H
#define __DS18S20_H


#ifdef __cplusplus
extern "C" {
    #endif

    #define  SkipROM         0xCC     //跳过ROM
    #define  SearchROM       0xF0     //搜索ROM
    #define  ReadROM         0x33     //读ROM
    #define  MatchROM        0x55     //匹配ROM
    #define  AlarmROM        0xEC     //告警ROM

    #define  StartConvert    0x44     //开始温度转换，在温度转换期间总线上输出0，转换结束后输出1
    #define  ReadScratchpad  0xBE     //读暂存器的9个字节
    #define  WriteScratchpad 0x4E     //写暂存器的温度告警TH和TL
    #define  CopyScratchpad  0x48     //将暂存器的温度告警复制到EEPROM，在复制期间总线上输出0，复制完后输出1
    #define  RecallEEPROM    0xB8     //将EEPROM的温度告警复制到暂存器中，复制期间输出0，复制完成后输出1
    #define  ReadPower       0xB4     //读电源的供电方式：0为寄生电源供电；1为外部电源供电

    #define  DS_PRECISION    0x7f     //精度配置寄存器 1f=9位; 3f=10位; 5f=11位; 7f=12位;  
    #define  DS_AlarmTH      0x64
    #define  DS_AlarmTL      0x8a
    #define  DS_CONVERT_TICK 1000

        // 定义温度传感器DS18B20连接的GPIO端口, 只需要修改下面代码即可任意单总线的引脚
    #define ONE_WIRE_PORT			GPIOB			    // 单总线端口
    #define ONE_WIRE_PIN       	    GPIO_PIN_12			// 连接到单总线的GPIO

    //使能GPIO时钟
    #define ONE_WIRE_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()

    #define	ONE_WIRE_OUT(n)      (n ? HAL_GPIO_WritePin(ONE_WIRE_PORT, ONE_WIRE_PIN, GPIO_PIN_SET) : HAL_GPIO_WritePin(ONE_WIRE_PORT, ONE_WIRE_PIN, GPIO_PIN_RESET))

    #define ONE_WIRE_READ()      HAL_GPIO_ReadPin(ONE_WIRE_PORT, ONE_WIRE_PIN)	//读单总线

    //IO方向设置
    //#define DS18B20_IO_IN()  {ONE_WIRE_PORT->MODER &= ~(3 << (12 * 2));ONE_WIRE_PORT->MODER |= 0 << 12 * 2;}	//PB12输入模式
    //#define DS18B20_IO_OUT() {ONE_WIRE_PORT->MODER &= ~(3 << (12 * 2));ONE_WIRE_PORT->MODER |= 1 << 12 * 2;} 	//PB12输出模式

    uint8_t DS18S20_Init(void);			    //初始化DS18B20
    void DS18S20_StartConvert(void);		        //开始温度转换
    void DS18S20_WriteByte(uint8_t dat);   //写入一个字节
    uint8_t DS18S20_ReadByte(void);		//读出一个字节
    static uint8_t DS18S20_ReadBit(void);		    //读出一个位
    static uint8_t DS18S20_WaitAck(void);			//检测是否存在DS18B20
    void DS18S20_Rst(void);			        //复位DS18B20 
    void DS18S20_ReadScratchpad(void);
    void DS18S20_ReadScratchpad(void);
    void DS18S20_StartConvert(void);
    void DS18B20_ReadId(uint8_t *pucId);
    void DS18S20_MatchRom(void);

    #ifdef __cplusplus
}
#endif

#endif
