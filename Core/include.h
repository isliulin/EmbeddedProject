/**
  ******************************************************************************
  * @file		include.h    
  * @author  
  * @version	 
  * @date    
  * @brief   
  *    
  ******************************************************************************
  * @attention
  *
  * 
  *
  ******************************************************************************
  */

#ifndef __INCLUDE_H
#define __INCLUDE_H

#ifdef __cplusplus
extern "C" {
#endif

/*
*********************************************************************************************************
*                                           宏定义
*********************************************************************************************************
*/

#ifdef _WIN32
#undef _WIN32
#endif

//0,不支持os
//1,支持os
#define SYSTEM_SUPPORT_OS		1		//定义系统文件夹是否支持OS

#define RGBLCD  1

#define DEBUG_TARGET    1


/*
*********************************************************************************************************
*                                        STANDARD LIBRARIES
*********************************************************************************************************
*/
#include <stdlib.h>
#include <string.h> 
#include <math.h>
#include <assert.h> 
#include <stdarg.h>
#include <stdio.h>


/*
*********************************************************************************************************
*                                        STM32 Include
*********************************************************************************************************
*/
//#include "cmsis_armclang.h"
//#include "stm32f7xx_hal_conf.h"
//#include "stm32f7xx.h"
#include "stm32f7xx_hal.h"   




/*
*********************************************************************************************************
*                                      FreeRTOS includes
*********************************************************************************************************
*/
#if SYSTEM_SUPPORT_OS
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"
#include "limits.h"
#include "list.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "croutine.h"
#include "event_groups.h"


#endif

extern osKernelState_t KernelState;

#ifndef IS_IRQ
#define IS_IRQ_MASKED()           ((__get_PRIMASK() != 0U) || ((KernelState == osKernelRunning) && (__get_BASEPRI() != 0U)))
#define IS_IRQ_MODE()             (__get_IPSR() != 0U)
#define IS_IRQ()                  (IS_IRQ_MODE() || IS_IRQ_MASKED())
#endif

/*
*********************************************************************************************************
*                                      FatFs includes
*********************************************************************************************************
*/
#include "ff_gen_drv.h"
#include "fatfs.h"

/*
*********************************************************************************************************
*                                      emWin includes
*********************************************************************************************************
*/
#ifdef DEBUG_TARGET
#include "GUI.h"
#include "LCD.h"
#include "GUI_ConfDefaults.h"
#include "GUI_Private.h"
#include "GUIDRV_Lin.h"
#include "WM.h"
#include "DIALOG.h"

#ifdef RGBLCD
#include "LCDConf_RGB.h"
#endif

#ifdef MCULCD
#include "LCDConf_MCU.h"
#include "GUIDRV_MCU.h"
#endif
#endif


/*
*********************************************************************************************************
*                                        APP / BSP
*********************************************************************************************************
*/
#include "main.h"
#include "sys.h"
#include "bsp.h"
#include "delay.h"
#include "stm32f7xx_it.h"

#ifdef DEBUG_TARGET
#include "bsp_driver_tftlcd.h"
#include "bsp_driver_ltdc.h"
#include "bsp_driver_touch.h"
#include "bsp_driver_ts_iic.h"
#include "GuiTask.h"
#include "gt911.h"
#include "ft5206.h"
#include "w25qxx.h"
#include "ds18s20.h"

#include "SEGGER_RTT_Conf.h"
#include "SEGGER_RTT.h"

#include "EventRecorder.h"

#include "App_Printf.h"


#endif


#include "sd_diskio_dma_rtos.h"
//#include "sd_diskio.h"
#include "usb_device.h"
#include "mx25l256.h"


#include "bsp_driver_gpio.h"
#include "bsp_driver_led.h"
#include "bsp_driver_fmc_sdram.h"
#include "bsp_driver_fmc_sram.h"  
#include "bsp_driver_timer.h"
#include "bsp_driver_sd.h"
#include "bsp_driver_rtc.h"
#include "bsp_driver_mpu.h"
#include "bsp_driver_qspi.h"
#include "bsp_driver_usart.h"
#include "bsp_driver_wifi.h"
#include "bsp_driver_gps.h"
//#include "bsp_driver_i2c_smbus.h"
#include "bsp_driver_smbus_gpio.h"
#include "bsp_driver_bat.h"
#include "bsp_driver_fpga.h"
#include "bsp_driver_temp.h"

#include "tdip.h"
#include "fdip.h"
#include "process.h"


//------------------------------------------------------  


   
//----------------------------------------------------





#ifdef __cplusplus
}
#endif

#endif //__INCLUDE_H

/****END OF FILE****/



