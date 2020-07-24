 /*
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

#include "include.h"


float g_fImax = 0;
float g_fImin = 0;
float g_faCurrent[SAVE_LEN] = {0};

IntData_U uIntData;

/*******************************************************
  * @brief  :
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @return :
*******************************************************/
void vGpsMsgReceiveCallback(TimerHandle_t tTimer)
{
    GPIO_PinState eIrqPinSta;
    uint8_t ucaGpsFifo[GPS_FIFO_LEN] = {0};
    uint32_t ulLen = 0;
    uint8_t ucSta;
    uint8_t ucLatitudeByte1, ucLatitudeByte2, ucLatitudeByte3, ucLatitudeByte4;
    uint32_t ulTempLatitude;
    uint8_t ucLongitudeByte1, ucLongitudeByte2, ucLongitudeByte3, ucLongitudeByte4, ucLongitudeByte5;
    uint32_t ulTempLongitude;

    xSemaphoreTake(xGpsMsgReadableSemaphore_Handle, portMAX_DELAY);//HAL_GPIO_EXTI_Callback里pps中断释放

    //eIrqPinSta = HAL_GPIO_ReadPin(FPGA_IRQ_GPIO_PORT, FPGA_IRQ_PIN);
    //if(GPIO_PIN_SET == eIrqPinSta)//
    {
        for(uint8_t ucCount = 0; ucCount < 5; ucCount++)//总共20个数据
        {
            ucSta = fpgaReadByte(FPGA_INT_STATUS_REG) & GPS_MSG_READABLE;
            if(ucSta == GPS_MSG_READABLE)//GPS信息准备就绪 0x03[1] == 1
            {
                for(uint8_t i = 0; i < 4; i++)//每4个FPGA会产生一次中断有效标志
                {
                    ucaGpsFifo[ulLen] = fpgaReadByte(FPGA_GPS_FIFO_REG);
                    ++ulLen;
                }
            }
        }

        if(ucaGpsFifo[0] == 'A')//GPS信号有效
        {
            ptGPSMsg->ucGPSStatus = ucaGpsFifo[0];

            ptGPSMsg->tUtc.ucHour = ucaGpsFifo[1];    //时
            ptGPSMsg->tUtc.ucHour = (ptGPSMsg->tUtc.ucHour >> 4) * 10 + (ptGPSMsg->tUtc.ucHour & 0x0F) + 8;

            ptGPSMsg->tUtc.ucMinute = ucaGpsFifo[2];    //分
            ptGPSMsg->tUtc.ucMinute = (ptGPSMsg->tUtc.ucMinute >> 4) * 10 + (ptGPSMsg->tUtc.ucMinute & 0x0F);

            ptGPSMsg->tUtc.ucSecond = ucaGpsFifo[3];    //秒
            ptGPSMsg->tUtc.ucSecond = (ptGPSMsg->tUtc.ucSecond >> 4) * 10 + (ptGPSMsg->tUtc.ucSecond & 0x0F);

            ptGPSMsg->tUtc.ucYear = ucaGpsFifo[4];    //年
            ptGPSMsg->tUtc.ucYear = (ptGPSMsg->tUtc.ucYear >> 4) * 10 + (ptGPSMsg->tUtc.ucYear & 0x0F);

            ptGPSMsg->tUtc.ucMonth = ucaGpsFifo[5];    //月
            ptGPSMsg->tUtc.ucMonth = (ptGPSMsg->tUtc.ucMonth >> 4) * 10 + (ptGPSMsg->tUtc.ucMonth & 0x0F);

            ptGPSMsg->tUtc.ucDate = ucaGpsFifo[6];    //日
            ptGPSMsg->tUtc.ucDate = (ptGPSMsg->tUtc.ucDate >> 4) * 10 + (ptGPSMsg->tUtc.ucDate & 0x0F);

            ptGPSMsg->ucSorN_Latitude = ucaGpsFifo[7];    //北纬/南纬,N:北纬;S:南纬

            ucLatitudeByte1 = ucaGpsFifo[8];
            ucLatitudeByte1 = (ucLatitudeByte1 >> 4) * 10 + (ucLatitudeByte1 & 0x0F);
            ucLatitudeByte2 = ucaGpsFifo[9];
            ucLatitudeByte2 = (ucLatitudeByte2 >> 4) * 10 + (ucLatitudeByte2 & 0x0F);
            ucLatitudeByte3 = ucaGpsFifo[10];
            ucLatitudeByte3 = (ucLatitudeByte3 >> 4) * 10 + (ucLatitudeByte3 & 0x0F);
            ucLatitudeByte4 = ucaGpsFifo[11];
            ucLatitudeByte4 = (ucLatitudeByte4 >> 4) * 10 + (ucLatitudeByte4 & 0x0F);

            ulTempLatitude = ucLatitudeByte1 * 1000000;
            ulTempLatitude = ulTempLatitude + ucLatitudeByte2 * 10000;
            ulTempLatitude = ulTempLatitude + ucLatitudeByte3 * 100;
            ulTempLatitude = ulTempLatitude + ucLatitudeByte4;
            ptGPSMsg->ulLatitude = ulTempLatitude;//纬度，这里扩大为ddmmmmmm，实际是ddmm.mmmm

            /*float fDd = (float)ucLatitudeByte1;
            float fMm = (float)ucLatitudeByte2;
            fMm = fMm / 60;
            fDd = fDd + fMm;*/

            ptGPSMsg->ucEorW_Longitude = ucaGpsFifo[12];    //东经/西经,E:东经;W:西经

            ucLongitudeByte1 = ucaGpsFifo[13];
            ucLongitudeByte1 = (ucLongitudeByte1 >> 4) * 10 + (ucLongitudeByte1 & 0x0F);
            ucLongitudeByte2 = ucaGpsFifo[14];
            ucLongitudeByte2 = (ucLongitudeByte2 >> 4) * 10 + (ucLongitudeByte2 & 0x0F);
            ucLongitudeByte3 = ucaGpsFifo[15];
            ucLongitudeByte3 = (ucLongitudeByte3 >> 4) * 10 + (ucLongitudeByte3 & 0x0F);
            ucLongitudeByte4 = ucaGpsFifo[16];
            ucLongitudeByte4 = (ucLongitudeByte4 >> 4) * 10 + (ucLongitudeByte4 & 0x0F);
            ucLongitudeByte5 = ucaGpsFifo[17];
            ucLongitudeByte5 = (ucLongitudeByte5 >> 4) * 10 + (ucLongitudeByte5 & 0x0F);

            ulTempLongitude = ucLongitudeByte1 * 100000000;
            ulTempLongitude = ulTempLongitude + ucLongitudeByte2 * 1000000;
            ulTempLongitude = ulTempLongitude + ucLongitudeByte3 * 10000;
            ulTempLongitude = ulTempLongitude + ucLongitudeByte4 * 100;
            ulTempLongitude = ulTempLongitude + ucLongitudeByte5;
            ptGPSMsg->ulLongitude = ulTempLongitude;//经度，这里扩大为dddmmmmmm，实际是dddmm.mmmm

            g_ucDeviceStatusFourthByte |= GPS_VALID;

            if((g_ucDeviceStatusThirdByte & WAIT_START_KEY) != WAIT_START_KEY)//“启动”键按下
            {
                if(g_ucSyncStart == 0)//同步启动标志
                {
                    xSemaphoreGive(xSyncStartSemaphore_Handle);//vSyncStartTask里获取
                    if(g_ucWorkMode == TDIP_MODE)
                    {
                        g_ucSyncStart = 1;
                    }
                }

                if(((g_ucWorkMode == FDIP_SINGLE_FREQUENCY) || (g_ucWorkMode == FDIP_DOUBLE_FREQUENCY)) && (g_ucWaveStart == 0))//波形开始
                {
                    xSemaphoreGive(xWaveStartSemaphore_Handle);//vFdipStartTask里获取
                    g_ucWaveStart = 1;
                }
                if(((g_ucWorkMode == FDIP_SINGLE_FREQUENCY) || (g_ucWorkMode == FDIP_DOUBLE_FREQUENCY)) && (g_ucFrequencyGroupCycleFlag == 0))
                {
                    xSemaphoreGive(xFrequencyGroupCycleSemaphore_Handle);//vFdipStartTask里获取 
                    g_ucFrequencyGroupCycleFlag = 1;
                }
            }

            if(!g_ucGpsTiming)
            {
                g_ucGpsTiming = 1;
                rtcSetDate(ptGPSMsg->tUtc.ucYear, ptGPSMsg->tUtc.ucMonth, ptGPSMsg->tUtc.ucDate); //写入RTC
                rtcSetTime(ptGPSMsg->tUtc.ucHour, ptGPSMsg->tUtc.ucMinute, ptGPSMsg->tUtc.ucSecond);
            }

        }
        else
        {
            g_ucDeviceStatusFourthByte &= (~GPS_VALID);
            memset(ptGPSMsg, 0, sizeof(*ptGPSMsg));
        }
    }
}

