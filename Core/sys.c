




//********************************************************************************
//修改说明
//无
//////////////////////////////////////////////////////////////////////////////////

#include "include.h"
//#include "sys.h"



//使能CPU的L1-Cache
void Cache_Enable( void )
{
    //    SCB_EnableICache();//使能I-Cache
    //    SCB_EnableDCache();//使能D-Cache
    //    SCB->CACR |= 1 << 2; //强制D-Cache透写,如不开启,实际使用中可能遇到各种问题

    /* Invalidate I-Cache : ICIALLU register*/
    SCB_InvalidateICache();

    /* Enable branch prediction */
    SCB->CCR |= ( 1 << 18 );
    __DSB();

    /* Invalidate I-Cache : ICIALLU register*/
    SCB_InvalidateICache();

    /* Enable I-Cache */
    SCB_EnableICache();

    SCB_InvalidateDCache();
    SCB_EnableDCache();
    SCB->CACR |= 1 << 2; //强制D-Cache透写,如不开启,实际使用中可能遇到各种问题
}

//时钟设置函数
//Fvco=Fs*(plln/pllm);
//Fsys=Fvco/pllp=Fs*(plln/(pllm*pllp));
//Fusb=Fvco/pllq=Fs*(plln/(pllm*pllq));

//Fvco:VCO频率
//Fsys:系统时钟频率
//Fusb:USB,SDIO,RNG等的时钟频率
//Fs:PLL输入时钟频率,可以是HSI,HSE等.
//plln:主PLL倍频系数(PLL倍频),取值范围:64~432.
//pllm:主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.
//pllp:系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)
//pllq:USB/SDIO/随机数产生器等的主PLL分频系数(PLL之后的分频),取值范围:2~15.

//外部晶振为25M的时候,推荐值:plln=432,pllm=25,pllp=2,pllq=9.
//得到:Fvco=25*(432/25)=432Mhz
//     Fsys=432/2=216Mhz
//     Fusb=432/9=48Mhz
//返回值:0,成功;1,失败
void Stm32_Clock_Init( uint32_t plln, uint32_t pllm, uint32_t pllp, uint32_t pllq )
{
    HAL_StatusTypeDef ret = HAL_OK;
    RCC_OscInitTypeDef RCC_OscInitStructure = {0};;
    RCC_ClkInitTypeDef RCC_ClkInitStructure = {0};;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};;

    __HAL_RCC_PWR_CLK_ENABLE(); //使能PWR时钟

    __HAL_PWR_VOLTAGESCALING_CONFIG( PWR_REGULATOR_VOLTAGE_SCALE1 ); //设置调压器输出电压级别，以便在器件未以最大频率工作

    RCC_OscInitStructure.OscillatorType = RCC_OSCILLATORTYPE_HSE;  //时钟源为HSE
    RCC_OscInitStructure.HSEState = RCC_HSE_ON;                    //打开HSE
    RCC_OscInitStructure.PLL.PLLState = RCC_PLL_ON;				 //打开PLL
    RCC_OscInitStructure.PLL.PLLSource = RCC_PLLSOURCE_HSE;        //PLL时钟源选择HSE
    RCC_OscInitStructure.PLL.PLLM = pllm;	//主PLL和音频PLL分频系数(PLL之前的分频)
    RCC_OscInitStructure.PLL.PLLN = plln; //主PLL倍频系数(PLL倍频)
    RCC_OscInitStructure.PLL.PLLP = pllp; //系统时钟的主PLL分频系数(PLL之后的分频)
    RCC_OscInitStructure.PLL.PLLQ = pllq; //USB/SDIO/随机数产生器等的主PLL分频系数(PLL之后的分频)
    ret = HAL_RCC_OscConfig( &RCC_OscInitStructure ); //初始化

    if ( ret != HAL_OK )
    {
        Error_Handler();
    }

    ret = HAL_PWREx_EnableOverDrive(); //开启Over-Driver功能

    if ( ret != HAL_OK )
    {
        Error_Handler();
    }

    //选中PLL作为系统时钟源并且配置HCLK,PCLK1和PCLK2
    RCC_ClkInitStructure.ClockType = ( RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 );
    RCC_ClkInitStructure.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK; //设置系统时钟时钟源为PLL
    RCC_ClkInitStructure.AHBCLKDivider = RCC_SYSCLK_DIV1; //AHB分频系数为1
    RCC_ClkInitStructure.APB1CLKDivider = RCC_HCLK_DIV4; //APB1分频系数为4
    RCC_ClkInitStructure.APB2CLKDivider = RCC_HCLK_DIV2; //APB2分频系数为2

    ret = HAL_RCC_ClockConfig( &RCC_ClkInitStructure, FLASH_LATENCY_7 ); //同时设置FLASH延时周期为7WS，也就是8个CPU周期。

    if ( ret != HAL_OK )
    {
        Error_Handler();
    }


    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC | RCC_PERIPHCLK_SDMMC1
            | RCC_PERIPHCLK_CLK48;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
    PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
    PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2;
    PeriphClkInitStruct.PLLSAIDivQ = 1;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
    PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
    PeriphClkInitStruct.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_CLK48;

    if ( HAL_RCCEx_PeriphCLKConfig( &PeriphClkInitStruct ) != HAL_OK )
    {
        Error_Handler();
    }
}

