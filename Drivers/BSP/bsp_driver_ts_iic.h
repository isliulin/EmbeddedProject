/**
  ******************************************************************************
  * @file		bsp_ts_iic.h    
  * @author  
  * @version 
  * @date    
  * @brief		触摸屏IIC总线驱动模块,该模块不包括应用层命令帧,仅包括IIC总线基本操作函数  
  ******************************************************************************
  * @history

  ******************************************************************************
  */

#ifndef __BSP_DRIVER_TS_IIC_H
#define __BSP_DRIVER_TS_IIC_H

#ifdef __cplusplus
extern "C" {
#endif
	
//#include "include.h"


//-----------------------------------------------------------------------------
// 定义IIC总线连接的GPIO端口, 用户只需要修改下面代码即可任意改变SCL和SDA的引脚
//STM32F7IIT6
#define TS_IIC_SCL_PORT			GPIOB			// SCL端口
#define TS_IIC_SDA_PORT     	GPIOB			// SCL端口
#define TS_IIC_SCL_PIN       	GPIO_PIN_8			// 连接到SCL时钟线的GPIO
#define TS_IIC_SDA_PIN       	GPIO_PIN_7			// 连接到SDA数据线的GPIO 

//STM32F7IGT6
//#define TS_IIC_SCL_PORT			GPIOH			// SCL端口
//#define TS_IIC_SDA_PORT     	GPIOI			// SCL端口
//#define TS_IIC_SCL_PIN       	GPIO_PIN_6			// 连接到SCL时钟线的GPIO
//#define TS_IIC_SDA_PIN       	GPIO_PIN_3			// 连接到SDA数据线的GPIO 

// 定义读写SCL和SDA的宏
#define TS_IIC_SCL_H()     HAL_GPIO_WritePin(TS_IIC_SCL_PORT, TS_IIC_SCL_PIN, GPIO_PIN_SET)     // SCL = 1
#define TS_IIC_SCL_L()     HAL_GPIO_WritePin(TS_IIC_SCL_PORT, TS_IIC_SCL_PIN, GPIO_PIN_RESET)   // SCL = 0 

#define TS_IIC_SDA_H()     HAL_GPIO_WritePin(TS_IIC_SDA_PORT, TS_IIC_SDA_PIN, GPIO_PIN_SET)     // SDA = 1/
#define TS_IIC_SDA_L()     HAL_GPIO_WritePin(TS_IIC_SDA_PORT, TS_IIC_SDA_PIN, GPIO_PIN_RESET)   // SDA = 0

#define TS_IIC_SDA_READ()  HAL_GPIO_ReadPin(TS_IIC_SDA_PORT, TS_IIC_SDA_PIN)	//读SDA口线状态
#define TS_IIC_SCL_READ()  HAL_GPIO_ReadPin(TS_IIC_SCL_PORT, TS_IIC_SCL_PIN)	//读SCL口线状态


//IIC操作函数
void TS_IIC_Init(void);                	//初始化IIC的IO口
void TS_IIC_Start(void);				//发送IIC开始信号
void TS_IIC_Stop(void);	  				//发送IIC停止信号
void TS_IIC_Send_Byte(uint8_t ucTxd);			//IIC发送一个字节
uint8_t TS_IIC_Read_Byte(uint8_t ucAck);	//IIC读取一个字节
uint8_t TS_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void TS_IIC_Ack(void);					//IIC发送ACK信号
void TS_IIC_NAck(void);					//IIC不发送ACK信号

#ifdef __cplusplus
}
#endif

#endif	//__BSP_DRIVER_TS_IIC_H







