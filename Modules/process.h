/**
  ******************************************************************************
  * @file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DATAPROCESS_H
#define __DATAPROCESS_H

#ifdef __cplusplus
extern "C" {
#endif


    #define AD_FIFO_LEN     96
    #define GPS_FIFO_LEN    20
    #define KAD		        ((float)(((2 * 2500) / (pow(2, 23) - 1))))
    #define SAVE_LEN        256

    typedef union 
    {
        int32_t iResult;

        struct
        {
            uint8_t ucLowByte;  //低字节(小端模式)		
            uint8_t ucMidByte;  //中间字节			
            uint8_t ucHighByte; //高字节	
            uint8_t ucSignByte; //符号
        }tBytes;
    }IntData_U;

    extern float g_fImax;
    extern float g_fImin;
    extern IntData_U uIntData;

  
    void dataProcessForCycle(int32_t *piOriginData);
    float dataProcessForFullWave(int32_t iOriginData);
    uint8_t dataBubbleSortFilter(int32_t *piSrc, int32_t *piDest, uint32_t ulTotal, uint32_t ulDiscardFontNum, uint32_t ulDiscardRearNum);
    uint8_t SyncStart(uint8_t ucWorkMode);
    void SyncStop(void);
    void vNewFileTask(void *pvParameters);
    void vGpsMsgReceiveCallback(TimerHandle_t tTimer);
    //void vAdDataReceiveCallback(TimerHandle_t tTimer); //    
    void dataSave(float *pfSrc, uint16_t usLen);
    void vSaveDataTask(void *pvParameters);
    void vAdDataReceiveTask(void *pvParameters);
    float readVoltage(void);
    float readTemperature(void);
    void StopSend(void);
    void vReadSendStopFlagTask(void *pvParameters);
    void statusReset(void);

#ifdef __cplusplus
}
#endif

#endif /* __DATAPROCESS_H */