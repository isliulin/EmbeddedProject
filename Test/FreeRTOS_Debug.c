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
  
#include "include.h"


void vInformationTask(void *pvParameters)
{
	uint8_t pcWriteBuffer[500];

	for(;;)
	{
		if( xSemaphoreTake(xDebugSemaphore_Handle, portMAX_DELAY) == pdTRUE )	//获取信号量，vUsart1RxTask里释放
		{
			SEGGER_RTT_SetTerminal(0);
			SEGGER_RTT_printf(0, "=================================================\r\n");
			SEGGER_RTT_printf(0, "任务名		    任务状态  优先级   剩余栈 任务序号\r\n");
			
//			printf("\r\n-------------------------------------------------------\r\n");
//			printf("任务名		    任务状态  优先级   剩余栈 任务序号\r\n");		
			vTaskList((char *)&pcWriteBuffer);
			SEGGER_RTT_printf(0, "%s\r\n", pcWriteBuffer);	
//			printf("%s\r\n", pcWriteBuffer);
			
			SEGGER_RTT_printf(0, "\r\n任务名		      运行计数	      使用率\r\n");
//			printf("\r\n任务名		      运行计数	      使用率\r\n");
			vTaskGetRunTimeStats((char *)&pcWriteBuffer);
			SEGGER_RTT_printf(0, "%s\r\n", pcWriteBuffer);		
//			printf("%s\r\n", pcWriteBuffer);	
		}	
	}
	
}


