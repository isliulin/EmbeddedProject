/**
  ******************************************************************************
    @file
    @author
    @version
    @date
    @brief
  ******************************************************************************
    @history

  ******************************************************************************
*/

#include "include.h"




/*******************************************************
  * @brief  :
  * @note   :
  * @param  :usMainFrequency ��Ƶ
  * @param  :ucFrequencyRatio Ƶ��
  * @param  :usLowFrequencyNumber ��Ƶ����������
  * @return :
*******************************************************/
void FDIP_Init(uint16_t usMainFrequency, uint8_t ucFrequencyRatio, uint16_t usSendCycleNumber)
{
    uint8_t ucaWriteBuf[4];
    uint32_t ulHighFreHalfPeriodCountValue;      //��Ƶ�����ڼ���ֵ
    uint32_t ulLowFreHalfPeriodCountValue;       //��Ƶ�����ڼ���ֵ
    uint32_t ulHalfPeriodCountAdjustValue;       //�����ڼ���У��ֵ
    uint32_t ulHighFreSendHalfPeriodValue;       //��Ƶ�����������
    uint32_t ulLowFreSendHalfPeriodValue;        //��Ƶ�����������

    BSP_FPGA_Init();

    /*����FPGA�ļ���ֵ*/
    double dCount;
    dCount = ((double)ulFpgaCountFrequency / usMainFrequency) / 2;
    ulHighFreHalfPeriodCountValue = (uint32_t)dCount;         //��Ƶ�����ڼ���ֵ
    ulHalfPeriodCountAdjustValue = usMainFrequency * 2;       //�����ڼ���У��ֵ

    if(g_ucFrequencyRatio == 0)//��Ƶ
    {
        fpgaWriteByte(FPGA_WORK_MODE_REG, FDIP_SINGLE_FREQUENCY);    //����ģʽ
        ulHighFreSendHalfPeriodValue = usSendCycleNumber * 2;//��Ƶ�����������
    }
    else//˫Ƶ
    {
        fpgaWriteByte(FPGA_WORK_MODE_REG, FDIP_DOUBLE_FREQUENCY);
        dCount = (double)usMainFrequency / ucFrequencyRatio;
        dCount = ((double)ulFpgaCountFrequency / dCount) / 2;
        ulLowFreHalfPeriodCountValue = (uint32_t)dCount;          //��Ƶ�����ڼ���ֵ
        ulLowFreSendHalfPeriodValue = usSendCycleNumber * 2;         //��Ƶ�����������
        ulHighFreSendHalfPeriodValue = ulLowFreSendHalfPeriodValue * ucFrequencyRatio;//��Ƶ�����������

        //��Ƶ�����ڼ���ֵ
        ucaWriteBuf[0] = ulLowFreHalfPeriodCountValue >> 24;
        ucaWriteBuf[1] = ulLowFreHalfPeriodCountValue >> 16;
        ucaWriteBuf[2] = ulLowFreHalfPeriodCountValue >> 8;
        ucaWriteBuf[3] = ulLowFreHalfPeriodCountValue;
        fpgaWriteBuffer(LOW_FREQUENCY_HALF_PERIOD_COUNT_1ST_BYTE_REG, ucaWriteBuf, 4);	//��Ƶ�����ڼ���ֵ
        /*fpgaWriteByte(LOW_FREQUENCY_HALF_PERIOD_COUNT_1ST_BYTE_REG, ulLowFreHalfPeriodCountValue >> 24);
        fpgaWriteByte(LOW_FREQUENCY_HALF_PERIOD_COUNT_2ND_BYTE_REG, ulLowFreHalfPeriodCountValue >> 16);
        fpgaWriteByte(LOW_FREQUENCY_HALF_PERIOD_COUNT_3RD_BYTE_REG, ulLowFreHalfPeriodCountValue >> 8);
        fpgaWriteByte(LOW_FREQUENCY_HALF_PERIOD_COUNT_4TH_BYTE_REG, ulLowFreHalfPeriodCountValue);*/

        //�����ڼ���У��ֵ
        ucaWriteBuf[0] = ulHalfPeriodCountAdjustValue >> 8;
        ucaWriteBuf[1] = ulHalfPeriodCountAdjustValue;
        fpgaWriteBuffer(LOW_FREQUENCY_ADJUST_HALF_PERIOD_MSB_REG, ucaWriteBuf, 2);	//��ƵУ�������ڼ���ֵ
        /*fpgaWriteByte(LOW_FREQUENCY_ADJUST_HALF_PERIOD_MSB_REG, ulHalfPeriodCountAdjustValue >> 8);
        fpgaWriteByte(LOW_FREQUENCY_ADJUST_HALF_PERIOD_LSB_REG, ulHalfPeriodCountAdjustValue);*/

        //��Ƶ�����������
        ucaWriteBuf[0] = ulLowFreSendHalfPeriodValue >> 8;
        ucaWriteBuf[1] = ulLowFreSendHalfPeriodValue;
        fpgaWriteBuffer(LOW_FREQUENCY_SEND_HALF_PERIOD_MSB_REG, ucaWriteBuf, 2);	//��Ƶ��������ڼ���ֵ
        /*fpgaWriteByte(LOW_FREQUENCY_SEND_HALF_PERIOD_MSB_REG, ulLowFreSendHalfPeriodValue >> 8);
        fpgaWriteByte(LOW_FREQUENCY_SEND_HALF_PERIOD_MSB_REG, ulLowFreSendHalfPeriodValue);*/
    }

    //��Ƶ�����ڼ���ֵ
    ucaWriteBuf[0] = ulHighFreHalfPeriodCountValue >> 24;
    ucaWriteBuf[1] = ulHighFreHalfPeriodCountValue >> 16;
    ucaWriteBuf[2] = ulHighFreHalfPeriodCountValue >> 8;
    ucaWriteBuf[3] = ulHighFreHalfPeriodCountValue;
    fpgaWriteBuffer(HIGH_FREQUENCY_HALF_PERIOD_COUNT_1ST_BYTE_REG, ucaWriteBuf, 4);	//��Ƶ�����ڼ���ֵ
    /*fpgaWriteByte(HIGH_FREQUENCY_HALF_PERIOD_COUNT_1ST_BYTE_REG, ulHighFreHalfPeriodCountValue >> 24);
    fpgaWriteByte(HIGH_FREQUENCY_HALF_PERIOD_COUNT_2ND_BYTE_REG, ulHighFreHalfPeriodCountValue >> 16);
    fpgaWriteByte(HIGH_FREQUENCY_HALF_PERIOD_COUNT_3RD_BYTE_REG, ulHighFreHalfPeriodCountValue >> 8);
    fpgaWriteByte(HIGH_FREQUENCY_HALF_PERIOD_COUNT_4TH_BYTE_REG, ulHighFreHalfPeriodCountValue);*/

    //�����ڼ���У��ֵ
    ucaWriteBuf[0] = ulHalfPeriodCountAdjustValue >> 8;
    ucaWriteBuf[1] = ulHalfPeriodCountAdjustValue;
    fpgaWriteBuffer(HIGH_FREQUENCY_ADJUST_HALF_PERIOD_MSB_REG, ucaWriteBuf, 2);	//��ƵУ�������ڼ���ֵ
    /*fpgaWriteByte(HIGH_FREQUENCY_ADJUST_HALF_PERIOD_MSB_REG, ulHalfPeriodCountAdjustValue >> 8);
    fpgaWriteByte(HIGH_FREQUENCY_ADJUST_HALF_PERIOD_LSB_REG, ulHalfPeriodCountAdjustValue);*/

    //��Ƶ�����������
    ucaWriteBuf[0] = ulHighFreSendHalfPeriodValue >> 8;
    ucaWriteBuf[1] = ulHighFreSendHalfPeriodValue;
    fpgaWriteBuffer(HIGH_FREQUENCY_SEND_HALF_PERIOD_MSB_REG, ucaWriteBuf, 2);	//��Ƶ��������ڼ���ֵ
    /*fpgaWriteByte(HIGH_FREQUENCY_SEND_HALF_PERIOD_MSB_REG, ulHighFreSendHalfPeriodValue >> 8);
    fpgaWriteByte(HIGH_FREQUENCY_SEND_HALF_PERIOD_LSB_REG, ulHighFreSendHalfPeriodValue);*/


    /*-------------------------------------*/

    //����AD��������
    //fpgaWriteByte(FPGA_AD_CONFIG_CMD_REG, 0x53);           //ADS1255��������WREG,Write to REG 3
    //fpgaWriteByte(FPGA_AD_CONFIG_DATA_REG, 0xA1);          //ADS1255��������,A/D Data Rate 1000SPS
    //fpgaWriteByte(FPGA_AD_CONFIG_DATA_REG, 0x92);          //ADS1255��������,A/D Data Rate 500SPS

    fpgaWriteByte(FPGA_AD_CMD_REG, AD_CMD_ENABLE);         //AD����ʹ��
    Delay_ms(2);
    fpgaWriteByte(FPGA_AD_CMD_REG, AD_CMD_DISABLE);        //AD�����ֹ

    //AD��У
    fpgaWriteByte(FPGA_AD_CONFIG_CMD_REG, 0xF0);           //ADS1255��������SELFCAL,Offset and Gain Self-Calibration
    fpgaWriteByte(FPGA_AD_CMD_REG, AD_CMD_ENABLE);         //AD����ʹ��
    Delay_ms(2);
    fpgaWriteByte(FPGA_AD_CMD_REG, AD_CMD_DISABLE);        //AD�����ֹ

    fpgaWriteByte(FPGA_AD_INPUT_SELECT_REG, CURRENT_CHANNEL);    //ADS1255����ѡ��,����ͨ��

    fpgaWriteByte(FPGA_CURRENT_GAIN_REG, 0x00);//��������1��

    //AD�ж�ʹ�� GPS�ж�ʹ�� �����ж�ʹ�� ��ѹ�ж�ʹ�� ��ѹ���ݸ����ж�ʹ�� ����ֹͣ�ж�ʹ�� GPS_PPS����64�������²����������Ч�ж�ʹ�� �����ж�ʹ��
    fpgaWriteByte(FPGA_INT_CTRL_REG, AD_INT_ENABLE | GPS_INT_ENABLE | OVERCURRENT_INT_ENABLE | OVERVOLTAGE_INT_ENABLE | VOLTAGE_VALUE_INT_ENABLE | SEND_STOP_INT_ENABLE | TXCO_64S_LOCK_INT_ENABLE | OTP_INT_ENABLE);
/*
    fpgaWriteByte(FPGA_INT_CTRL_REG, AD_INT_ENABLE);               //AD�ж�ʹ��
    fpgaWriteByte(FPGA_INT_CTRL_REG, GPS_INT_ENABLE);              //GPS�ж�ʹ��
    fpgaWriteByte(FPGA_INT_CTRL_REG, OVERCURRENT_INT_ENABLE);      //�����ж�ʹ��
    fpgaWriteByte(FPGA_INT_CTRL_REG, OVERVOLTAGE_INT_ENABLE);      //��ѹ�ж�ʹ��
    fpgaWriteByte(FPGA_INT_CTRL_REG, VOLTAGE_VALUE_INT_ENABLE);    //��ѹ���ݸ����ж�ʹ��
    fpgaWriteByte(FPGA_INT_CTRL_REG, SEND_STOP_INT_ENABLE);        //����ֹͣ�ж�ʹ��
    fpgaWriteByte(FPGA_INT_CTRL_REG, TXCO_64S_LOCK_INT_ENABLE);    //GPS_PPS����64�������²����������Ч�ж�ʹ��
    fpgaWriteByte(FPGA_INT_CTRL_REG, OTP_INT_ENABLE);              //�����ж�ʹ��
*/


    //HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);  //�� PA8��FPGA_PPS_PIN�ж�
    //HAL_NVIC_EnableIRQ(EXTI1_IRQn);    //�� PA1��GPS_PPS_PIN�ж�    
}

