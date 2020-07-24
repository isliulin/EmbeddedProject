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

#ifndef __BSP_DRIVER_SMBUS_GPIO_H
#define __BSP_DRIVER_SMBUS_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif
	
//#include "include.h"

//-----------------------------------------------------------------------------

// 定义SMBUS总线连接的GPIO端口, 只需要修改下面代码即可任意改变SCL和SDA的引脚
#define SMBUS_SCL_PORT			GPIOH			    // SCL端口
#define SMBUS_SDA_PORT     	    GPIOH			    // SCL端口
#define SMBUS_SCL_PIN       	GPIO_PIN_4			// 连接到SCL时钟线的GPIO
#define SMBUS_SDA_PIN       	GPIO_PIN_5			// 连接到SDA数据线的GPIO

//使能GPIO时钟
#define SMBUS_SCL_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOH_CLK_ENABLE()
#define SMBUS_SDA_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOH_CLK_ENABLE()

// 定义读写SCL和SDA的宏
#define SMBUS_SCL_H()     HAL_GPIO_WritePin(SMBUS_SCL_PORT, SMBUS_SCL_PIN, GPIO_PIN_SET)     // SCL = 1
#define SMBUS_SCL_L()     HAL_GPIO_WritePin(SMBUS_SCL_PORT, SMBUS_SCL_PIN, GPIO_PIN_RESET)   // SCL = 0 

#define SMBUS_SDA_H()     HAL_GPIO_WritePin(SMBUS_SDA_PORT, SMBUS_SDA_PIN, GPIO_PIN_SET)     // SDA = 1/
#define SMBUS_SDA_L()     HAL_GPIO_WritePin(SMBUS_SDA_PORT, SMBUS_SDA_PIN, GPIO_PIN_RESET)   // SDA = 0

#define SMBUS_SDA_READ()  HAL_GPIO_ReadPin(SMBUS_SDA_PORT, SMBUS_SDA_PIN)	//读SDA口线状态
#define SMBUS_SCL_READ()  HAL_GPIO_ReadPin(SMBUS_SCL_PORT, SMBUS_SCL_PIN)	//读SCL口线状态


//IIC操作函数
void BSP_SMBus_Init(void);                	//初始化IIC的IO口
void SMBusStart(void);				        //发送IIC开始信号
void SMBusStop(void);	  				    //发送IIC停止信号
void SMBusSendByte(uint8_t ucTxd);			//IIC发送一个字节
uint8_t SMBusReadByte(void);	    //IIC读取一个字节
uint8_t SMBusWaitAck(void); 				//IIC等待ACK信号
void SMBusAck(void);					    //IIC发送ACK信号
void SMBusNAck(void);					    //IIC不发送ACK信号

#ifdef __cplusplus
}
#endif

#endif //__BSP_DRIVER_SMBUS_GPIO_H







