/**
  ******************************************************************************
    @file    stm32f7xx_it.c
    @brief   Interrupt Service Routines.
  ******************************************************************************



  ******************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include "include.h"



extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

extern void xPortSysTickHandler(void);


/******************************************************************************/
/*            Cortex-M7 Processor Interruption and Exception Handlers         */
/******************************************************************************/

void BSP_NVIC_Init(void)
{
	
}

/**
    @brief This function handles Non maskable interrupt.
*/
void NMI_Handler(void)
{

}

/**
    @brief This function handles Hard fault interrupt.
*/
void HardFault_Handler(void)
{
	BSP_LED_ON(ERROR_LED);
    for(;;)
    {
		
    }
}


/**
    @brief This function handles Pre-fetch fault, memory access fault.
*/
void BusFault_Handler(void)
{
    for(;;)
    {
		
    }
}

/**
    @brief This function handles Undefined instruction or illegal state.
*/
void UsageFault_Handler(void)
{
   for(;;)
    {
        
    }
}

/**
    @brief This function handles Debug monitor.
*/
void DebugMon_Handler(void)
{
    
}

/**
    @brief  This function handles SysTick Handler.
    @param  None
    @retval None
*/
void SysTick_Handler(void)
{
	HAL_IncTick();
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)	//系统已经运行
    {
	#endif  /* INCLUDE_xTaskGetSchedulerState */
        xPortSysTickHandler();
	#if (INCLUDE_xTaskGetSchedulerState  == 1 )
    }
	#endif  /* INCLUDE_xTaskGetSchedulerState */	
}

/******************************************************************************/
/* STM32F7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f7xx.s).                    */
/******************************************************************************/

/**
* @brief This function handles USB On The Go FS global interrupt.
* USB操作时FATFS不能操作SD卡，同样在FATFS操作SD卡时，USB不能操作.互斥
*/
void OTG_FS_IRQHandler(void)
{
    BaseType_t pxHigherPriorityTaskWoken;

	if(pdTRUE == xSemaphoreTakeFromISR(xFatfsMscSemaphore_Handle, &pxHigherPriorityTaskWoken))//获取信号量，在操作完SD卡后释放
	{
		HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
    }
    xSemaphoreGiveFromISR(xFatfsMscSemaphore_Handle, &pxHigherPriorityTaskWoken);	//释放信号量，在需要操作SD卡的地方获取	
	
}


/**
  * @brief  This function handles DMA2 Stream 3 interrupt request.
  * @param  None
  * @retval None
  */
void BSP_SDMMC1_DMA_Rx_IRQHandler(void)
{
	HAL_DMA_IRQHandler(uSdHandle.hdmarx);
}

/**
  * @brief  This function handles DMA2 Stream 6 interrupt request.
  * @param  None
  * @retval None
  */
void BSP_SDMMC1_DMA_Tx_IRQHandler(void)
{
	HAL_DMA_IRQHandler(uSdHandle.hdmatx);
}

/**
  * @brief  This function handles SDIO interrupt request.
  * @param  None
  * @retval None
  */
void SDMMC1_IRQHandler(void)
{
	HAL_SD_IRQHandler(&uSdHandle);
}   


/**
  * @brief  This function handles DMA RX interrupt request.
  * @param  None
  * @retval None
  * @Note   
  */
void USART1_DMA_RX_IRQHandler(void)
{
//	BaseType_t pxHigherPriorityTaskWoken;
	
	HAL_DMA_IRQHandler(huart1.hdmarx);
	
/*	if(__HAL_DMA_GET_FLAG(&USART1RxDMA_Handler, DMA_FLAG_TCIF2_6))//等待DMA2_Steam2传输完成
	{
		__HAL_DMA_DISABLE(&USART1RxDMA_Handler);
		__HAL_DMA_CLEAR_FLAG(&USART1RxDMA_Handler, DMA_FLAG_TCIF2_6);//清除DMA2_Steam7传输完成标志
		__HAL_DMA_ENABLE(&USART1RxDMA_Handler);		
		
		//给出二值信号量，发送接收到新数据标志，供vUsart1RxTask获取
		xSemaphoreGiveFromISR(xUsart1RxSemaphore_Handle, &pxHigherPriorityTaskWoken);	//释放二值信号量
		
		//如果需要的话进行一次任务切换，系统会判断是否需要进行切换
		portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);		
	}		*/
}

/**
  * @brief  This function handles DMA TX interrupt request.
  * @param  None
  * @retval None
  * @Note   
  */
void USART1_DMA_TX_IRQHandler(void)
{
	BaseType_t pxHigherPriorityTaskWoken;
	
	HAL_DMA_IRQHandler(huart1.hdmatx);

	if(__HAL_DMA_GET_FLAG(&USART1TxDMA_Handler, DMA_FLAG_TCIF3_7))//等待DMA2_Steam7传输完成
	{
		__HAL_DMA_DISABLE(&USART1TxDMA_Handler);
		__HAL_DMA_CLEAR_FLAG(&USART1TxDMA_Handler, DMA_FLAG_TCIF3_7);//清除DMA2_Steam7传输完成标志
		__HAL_DMA_ENABLE(&USART1TxDMA_Handler);	
		
		//xSemaphoreGiveFromISR(xUsart1TxSemaphore_Handle, &pxHigherPriorityTaskWoken);	//vUsart1TxTask获取
		//
		////如果需要的话进行一次任务切换，系统会判断是否需要进行切换
		//portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);			
	}	
}

		
/**
  * @brief  This function handles USART1 interrupt request.  
  * @param  None
  * @retval None
  * @Note        
  */
