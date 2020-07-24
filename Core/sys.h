

#ifndef _SYS_H
#define _SYS_H


#include "stm32f7xx.h"


//0,不支持os
//1,支持os

///////////////////////////////////////////////////////////////////////////////////
//定义一些常用的数据类型短关键字
//typedef int32_t  s32;
//typedef int16_t s16;
//typedef int8_t  s8;

//typedef const int32_t sc32;
//typedef const int16_t sc16;
//typedef const int8_t sc8;

//typedef __IO int32_t  vs32;
//typedef __IO int16_t  vs16;
//typedef __IO int8_t   vs8;

//typedef __I int32_t vsc32;
//typedef __I int16_t vsc16;
//typedef __I int8_t vsc8;

//typedef uint32_t  uint32_t;
//typedef uint16_t u16;
//typedef uint8_t  uint8_t;

//typedef const uint32_t uc32;
//typedef const uint16_t uc16;
//typedef const uint8_t uc8;

//typedef __IO uint32_t  vu32;
//typedef __IO uint16_t vu16;
//typedef __IO uint8_t  vu8;

//typedef __I uint32_t vuc32;
//typedef __I uint16_t vuc16;
//typedef __I uint8_t vuc8;


#define ON	1
#define OFF	0
#define Write_Through() (*(__IO uint32_t*)0XE000EF9C=1UL<<2) //Cache透写模式 和SCB->CACR|=1<<2;作用一致




void SystemClock_Config( void );

uint8_t Get_ICahceSta(void);//判断I_Cache是否打开
uint8_t Get_DCahceSta(void);//判断I_Dache是否打开

void Cache_Enable(void);
void Stm32_Clock_Init(uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq); //时钟系统配置
//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void); //关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(uint32_t addr);	//设置堆栈地址
void Sys_Soft_Reset(void);

#endif