/*******************************************************
  * @brief  :同步启动任务
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @return :
*******************************************************/
//void vSyncStartTask(void *pvParameters)
void vSyncStart(void)
{
    g_ucSyncStart = 0;//同步启动标志    

    xSemaphoreTake(xSyncStartSemaphore_Handle, portMAX_DELAY);//vGpsMsgReceiveCallback释放

    g_ucWaveStart = 0;//下一个PPS来时波形正式开始

    g_ucDeviceStatusThirdByte &= (~WAIT_SYNC_START_TRIGGER);

    fpgaWriteByte(FPGA_SYS_RST_REG, SYS_RST | PROTECT_RST);             //系统复位,保护复位  
    Delay_ms(1);
    fpgaWriteByte(FPGA_SYS_RST_REG, SYS_SET | PROTECT_SET);             //系统置位,保护置位
    Delay_ms(1);
    fpgaWriteByte(FPGA_SEND_CTRL_REG, SEND_START_CTRL);    //发射开始
    fpgaWriteByte(FPGA_SAMPLE_CMD_REG, SAMPLE_ENABLE);     //采样使能

    if(g_ucFirstSyncStart == 1)
    {
        tStartTime.ucHours = ptGPSMsg->tUtc.ucHour;
        tStartTime.ucMinutes = ptGPSMsg->tUtc.ucMinute;
        tStartTime.ucSeconds = ptGPSMsg->tUtc.ucSecond + 1;//下一秒就是真正的启动时刻 

        g_ucFirstSyncStart = 0;

        xTaskCreate((TaskFunction_t)vNewFileTask,
                    (const char *)"NewFileTask",
                    (uint16_t)512,
                    (void *)NULL,
                    NEW_FILE_TASK_PRIO,
                    (TaskHandle_t *)&xNewFileTask_Handle);

        //数据接收任务
        xTaskCreate((TaskFunction_t)vAdDataReceiveTask,
                    (const char *)"AdDataReceiveTask",
                    (uint16_t)1024,
                    (void *)NULL,
                    AD_DATA_RECEIVE_TASK_PRIO,
                    (TaskHandle_t *)&xAdDataReceiveTask_Handle);

        if(g_ucSaveMode == CYCLIC_STORAGE)//周期存储
        {
            xTaskCreate((TaskFunction_t)vSaveDataTask,
                        (const char *)"SaveDataTask",
                        (uint16_t)1024,
                        (void *)NULL,
                        SAVE_DATE_TASK_PRIO,
                        (TaskHandle_t *)&xSaveDateTask_Handle);
        }

        //xTaskNotifyGive(xNewFileTask_Handle);//发送任务通知，vNewFileTask接收

        /*if((g_ucWorkMode == FDIP_SINGLE_FREQUENCY) || (g_ucWorkMode == FDIP_DOUBLE_FREQUENCY))
        {
            xTaskCreate((TaskFunction_t)vReadSendStopFlagTask,
                        (const char *)"ReadSendStopFlagTask",
                        (uint16_t)256,
                        (void *)NULL,
                        READ_SEND_STOP_FLAG_TASK_PRIO,
                        (TaskHandle_t *)&xReadSendStopFlagTask_Handle);
        }*/
    }
    SEND_LED_ON(SEND_LED);
    //vTaskDelete(xSyncStartTask_Handle); //删除本任务
    //vTaskDelay(pdMS_TO_TICKS(5));
}