/*******************************************************
    @brief  :
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
void SystemClock_Config( void )
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Configure LSE Drive Capability
    */
    HAL_PWR_EnableBkUpAccess();
    /** Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG( PWR_REGULATOR_VOLTAGE_SCALE1 );
    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 432;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 9;

    if ( HAL_RCC_OscConfig( &RCC_OscInitStruct ) != HAL_OK )
    {
        Error_Handler();
    }

    /** Activate the Over-Drive mode
    */
    if ( HAL_PWREx_EnableOverDrive() != HAL_OK )
    {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB busses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if ( HAL_RCC_ClockConfig( &RCC_ClkInitStruct, FLASH_LATENCY_7 ) != HAL_OK )
    {
        Error_Handler();
    }

    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC | RCC_PERIPHCLK_SDMMC1
            | RCC_PERIPHCLK_CLK48;
    PeriphClkInitStruct.PLLSAI.PLLSAIN = 360;
    PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
    PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
    PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4;
    PeriphClkInitStruct.PLLSAIDivQ = 1;
    PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_4;
    PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLL;
    PeriphClkInitStruct.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_CLK48;

    if ( HAL_RCCEx_PeriphCLKConfig( &PeriphClkInitStruct ) != HAL_OK )
    {
        Error_Handler();
    }
}


//判断I_Cache是否打开
//返回值:0 关闭，1 打开
uint8_t Get_ICahceSta( void )
{
    uint8_t sta;
    sta = ( ( SCB->CCR ) >> 17 ) & 0x01;
    return sta;
}

//判断I_Dache是否打开
//返回值:0 关闭，1 打开
uint8_t Get_DCahceSta( void )
{
    uint8_t sta;
    sta = ( ( SCB->CCR ) >> 16 ) & 0x01;
    return sta;
}

//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI
//__asm void WFI_SET( void )
//{
//    WFI;
//}
void WFI_SET( void )
{
    asm( "WFI" );
}

//关闭所有中断(但是不包括fault和NMI中断)
//__asm void INTX_DISABLE( void )
//{
//    CPSID   I
//    BX      LR
//}
void INTX_DISABLE( void )
{
    asm( "CPSID	I\n\t"
         "BX		LR" );
}

//开启所有中断
//__asm void INTX_ENABLE( void )
//{
//    CPSIE   I
//    BX      LR
//}
void INTX_ENABLE( void )
{
    asm( "CPSIE   I\n\t"
         "BX      LR" );
}

//设置栈顶地址
//addr:栈顶地址
//__asm void MSR_MSP( uint32_t addr )
//{
//    MSR MSP, r0 			//set Main Stack value
//    BX r14
//}
void MSR_MSP( uint32_t addr )
{
    asm( "MSR MSP, r0\n\t"		//set Main Stack value
         "BX r14" );
}


void Sys_Soft_Reset( void )
{
    SCB->AIRCR = 0X05FA0000 | ( uint32_t )0x04;
}

//---------------------------------------------
