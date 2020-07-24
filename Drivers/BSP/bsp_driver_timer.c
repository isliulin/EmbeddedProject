/**
  ******************************************************************************
    @file
    @author
    @version
    @date
    @brief   定时器中断驱动代码
  ******************************************************************************
    @history

  ******************************************************************************
*/


#include "include.h"

//extern __IO int32_t OS_TimeMS;

TIM_HandleTypeDef TIM3_Handler;	//TIM3句柄
TIM_HandleTypeDef TIM4_Handler;      //定时器句柄

//FreeRTOS时间统计所用的节拍计数器
volatile unsigned long long FreeRTOSRunTimeTicks;

//初始化TIM3使其为FreeRTOS的时间统计提供时基
void ConfigureTimeForRunTimeStats( void )
{
    //定时器3初始化，定时器时钟为108M，分频系数为108-1，所以定时器3的频率
    //为108M/108=1M，自动重装载为50-1，那么定时器周期就是50us
    FreeRTOSRunTimeTicks = 0;
    TIM3_Init( 50 - 1, 108 - 1 );	//初始化TIM3
}

//通用定时器3中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!(定时器3挂在APB1上，时钟为HCLK/2)
void TIM3_Init( uint16_t arr, uint16_t psc )
{
    TIM3_Handler.Instance = TIM3;                        //通用定时器3
    TIM3_Handler.Init.Prescaler = psc;                   //分频系数
    TIM3_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;  //向上计数器
    TIM3_Handler.Init.Period = arr;                      //自动装载值
    TIM3_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; //时钟分频因子
    HAL_TIM_Base_Init( &TIM3_Handler );

    HAL_TIM_Base_Start_IT( &TIM3_Handler ); //使能定时器3和定时器3更新中断：TIM_IT_UPDATE
}

//通用定时器4中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器4!(定时器4挂在APB1上，时钟为HCLK/2)
void TIM4_Init( uint16_t arr, uint16_t psc )
{
    TIM4_Handler.Instance = TIM4;                        //通用定时器4
    TIM4_Handler.Init.Prescaler = psc;                   //分频
    TIM4_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;  //向上计数器
    TIM4_Handler.Init.Period = arr;                      //自动装载值
    TIM4_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init( &TIM4_Handler );

    HAL_TIM_Base_Start_IT( &TIM4_Handler ); //使能定时器3和定时器3中断
}

//定时器底册驱动，开启时钟，设置中断优先级
//此函数会被HAL_TIM_Base_Init()函数调用
void HAL_TIM_Base_MspInit( TIM_HandleTypeDef *htim )
{
    if ( htim->Instance == TIM3 )
    {
        __HAL_RCC_TIM3_CLK_ENABLE();            //使能TIM3时钟
        HAL_NVIC_SetPriority( TIM3_IRQn, 10, 0 ); //设置中断优先级，抢占优先级1，子优先级0
        HAL_NVIC_EnableIRQ( TIM3_IRQn );        //开启ITM3中断
    }

    else if ( htim->Instance == TIM4 )
    {
        __HAL_RCC_TIM4_CLK_ENABLE();            //使能TIM4时钟
        HAL_NVIC_SetPriority( TIM4_IRQn, 10, 0 ); //设置中断优先级，抢占优先级2，子优先级0
        HAL_NVIC_EnableIRQ( TIM4_IRQn );        //开启ITM4中断
    }
}

//定时器3中断服务函数
void TIM3_IRQHandler( void )
{
    HAL_TIM_IRQHandler( &TIM3_Handler );
}

//定时器4中断服务函数
void TIM4_IRQHandler( void )
{
    HAL_TIM_IRQHandler( &TIM4_Handler );
}

//回调函数，定时器中断服务函数调用
void HAL_TIM_PeriodElapsedCallback( TIM_HandleTypeDef *htim )
{
    if ( htim == ( &TIM3_Handler ) )
    {
        FreeRTOSRunTimeTicks++;
        //		OS_TimeMS++;
    }

    else if ( htim == ( &TIM4_Handler ) )
    {
        //        GUI_TOUCH_Exec();
    }
}