/*******************************************************
  * @brief  :同步启动
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @return :
*******************************************************/
uint8_t SyncStart(uint8_t ucWorkMode)
{
    //同步启动任务
    vSyncStart();
    /*xTaskCreate((TaskFunction_t)vSyncStartTask,
        (const char *)"SyncStartTask",
                (uint16_t)2048,
                (void *)NULL,
                SYNC_START_TASK_PRIO,
                (TaskHandle_t *)&xSyncStartTask_Handle);*/
    return TRUE;
}

/**
 * @brief :
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
void vReadSendStopFlagTask(void *pvParameters)
{
    uint8_t ucSta;

    //(void)pvParameters;

    for(;;)
    {
        ucSta = fpgaReadByte(SEND_STOP_FLAG);

        if((ucSta & END_OF_SEND_PERIOD) == END_OF_SEND_PERIOD) //发射周期结束
        {
            if(g_ucSendStopFlag == 0)
            {
                g_ucSendStopFlag = 1;
                xSemaphoreGive(xSendStopFlagSemaphore_Handle);  //vFdipStartTask接收
            }
        }

        if((ucSta & END_OF_STOP_KEY) == END_OF_STOP_KEY) //停止键停止
        {
            ucSta = fpgaReadByte(SEND_STOP_FLAG);
        }

        if((ucSta & END_OF_CMD) == END_OF_CMD) //
        {
            ucSta = fpgaReadByte(SEND_STOP_FLAG);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/*******************************************************
  * @brief  :同步停止
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @return :
*******************************************************/
void SyncStop(void)
{
    fpgaWriteByte(FPGA_SEND_CTRL_REG, SEND_STOP_CTRL);     //发射停止
    fpgaWriteByte(FPGA_SAMPLE_CMD_REG, SAMPLE_DISABLE);    //采样禁止

    if(((g_ucWorkMode == FDIP_SINGLE_FREQUENCY) || (g_ucWorkMode == FDIP_DOUBLE_FREQUENCY)) && (xSyncStartSemaphore_Handle != NULL))//如果信号量没有被接收，则接收一次，防止停止再启动出问题
    {
        xSemaphoreTake(xSyncStartSemaphore_Handle, (TickType_t)10);
    }
}

