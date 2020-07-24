/**
  ******************************************************************************
    @file    	gt911.h
    @author
    @version
    @date
    @brief   7寸电容触摸屏gt911 驱动代码
  ******************************************************************************
    @history

  ******************************************************************************
*/


//

#ifndef __GT911_H
#define __GT911_H


#ifdef __cplusplus
extern "C" {
#endif


//#include "include.h"


//GT911 的从设备地址有两组可选，两组地址分别为：0xBA/0xBB和0x28/0x29
#define GT911_IIC_ADDR	0xBA

#define GT911_MAX_HEIGHT                  480
#define GT911_MAX_WIDTH                   800

/********************************部分寄存器定义***************************/
#define GT911_READ_XY_REG 			0x814E	// 当前检测到的触摸情况
#define GT911_CLEARBUF_REG 			0x814E	// 清除坐标寄存器
#define GT911_CONFIG_REG			0x8047	// 配置参数寄存器
#define GT911_COMMAND_REG   		0x8040  // 实时命令
#define GT911_PRODUCT_ID_REG 		0x8140 	//productid
#define GT911_VENDOR_ID_REG			0x814A  // 当前模组选项信息
#define GT911_CONFIG_VERSION_REG	0x8047  // 配置文件版本号
#define GT911_CONFIG_CHECKSUM_REG	0x80FF  // 配置文件校验码
#define GT911_FIRMWARE_VERSION_REG	0x8144  // 固件版本号

#define GT911_TP1_REG 		0X8150  	//第一个触摸点数据地址
#define GT911_TP2_REG 		0X8158		//第二个触摸点数据地址
#define GT911_TP3_REG 		0X8160		//第三个触摸点数据地址
#define GT911_TP4_REG 		0X8168		//第四个触摸点数据地址
#define GT911_TP5_REG 		0X8170		//第五个触摸点数据地址 

// 定义TS复位与中断引脚连接的GPIO端口  
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

//定义写RST和INT的宏
#define GT911_RST_H()     HAL_GPIO_WritePin(TS_RST_PORT, TS_RST_PIN, GPIO_PIN_SET)     // SCL = 1
#define GT911_RST_L()     HAL_GPIO_WritePin(TS_RST_PORT, TS_RST_PIN, GPIO_PIN_RESET)   // SCL = 0 

#define GT911_INT_H()     HAL_GPIO_WritePin(TS_INT_PORT, TS_INT_PIN, GPIO_PIN_SET)     // SCL = 1
#define GT911_INT_L()     HAL_GPIO_WritePin(TS_INT_PORT, TS_INT_PIN, GPIO_PIN_RESET)   // SCL = 0 


extern TS_DrvTypeDef gt911_ts_drv;

extern void GT911_Init( void );
extern uint8_t GT911_OnePiontScan( uint8_t mode );
extern uint8_t GT911_Scan( uint8_t ucMode );

#ifdef __cplusplus
}
#endif

#endif /*__GT911_H */

/****END OF FILE****/

















