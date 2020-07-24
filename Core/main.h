/**
  ******************************************************************************
  * @file    main.h 
  * @author  
  * @version 
  * @date    
  * @brief   Header for main.c module
  ******************************************************************************
**/
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
//#include "include.h"

#include "stm32f7xx_hal.h"

#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"
#include "limits.h"
#include "list.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "croutine.h"
#include "semphr.h"
#include "event_groups.h"

/*
    the following Timeout is useful to give the control back to the applications
    in case of errors in either BSP_SD_ReadCpltCallback() or BSP_SD_WriteCpltCallback()
    the value by default is as defined in the BSP platform driver otherwise 30 secs

*/


//#if 1
//#ifdef __NVIC_PRIO_BITS 
//#undef __NVIC_PRIO_BITS 
//#define __NVIC_PRIO_BITS      4 
//#endif 
//#endif

extern TaskHandle_t xStartTask_Handle;
//extern TaskHandle_t xUsart1TxTask_Handle;
extern TaskHandle_t xUsart1RxTask_Handle;
extern TaskHandle_t xInitHardwareTask_Handle;
extern TaskHandle_t xWriteToSDTask_Handle;
//extern TaskHandle_t xDataProcess_Handle;
extern TaskHandle_t xLedTask_Handle;
extern TaskHandle_t xSaveDateTask_Handle;
extern TaskHandle_t xAdDataReceiveTask_Handle;
extern TaskHandle_t xFdipStartTask_Handle;
//extern TaskHandle_t xReadStartStopStatusRegTask_Handle;
//extern TaskHandle_t xQueryCurrentFile_Handle;
extern TaskHandle_t xReadIntStatusRegTask_Handle;
extern TaskHandle_t xReadDeviceStatusTask_Handle;
extern TaskHandle_t xSyncStartTask_Handle;
extern TaskHandle_t xNewFileTask_Handle;
extern TaskHandle_t xStopSendTask_Handle;
extern TaskHandle_t xReadSendStopFlagTask_Handle;
extern TaskHandle_t xTdipStartTask_Handle;
extern TaskHandle_t xReadSysInfTask_Handle;

#ifdef DEBUG_TARGET
extern TaskHandle_t xTouchTask_Handle;
extern TaskHandle_t xDebugTask_Handle;
extern TaskHandle_t xGUITask_Handle;
extern TaskHandle_t xFatfsTestTask_Handle;
#endif

extern QueueHandle_t xSDQueue_Handle;				             //SD队列句柄
/*extern QueueHandle_t xUsart1TxQueue_Handle;			         //USART1发送队列句柄*/
extern QueueHandle_t xWriteToSDQueue_Handle;                     //写入SD卡的队列句柄

extern SemaphoreHandle_t xFatfsMscSemaphore_Handle;	             //FATFS和USB信号量句柄
extern SemaphoreHandle_t xATCmdAckSemaphore_Handle;	             //AT命令应答信号量句柄
extern SemaphoreHandle_t xUsart1RxSemaphore_Handle;	             //USART1接收信号量句柄
/*extern SemaphoreHandle_t xUsart1TxSemaphore_Handle;	         //USART1发送信号量句柄*/
/*extern SemaphoreHandle_t xAdDataReadableSemaphore_Handle;      //AD数据信号量句柄*/
extern SemaphoreHandle_t xSyncStartSemaphore_Handle;             //句柄
extern SemaphoreHandle_t xGpsMsgReadableSemaphore_Handle;        //GPS信息信号量句柄
extern SemaphoreHandle_t xFdipSaveSemaphore_Handle;              //FDIP存储信号量句柄
extern SemaphoreHandle_t xStartSucessSemaphore_Handle;           //启动成功信号量句柄
extern SemaphoreHandle_t xFrequencyGroupCycleSemaphore_Handle;   //频组间隔信号量句柄
extern SemaphoreHandle_t xSendStopFlagSemaphore_Handle;          //发射停止标志信号量
extern SemaphoreHandle_t xWaveStartSemaphore_Handle;             //波形开始信号量

extern TimerHandle_t xGpsMsgReceiveTimer_Handle;                 //GPS信息接收定时器句柄

#ifdef DEBUG_TARGET
extern SemaphoreHandle_t xFontSemaphore_Handle;        //字体读写完成信号量句柄
extern SemaphoreHandle_t xPrintfMutexSemaphore_Handle; //App_Printf互斥信号量句柄
#endif

#define START_TASK_PRIO                  ((UBaseType_t)1)//256
#define DEBUG_TASK_PRIO                  ((UBaseType_t)1)//1024
#define GUI_TASK_PRIO                    ((UBaseType_t)1)//2048
#define READ_SYS_INF_TASK_PRIO           ((UBaseType_t)1)//256
#define LED_TASK_PRIO                    ((UBaseType_t)2)//64 
#define TOUCH_TASK_PRIO                  ((UBaseType_t)3)//256 
#define READ_DEVICE_STATUS_TASK_PRIO     ((UBaseType_t)4)//256
#define READ_INT_STATUS_REG_TASK_PRIO    ((UBaseType_t)5)//256
#define INIT_HARDWARE_TASK_PRIO          ((UBaseType_t)6)//2048
#define USART1_RX_TASK_PRIO              ((UBaseType_t)7)//4096
#define SAVE_DATE_TASK_PRIO              ((UBaseType_t)8)//1024 未用
#define NEW_FILE_TASK_PRIO               ((UBaseType_t)9)//512
#define READ_SEND_STOP_FLAG_TASK_PRIO    ((UBaseType_t)10)//256
#define WRITE_TO_SD_TASK_PRIO            ((UBaseType_t)11)//2048
#define FDIP_START_TASK_PRIO             ((UBaseType_t)12)//1024
#define TDIP_START_TASK_PRIO             ((UBaseType_t)12)//1024
#define STOP_SEND_TASK_PRIO              ((UBaseType_t)13)//2048
#define SYNC_START_TASK_PRIO             ((UBaseType_t)14)//2048 未用，改成函数
#define AD_DATA_RECEIVE_TASK_PRIO        ((UBaseType_t)15)//1024



void vStartTask(void *pvParameters);
void vInitHardwareTask(void *pvParameters);
void vLedTask(void *pvParameters);
void vReadIntStatusRegTask(void *pvParameters);
void vReadDeviceStatusTask(void *pvParameters);
void clearIntStatusReg(void);
void vReadSysInfTask(void *pvParameters);

#ifdef DEBUG_TARGET
void vTouchTask(void *pvParameters);
void vDebugTask(void *pvParameters);
#endif


void Error_Handler(void);

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