/**
 * @brief :
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
void statusReset(void)
{
    g_ucDeviceStatusFirstByte = 0;    //设备状态第一字节
    g_ucDeviceStatusSecondByte = 0;   //设备状态第二字节
    g_ucDeviceStatusThirdByte = 0;    //设备状态第三字节
    g_ucDeviceStatusFourthByte = 0;   //设备状态第四字节
    g_ucWorkStatus = 0x00;            //工作状态 0x00:待机，0x01: 发射准备状态，0x02:正供，0x03:正停供，0x04:负供，0x05:负停供
    g_ucFrequencyGroupIndex = 0;      //频组序号
    g_ucFirstSyncStart = 1;           //第一次同步启动
    g_ucFirstStart = 1;               //第一次启动
    g_ucSyncStart = 1;                //同步启动标志
    g_ucFrequencyGroupCycleFlag = 1;  //频组间隔标志
    g_ucSendToClientFlag = 1;         //发送给客户端标志，为1时表示已经把启动键按下的状态发送给客户端了
    g_ucSendStopFlag = 0;
    g_ucWaveStart = 1;
    g_ucGpsTiming = 0;                //GPS授时标志
}

/**
 * @brief :
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
void vStopSendTask(void *pvParameters)
//void vStopSend(void)
{
    //(void)pvParameters;

    statusReset();

    SyncStop();

    fpgaWriteByte(FPGA_INT_CTRL_REG, AD_INT_DISABLE);    //AD中断禁止

    vTaskDelete(xAdDataReceiveTask_Handle); //删除vAdDataReceiveTask

    if(g_ucSaveMode == CYCLIC_STORAGE)//周期存储
    {
        vTaskDelete(xSaveDateTask_Handle);      //删除vSaveDataTask      
    }

    if((g_ucWorkMode == FDIP_SINGLE_FREQUENCY) || (g_ucWorkMode == FDIP_DOUBLE_FREQUENCY))
    {
        //vTaskDelete(xReadSendStopFlagTask_Handle); //删除vReadSendStopFlagTask       
        vTaskDelete(xFdipStartTask_Handle);        //删除vFdipStartTask 
    }

    f_close(&tWorkFile);

    xSemaphoreGive(xFatfsMscSemaphore_Handle); //释放信号量，在OTG_FS_IRQHandler获取
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);

    clearIntStatusReg();    //清中断状态寄存器

    SEND_LED_OFF(SEND_LED);
    OVERCURRENT_LED_OFF(OVERCURRENT_LED);
    FAULT_LED_OFF(FAULT_LED);

    vTaskDelete(xStopSendTask_Handle); //删除本任务
    vTaskDelay(pdMS_TO_TICKS(20));
}

void StopSend(void)
{
    //vStopSend();
    xTaskCreate((TaskFunction_t)vStopSendTask,
                (const char *)"StopTask",
                (uint16_t)2048,
                (void *)NULL,
                STOP_SEND_TASK_PRIO,
                (TaskHandle_t *)&xStopSendTask_Handle);
}

/*******************************************************
  * @brief  :新建文件任务
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @return :
*******************************************************/
void vNewFileTask(void *pvParameters)
{
    FRESULT eResult;
    char caFoldPath[48] = {0};
    char caBuf[48] = {0};
    uint32_t ulBytesWritten;
    uint8_t ucTmp, ucTempCount;
    uint8_t ucWriteNum = 0;

    //(void)pvParameters;
    //(void)pvParameters;

    //ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //等待任务通知,vSyncStartTask释放

    xSemaphoreTake(xFatfsMscSemaphore_Handle, portMAX_DELAY); //获取信号量，在OTG_FS_IRQHandler释放
    HAL_NVIC_DisableIRQ(OTG_FS_IRQn);

    //tSDInfo = sdGetSDInfo();

    if(g_ucWorkMode == TDIP_MODE)//TDIP_MODE
    {
        sprintf(caBuf, "TDIP%02d%02d%02d.dat", ptGPSMsg->tUtc.ucYear, ptGPSMsg->tUtc.ucMonth, ptGPSMsg->tUtc.ucDate);

        tWriteToSD.ucType = g_ucWorkMode;     //写入的类型，0x00:TDIP发射,0x01:FDIP单频,0x02:FDIP双频
        strcpy(tWriteToSD.caFileName, caBuf);   //文件名     

        for(int i = 0; i < 4; i++)//以4个0xA5作为每一段的开始
        {
            tWriteToSD.caContent[i] = 0xA5;
        }

        tWriteToSD.caContent[4]  = 'T';    //TDIP_MODE-DDF
        tWriteToSD.caContent[5]  = 'D';
        tWriteToSD.caContent[6]  = 'I';
        tWriteToSD.caContent[7]  = 'P';
        tWriteToSD.caContent[8]  = '-';
        tWriteToSD.caContent[9]  = 'D';    
        tWriteToSD.caContent[10] = 'D';
        tWriteToSD.caContent[11] = 'F';
        
        tWriteToSD.caContent[12] = g_ucSaveMode;
        tWriteToSD.caContent[13] = g_ucPulseWidth;
        tWriteToSD.caContent[14] = g_ucDutyRatio;
        tWriteToSD.caContent[15] = tStartTime.ucHours;
        tWriteToSD.caContent[16] = tStartTime.ucMinutes;
        tWriteToSD.caContent[17] = tStartTime.ucSeconds;
        //tWriteToSD.caContent[18] = 0;
        ucWriteNum = 18;

        strcpy(caFoldPath, g_caSDPath);     //SD盘符 0:/
        strcat(caFoldPath, g_caTDIPFolder); //文件夹 TDIP_MODE
        strcat(caFoldPath, "/");
        strcat(caFoldPath, tWriteToSD.caFileName);          //文件名 
    }
    else if((g_ucWorkMode == FDIP_SINGLE_FREQUENCY) || (g_ucWorkMode == FDIP_DOUBLE_FREQUENCY))//FDIP
    {
        sprintf(caBuf, "FDIP%02d%02d%02d.dat", ptGPSMsg->tUtc.ucYear, ptGPSMsg->tUtc.ucMonth, ptGPSMsg->tUtc.ucDate);

        tWriteToSD.ucType = g_ucWorkMode;                  //0x00:TDIP发射,0x01:FDIP单频,0x02:FDIP双频
        strcpy(tWriteToSD.caFileName, caBuf);   //文件名

        //存储参数：频组数 + 频比 + 发射周期数(两个字节) + 频组发射间隔 + 循环次数 + 主频1 + 主频2 + ...主频N1
        for(int i = 0; i < 4; i++)//以4个0xA5作为每一段的开始
        {
            tWriteToSD.caContent[i] = 0xA5;
        }

        tWriteToSD.caContent[4]  = 'F';    //FDIP2F-DDF
        tWriteToSD.caContent[5]  = 'D';
        tWriteToSD.caContent[6]  = 'I';
        tWriteToSD.caContent[7]  = 'P';
        tWriteToSD.caContent[8]  = '2';
        tWriteToSD.caContent[9]  = 'F';
        tWriteToSD.caContent[10] = '-';
        tWriteToSD.caContent[11] = 'D';
        tWriteToSD.caContent[12] = 'D';
        tWriteToSD.caContent[13] = 'F';

        tWriteToSD.caContent[14] = g_ucSaveMode;            //存储方式
        tWriteToSD.caContent[15] = g_ucFrequencyGroup;      //频组数
        tWriteToSD.caContent[16] = g_ucFrequencyRatio;      //频比
        tWriteToSD.caContent[17] = g_usSendCycleNumber >> 8;//发射周期数
        tWriteToSD.caContent[18] = g_usSendCycleNumber;
        tWriteToSD.caContent[19] = g_ucFrequencyGroupCycle; //频组发射间隔
        tWriteToSD.caContent[20] = g_ucNumberOfCycles;     //循环次数

        ucTmp = 21;
        for(ucTempCount = 0; ucTempCount < g_ucFrequencyGroup; ucTempCount++)//主频
        {
            tWriteToSD.caContent[ucTmp + ucTempCount] = g_usaMainFrequency[ucTempCount] >> 8;
            tWriteToSD.caContent[ucTmp + 1 + ucTempCount] = g_usaMainFrequency[ucTempCount];
            ucTmp++;
        }

        tWriteToSD.caContent[ucTmp + 1 + ucTempCount] = tStartTime.ucHours;        //启动时间,时分秒
        tWriteToSD.caContent[ucTmp + 2 + ucTempCount] = tStartTime.ucMinutes;
        tWriteToSD.caContent[ucTmp + 3 + ucTempCount] = tStartTime.ucSeconds;
        //tWriteToSD.caContent[ucTmp + 4 + ucTempCount] = 0;

        ucWriteNum = ucTmp + 4 + ucTempCount;

        strcpy(caFoldPath, g_caSDPath);     //SD盘符 0:/
        strcat(caFoldPath, g_caFDIPFolder); //文件夹 TDIP_MODE
        strcat(caFoldPath, "/");
        strcat(caFoldPath, tWriteToSD.caFileName);          //文件名 
    }

    eResult = f_open(&tWorkFile, caFoldPath, FA_OPEN_APPEND | FA_WRITE); //创建一个新文件，如果文件已存在，打开文件追加
    if(eResult == FR_OK)
    {
        eResult = f_write(&tWorkFile, tWriteToSD.caContent, ucWriteNum, (uint32_t *)&ulBytesWritten);
        f_sync(&tWorkFile);
    }
    else
    {
        f_close(&tWorkFile);
        SDError();
        //Error_Handler();
    }

    vTaskDelete(xNewFileTask_Handle); //删除本任务  
    vTaskDelay(pdMS_TO_TICKS(1));
}

