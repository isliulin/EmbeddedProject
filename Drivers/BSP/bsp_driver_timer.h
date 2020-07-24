
//定时器驱动代码

#ifndef __BSP_DRIVER_TIMER_H
#define __BSP_DRIVER_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif
	
//#include "include.h"

extern TIM_HandleTypeDef TIM3_Handler;      //定时器句柄 

void TIM3_Init(uint16_t arr, uint16_t psc);
void TIM4_Init(uint16_t arr, uint16_t psc);

extern volatile unsigned long long FreeRTOSRunTimeTicks;
void ConfigureTimeForRunTimeStats(void);
	

#ifdef __cplusplus
}
#endif

#endif //__BSP_DRIVER_TIMER_H

