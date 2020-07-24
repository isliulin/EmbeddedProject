/**
  ******************************************************************************
  * @file		   
  * @author  
  * @version 
  * @date    
  * @brief		 
  ******************************************************************************
  * @history

  ******************************************************************************
  */

#include "include.h"

/*******************************************************
  * @brief  : 电容触摸芯片IIC接口初始化
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void BSP_SMBus_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    SMBUS_SCL_GPIO_CLK_ENABLE();
    SMBUS_SDA_GPIO_CLK_ENABLE();

    //SDA
    GPIO_Initure.Pin = SMBUS_SDA_PIN;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD;      //开漏模式输出
    GPIO_Initure.Pull = GPIO_PULLUP;              //
    GPIO_Initure.Speed = GPIO_SPEED_LOW;       //
    HAL_GPIO_Init(SMBUS_SDA_PORT, &GPIO_Initure); //初始化

    //SCL
    GPIO_Initure.Pin = SMBUS_SCL_PIN;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD;      //开漏模式输出
    GPIO_Initure.Pull = GPIO_PULLUP;              //
    GPIO_Initure.Speed = GPIO_SPEED_LOW;       //
    HAL_GPIO_Init(SMBUS_SCL_PORT, &GPIO_Initure); //初始化

    SMBusStop();
}

/*******************************************************
  * @brief  : SMBus总线启动信号
  * @note   : 当SCL高电平时，SDA出现一个下跳沿表示启动信号START:when CLK is high,DATA change form high to low
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void SMBusStart(void)
{
    // 当SCL高电平时，SDA出现一个下跳沿表示SMBus总线启动信号
    SMBUS_SDA_H(); // Set SDA line
    Delay_us(5);   // Wait a few microseconds
    SMBUS_SCL_H(); // Set SCL line
    Delay_us(5);   // Generate bus free time between Stop
    SMBUS_SDA_L(); // Clear SDA line
    Delay_us(5);   // Hold time after (Repeated) Start
    // Condition. After this period, the first clock is generated.
    //(Thd:sta=4.0us min)
    SMBUS_SCL_L(); // Clear SCL line
    Delay_us(5);   // Wait a few microseconds
}

/*******************************************************
  * @brief  : SMBus总线停止信号
  * @note   : 当SCL高电平时，SDA出现一个上跳沿表示总线停止信号STOP:when CLK is high DATA change form low to high
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void SMBusStop(void)
{
    // 当SCL高电平时，SDA出现一个上跳沿表示SMBus总线停止信号
    SMBUS_SCL_L(); // Clear SCL line
    Delay_us(5);   // Wait a few microseconds
    SMBUS_SDA_L(); // Clear SDA line
    Delay_us(5);   // Wait a few microseconds
    SMBUS_SCL_H(); // Set SCL line
    Delay_us(5);   // Stop condition setup time(Tsu:sto=4.0us min)
    SMBUS_SDA_H(); // Set SDA line
	Delay_us(20); 
}

/*******************************************************
  * @brief  : 等待应答信号到来
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 1:接收应答失败   0:接收应答成功
*******************************************************/
uint8_t SMBusWaitAck(void)
{
    volatile uint8_t ucAck = 1;
    uint32_t ulErrTime;

    SMBUS_SDA_H(); /* CPU释放SDA总线 */
    Delay_us(2);
    SMBUS_SCL_H(); /* CPU驱动SCL = 1, 此时器件会返回ACK应答 */
    Delay_us(5);

    ulErrTime = 0;
    while (SMBUS_SDA_READ())
    {
        ulErrTime++;
        if (ulErrTime > 25000)
        {
            SMBusStop();
            ucAck = 1;
            //            SMBUS_SDA_H();
            return ucAck;
        }
    }
    ucAck = 0;
    SMBUS_SCL_L();
    Delay_us(3);
    return ucAck;
}

/*******************************************************
  * @brief  : 产生ACK应答
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void SMBusAck(void)
{
    SMBUS_SDA_L();
    Delay_us(2);   // Tsu:dat = 250ns minimum
    SMBUS_SCL_H(); // Set SCL line
    Delay_us(6);   // High Level of Clock Pulse
    SMBUS_SCL_L(); // Clear SCL line
    Delay_us(3);   // Low Level of Clock Pulse
}

/*******************************************************
  * @brief  : 不产生ACK应答
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void SMBusNAck(void)
{
    SMBUS_SDA_H();
    Delay_us(2);   // Tsu:dat = 250ns minimum
    SMBUS_SCL_H(); // Set SCL line
    Delay_us(6);   // High Level of Clock Pulse
    SMBUS_SCL_L(); // Clear SCL line
    Delay_us(3);   // Low Level of Clock Pulse
}

/*******************************************************
  * @brief  : Send a bit
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 	
*******************************************************/
void SMBusSendBit(uint8_t ucBitout)
{
    if (ucBitout == 0)
    {
        SMBUS_SDA_L();
    }
    else
    {
        SMBUS_SDA_H();
    }
    Delay_us(2);   // Tsu:dat = 250ns minimum
    SMBUS_SCL_H(); // Set SCL line
    Delay_us(6);   // High Level of Clock Pulse
    SMBUS_SCL_L(); // Clear SCL line
    Delay_us(3);   // Low Level of Clock Pulse
                   //	SMBUS_SDA_H();				    // Master release SDA line ,
    return;
}

/*******************************************************
  * @brief  : 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 	
*******************************************************/
void SMBusSendByte(uint8_t ucTxd)
{
    uint8_t ucBitCounter;
    uint8_t ucOut;

    for (ucBitCounter = 8; ucBitCounter; ucBitCounter--)
    {
        if (ucTxd & 0x80)
        {
            ucOut = 1; // If the current bit of Tx_buffer is 1 set bit_out
        }
        else
        {
            ucOut = 0; // else clear bit_out
        }
        SMBusSendBit(ucOut); // Send the current bit on SDA
        ucTxd <<= 1;         // Get next bit for checking
    }
    Delay_us(10);
}

/*******************************************************
  * @brief  : Receive a bit 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
uint8_t SMBusReadBit(void)
{
    uint8_t ucRevBit;

    SMBUS_SDA_H(); //上拉，当作输入
    Delay_us(2);   // High Level of Clock Pulse
    SMBUS_SCL_H(); // Set SCL line
    Delay_us(5);   // High Level of Clock Pulse

    if (SMBUS_SDA_READ())
    {
        ucRevBit = 1;
    }
    else
    {
        ucRevBit = 0;
    }

    SMBUS_SCL_L(); // Clear SCL line
    Delay_us(5);   // Low Level of Clock Pulse
    return ucRevBit;
}

/*******************************************************
  * @brief  : 读1个字节 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
uint8_t SMBusReadByte(void)
{
    uint8_t ucRX;
    uint8_t ucBitCounter;

    for (ucBitCounter = 8; ucBitCounter; ucBitCounter--)
    {
        if (SMBusReadBit()) // Get a bit from the SDA line
        {
            ucRX <<= 1; // If the bit is HIGH save 1  in RX_buffer
            ucRX |= 0x01;
        }
        else
        {
            ucRX <<= 1; // If the bit is LOW save 0 in RX_buffer
            ucRX &= 0xfe;
        }
    }

    return ucRX;
}

//------------------------------------------------------
