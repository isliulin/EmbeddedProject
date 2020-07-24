/**
  ******************************************************************************
  * File Name          : RTC.h
  * Description        : This file provides code for the configuration
  *                      of the RTC instances.
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_DRIVER_RTC_H
#define __BSP_DRIVER_RTC_H

#ifdef __cplusplus
extern "C" {
#endif



extern RTC_HandleTypeDef rtcHandle;

void BSP_RTC_Init(void);
HAL_StatusTypeDef rtcSetDate(uint8_t ucYear, uint8_t ucMonth, uint8_t ucDate);
HAL_StatusTypeDef rtcSetTime(uint8_t ucHour, uint8_t ucMinute, uint8_t ucSecond);
void rtcGetCalendar(void);

#ifdef __cplusplus
}
#endif

#endif //__BSP_DRIVER_RTC_H