void USART1_IRQHandler(void)
{	
	uint32_t ulReturn;	
    
	// 进入临界段，临界段可以嵌套/
	ulReturn = taskENTER_CRITICAL_FROM_ISR();	
    
	HAL_UART_IRQHandler(&huart1);	//这句很重要，去掉后发送只能发送一次
      
	if((READ_REG(huart1.Instance->ISR) & USART_ISR_IDLE) != RESET)//是空闲中断
	{
        __HAL_UART_CLEAR_IT(&huart1, UART_CLEAR_IDLEF);
		USART1_IdelCallback();		
	}

    // 退出临界段
	taskEXIT_CRITICAL_FROM_ISR( ulReturn );		
}

/**
  * @brief This function handles EXTI line1 interrupt.
  * PA1，GPS_PPS_PIN
  */
void EXTI1_IRQHandler(void)
{  
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1); 
}

/**
  * @brief This function handles EXTI line3 interrupt.
  * PB3，EXTERNAL_POWER_PIN
  */
void EXTI3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}

/**
  * @brief This function handles EXTI line[9:5] interrupts.
  * PB5，GPGA_IRQ_PIN
  * PA8，FPGA_PPS_PIN
  */
void EXTI9_5_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
}

/*******************************************************
  * @brief  : 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @return : 
*******************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    BaseType_t pxHigherPriorityTaskWoken;

/*if(GPIO_Pin == FPGA_IRQ_PIN)
    {     
      
        if(fpgaReadByte(FPGA_INT_STATUS_REG) & GPS_MSG_READABLE == GPS_MSG_READABLE)    //GPS信息准备就绪
        {
            //连续读取GPS的FIFO寄存器4次
            ptGPSMsg->ucGPSStatus = fpgaReadByte(FPGA_GPS_FIFO_REG);    //GPS信号有效标志 'A' 'V'
            if(ptGPSMsg->ucGPSStatus == 'A')//GPS信号有效
            {
                tFpgaTime.ucHours   = fpgaReadByte(FPGA_GPS_FIFO_REG);    //时
                tFpgaTime.ucMinutes = fpgaReadByte(FPGA_GPS_FIFO_REG);    //分
                tFpgaTime.ucSeconds = fpgaReadByte(FPGA_GPS_FIFO_REG);    //秒
            }
            else//GPS信号无效
            {
                fpgaReadByte(FPGA_GPS_FIFO_REG);    //时
                fpgaReadByte(FPGA_GPS_FIFO_REG);    //分
                fpgaReadByte(FPGA_GPS_FIFO_REG);    //秒
            }
        }

        if(fpgaReadByte(FPGA_INT_STATUS_REG) & AD_DATA_READABLE == AD_DATA_READABLE)//AD数据准备就绪
        {            
            xSemaphoreGiveFromISR(xAdDataReadableSemaphore_Handle, &pxHigherPriorityTaskWoken);	//释放信号量，在vAdDataProcessCallback获取 
        }
    }
    else */
    if(GPIO_Pin == FPGA_PPS_PIN)
    {        
        xSemaphoreGiveFromISR(xGpsMsgReadableSemaphore_Handle, &pxHigherPriorityTaskWoken);	//释放信号量，在vGpsMsgReceiveCallback获取 
    }   
    else if(GPIO_Pin == EXTERNAL_POWER_PIN)
    {
		if(HAL_GPIO_ReadPin(EXTERNAL_POWER_GPIO_PORT, EXTERNAL_POWER_PIN))
		{
			g_ucExternalPower = 0;	//外部供电，0表示由电池供电，1为外部供电
		}
		else
		{
			g_ucExternalPower = 1;
		}		
    }   
}

/**
  * @brief  This function handles UART interrupt request.  
  * @param  None
  * @retval None
  * @Note     
  *
void USART2_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart2);

	//处理接收中断
	if(__HAL_UART_GET_IT( &huart2, UART_IT_RXNE ) != RESET)
	{		        	
        HAL_UART_Receive(&huart2, g_ucRxBuffer2, RXBUFFERSIZE);      
		if(g_ucGPSMsgValid == 0)
		{
			if ('$' == g_ucRxBuffer2[0])
			{				
                g_ulUSART2Count = 0;
                g_ucGPSRecvState = FSA_INIT;
			}
            if (g_ucGPSRecvState == FSA_INIT)
            {
                g_ucUSART2_RX_BUF[g_ulUSART2Count] = g_ucRxBuffer2[0];
                g_ulUSART2Count++;

                if (g_ucRxBuffer2[0] == '\n')
                {					
                    g_ucGPSMsgValid = 1;
                    g_ucGPSRecvState = FSA_NULL;
                    g_ucUSART2_RX_BUF[g_ulUSART2Count] = 0;
                }		
				if ( g_ulUSART2Count == USART2_RX_LEN )
				{
					g_ulUSART2Count = 0;
				}
            }
		}			
	}
	
	//处理发送缓冲区空中断
	if(__HAL_UART_GET_IT( &huart2, UART_IT_TXE ) != RESET)
	{		       		
        HAL_UART_Transmit(&huart2, g_ucRxBuffer2, 1, 1000);        
	}
}
*/
//---------------------------------------------------------------------