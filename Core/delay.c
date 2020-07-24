/**
  ******************************************************************************
    @file
    @author
    @version
    @date
    @brief
  ******************************************************************************
    @history

  ******************************************************************************
*/

#include "include.h"

static uint32_t fac_us = 0;							//us延时倍乘数

#if SYSTEM_SUPPORT_OS
static uint16_t fac_ms = 0;							//ms延时倍乘数,在os下,代表每个节拍的ms数
#endif

extern HAL_TickFreqTypeDef uwTickFreq;

//初始化延迟函数
//当使用ucos的时候,此函数会初始化ucos的时钟节拍
//SYSTICK的时钟固定为AHB时钟的1/8
//SYSCLK:系统时钟频率
void Delay_Init(uint8_t ucSYSCLK)
{
#if SYSTEM_SUPPORT_OS 						//如果需要支持OS.
    uint32_t reload;
#endif
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//SysTick频率为HCLK
    fac_us = ucSYSCLK;						  //不论是否使用OS,fac_us都需要使用
#if SYSTEM_SUPPORT_OS 						//如果需要支持OS.
    reload = ucSYSCLK;					      //每秒钟的计数次数 单位为K
    reload *= 1000000 / configTICK_RATE_HZ;	//根据delay_ostickspersec设定溢出时间
    //reload为24位寄存器,最大值:16777216,在216M下,约合77.7ms左右
    fac_ms = 1000 / configTICK_RATE_HZ;		//代表OS可以延时的最少单位
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; //开启SYSTICK中断
    SysTick->LOAD = reload; 					//每1/OS_TICKS_PER_SEC秒中断一次
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; //开启SYSTICK
#endif
}

#if SYSTEM_SUPPORT_OS 						//如果需要支持OS.
//延时nus
//nus:要延时的us数.
//nus:0~204522252(最大值即2^32/fac_us@fac_us=21)
void Delay_us(uint32_t ulTime_us)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;				//LOAD的值
    ticks = ulTime_us * fac_us; 						//需要的节拍数
    told = SysTick->VAL;        				//刚进入时的计数器值

    for(;;)
    {
        tnow = SysTick->VAL;

        if(tnow != told)
        {
            if(tnow < told)
            {
                tcnt += told - tnow;    //这里注意一下SYSTICK是一个递减的计数器就可以了.
            }
            else
            {
                tcnt += reload - tnow + told;
            }

            told = tnow;

            if(tcnt >= ticks)
            {
                break;    //时间超过/等于要延迟的时间,则退出.
            }
        }
    };
}

//延时nms,会引起任务调度
//nms:要延时的ms数
//nms:0~65535
void Delay_ms(uint32_t ulTime_ms)
{
    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)  //系统已经运行
    {
        if(ulTime_ms >= fac_ms)						//延时的时间大于OS的最少时间周期
        {
            vTaskDelay(ulTime_ms / fac_ms);	 		//FreeRTOS延时
        }

        ulTime_ms %= fac_ms;						//OS已经无法提供这么小的延时了,采用普通方式延时
    }

    Delay_us((uint32_t)(ulTime_ms * 1000));				//
}

//延时nms,不会引起任务调度
//nms:要延时的ms数
void Delay_xms(uint32_t ulTime_ms)
{
	uint32_t i;
	
	for(i=0; i < ulTime_ms; i++) 
	{
		Delay_us(1000);
	}	
}

/**
  * @brief This function provides minimum delay (in milliseconds) based
  *        on variable incremented.
  * @note In the default implementation , SysTick timer is the source of time base.
  *       It is used to generate interrupts at regular time intervals where uwTick
  *       is incremented.
  * @note 
  * @param Delay  specifies the delay time length, in milliseconds.
  * @retval None
  */
void HAL_Delay(uint32_t Delay)
{
	Delay_xms(Delay);
}
#else  //不用os时
//延时nus
//nus为要延时的us数.
//注意:nus的值不要大于1000us
void Delay_us(uint32_t ulTime_us)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;				//LOAD的值
    ticks = ulTime_us * fac_us; 						//需要的节拍数
    told = SysTick->VAL;        				//刚进入时的计数器值

    for(;;)
    {
        tnow = SysTick->VAL;

        if(tnow != told)
        {
            if(tnow < told) {
                tcnt += told - tnow;    //这里注意一下SYSTICK是一个递减的计数器就可以了.
            }
            else {
                tcnt += reload - tnow + told;
            }

            told = tnow;

            if(tcnt >= ticks) {
                break;    //时间超过/等于要延迟的时间,则退出.
            }
        }
    };
}

//延时nms
//nms:要延时的ms数
void Delay_ms(uint32_t ulTime_ms)
{
    uint32_t i;

    for(i = 0; i < ulTime_ms; i++) {
        Delay_us(1000);
    }
}

#endif

