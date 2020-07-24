/**
  ******************************************************************************
    File Name          : RTC.c
    Description        : This file provides code for the configuration
                         of the RTC instances.
  ******************************************************************************


  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "include.h"

#define RTC_ASYNCH_PREDIV 0x7F  /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV 0x00FF /* LSE as RTC clock */


RTC_HandleTypeDef rtcHandle;


/*******************************************************
  * @brief  :
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @retval :
*******************************************************/
void BSP_RTC_Init(void)
{
    rtcHandle.Instance            = RTC;
    rtcHandle.Init.HourFormat     = RTC_HOURFORMAT_24;
    rtcHandle.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV; //设置异步预分频器的值
    rtcHandle.Init.SynchPrediv    = RTC_SYNCH_PREDIV;   //设置同步预分频器的值   分频系数 = 128 * 256 = 32768
    rtcHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
    rtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    rtcHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;

    if(HAL_RTC_Init(&rtcHandle) != HAL_OK)
    {
    }
     
    if(HAL_RTCEx_BKUPRead(&rtcHandle, RTC_BKP_DR0) != 0x32F2)
    {
        rtcSetTime(16, 7, 55);
        rtcSetDate(20, 6, 03);

        HAL_RTCEx_BKUPWrite(&rtcHandle, RTC_BKP_DR0, 0x32F2);
    }
    else
    {
        // Allow access to RTC
        __HAL_RCC_PWR_CLK_ENABLE();
        HAL_PWR_EnableBkUpAccess();//取消备份区域写保护

        // RTC clock enable
        __HAL_RCC_RTC_ENABLE();
    }

    /*
    if(HAL_RTCEx_BKUPRead(&rtcHandle, RTC_BKP_DR0) != 0x32F2)
    {
        rtcHandle.Instance            = RTC;
        rtcHandle.Init.HourFormat     = RTC_HOURFORMAT_24;
        rtcHandle.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV; //设置异步预分频器的值
        rtcHandle.Init.SynchPrediv    = RTC_SYNCH_PREDIV;   //设置同步预分频器的值   分频系数 = 128 * 256 = 32768
        rtcHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
        rtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
        rtcHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_OPENDRAIN;

        if(HAL_RTC_Init(&rtcHandle) != HAL_OK)
        {
            //Error_Handler();
        }

        rtcSetTime(8, 00, 00);
        rtcSetDate(20, 06, 01);

        HAL_RTCEx_BKUPWrite(&rtcHandle, RTC_BKP_DR0, 0x32F2);
    }*/
}

/*******************************************************
  * @brief  :
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @retval :
*******************************************************/
void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
    if(hrtc->Instance == RTC)
    {
        RCC_OscInitTypeDef tRCCOscInitStruct;
        RCC_PeriphCLKInitTypeDef tPeriphClkInitStruct;

        __HAL_RCC_PWR_CLK_ENABLE();

        HAL_PWR_EnableBkUpAccess();//取消备份区域写保护

        tRCCOscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_LSI;
        tRCCOscInitStruct.PLL.PLLState = RCC_PLL_NONE;
        tRCCOscInitStruct.LSEState = RCC_LSE_ON;
        tRCCOscInitStruct.LSIState = RCC_LSI_OFF;
        if(HAL_RCC_OscConfig(&tRCCOscInitStruct) != HAL_OK)
        {
            Error_Handler();
        }

        // Connect LSE to RTC
        tPeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
        tPeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
        if(HAL_RCCEx_PeriphCLKConfig(&tPeriphClkInitStruct) != HAL_OK)
        {
            Error_Handler();
        }

        /* RTC clock enable */
        __HAL_RCC_RTC_ENABLE();

    }
}

/*******************************************************
  * @brief  :
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @retval :
*******************************************************/
void HAL_RTC_MspDeInit(RTC_HandleTypeDef *hrtc)
{
    if(hrtc->Instance == RTC)
    {
        /*##-1- Reset peripherals ##################################################*/
        __HAL_RCC_RTC_DISABLE();

        /*##-2- Disables the PWR Clock and Disables access to the backup domain ###################################*/
        HAL_PWR_DisableBkUpAccess();
        __HAL_RCC_PWR_CLK_DISABLE();
    }
}

/*******************************************************
  * @brief  : RTC日期设置
  * @note   :
  * @param  : year,month,date:年(0~99),月(1~12),日(0~31)
  * @param  :
  * @param  :
  * @retval :     HAL_OK       = 0x00U,
    HAL_ERROR    = 0x01U,
    HAL_BUSY     = 0x02U,
    HAL_TIMEOUT  = 0x03U
*******************************************************/
HAL_StatusTypeDef rtcSetDate(uint8_t ucYear, uint8_t ucMonth, uint8_t ucDate)
{
    RTC_DateTypeDef tRTCDateStructure;

    tRTCDateStructure.WeekDay = 0;//这里必须要设置星期，否则读取年份不对
    tRTCDateStructure.Date    = ucDate;
    tRTCDateStructure.Month   = ucMonth;
    tRTCDateStructure.Year    = ucYear;
    return HAL_RTC_SetDate(&rtcHandle, &tRTCDateStructure, RTC_FORMAT_BIN);
}

/*******************************************************
  * @brief  : RTC时间设置
  * @note   :
  * @param  : hour,min,sec:小时,分钟,秒钟
  * @param  :
  * @param  :
  * @retval :     HAL_OK       = 0x00U,
    HAL_ERROR    = 0x01U,
    HAL_BUSY     = 0x02U,
    HAL_TIMEOUT  = 0x03U
*******************************************************/
HAL_StatusTypeDef rtcSetTime(uint8_t ucHour, uint8_t ucMinute, uint8_t ucSecond)
{
    RTC_TimeTypeDef tRTCTimeStructure;

    tRTCTimeStructure.Hours          = ucHour;
    tRTCTimeStructure.Minutes        = ucMinute;
    tRTCTimeStructure.Seconds        = ucSecond;
    tRTCTimeStructure.TimeFormat     = RTC_HOURFORMAT_24;
    tRTCTimeStructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    tRTCTimeStructure.StoreOperation = RTC_STOREOPERATION_RESET;
    return HAL_RTC_SetTime(&rtcHandle, &tRTCTimeStructure, RTC_FORMAT_BIN);
}

/*******************************************************
  * @brief  :
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @return :
*******************************************************/
void rtcGetCalendar(void)
{
    //HAL_RTC_GetTime()函数必须先于HAL_RTC_GetDate()函数调用
    HAL_RTC_GetTime(&rtcHandle, &tTimeStructureget, RTC_FORMAT_BIN);/* Get the RTC current Time */
    HAL_RTC_GetDate(&rtcHandle, &tDateStructureget, RTC_FORMAT_BIN);/* Get the RTC current Date */
}

//--------------------------------------------------------------------------------------------