/*******************************************************
  * @brief  :数据接收任务
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @return :
*******************************************************/
void vAdDataReceiveTask(void *pvParameters)
{
    uint32_t ulCurrentCount = 0;
    uint32_t ulAdFifoCount = 0;
    uint32_t ulLen = 0;
    GPIO_PinState eIrqPinSta;
    uint8_t ucSta;
    static uint16_t s_usCount = 0;
    uint8_t ucaAdFifo[AD_FIFO_LEN] = {0};
    int32_t iaCurrent[AD_FIFO_LEN / 3] = {0};
    uint32_t ulTCount = 0;
    float fAverage = 0;

    //(void)pvParameters;

    for(;;)
    {
        //EventStartA(0);// 

        //eIrqPinSta = HAL_GPIO_ReadPin(FPGA_IRQ_GPIO_PORT, FPGA_IRQ_PIN);
        //if(GPIO_PIN_SET == eIrqPinSta)//
        {
            /*ucSta = fpgaReadByte(FPGA_INT_STATUS_REG);
            ucSta = ucSta & AD_DATA_READABLE;
            if(ucSta == AD_DATA_READABLE)//AD数据准备就绪*/
            if((fpgaReadByte(FPGA_INT_STATUS_REG) & AD_DATA_READABLE) == AD_DATA_READABLE)//AD数据准备就绪
            {
                for(ulLen = 0; ulLen < AD_FIFO_LEN; ulLen++)//一次性读取AD_FIFO_LEN(96)个字节
                {
                    ucaAdFifo[ulLen] = fpgaReadByte(FPGA_AD_FIFO_REG);
                }

                ulAdFifoCount = 0;

                //AD原始数据转换为电流数据
                for(ulCurrentCount = 0; ulCurrentCount < (AD_FIFO_LEN / 3); ulCurrentCount++)
                {
                    //if(ulAdFifoCount < AD_FIFO_LEN)
                    {
                        uIntData.tBytes.ucHighByte = ucaAdFifo[ulAdFifoCount];
                        uIntData.tBytes.ucMidByte = ucaAdFifo[ulAdFifoCount + 1];
                        uIntData.tBytes.ucLowByte = ucaAdFifo[ulAdFifoCount + 2];

                        if((ucaAdFifo[ulAdFifoCount] & 0x80) == 0x80)
                        {
                            uIntData.tBytes.ucSignByte = 0xFF;
                        }
                        else
                        {
                            uIntData.tBytes.ucSignByte = 0x00;
                        }
                        ulAdFifoCount = ulAdFifoCount + 3;
                    }

                    if(g_ucSaveMode == FULL_WAVEFORM_STORAGE)//全波形存储
                    {
                        g_faCurrent[s_usCount] = dataProcessForFullWave(uIntData.iResult);
                        s_usCount++;

                        if(s_usCount % (AD_FIFO_LEN / 3) == 0)//32个数提取一个幅值
                        {
                            fAverage = 0;
                            int i = s_usCount - (AD_FIFO_LEN / 3);
                            for(int j = 0; j < (AD_FIFO_LEN / 3); j++)
                            {
                                fAverage = fAverage + g_faCurrent[i + j];
                            }
                            fAverage = fAverage / (AD_FIFO_LEN / 3);
                            if(fAverage > g_fImax)
                            {
                                g_fImax = fAverage;
                            }
                            if(fAverage < g_fImin)
                            {
                                g_fImin = fAverage;
                            }

                            //App_Printf("g_fImax = %fmA\r\n", g_fImax);
                            //App_Printf("g_fImin = %fmA\r\n", g_fImin);
                        }

                        if(s_usCount == SAVE_LEN)//SAVE_LEN个数存一次
                        {
                            if(g_ucSaveMode != 0xFF)//
                            {
                                dataSave(g_faCurrent, SAVE_LEN);
                            }
                            s_usCount = 0;
                        }
                    }
                    /*else if(g_ucSaveMode == CYCLIC_STORAGE)//周期存储
                    {
                        iaCurrent[ulCurrentCount] = uIntData.iResult;
                    }*/
                } //end of for(ulCurrentCount = 0; ulCurrentCount < (AD_FIFO_LEN / 3); ulCurrentCount++)

                /*if(g_ucSaveMode == CYCLIC_STORAGE)//周期存储
                {
                    dataProcessForCycle(iaCurrent);
                    if(g_ucSaveMode != 0xFF)//
                    {
                        xTaskNotifyGive(xSaveDateTask_Handle);//发送任务通知，vSaveDataTask接收
                    }
                }*/
            } // if(ucSta == AD_DATA_READABLE)//AD数据准备就绪
        }

        vTaskDelay(pdMS_TO_TICKS(15));
        ulTCount++;

        if(g_ucWorkMode == TDIP_MODE)
        {
            if(ulTCount * 15 >= (g_ucPulseWidth * 4) * 1000)//一个周期取一个平均
            {
                //g_fCurrent = (g_fImax - g_fImin) / 2;//电流都是正的，不能用正负平均
                g_fCurrent = g_fImax;//
                ulTCount = 0;
                g_fImax = 0;
                g_fImin = 0;
                App_Printf("g_fCurrent = %fmA\r\n", g_fCurrent);
            }
        }
        if((g_ucWorkMode == FDIP_SINGLE_FREQUENCY) || (g_ucWorkMode == FDIP_DOUBLE_FREQUENCY))//FDIP
        {
            if(ulTCount * 15 >= g_fFrequencyGroupTime * 1000)//一个频组取一个平均
            {
                //g_fCurrent = (g_fImax - g_fImin) / 2;//电流都是正的，不能用正负平均
                g_fCurrent = g_fImax;//
                ulTCount = 0;
                g_fImax = 0;
                g_fImin = 0;
                App_Printf("g_fCurrent = %fmA\r\n", g_fCurrent);
            }
        }


       // EventStopA(0);
    }
}