/*******************************************************
  * @brief  :
  * @note   :Ƶ����+Ƶ��+��Ƶ����������(�����ֽ�)+Ƶ�鷢����+ѭ������+��Ƶ1+��Ƶ2+...��ƵN
  * @param  :
  * @param  :
  * @param  :
  * @return :
*******************************************************/
void vFdipStartTask(void *pvParameters)
{
    uint32_t ulDelayTime;
    float fTempVal;
    uint8_t ucNumberOfCycles = 0, ucConut = 0;
    TickType_t xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();//��ȡ��ǰtick
    //(void)pvParameters;

    for(;;)
    {
        if(g_usSendCycleNumber == 0)//����������Ϊ0��ʾ��һ��Ƶ������ѭ��
        {
            FDIP_Init(g_usaMainFrequency[0], g_ucFrequencyRatio, g_usSendCycleNumber);
            SyncStart(g_ucWorkMode);
            g_ucFrequencyGroupIndex++;
            vTaskSuspend(xFdipStartTask_Handle);      //����vFdipStartTask
            vTaskDelay(pdMS_TO_TICKS(20));
            break;
        }
        else//������������Ϊ0
        {
            if(g_ucNumberOfCycles == 0)//ѭ������Ϊ0ʱ��ʾ����ѭ��
            {
                for(;;)
                {
                    for(ucConut = 0; ucConut < g_ucFrequencyGroup; ucConut++)
                    {
                        FDIP_Init(g_usaMainFrequency[ucConut], g_ucFrequencyRatio, g_usSendCycleNumber);
                        SyncStart(g_ucWorkMode);

                        g_ucFrequencyGroupIndex++;
                        g_ucSendStopFlag = 0;

                        if(g_ucWorkMode == FDIP_SINGLE_FREQUENCY)//��Ƶ
                        {
                            fTempVal = (float)1 / g_usaMainFrequency[ucConut];//��Ƶ����
                        }
                        else if(g_ucWorkMode == FDIP_DOUBLE_FREQUENCY)//˫Ƶ
                        {
                            g_fLowFrequency = (float)g_usaMainFrequency[ucConut] / g_ucFrequencyRatio; //��ƵƵ��
                            fTempVal = (float)1 / g_fLowFrequency;//��Ƶ����
                        }

                        g_fFrequencyGroupTime = fTempVal * g_usSendCycleNumber;
                        ulDelayTime = (uint32_t)fTempVal;

                        xSemaphoreTake(xWaveStartSemaphore_Handle, portMAX_DELAY);//vGpsMsgReceiveCallback�ͷ�,������ʽ��ʼ

                        //vTaskDelay(pdMS_TO_TICKS(1000));//ÿ��������Ҫ����һ��PPS����������ʼ����
                        vTaskDelay(pdMS_TO_TICKS(g_fFrequencyGroupTime * 1000));//һ��Ƶ����Ҫ��ʱ��
                        //vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(fTempVal * 1000 + 1000));

                        //xSemaphoreTake(xSendStopFlagSemaphore_Handle, portMAX_DELAY);//vReadSendStopFlagTask�ͷ�,�������ڽ���
                        xSemaphoreGive(xFdipSaveSemaphore_Handle);//��vSaveDataTask����

                        SyncStop();//һ��Ƶ����

                        g_ucDeviceStatusFirstByte = NEGATIVE_STOP_SUPPLY;

                        g_ucFrequencyGroupCycleFlag = 0;
                        xSemaphoreTake(xFrequencyGroupCycleSemaphore_Handle, portMAX_DELAY);//vGpsMsgReceiveCallback�ͷ�
                    }
                    ucConut = 0;
                    g_ucFrequencyGroupIndex = 0;
                }
            }
            else//ѭ��������Ϊ0
            {
                for(ucNumberOfCycles = 0; ucNumberOfCycles < g_ucNumberOfCycles; ucNumberOfCycles++)
                {
                    for(ucConut = 0; ucConut < g_ucFrequencyGroup; ucConut++)
                    {
                        FDIP_Init(g_usaMainFrequency[ucConut], g_ucFrequencyRatio, g_usSendCycleNumber);
                        SyncStart(g_ucWorkMode);

                        g_ucFrequencyGroupIndex++;
                        g_ucSendStopFlag = 0;

                        if(g_ucWorkMode == FDIP_SINGLE_FREQUENCY)//��Ƶ
                        {
                            fTempVal = (float)1 / g_usaMainFrequency[ucConut];//��Ƶ����
                        }
                        else if(g_ucWorkMode == FDIP_DOUBLE_FREQUENCY)//˫Ƶ
                        {
                            g_fLowFrequency = (float)g_usaMainFrequency[ucConut] / g_ucFrequencyRatio; //��ƵƵ��
                            fTempVal = (float)1 / g_fLowFrequency;//��Ƶ����
                        }

                        g_fFrequencyGroupTime = fTempVal * g_usSendCycleNumber;
                        ulDelayTime = (uint32_t)g_fFrequencyGroupTime;

                        xSemaphoreTake(xWaveStartSemaphore_Handle, portMAX_DELAY);//vGpsMsgReceiveCallback�ͷ�,������ʽ��ʼ

                        //vTaskDelay(pdMS_TO_TICKS(1000));//ÿ��������Ҫ����һ��PPS����������ʼ����
                        vTaskDelay(pdMS_TO_TICKS(g_fFrequencyGroupTime * 1000));//һ��Ƶ����Ҫ��ʱ��
                        //vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(fTempVal * 1000 + 1000));

                        //xSemaphoreTake(xSendStopFlagSemaphore_Handle, portMAX_DELAY);//vReadSendStopFlagTask�ͷ�,�������ڽ���
                        xSemaphoreGive(xFdipSaveSemaphore_Handle);//��vSaveDataTask����

                        SyncStop();//һ��Ƶ����

                        g_ucDeviceStatusFirstByte = NEGATIVE_STOP_SUPPLY;

                        g_ucFrequencyGroupCycleFlag = 0;
                        xSemaphoreTake(xFrequencyGroupCycleSemaphore_Handle, portMAX_DELAY);//vGpsMsgReceiveCallback�ͷ�
                    }
                    ucConut = 0;
                    g_ucFrequencyGroupIndex = 0;
                }
                //StopSend();
                break;
            }//ѭ��������Ϊ0
        } //������������Ϊ0       
    } //for(;;)  
    if(g_usSendCycleNumber != 0) // ������������Ϊ0
    {
        StopSend();
    }
}










//----------------------------------------------------------------------------------------