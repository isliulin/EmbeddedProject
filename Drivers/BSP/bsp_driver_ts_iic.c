/**
  ******************************************************************************
  * @file		bsp_ts_iic.c    
  * @author  
  * @version 
  * @date    
  * @brief		触摸屏IIC总线驱动模块,该模块不包括应用层命令帧,仅包括IIC总线基本操作函数  
  ******************************************************************************
  * @history

  ******************************************************************************
  */
	   
 	
#include "include.h"


/*******************************************************
  * @brief  :控制IIC速度的延时 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void TS_Delay(void)
{
	Delay_us(5);
} 


/*******************************************************
  * @brief  : 电容触摸芯片IIC接口初始化
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void TS_IIC_Init(void)
{	
    GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOI_CLK_ENABLE();	
    __HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟
 
	//TS_SDA
	GPIO_Initure.Pin = TS_IIC_SDA_PIN;   
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD;  //开漏模式输出
	GPIO_Initure.Pull = GPIO_PULLUP;          //
	GPIO_Initure.Speed = GPIO_SPEED_MEDIUM;     //高速
	HAL_GPIO_Init(TS_IIC_SDA_PORT, &GPIO_Initure);     //初始化

	//TS_SCL
	GPIO_Initure.Pin = TS_IIC_SCL_PIN;   
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_OD;  //开漏模式输出
	GPIO_Initure.Pull = GPIO_PULLUP;          //
	GPIO_Initure.Speed = GPIO_SPEED_MEDIUM;     //高速
	HAL_GPIO_Init(TS_IIC_SCL_PORT, &GPIO_Initure);     //初始化	
	
	TS_IIC_Stop();
}


/*******************************************************
  * @brief  : 产生IIC起始信号
  * @note   : 
  * @param  : 当SCL高电平时，SDA出现一个下跳沿表示启动信号START:when CLK is high,DATA change form high to low
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void TS_IIC_Start(void)
{	
	TS_IIC_SDA_H();	  	  
	TS_IIC_SCL_H();
	TS_Delay();
 	TS_IIC_SDA_L(); 
	TS_Delay();
	TS_IIC_SCL_L();	//钳住I2C总线，准备发送或接收数据 
	TS_Delay();
}	 

/*******************************************************
  * @brief  : 产生IIC停止信号
  * @note   : // 当SCL高电平时，SDA出现一个上跳沿表示总线停止信号STOP:when CLK is high DATA change form low to high
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void TS_IIC_Stop(void)
{
	TS_IIC_SDA_L();	
	TS_IIC_SCL_H();
	TS_Delay(); 
	TS_IIC_SDA_H();
	TS_Delay(); 
}
    
/*******************************************************
  * @brief  : 等待应答信号到来
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
* @retval : 1:接收应答失败   0:接收应答成功
*******************************************************/
uint8_t TS_IIC_Wait_Ack(void)
{
	uint32_t ucErrTime = 0;

	TS_IIC_SDA_H();		//CPU释放SDA总线
	TS_Delay();	
	TS_IIC_SCL_H();		//CPU驱动SCL = 1, 此时器件会返回ACK应答
	TS_Delay();		

	while(TS_IIC_SDA_READ())
	{
		ucErrTime++;
		if(ucErrTime > 25000)
		{
			TS_IIC_Stop();
			return 1;
		} 
		TS_Delay();
	}
	TS_IIC_SCL_L();//时钟输出0 	 
	TS_Delay();	
	return 0;  
} 

/*******************************************************
  * @brief  : 产生ACK应答
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void TS_IIC_Ack(void)
{
	TS_IIC_SDA_L();
	TS_Delay();
	TS_IIC_SCL_H();	
	TS_Delay();
	TS_IIC_SCL_L();	
	TS_Delay();	
	TS_IIC_SDA_H();	
}

/*******************************************************
  * @brief  : 不产生ACK应答
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void TS_IIC_NAck(void)
{
	TS_IIC_SDA_H();
	TS_Delay();
	TS_IIC_SCL_H();
	TS_Delay();	
	TS_IIC_SCL_L();	
	TS_Delay();
}		
	
/*******************************************************
  * @brief  : IIC发送一个字节
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
* @retval : 返回从机有无应答 1:有应答,0:无应答	
*******************************************************/
void TS_IIC_Send_Byte(uint8_t ucTxd)
{                        
    uint8_t i;   

    TS_IIC_SCL_L();//拉低时钟开始数据传输
	TS_Delay();
	
	//先发送字节的高位bit7
	for(i = 0; i < 8; i++)
    {  		
        if (ucTxd & 0x80)
        {
            TS_IIC_SDA_H();   // If the current bit of Tx_buffer is 1 set bit_out
        }
        else
        {
            TS_IIC_SDA_L();  // else clear bit_out
        }	
		TS_Delay(); 
		TS_IIC_SCL_H(); 
		TS_Delay();
		TS_IIC_SCL_L();
		if (i == 7)
		{
			 TS_IIC_SDA_H(); // 释放总线
		}		
        ucTxd <<= 1; 
		TS_Delay();
    }
} 	 

/*******************************************************
  * @brief  : 读1个字节 
  * @note   : 
  * @param  : ack=1时，发送ACK，ack=0，发送nACK
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
uint8_t TS_IIC_Read_Byte(uint8_t ucAck)
{
	uint8_t i, ucReceive = 0;

	//读到第1个bit为数据的bit7
	for(i = 0; i < 8; i++ )
	{ 		
		ucReceive <<= 1; 
		TS_IIC_SCL_H();		 
		TS_Delay();
		
		if(TS_IIC_SDA_READ())
		{
			ucReceive++;
		}		
		TS_IIC_SCL_L(); 
        TS_Delay(); 
	}	  				 
	if (!ucAck)
	{
		TS_IIC_NAck();//发送nACK
	}	
	else 
	{
		TS_IIC_Ack(); //发送ACK 
	}	
	
 	return ucReceive;
}

//------------------------------------------------------
