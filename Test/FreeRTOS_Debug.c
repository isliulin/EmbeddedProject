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
		if( xSemaphoreTake(xDebugSemaphore_Handle, portMAX_DELAY) == pdTRUE )	//��ȡ�ź�����vUsart1RxTask���ͷ�
		{
			SEGGER_RTT_SetTerminal(0);
			SEGGER_RTT_printf(0, "=================================================\r\n");
			SEGGER_RTT_printf(0, "������		    ����״̬  ���ȼ�   ʣ��ջ �������\r\n");
			
//			printf("\r\n-------------------------------------------------------\r\n");
//			printf("������		    ����״̬  ���ȼ�   ʣ��ջ �������\r\n");		
			vTaskList((char *)&pcWriteBuffer);
			SEGGER_RTT_printf(0, "%s\r\n", pcWriteBuffer);	
//			printf("%s\r\n", pcWriteBuffer);
			
			SEGGER_RTT_printf(0, "\r\n������		      ���м���	      ʹ����\r\n");
//			printf("\r\n������		      ���м���	      ʹ����\r\n");
			vTaskGetRunTimeStats((char *)&pcWriteBuffer);
			SEGGER_RTT_printf(0, "%s\r\n", pcWriteBuffer);		
//			printf("%s\r\n", pcWriteBuffer);	
		}	
	}
	
}


