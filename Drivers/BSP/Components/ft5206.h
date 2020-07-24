//7寸电容触摸屏-FT5206 驱动代码

#ifndef __FT5206_H
#define __FT5206_H

//#include "include.h"

// 定义FT5206复位与中断引脚连接的GPIO端口
//STM32F7IIT6
#define TS_RST_PORT		GPIOE			// GPIO端口
#define TS_INT_PORT		GPIOE			// GPIO端口
#define TS_RST_PIN		GPIO_PIN_3			// 连接到RST时钟线的PE3---TS_RST
#define TS_INT_PIN		GPIO_PIN_4			// 连接到INT数据线PE4---TS_INT

//STM32F7IGT6
//#define TS_RST_PORT		GPIOI			// GPIO端口
//#define TS_INT_PORT		GPIOH			// GPIO端口
//#define TS_RST_PIN		GPIO_PIN_8			// 连接到RST时钟线的PI8---TS_RST
//#define TS_INT_PIN		GPIO_PIN_7			// 连接到INT数据线PH7---TS_INT


//与电容触摸屏连接的芯片引脚(未包含IIC引脚)
//IO操作函数
#define FT_RST(n)  (n ? HAL_GPIO_WritePin(TS_RST_PORT, TS_RST_PIN, GPIO_PIN_SET) : HAL_GPIO_WritePin(TS_RST_PORT, TS_RST_PIN, GPIO_PIN_RESET))//FT5206复位引脚
#define FT_INT      HAL_GPIO_ReadPin(TS_INT_PORT, TS_INT_PIN)  //


//I2C读写命令
#define FT_CMD_WR 				0X70    	//写命令
#define FT_CMD_RD 				0X71		//读命令

//FT5206 部分寄存器定义
#define FT_DEVIDE_MODE 			0x00   		//FT5206模式控制寄存器
#define FT_REG_NUM_FINGER       0x02		//触摸状态寄存器

#define FT_TP1_REG 				0X03	  	//第一个触摸点数据地址
#define FT_TP2_REG 				0X09		//第二个触摸点数据地址
#define FT_TP3_REG 				0X0F		//第三个触摸点数据地址
#define FT_TP4_REG 				0X15		//第四个触摸点数据地址
#define FT_TP5_REG 				0X1B		//第五个触摸点数据地址  


#define	FT_ID_G_LIB_VERSION		0xA1		//版本		
#define FT_ID_G_MODE 			0xA4   		//FT5206中断模式控制寄存器
#define FT_ID_G_THGROUP			0x80   		//触摸有效值设置寄存器
#define FT_ID_G_PERIODACTIVE	0x88   		//激活状态周期设置寄存器

extern TS_DrvTypeDef ft5206_ts_drv;


uint8_t FT5206_WR_Reg( uint16_t reg, uint8_t *buf, uint8_t len );
void FT5206_RD_Reg( uint16_t reg, uint8_t *buf, uint8_t len );
void FT5206_Init( void );
uint8_t FT5206_Scan( uint8_t mode );

#endif

















