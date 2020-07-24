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

#ifndef __BSP_DRIVER_TOUCH_H
#define __BSP_DRIVER_TOUCH_H

	   
#ifdef __cplusplus
extern "C" {
#endif

//#include "include.h"

#define TS_PRES_DOWN			0x80  //触屏被按下	  
#define TS_CATH_PRES 			0x40  //有按键按下了 
#define TS_MAX_TOUCH_POINT  	5    //电容屏支持的最大点数

////电阻屏芯片连接引脚	   
//#define PEN         HAL_GPIO_ReadPin(GPIOH,GPIO_PIN_7) //T_PEN
//#define DOUT        HAL_GPIO_ReadPin(GPIOG,GPIO_PIN_3) //T_MISO
//#define TDIN(n)     (n?HAL_GPIO_WritePin(GPIOI,GPIO_PIN_3,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOI,GPIO_PIN_3,GPIO_PIN_RESET))//T_MOSI
//#define TCLK(n)     (n?HAL_GPIO_WritePin(GPIOH,GPIO_PIN_6,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOH,GPIO_PIN_6,GPIO_PIN_RESET))//T_SCK
//#define TCS(n)      (n?HAL_GPIO_WritePin(GPIOI,GPIO_PIN_8,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOI,GPIO_PIN_8,GPIO_PIN_RESET))//T_CS  
// 
 
//触摸屏控制器
typedef struct
{
	void 		(*Init)(void);			//初始化触摸屏控制器
	uint8_t 	(*Scan)(uint8_t);	//扫描触摸屏.0,屏幕扫描;1,物理坐标;	
	uint16_t 	x[TS_MAX_TOUCH_POINT];	//当前坐标,电容屏有最多5组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,触屏的坐标,用
	uint16_t 	y[TS_MAX_TOUCH_POINT];	//x[4],y[4]存储第一次按下时的坐标.
								
	uint8_t  	ucStatus;		//笔的状态: 
								//b7:按下1/松开0; 
	                            //b6:0,没有按键按下;1,有按键按下. 
								//b5:保留
								//b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)	
	uint8_t 	ucTouchType;	//新增的参数,当触摸屏的左右上下完全颠倒时需要用到.
								//b0:0,竖屏(适合左右为X坐标,上下为Y坐标的TS)
								//   1,横屏(适合左右为Y坐标,上下为X坐标的TS) 
								//b1~6:保留.
								//b7:0,电阻屏
								//   1,电容屏 
}TS_DrvTypeDef;

extern TS_DrvTypeDef *ts_driver;	 	//触屏控制器在touch.c里面定义

//电阻屏/电容屏 共用函数
//uint8_t TS_Scan(uint8_t tp);								//扫描
void BSP_TS_Init(void);								//初始化
 
#ifdef __cplusplus
}
#endif

#endif //__BSP_DRIVER_TOUCH_H

















