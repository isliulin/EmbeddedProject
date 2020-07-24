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

    xSemaphoreTake(xGpsMsgReadableSemaphore_Handle, portMAX_DELAY);//HAL_GPIO_EXTI_Callback��pps�ж��ͷ�

    //eIrqPinSta = HAL_GPIO_ReadPin(FPGA_IRQ_GPIO_PORT, FPGA_IRQ_PIN);
    //if(GPIO_PIN_SET == eIrqPinSta)//
    {
        for(uint8_t ucCount = 0; ucCount < 5; ucCount++)//�ܹ�20������
        {
            ucSta = fpgaReadByte(FPGA_INT_STATUS_REG) & GPS_MSG_READABLE;
            if(ucSta == GPS_MSG_READABLE)//GPS��Ϣ׼������ 0x03[1] == 1
            {
                for(uint8_t i = 0; i < 4; i++)//ÿ4��FPGA�����һ���ж���Ч��־
                {
                    ucaGpsFifo[ulLen] = fpgaReadByte(FPGA_GPS_FIFO_REG);
                    ++ulLen;
                }
            }
        }

        if(ucaGpsFifo[0] == 'A')//GPS�ź���Ч
        {
            ptGPSMsg->ucGPSStatus = ucaGpsFifo[0];

            ptGPSMsg->tUtc.ucHour = ucaGpsFifo[1];    //ʱ
            ptGPSMsg->tUtc.ucHour = (ptGPSMsg->tUtc.ucHour >> 4) * 10 + (ptGPSMsg->tUtc.ucHour & 0x0F) + 8;

            ptGPSMsg->tUtc.ucMinute = ucaGpsFifo[2];    //��
            ptGPSMsg->tUtc.ucMinute = (ptGPSMsg->tUtc.ucMinute >> 4) * 10 + (ptGPSMsg->tUtc.ucMinute & 0x0F);

            ptGPSMsg->tUtc.ucSecond = ucaGpsFifo[3];    //��
            ptGPSMsg->tUtc.ucSecond = (ptGPSMsg->tUtc.ucSecond >> 4) * 10 + (ptGPSMsg->tUtc.ucSecond & 0x0F);

            ptGPSMsg->tUtc.ucYear = ucaGpsFifo[4];    //��
            ptGPSMsg->tUtc.ucYear = (ptGPSMsg->tUtc.ucYear >> 4) * 10 + (ptGPSMsg->tUtc.ucYear & 0x0F);

            ptGPSMsg->tUtc.ucMonth = ucaGpsFifo[5];    //��
            ptGPSMsg->tUtc.ucMonth = (ptGPSMsg->tUtc.ucMonth >> 4) * 10 + (ptGPSMsg->tUtc.ucMonth & 0x0F);

            ptGPSMsg->tUtc.ucDate = ucaGpsFifo[6];    //��
            ptGPSMsg->tUtc.ucDate = (ptGPSMsg->tUtc.ucDate >> 4) * 10 + (ptGPSMsg->tUtc.ucDate & 0x0F);

            ptGPSMsg->ucSorN_Latitude = ucaGpsFifo[7];    //��γ/��γ,N:��γ;S:��γ

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
            ptGPSMsg->ulLatitude = ulTempLatitude;//γ�ȣ���������Ϊddmmmmmm��ʵ����ddmm.mmmm

            /*float fDd = (float)ucLatitudeByte1;
            float fMm = (float)ucLatitudeByte2;
            fMm = fMm / 60;
            fDd = fDd + fMm;*/

            ptGPSMsg->ucEorW_Longitude = ucaGpsFifo[12];    //����/����,E:����;W:����

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
            ptGPSMsg->ulLongitude = ulTempLongitude;//���ȣ���������Ϊdddmmmmmm��ʵ����dddmm.mmmm

            g_ucDeviceStatusFourthByte |= GPS_VALID;

            if((g_ucDeviceStatusThirdByte & WAIT_START_KEY) != WAIT_START_KEY)//��������������
            {
                if(g_ucSyncStart == 0)//ͬ��������־
                {
                    xSemaphoreGive(xSyncStartSemaphore_Handle);//vSyncStartTask���ȡ
                    if(g_ucWorkMode == TDIP_MODE)
                    {
                        g_ucSyncStart = 1;
                    }
                }

                if(((g_ucWorkMode == FDIP_SINGLE_FREQUENCY) || (g_ucWorkMode == FDIP_DOUBLE_FREQUENCY)) && (g_ucWaveStart == 0))//���ο�ʼ
                {
                    xSemaphoreGive(xWaveStartSemaphore_Handle);//vFdipStartTask���ȡ
                    g_ucWaveStart = 1;
                }
                if(((g_ucWorkMode == FDIP_SINGLE_FREQUENCY) || (g_ucWorkMode == FDIP_DOUBLE_FREQUENCY)) && (g_ucFrequencyGroupCycleFlag == 0))
                {
                    xSemaphoreGive(xFrequencyGroupCycleSemaphore_Handle);//vFdipStartTask���ȡ 
                    g_ucFrequencyGroupCycleFlag = 1;
                }
            }

            if(!g_ucGpsTiming)
            {
                g_ucGpsTiming = 1;
                rtcSetDate(ptGPSMsg->tUtc.ucYear, ptGPSMsg->tUtc.ucMonth, ptGPSMsg->tUtc.ucDate); //д��RTC
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
  * @brief  :ͬ����������
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @return :
*******************************************************/
//void vSyncStartTask(void *pvParameters)
void vSyncStart(void)
{
    g_ucSyncStart = 0;//ͬ��������־    

    xSemaphoreTake(xSyncStartSemaphore_Handle, portMAX_DELAY);//vGpsMsgReceiveCallback�ͷ�

    g_ucWaveStart = 0;//��һ��PPS��ʱ������ʽ��ʼ

    g_ucDeviceStatusThirdByte &= (~WAIT_SYNC_START_TRIGGER);

    fpgaWriteByte(FPGA_SYS_RST_REG, SYS_RST | PROTECT_RST);             //ϵͳ��λ,������λ  
    Delay_ms(1);
    fpgaWriteByte(FPGA_SYS_RST_REG, SYS_SET | PROTECT_SET);             //ϵͳ��λ,������λ
    Delay_ms(1);
    fpgaWriteByte(FPGA_SEND_CTRL_REG, SEND_START_CTRL);    //���俪ʼ
    fpgaWriteByte(FPGA_SAMPLE_CMD_REG, SAMPLE_ENABLE);     //����ʹ��

    if(g_ucFirstSyncStart == 1)
    {
        tStartTime.ucHours = ptGPSMsg->tUtc.ucHour;
        tStartTime.ucMinutes = ptGPSMsg->tUtc.ucMinute;
        tStartTime.ucSeconds = ptGPSMsg->tUtc.ucSecond + 1;//��һ���������������ʱ�� 

        g_ucFirstSyncStart = 0;

        xTaskCreate((TaskFunction_t)vNewFileTask,
                    (const char *)"NewFileTask",
                    (uint16_t)512,
                    (void *)NULL,
                    NEW_FILE_TASK_PRIO,
                    (TaskHandle_t *)&xNewFileTask_Handle);

        //���ݽ�������
        xTaskCreate((TaskFunction_t)vAdDataReceiveTask,
                    (const char *)"AdDataReceiveTask",
                    (uint16_t)1024,
                    (void *)NULL,
                    AD_DATA_RECEIVE_TASK_PRIO,
                    (TaskHandle_t *)&xAdDataReceiveTask_Handle);

        if(g_ucSaveMode == CYCLIC_STORAGE)//���ڴ洢
        {
            xTaskCreate((TaskFunction_t)vSaveDataTask,
                        (const char *)"SaveDataTask",
                        (uint16_t)1024,
                        (void *)NULL,
                        SAVE_DATE_TASK_PRIO,
                        (TaskHandle_t *)&xSaveDateTask_Handle);
        }

        //xTaskNotifyGive(xNewFileTask_Handle);//��������֪ͨ��vNewFileTask����

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
    //vTaskDelete(xSyncStartTask_Handle); //ɾ��������
    //vTaskDelay(pdMS_TO_TICKS(5));
}

/*******************************************************
  * @brief  :ͬ������
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @return :
*******************************************************/
uint8_t SyncStart(uint8_t ucWorkMode)
{
    //ͬ����������
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

        if((ucSta & END_OF_SEND_PERIOD) == END_OF_SEND_PERIOD) //�������ڽ���
        {
            if(g_ucSendStopFlag == 0)
            {
                g_ucSendStopFlag = 1;
                xSemaphoreGive(xSendStopFlagSemaphore_Handle);  //vFdipStartTask����
            }
        }

        if((ucSta & END_OF_STOP_KEY) == END_OF_STOP_KEY) //ֹͣ��ֹͣ
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
  * @brief  :ͬ��ֹͣ
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @return :
*******************************************************/
void SyncStop(void)
{
    fpgaWriteByte(FPGA_SEND_CTRL_REG, SEND_STOP_CTRL);     //����ֹͣ
    fpgaWriteByte(FPGA_SAMPLE_CMD_REG, SAMPLE_DISABLE);    //������ֹ

    if(((g_ucWorkMode == FDIP_SINGLE_FREQUENCY) || (g_ucWorkMode == FDIP_DOUBLE_FREQUENCY)) && (xSyncStartSemaphore_Handle != NULL))//����ź���û�б����գ������һ�Σ���ֹֹͣ������������
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
    g_ucDeviceStatusFirstByte = 0;    //�豸״̬��һ�ֽ�
    g_ucDeviceStatusSecondByte = 0;   //�豸״̬�ڶ��ֽ�
    g_ucDeviceStatusThirdByte = 0;    //�豸״̬�����ֽ�
    g_ucDeviceStatusFourthByte = 0;   //�豸״̬�����ֽ�
    g_ucWorkStatus = 0x00;            //����״̬ 0x00:������0x01: ����׼��״̬��0x02:������0x03:��ͣ����0x04:������0x05:��ͣ��
    g_ucFrequencyGroupIndex = 0;      //Ƶ�����
    g_ucFirstSyncStart = 1;           //��һ��ͬ������
    g_ucFirstStart = 1;               //��һ������
    g_ucSyncStart = 1;                //ͬ��������־
    g_ucFrequencyGroupCycleFlag = 1;  //Ƶ������־
    g_ucSendToClientFlag = 1;         //���͸��ͻ��˱�־��Ϊ1ʱ��ʾ�Ѿ������������µ�״̬���͸��ͻ�����
    g_ucSendStopFlag = 0;
    g_ucWaveStart = 1;
    g_ucGpsTiming = 0;                //GPS��ʱ��־
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

    fpgaWriteByte(FPGA_INT_CTRL_REG, AD_INT_DISABLE);    //AD�жϽ�ֹ

    vTaskDelete(xAdDataReceiveTask_Handle); //ɾ��vAdDataReceiveTask

    if(g_ucSaveMode == CYCLIC_STORAGE)//���ڴ洢
    {
        vTaskDelete(xSaveDateTask_Handle);      //ɾ��vSaveDataTask      
    }

    if((g_ucWorkMode == FDIP_SINGLE_FREQUENCY) || (g_ucWorkMode == FDIP_DOUBLE_FREQUENCY))
    {
        //vTaskDelete(xReadSendStopFlagTask_Handle); //ɾ��vReadSendStopFlagTask       
        vTaskDelete(xFdipStartTask_Handle);        //ɾ��vFdipStartTask 
    }

    f_close(&tWorkFile);

    xSemaphoreGive(xFatfsMscSemaphore_Handle); //�ͷ��ź�������OTG_FS_IRQHandler��ȡ
    HAL_NVIC_EnableIRQ(OTG_FS_IRQn);

    clearIntStatusReg();    //���ж�״̬�Ĵ���

    SEND_LED_OFF(SEND_LED);
    OVERCURRENT_LED_OFF(OVERCURRENT_LED);
    FAULT_LED_OFF(FAULT_LED);

    vTaskDelete(xStopSendTask_Handle); //ɾ��������
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
  * @brief  :�½��ļ�����
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

    //ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //�ȴ�����֪ͨ,vSyncStartTask�ͷ�

    xSemaphoreTake(xFatfsMscSemaphore_Handle, portMAX_DELAY); //��ȡ�ź�������OTG_FS_IRQHandler�ͷ�
    HAL_NVIC_DisableIRQ(OTG_FS_IRQn);

    //tSDInfo = sdGetSDInfo();

    if(g_ucWorkMode == TDIP_MODE)//TDIP_MODE
    {
        sprintf(caBuf, "TDIP%02d%02d%02d.dat", ptGPSMsg->tUtc.ucYear, ptGPSMsg->tUtc.ucMonth, ptGPSMsg->tUtc.ucDate);

        tWriteToSD.ucType = g_ucWorkMode;     //д������ͣ�0x00:TDIP����,0x01:FDIP��Ƶ,0x02:FDIP˫Ƶ
        strcpy(tWriteToSD.caFileName, caBuf);   //�ļ���     

        for(int i = 0; i < 4; i++)//��4��0xA5��Ϊÿһ�εĿ�ʼ
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

        strcpy(caFoldPath, g_caSDPath);     //SD�̷� 0:/
        strcat(caFoldPath, g_caTDIPFolder); //�ļ��� TDIP_MODE
        strcat(caFoldPath, "/");
        strcat(caFoldPath, tWriteToSD.caFileName);          //�ļ��� 
    }
    else if((g_ucWorkMode == FDIP_SINGLE_FREQUENCY) || (g_ucWorkMode == FDIP_DOUBLE_FREQUENCY))//FDIP
    {
        sprintf(caBuf, "FDIP%02d%02d%02d.dat", ptGPSMsg->tUtc.ucYear, ptGPSMsg->tUtc.ucMonth, ptGPSMsg->tUtc.ucDate);

        tWriteToSD.ucType = g_ucWorkMode;                  //0x00:TDIP����,0x01:FDIP��Ƶ,0x02:FDIP˫Ƶ
        strcpy(tWriteToSD.caFileName, caBuf);   //�ļ���

        //�洢������Ƶ���� + Ƶ�� + ����������(�����ֽ�) + Ƶ�鷢���� + ѭ������ + ��Ƶ1 + ��Ƶ2 + ...��ƵN1
        for(int i = 0; i < 4; i++)//��4��0xA5��Ϊÿһ�εĿ�ʼ
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

        tWriteToSD.caContent[14] = g_ucSaveMode;            //�洢��ʽ
        tWriteToSD.caContent[15] = g_ucFrequencyGroup;      //Ƶ����
        tWriteToSD.caContent[16] = g_ucFrequencyRatio;      //Ƶ��
        tWriteToSD.caContent[17] = g_usSendCycleNumber >> 8;//����������
        tWriteToSD.caContent[18] = g_usSendCycleNumber;
        tWriteToSD.caContent[19] = g_ucFrequencyGroupCycle; //Ƶ�鷢����
        tWriteToSD.caContent[20] = g_ucNumberOfCycles;     //ѭ������

        ucTmp = 21;
        for(ucTempCount = 0; ucTempCount < g_ucFrequencyGroup; ucTempCount++)//��Ƶ
        {
            tWriteToSD.caContent[ucTmp + ucTempCount] = g_usaMainFrequency[ucTempCount] >> 8;
            tWriteToSD.caContent[ucTmp + 1 + ucTempCount] = g_usaMainFrequency[ucTempCount];
            ucTmp++;
        }

        tWriteToSD.caContent[ucTmp + 1 + ucTempCount] = tStartTime.ucHours;        //����ʱ��,ʱ����
        tWriteToSD.caContent[ucTmp + 2 + ucTempCount] = tStartTime.ucMinutes;
        tWriteToSD.caContent[ucTmp + 3 + ucTempCount] = tStartTime.ucSeconds;
        //tWriteToSD.caContent[ucTmp + 4 + ucTempCount] = 0;

        ucWriteNum = ucTmp + 4 + ucTempCount;

        strcpy(caFoldPath, g_caSDPath);     //SD�̷� 0:/
        strcat(caFoldPath, g_caFDIPFolder); //�ļ��� TDIP_MODE
        strcat(caFoldPath, "/");
        strcat(caFoldPath, tWriteToSD.caFileName);          //�ļ��� 
    }

    eResult = f_open(&tWorkFile, caFoldPath, FA_OPEN_APPEND | FA_WRITE); //����һ�����ļ�������ļ��Ѵ��ڣ����ļ�׷��
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

    vTaskDelete(xNewFileTask_Handle); //ɾ��������  
    vTaskDelay(pdMS_TO_TICKS(1));
}

/*******************************************************
  * @brief  :���ݽ�������
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
            if(ucSta == AD_DATA_READABLE)//AD����׼������*/
            if((fpgaReadByte(FPGA_INT_STATUS_REG) & AD_DATA_READABLE) == AD_DATA_READABLE)//AD����׼������
            {
                for(ulLen = 0; ulLen < AD_FIFO_LEN; ulLen++)//һ���Զ�ȡAD_FIFO_LEN(96)���ֽ�
                {
                    ucaAdFifo[ulLen] = fpgaReadByte(FPGA_AD_FIFO_REG);
                }

                ulAdFifoCount = 0;

                //ADԭʼ����ת��Ϊ��������
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

                    if(g_ucSaveMode == FULL_WAVEFORM_STORAGE)//ȫ���δ洢
                    {
                        g_faCurrent[s_usCount] = dataProcessForFullWave(uIntData.iResult);
                        s_usCount++;

                        if(s_usCount % (AD_FIFO_LEN / 3) == 0)//32������ȡһ����ֵ
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

                        if(s_usCount == SAVE_LEN)//SAVE_LEN������һ��
                        {
                            if(g_ucSaveMode != 0xFF)//
                            {
                                dataSave(g_faCurrent, SAVE_LEN);
                            }
                            s_usCount = 0;
                        }
                    }
                    /*else if(g_ucSaveMode == CYCLIC_STORAGE)//���ڴ洢
                    {
                        iaCurrent[ulCurrentCount] = uIntData.iResult;
                    }*/
                } //end of for(ulCurrentCount = 0; ulCurrentCount < (AD_FIFO_LEN / 3); ulCurrentCount++)

                /*if(g_ucSaveMode == CYCLIC_STORAGE)//���ڴ洢
                {
                    dataProcessForCycle(iaCurrent);
                    if(g_ucSaveMode != 0xFF)//
                    {
                        xTaskNotifyGive(xSaveDateTask_Handle);//��������֪ͨ��vSaveDataTask����
                    }
                }*/
            } // if(ucSta == AD_DATA_READABLE)//AD����׼������
        }

        vTaskDelay(pdMS_TO_TICKS(15));
        ulTCount++;

        if(g_ucWorkMode == TDIP_MODE)
        {
            if(ulTCount * 15 >= (g_ucPulseWidth * 4) * 1000)//һ������ȡһ��ƽ��
            {
                //g_fCurrent = (g_fImax - g_fImin) / 2;//�����������ģ�����������ƽ��
                g_fCurrent = g_fImax;//
                ulTCount = 0;
                g_fImax = 0;
                g_fImin = 0;
                App_Printf("g_fCurrent = %fmA\r\n", g_fCurrent);
            }
        }
        if((g_ucWorkMode == FDIP_SINGLE_FREQUENCY) || (g_ucWorkMode == FDIP_DOUBLE_FREQUENCY))//FDIP
        {
            if(ulTCount * 15 >= g_fFrequencyGroupTime * 1000)//һ��Ƶ��ȡһ��ƽ��
            {
                //g_fCurrent = (g_fImax - g_fImin) / 2;//�����������ģ�����������ƽ��
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
  * @brief  :���ݴ���
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @return :
*******************************************************/
float dataProcessForFullWave(int32_t iOriginData)
{
    float fTemp = 0;

    fTemp = iOriginData * KAD;//�õ�����ֵ
    //fTemp = fTemp / 2;//Ĭ������2��
    fTemp = fTemp * 5;

    #ifdef DEBUG_TARGET
    //SEGGER_RTT_SetTerminal(2);
    //SEGGER_RTT_printf(0, "I = %d\r\n", fTemp);//��֧�ָ�����
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

    //strcpy(tWriteToSD.caContent, caBuf);    //����
    xQueueSend(xWriteToSDQueue_Handle, caBuf, 500); //vWriteToSDTask���ȡ    
}

/*******************************************************
  * @brief  :���ݴ���
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

    dataBubbleSortFilter(piOriginData, iaDest, AD_FIFO_LEN / 3, 10, 10); //���򣬲�����������С��10������

    for(int i = 0; i < (AD_FIFO_LEN / 3 - 20); i++)
    {
        iSum = iSum + iaDest[i];
    }
    fIvalue = (float)iSum / (AD_FIFO_LEN / 3 - 20);//ȡƽ��
    fIvalue = fIvalue * KAD;//�õ�����ֵ
    fIvalue = fIvalue * 5;
    //fIvalue = fIvalue / 2;//Ĭ������2��
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
  * @brief  :�洢��������
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

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //�ȴ�����֪ͨ,vAdDataReceiveTask�ͷ�

        if(g_ucWorkMode == TDIP_MODE)
        {
            /*//�洢��ʽ������+ʱ��(ʱ���ʽ:19/07/05 12:00:00)
            sprintf(caBuf, "%.3f %d/%d/%d %d:%d:%d\r\n", g_fCurrent, ptGPSMsg->tUtc.ucYear, ptGPSMsg->tUtc.ucMonth, ptGPSMsg->tUtc.ucDate, ptGPSMsg->tUtc.ucHour, ptGPSMsg->tUtc.ucMinute, ptGPSMsg->tUtc.ucSecond);
            strcpy(tWriteToSD.caContent, caBuf);    //����*/
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

            xQueueSend(xWriteToSDQueue_Handle, caBuf, 300); //vWriteToSDTask���ȡ

            vTaskDelay(pdMS_TO_TICKS(g_ucPulseWidth * 1000));
        }
        else if((g_ucWorkMode == FDIP_SINGLE_FREQUENCY) || (g_ucWorkMode == FDIP_DOUBLE_FREQUENCY))
        {
            xSemaphoreTake(xFdipSaveSemaphore_Handle, portMAX_DELAY);

           /* //�洢��ʽ��Ƶ�����+����+ʱ��(ʱ���ʽ:19/07/05 12:00:00)
            sprintf(caBuf, "%d %.3f %d/%d/%d %d:%d:%d\r\n", g_ucFrequencyGroupIndex, g_fCurrent, ptGPSMsg->tUtc.ucYear, ptGPSMsg->tUtc.ucMonth, ptGPSMsg->tUtc.ucDate, ptGPSMsg->tUtc.ucHour, ptGPSMsg->tUtc.ucMinute, ptGPSMsg->tUtc.ucSecond);
            strcpy(tWriteToSD.caContent, caBuf);    //����*/

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

            xQueueSend(xWriteToSDQueue_Handle, tWriteToSD.caContent, 300); //vWriteToSDTask���ȡ

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
            uint8_t ucLowByte;  //���ֽ�(С��ģʽ)		
            uint8_t ucMidByte;  //�м��ֽ�			
            uint8_t ucHighByte; //���ֽ�	
            uint8_t ucSignByte; //����
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

    g_fVoltage = IntData_U.iResult * KAD;//�õ�����ֵ
    //g_fVoltage = g_fVoltage / 2;//Ĭ������2��
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
            uint8_t ucLowByte;  //���ֽ�
            uint8_t ucHighByte; //���ֽ�	
            uint8_t ucSignByte; //����
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

    g_fTemperature = sData_U.sResult * 0.0625; //ϵ��0.0625
    g_fTemperature = g_fTemperature / 2;//Ĭ������2��
    return g_fTemperature;
}

/*******************************************************
    @brief  :ð�����򲢶���ǰulDiscardFontNum���ͺ�ulDiscardRearNum������
    @note   :��С��������
    @param  :*iSrcԴ����ָ�룬*iDestĿ������ָ�룬����ǰdiscard_font���ͺ�discard_rear�����ݺ������ָ��
    @param  :total:�����ܸ���
    @param  :ulDiscardFont��ǰ��Ҫ���������ݸ���,ulDiscardRear����Ҫ���������ݸ���
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

    //ð������С��������
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

    //���a�����е�ulDiscardFontNum������ulTotal - ulDiscardRearNum���������浽dest����
    for(i = ulDiscardFontNum; i < ulTotal - ulDiscardRearNum; i++)
    {
        piDest[i - ulDiscardFontNum] = piSrc[i];
    }

    return TRUE;
}






//----------------------------------------------------------------------------------