/*******************************************************
  * @brief  :数据处理
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @return :
*******************************************************/
float dataProcessForFullWave(int32_t iOriginData)
{
    float fTemp = 0;

    fTemp = iOriginData * KAD;//得到电流值
    //fTemp = fTemp / 2;//默认增益2倍
    fTemp = fTemp * 5;

    #ifdef DEBUG_TARGET
    //SEGGER_RTT_SetTerminal(2);
    //SEGGER_RTT_printf(0, "I = %d\r\n", fTemp);//不支持浮点数
    //App_Printf("I = %fmA\r\n", fTemp);
    #endif

    return fTemp;
}

/**
 * @brief :
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
void dataSave(float *pfSrc, uint16_t usLen)
{
    union
    {
        float fValue;
        char caChar[4];
    } FtoC_U;

    __attribute__((aligned(4))) char caBuf[1024] = {0};//256*4
    uint8_t ucTemp = 0;;

    for(int i = 0; i < usLen; i++)
    {
        FtoC_U.fValue = pfSrc[i];
        caBuf[i + ucTemp] = FtoC_U.caChar[0];
        caBuf[i + 1 + ucTemp] = FtoC_U.caChar[1];
        caBuf[i + 2 + ucTemp] = FtoC_U.caChar[2];
        caBuf[i + 3 + ucTemp] = FtoC_U.caChar[3];
        ucTemp = ucTemp + 3;
    }

    //strcpy(tWriteToSD.caContent, caBuf);    //内容
    xQueueSend(xWriteToSDQueue_Handle, caBuf, 500); //vWriteToSDTask里获取    
}

/*******************************************************
  * @brief  :数据处理
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @return :
*******************************************************/
void dataProcessForCycle(int32_t *piOriginData)
{
    int32_t iaDest[AD_FIFO_LEN / 3] = {0};
    int32_t iSum = 0;
    float fIvalue = 0;

    dataBubbleSortFilter(piOriginData, iaDest, AD_FIFO_LEN / 3, 10, 10); //排序，并丢弃最大和最小的10个数据

    for(int i = 0; i < (AD_FIFO_LEN / 3 - 20); i++)
    {
        iSum = iSum + iaDest[i];
    }
    fIvalue = (float)iSum / (AD_FIFO_LEN / 3 - 20);//取平均
    fIvalue = fIvalue * KAD;//得到电流值
    fIvalue = fIvalue * 5;
    //fIvalue = fIvalue / 2;//默认增益2倍
    g_fCurrent = fIvalue;

    #ifdef DEBUG_TARGET  
    App_Printf("g_fCurrent = %f\r\n", fIvalue);
    #endif
/*
    if(fIvalue > g_fImax)
    {
        g_fImax = fIvalue;
    }
    if(fIvalue < g_fImin)
    {
        g_fImin = fIvalue;
    }
    g_fCurrent = (float)(g_fImax - g_fImin) / 2;*/

}

