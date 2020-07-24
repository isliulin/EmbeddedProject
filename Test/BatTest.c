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

/*******************************************************
  * @brief  : 
  * @note   :
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void batTest(void)
{
    uint16_t usVal;
    uint8_t ucaBuf[50];
    float_t fTemp;
    int16_t sCurrent;      
            
//    GUI_SetBkColor(GUI_DARKBLUE); //设置背景颜色
//    GUI_Clear();              //清屏
//    GUI_SetFont(&XBF_Font24); //设置字体
//    GUI_SetColor(GUI_WHITE); //设置前景色(如文本，画线等颜色)
       
//    usVal = batCheck(BAT_SLAVE_ADDRESS);
//    if (0 == usVal)
//    {
//        for (;;)
//        {
            GUI_ClearRect(0, 0, 300, 300);
            usVal = batMode();
            sprintf((char *)ucaBuf, "模式：%#X", usVal);
            GUI_DispStringAt((char *)ucaBuf, 0, 0);
            vTaskDelay(pdMS_TO_TICKS(1000));
            
            usVal = batStatus();
            sprintf((char *)ucaBuf, "状态：%#X", usVal);
            GUI_DispStringAt((char *)ucaBuf, 0, 24);
            vTaskDelay(pdMS_TO_TICKS(1000));
            
            usVal = batTemperature(); //单位为0.1°K   开氏度＝摄氏度＋273.15 [°C] = [K] − 273.15          
            fTemp = (float)usVal / 10;
            fTemp = fTemp - 273.15;
            sprintf((char *)ucaBuf, "温度：%.2f", fTemp);
            GUI_DispStringAt((char *)ucaBuf, 0, 48);
            vTaskDelay(pdMS_TO_TICKS(1000));
    
            usVal = batVoltage();
            fTemp = (float)usVal / 1000;
            sprintf((char *)ucaBuf, "电压：%.2f", fTemp);
            strcat((char *)ucaBuf, "V");
            GUI_DispStringAt((char *)ucaBuf, 0, 72);
            vTaskDelay(pdMS_TO_TICKS(1000));

            sCurrent = batCurrent();
            fTemp = (float)sCurrent / 1000;
            sprintf((char *)ucaBuf, "电流：%.2f", fTemp);
            strcat((char *)ucaBuf, "A");
            GUI_DispStringAt((char *)ucaBuf, 0, 96);
            vTaskDelay(pdMS_TO_TICKS(1000));

            usVal = batRelativeStateOfCharge();
            sprintf((char *)ucaBuf, "相对剩余：%d", usVal);
            strcat((char *)ucaBuf, "%");
            GUI_DispStringAt((char *)ucaBuf, 0, 120);
            vTaskDelay(pdMS_TO_TICKS(1000));

            usVal = batAbsoluteStateOfCharge();
            sprintf((char *)ucaBuf, "绝对剩余：%d", usVal);
            strcat((char *)ucaBuf, "%");
            GUI_DispStringAt((char *)ucaBuf, 0, 144);
            vTaskDelay(pdMS_TO_TICKS(1000));

            usVal = batRemainingCapacity();
            fTemp = (float)usVal / 1000;
            sprintf((char *)ucaBuf, "剩余电量：%.2f", fTemp);
            strcat((char *)ucaBuf, "Ah");
            GUI_DispStringAt((char *)ucaBuf, 0, 168);
            vTaskDelay(pdMS_TO_TICKS(1000));

            usVal = batFullChargeCapacity();
            fTemp = (float)usVal / 1000;
            sprintf((char *)ucaBuf, "总容量：%.2f", fTemp);
            strcat((char *)ucaBuf, "Ah");
            GUI_DispStringAt((char *)ucaBuf, 0, 192);
            vTaskDelay(pdMS_TO_TICKS(1000));

            usVal = batChargingCurrent();
			fTemp = (float)usVal / 1000;
            sprintf((char *)ucaBuf, "充电电流：%.2f", fTemp);
			strcat((char *)ucaBuf, "A");
            GUI_DispStringAt((char *)ucaBuf, 0, 216);
            vTaskDelay(pdMS_TO_TICKS(1000));

            usVal = batChargingVoltage();
			fTemp = (float)usVal / 1000;
            sprintf((char *)ucaBuf, "充电电压：%.2f", fTemp);
			strcat((char *)ucaBuf, "V");
            GUI_DispStringAt((char *)ucaBuf, 0, 240);
            vTaskDelay(pdMS_TO_TICKS(1000));
   
            usVal = batSerialNumber();
            sprintf((char *)ucaBuf, "序列号：%#X", usVal);
            GUI_DispStringAt((char *)ucaBuf, 0, 264);
            vTaskDelay(pdMS_TO_TICKS(10000));  
//        }        
//    }


}

