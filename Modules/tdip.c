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
  * @param  :
  * @param  :
  * @param  :
  * @return :
*******************************************************/
void TDIP_Init(void)
{
    uint8_t ucaWriteBuf[4];
    uint32_t ulSupplyCountVal, ulStopCountVal;

    BSP_FPGA_Init();
    fpgaWriteByte(FPGA_WORK_MODE_REG, TDIP_MODE);         //����ģʽ
       
    //����FPGA�ļ���ֵ
    double dCount;
    dCount = (double)ulFpgaCountFrequency * g_ucPulseWidth;//����ֵ=16368000*���� 
    ulSupplyCountVal = (uint32_t)dCount;

    //ͣ =���� - �� * ռ��/ ռ
    dCount = (double)g_ucPulseWidth * g_ucDutyRatio / 100;
    dCount = (double)g_ucPulseWidth - dCount;
    double dTemp = (double)g_ucDutyRatio / 100;
    dCount = dCount / dTemp;

    dCount = ulFpgaCountFrequency * dCount;
    ulStopCountVal = (uint32_t)dCount;

    ucaWriteBuf[0] = ulSupplyCountVal >> 24;
    ucaWriteBuf[1] = ulSupplyCountVal >> 16;
    ucaWriteBuf[2] = ulSupplyCountVal >> 8;
    ucaWriteBuf[3] = ulSupplyCountVal;
    fpgaWriteBuffer(FPGA_POWER_TIME_1ST_BYTE_REG, ucaWriteBuf, 4);
        
    ucaWriteBuf[0] = ulStopCountVal >> 24;
    ucaWriteBuf[1] = ulStopCountVal >> 16;
    ucaWriteBuf[2] = ulStopCountVal >> 8;
    ucaWriteBuf[3] = ulStopCountVal;
    fpgaWriteBuffer(FPGA_STOP_TIME_1ST_BYTE_REG, ucaWriteBuf, 4);
        
    //����AD��������
    //fpgaWriteByte(FPGA_AD_CONFIG_CMD_REG, 0x53);           //ADS1255��������WREG,Write to REG 3
    //fpgaWriteByte(FPGA_AD_CONFIG_DATA_REG, 0xA1);        //ADS1255��������,A/D Data Rate 1000SPS
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
    fpgaWriteByte(FPGA_INT_CTRL_REG, AD_INT_ENABLE | GPS_INT_ENABLE | OVERCURRENT_INT_ENABLE | OVERVOLTAGE_INT_ENABLE| VOLTAGE_VALUE_INT_ENABLE | SEND_STOP_INT_ENABLE | TXCO_64S_LOCK_INT_ENABLE | OTP_INT_ENABLE);
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
}


/** 
 * @brief : 
 * @note  :
 * @param : 
 * @param : 
 * @param : 
 * @return:
 */
void vTdipStartTask(void *pvParameters)
{
    (void)pvParameters;

    TDIP_Init();
    SyncStart(g_ucWorkMode);
    if(eTaskGetState(xTdipStartTask_Handle) != eDeleted)
    {
        vTaskDelete(xTdipStartTask_Handle); //ɾ��vTdipStartTask����
    }
}








//----------------------------------------------------------------------------------------