/*******************************************************
  * @brief  :存储数据任务
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @return :
*******************************************************/
void vSaveDataTask(void *pvParameters)
{
    union
    {
        float fValue;
        char caChar[4];
    } FtoC_U;

    //(void)pvParameters;

    for(;;)
    {
        char caBuf[48] = {0};

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //等待任务通知,vAdDataReceiveTask释放

        if(g_ucWorkMode == TDIP_MODE)
        {
            /*//存储格式：电流+时间(时间格式:19/07/05 12:00:00)
            sprintf(caBuf, "%.3f %d/%d/%d %d:%d:%d\r\n", g_fCurrent, ptGPSMsg->tUtc.ucYear, ptGPSMsg->tUtc.ucMonth, ptGPSMsg->tUtc.ucDate, ptGPSMsg->tUtc.ucHour, ptGPSMsg->tUtc.ucMinute, ptGPSMsg->tUtc.ucSecond);
            strcpy(tWriteToSD.caContent, caBuf);    //内容*/
            FtoC_U.fValue = g_fCurrent;
            caBuf[0] = ptGPSMsg->tUtc.ucYear;
            caBuf[1] = ptGPSMsg->tUtc.ucMonth;
            caBuf[2] = ptGPSMsg->tUtc.ucDate;
            caBuf[3] = ptGPSMsg->tUtc.ucHour;
            caBuf[4] = ptGPSMsg->tUtc.ucMinute;
            caBuf[5] = ptGPSMsg->tUtc.ucSecond;
            caBuf[6] = FtoC_U.caChar[0];
            caBuf[7] = FtoC_U.caChar[1];
            caBuf[8] = FtoC_U.caChar[2];
            caBuf[9] = FtoC_U.caChar[3];

            xQueueSend(xWriteToSDQueue_Handle, caBuf, 300); //vWriteToSDTask里获取

            vTaskDelay(pdMS_TO_TICKS(g_ucPulseWidth * 1000));
        }
        else if((g_ucWorkMode == FDIP_SINGLE_FREQUENCY) || (g_ucWorkMode == FDIP_DOUBLE_FREQUENCY))
        {
            xSemaphoreTake(xFdipSaveSemaphore_Handle, portMAX_DELAY);

           /* //存储格式：频组序号+电流+时间(时间格式:19/07/05 12:00:00)
            sprintf(caBuf, "%d %.3f %d/%d/%d %d:%d:%d\r\n", g_ucFrequencyGroupIndex, g_fCurrent, ptGPSMsg->tUtc.ucYear, ptGPSMsg->tUtc.ucMonth, ptGPSMsg->tUtc.ucDate, ptGPSMsg->tUtc.ucHour, ptGPSMsg->tUtc.ucMinute, ptGPSMsg->tUtc.ucSecond);
            strcpy(tWriteToSD.caContent, caBuf);    //内容*/

            FtoC_U.fValue = g_fCurrent;

            caBuf[0] = ptGPSMsg->tUtc.ucYear;
            caBuf[1] = ptGPSMsg->tUtc.ucMonth;
            caBuf[2] = ptGPSMsg->tUtc.ucDate;
            caBuf[3] = ptGPSMsg->tUtc.ucHour;
            caBuf[4] = ptGPSMsg->tUtc.ucMinute;
            caBuf[5] = ptGPSMsg->tUtc.ucSecond;
            caBuf[6] = g_ucFrequencyGroupIndex;//
            caBuf[7] = FtoC_U.caChar[0];
            caBuf[8] = FtoC_U.caChar[1];
            caBuf[9] = FtoC_U.caChar[2];
            caBuf[10] = FtoC_U.caChar[3];

            xQueueSend(xWriteToSDQueue_Handle, tWriteToSD.caContent, 300); //vWriteToSDTask里获取

            //vTaskDelay(pdMS_TO_TICKS((1 / g_fLowFrequency) * 1000));
        }
    }
}

/**
 * @brief :
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
float readVoltage(void)
{
    uint8_t ucaReceiveBuf[3] = {0};

    union
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

    //IntData_U.tBytes.ucHighByte = fpgaReadByte(VOLTAGE_VALUE_1ST_BYTE_REG);
    //IntData_U.tBytes.ucMidByte  = fpgaReadByte(VOLTAGE_VALUE_2ND_BYTE_REG);
    //IntData_U.tBytes.ucLowByte  = fpgaReadByte(VOLTAGE_VALUE_3RD_BYTE_REG);

    fpgaReadBuffer(VOLTAGE_VALUE_1ST_BYTE_REG, ucaReceiveBuf, 3);
    IntData_U.tBytes.ucHighByte = ucaReceiveBuf[0];
    IntData_U.tBytes.ucMidByte = ucaReceiveBuf[1];
    IntData_U.tBytes.ucLowByte = ucaReceiveBuf[2];

    if((IntData_U.tBytes.ucHighByte & 0x80) == 0x80)
    {
        IntData_U.tBytes.ucSignByte = 0xFF;
    }
    else
    {
        IntData_U.tBytes.ucSignByte = 0x00;
    }

    g_fVoltage = IntData_U.iResult * KAD;//得到电流值
    //g_fVoltage = g_fVoltage / 2;//默认增益2倍
    g_fVoltage = g_fVoltage * 500;

    #ifdef DEBUG_TARGET
    //App_Printf("U = %fmV\r\n", g_fVoltage);
    #endif

    return g_fVoltage;
}

/**
 * @brief :
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
float readTemperature(void)
{
    union
    {
        int16_t sResult;

        struct
        {
            uint8_t ucLowByte;  //低字节
            uint8_t ucHighByte; //高字节	
            uint8_t ucSignByte; //符号
        }tBytes;
    }sData_U;

    sData_U.tBytes.ucHighByte = fpgaReadByte(FPGA_TEMP_MSB_REG);
    sData_U.tBytes.ucLowByte = fpgaReadByte(FPGA_TEMP_LSB_REG);

    if((sData_U.tBytes.ucHighByte & 0x80) == 0x80)
    {
        sData_U.tBytes.ucSignByte = 0xFF;
    }
    else
    {
        sData_U.tBytes.ucSignByte = 0x00;
    }

    g_fTemperature = sData_U.sResult * 0.0625; //系数0.0625
    g_fTemperature = g_fTemperature / 2;//默认增益2倍
    return g_fTemperature;
}

/*******************************************************
    @brief  :冒泡排序并丢弃前ulDiscardFontNum个和后ulDiscardRearNum个数据
    @note   :从小到大排列
    @param  :*iSrc源数据指针，*iDest目标数据指针，丢弃前discard_font个和后discard_rear个数据后的数据指针
    @param  :total:数据总个数
    @param  :ulDiscardFont：前面要丢弃的数据个数,ulDiscardRear后面要丢弃的数据个数
    @return :
*******************************************************/
uint8_t dataBubbleSortFilter(int32_t *piSrc, int32_t *piDest, uint32_t ulTotal, uint32_t ulDiscardFontNum, uint32_t ulDiscardRearNum)
{
    uint32_t i, j;
    int32_t iTmp;

    if((ulDiscardFontNum + ulDiscardRearNum) > ulTotal)
    {
        return FALSE;
    }

    //冒泡排序，小到大排列
    for(j = ulTotal; j > 0; j--)
    {
        for(i = 0; i < j; i++)
        {
            if(piSrc[j] < piSrc[i])
            {
                iTmp = piSrc[j];
                piSrc[j] = piSrc[i];
                piSrc[i] = iTmp;
            }
        }
    }

    //获得a数组中第ulDiscardFontNum个到第ulTotal - ulDiscardRearNum个数，储存到dest当中
    for(i = ulDiscardFontNum; i < ulTotal - ulDiscardRearNum; i++)
    {
        piDest[i - ulDiscardFontNum] = piSrc[i];
    }

    return TRUE;
}






//----------------------------------------------------------------------------------
