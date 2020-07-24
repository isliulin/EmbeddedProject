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
#ifndef __BSP_DRIVER_FPGA_H
#define __BSP_DRIVER_FPGA_H

#ifdef __cplusplus
extern "C" {
    #endif

    #define FPGA_BASE_ADDR  SRAM_BANK_ADDR       

    #define FPGA_SYS_RST_REG               (0x00)    //W,ϵͳ��λ�Ĵ���
    #define SYS_RST                        (0<<0)    //ϵͳ��λ
    #define SYS_SET                        (1<<0)    //ϵͳ��λ
    #define PROTECT_RST                    (0<<1)    //������λ
    #define PROTECT_SET                    (1<<1)    //������λ

    #define FPGA_SYS_STATUS_REG            (0x01)    //R,ϵͳ״̬,����

    //�ж�
    #define FPGA_INT_CTRL_REG              (0x02)    //W,�жϿ��ƼĴ���
    #define AD_INT_ENABLE                  (1<<0)    //AD�ж�ʹ��
    #define AD_INT_DISABLE                 (0<<0)    //AD�жϽ�ֹ
    #define GPS_INT_ENABLE                 (1<<1)    //GPS�ж�ʹ��
    #define GPS_INT_DISABLE                (0<<1)    //GPS�жϽ�ֹ
    #define OVERCURRENT_INT_ENABLE         (1<<2)    //�����ж�ʹ��
    #define OVERCURRENT_INT_DISABLE        (0<<2)    //�����жϽ�ֹ
    #define OVERVOLTAGE_INT_ENABLE         (1<<3)    //��ѹ�ж�ʹ��
    #define OVERVOLTAGE_INT_DISABLE        (0<<3)    //��ѹ�жϽ�ֹ
    #define VOLTAGE_VALUE_INT_ENABLE       (1<<4)    //��ѹ���ݸ����ж�ʹ��
    #define VOLTAGE_VALUE_INT_DISABLE      (0<<4)    //��ѹ���ݸ����жϽ�ֹ
    #define SEND_STOP_INT_ENABLE           (1<<5)    //����ֹͣ�ж�ʹ��
    #define SEND_STOP_INT_DISABLE          (0<<5)    //����ֹͣ�жϽ�ֹ
    #define TXCO_64S_LOCK_INT_ENABLE       (1<<6)    //GPS_PPS����64�������²����������Ч�ж�ʹ��
    #define TXCO_64S_LOCK_INT_DISABLE      (0<<6)    //GPS_PPS����64�������²����������Ч�жϽ�ֹ
    #define OTP_INT_ENABLE                 (1<<7)    //�����ж�ʹ��
    #define OTP_INT_DISABLE                (0<<7)    //�����жϽ�ֹ

    #define FPGA_INT_STATUS_REG            (0x03)    //R,�ж�״̬�Ĵ���  
    #define AD_DATA_READABLE               (1<<0)    //AD����׼������
    #define GPS_MSG_READABLE               (1<<1)    //GPS��Ϣ׼������
    #define OVERCURRENT_INT_STATUS         (1<<2)    //�����ж�״̬
    #define OVERVOLTAGE_INT_STATUS         (1<<3)    //��ѹ�ж�״̬
    #define VOLTAGE_VALUE_READABLE         (1<<4)    //��ѹ����׼������
    #define SEND_STOP_INT_STATUS           (1<<5)    //����ֹͣ�ж�״̬
    #define TXCO_64S_LOCK_INT_STATUS       (1<<6)    //GPS_PPS����64�������²����������Ч�ж�״̬
    #define OTP_INT_STATUS                 (1<<7)    //�����ж�״̬

    #define CLEAR_FPGA_INT_STATUS_REG      (0x04)    //W,���ж�״̬�Ĵ���
    #define CLEAR_OVERCURRENT_STATUS       (1<<2)    //������ж�
    #define CLEAR_OVERVOLTAGE_STATUS       (1<<3)    //���ѹ�ж�
    #define CLEAR_SEND_STOP_INT_STATUS     (1<<5)    //�巢��ֹͣ�ж�
    #define CLEAR_TXCO_64S_LOCK_INT_STATUS (1<<6)    //��GPS_PPS����64�������²����������Ч�ж�״̬

    #define PROTECT_STATUS_REG             (0x05)    //R,����״̬�Ĵ���
    #define PROTECT_STATUS                 (1<<0)    //����
    #define OVERVOLTAGE_STATUS             (1<<1)    //��ѹ
    #define OVERCURRENT_STATUS             (1<<2)    //����
    #define IGBT_STATUS                    (1<<3)    //IGBT

    //#define ERROR_STATUS_REG               (0x0B)    //R,����״̬�Ĵ���
    //#define OVERCURRENT_STATUS             (1<<0)    //����
    //#define OVERVOLTAGE_STATUS             (1<<1)    //��ѹ
    //#define OTP_STATUS                     (1<<2)    //����

    #define START_STOP_STATUS_REG          (0x0C)    //R,����ֹͣ״̬�Ĵ���(Key_Value)
    #define SEND_STOP_STATUS               (1<<0)    //����ֹͣ,��ֹͣ����������Ϊ��1��������Ϊ��0��
    #define SEND_START_READY               (1<<1)    //�������,�����䡱��������Ϊ��1��������Ϊ��0��

    #define VOLTAGE_VALUE_1ST_BYTE_REG     (0x0D)    //R,��ѹֵ(ADԭʼ��)��һ�ֽڼĴ���
    #define VOLTAGE_VALUE_2ND_BYTE_REG     (0x0E)    //R,��ѹֵ(ADԭʼ��)�ڶ��ֽڼĴ���
    #define VOLTAGE_VALUE_3RD_BYTE_REG     (0x0F)    //R,��ѹֵ(ADԭʼ��)�����ֽڼĴ���

    #define TXCO_64S_COUNT_1ST_BYTE_REG    (0x10)    //W,�²�����64��Ĭ�ϱ�׼����ֵ��һ�ֽڼĴ���
    #define TXCO_64S_COUNT_2ND_BYTE_REG    (0x11)    //W,�²�����64��Ĭ�ϱ�׼����ֵ�ڶ��ֽڼĴ���
    #define TXCO_64S_COUNT_3RD_BYTE_REG    (0x12)    //W,�²�����64��Ĭ�ϱ�׼����ֵ�����ֽڼĴ���
    #define TXCO_64S_COUNT_4TH_BYTE_REG    (0x13)    //W,�²�����64��Ĭ�ϱ�׼����ֵ�����ֽڼĴ���

    #define TXCO_SET_REG                   (0x10)    //W,TXCO���üĴ���
    #define TXCO_SET_ENABLE                (1<<0)    //Ĭ�ϱ�׼����ֵ���ñ�ʶ����1����Ч

    #define FPGA_GPS_FIFO_REG              (0x15)    //R,GPS��FIFO�Ĵ�����GPS����֡/��

    #define FPGA_SEND_CTRL_REG             (0x20)    //W,������ƼĴ���
    #define SEND_START_CTRL                (1<<0)    //���俪ʼ
    #define SEND_STOP_CTRL                 (0<<0)    //����ֹͣ

    #define FPGA_WORK_MODE_REG             (0x21)    //W,����ģʽ�Ĵ���,����ģʽ����0x00��ʱ����,��0x01����Ƶ,��0x02��˫Ƶ,��0x03����Ƶ

    #define FPGA_POWER_TIME_1ST_BYTE_REG   (0x22)    //W,ʱ����Ĺ���ʱ���һ�ֽڼĴ���
    #define FPGA_POWER_TIME_2ND_BYTE_REG   (0x23)    //W,ʱ����Ĺ���ʱ��ڶ��ֽڼĴ���
    #define FPGA_POWER_TIME_3RD_BYTE_REG   (0x24)    //W,ʱ����Ĺ���ʱ������ֽڼĴ���
    #define FPGA_POWER_TIME_4TH_BYTE_REG   (0x25)    //W,ʱ����Ĺ���ʱ������ֽڼĴ���

    #define FPGA_STOP_TIME_1ST_BYTE_REG    (0x26)    //W,ʱ�����ֹͣ����ʱ���һ�ֽڼĴ���
    #define FPGA_STOP_TIME_2ND_BYTE_REG    (0x27)    //W,ʱ�����ֹͣ����ʱ��ڶ��ֽڼĴ���
    #define FPGA_STOP_TIME_3RD_BYTE_REG    (0x28)    //W,ʱ�����ֹͣ����ʱ������ֽڼĴ���
    #define FPGA_STOP_TIME_4TH_BYTE_REG    (0x29)    //W,ʱ�����ֹͣ����ʱ������ֽڼĴ���

    #define HIGH_FREQUENCY_HALF_PERIOD_COUNT_1ST_BYTE_REG      (0x2A)    //W,˫Ƶ�ĸ�Ƶ�����ڼ���ֵ��һ�ֽڼĴ���
    #define HIGH_FREQUENCY_HALF_PERIOD_COUNT_2ND_BYTE_REG      (0x2B)    //W,˫Ƶ�ĸ�Ƶ�����ڼ���ֵ�ڶ��ֽڼĴ���
    #define HIGH_FREQUENCY_HALF_PERIOD_COUNT_3RD_BYTE_REG      (0x2C)    //W,˫Ƶ�ĸ�Ƶ�����ڼ���ֵ�����ֽڼĴ��� 
    #define HIGH_FREQUENCY_HALF_PERIOD_COUNT_4TH_BYTE_REG      (0x2D)    //W,˫Ƶ�ĸ�Ƶ�����ڼ���ֵ�����ֽڼĴ��� 

    #define HIGH_FREQUENCY_ADJUST_HALF_PERIOD_MSB_REG	       (0x2F)    //W,˫Ƶ�ĸ�ƵУ�������ڸ��ֽڼĴ��� 
    #define HIGH_FREQUENCY_ADJUST_HALF_PERIOD_LSB_REG          (0x30)    //W,˫Ƶ�ĸ�ƵУ�������ڵ��ֽڼĴ���

    #define HIGH_FREQUENCY_SEND_HALF_PERIOD_MSB_REG            (0x31)    //W,˫Ƶ�ĸ�Ƶ��������ڸ��ֽڼĴ���
    #define HIGH_FREQUENCY_SEND_HALF_PERIOD_LSB_REG            (0x32)    //W,˫Ƶ�ĸ�Ƶ��������ڵ��ֽڼĴ���

    #define LOW_FREQUENCY_HALF_PERIOD_COUNT_1ST_BYTE_REG       (0x33)    //W,˫Ƶ�ĵ�Ƶ�����ڼ���ֵ��һ�ֽڼĴ���
    #define LOW_FREQUENCY_HALF_PERIOD_COUNT_2ND_BYTE_REG       (0x34)    //W,˫Ƶ�ĵ�Ƶ�����ڼ���ֵ�ڶ��ֽڼĴ���
    #define LOW_FREQUENCY_HALF_PERIOD_COUNT_3RD_BYTE_REG       (0x35)    //W,˫Ƶ�ĵ�Ƶ�����ڼ���ֵ�����ֽڼĴ��� 
    #define LOW_FREQUENCY_HALF_PERIOD_COUNT_4TH_BYTE_REG       (0x36)    //W,˫Ƶ�ĵ�Ƶ�����ڼ���ֵ�����ֽڼĴ��� 

    #define LOW_FREQUENCY_ADJUST_HALF_PERIOD_MSB_REG	       (0x37)    //W,˫Ƶ�ĵ�ƵУ�������ڵ�һ�ֽڼĴ��� 
    #define LOW_FREQUENCY_ADJUST_HALF_PERIOD_LSB_REG           (0x38)    //W,˫Ƶ�ĵ�ƵУ�������ڵڶ��ֽڼĴ���

    #define LOW_FREQUENCY_SEND_HALF_PERIOD_MSB_REG             (0x39)    //W,˫Ƶ�ĵ�Ƶ��������ڵ�һ�ֽڼĴ���
    #define LOW_FREQUENCY_SEND_HALF_PERIOD_LSB_REG             (0x3A)    //W,˫Ƶ�ĵ�Ƶ��������ڵڶ��ֽڼĴ���

    #define FREQUENCY_RATIO                                    (0x3B)    //W,˫Ƶ��Ƶ��

    #define FPGA_VOLTAGE_GAIN_REG          (0x50)    //W,��ѹ����Ĵ���
    #define FPGA_CURRENT_GAIN_REG          (0x51)    //W,��������Ĵ���

    #define FPGA_AD_INPUT_SELECT_REG       (0x52)    //W,ADS1255����ѡ��Ĵ���,��ѹ/����ͨ��ѡ��
    #define VOLTAGE_CHANNEL                (0x00)    //��ѹͨ��
    #define CURRENT_CHANNEL                (0x01)    //����ͨ��

    #define FPGA_AD_RST_REG                (0x53)    //W,AD��λ�Ĵ���
    #define SAMPLE_RST                     (0<<0)    //AD��λ
    #define SAMPLE_SET                     (1<<0)    //AD��λ

    #define FPGA_AD_CMD_REG                (0x54)    //W,AD����Ĵ���
    #define AD_CMD_DISABLE                 (0<<0)    //AD�����ֹ
    #define AD_CMD_ENABLE                  (1<<0)    //AD����ʹ��

    #define FPGA_AD_FIFO_RST_REG           (0x55)    //W,AD��FIFO��λ�Ĵ���
    #define AD_FIFO_RST                    (1<<0)    //AD��FIFO��λ
    #define AD_FIFO_SET                    (0<<0)    //AD��FIFO��λ

    #define FPGA_SAMPLE_CMD_REG            (0x56)    //R,��������Ĵ���
    #define SAMPLE_DISABLE                 (0<<0)    //������ֹ
    #define SAMPLE_ENABLE                  (1<<0)    //����ʹ��

    #define FPGA_TEMP_MSB_REG              (0x80)    //R,�¶ȸ��ֽڼĴ���,�����Ʋ��룬ϵ��0.0625
    #define FPGA_TEMP_LSB_REG              (0x81)    //R,�¶ȵ��ֽڼĴ���

    #define WORK_STATUS_REG                (0x82)    //R,����״̬�Ĵ���,0x00:����,0x01:����,0x02:��ͣ��,0x03:����,0x04:��ͣ��

    #define SEND_STOP_FLAG                 (0x83)    //R,����ֹͣ��־
    #define END_OF_SEND_PERIOD             (1<<0)    //�������ڽ���
    #define END_OF_STOP_KEY                (1<<1)    //ֹͣ��ֹͣ
    #define END_OF_PROTECT                 (1<<2)    //����ֹͣ 
    #define END_OF_CMD                     (1<<3)    //����ֹͣ

   /*#define CLEAR_SEND_STOP_FLAG           (0x84)    //W,�巢��ֹͣ��־
    #define CLEAR_END_OF_SEND_PERIOD       (1<<0)    //�巢�����ڽ���
    #define CLEAR_END_OF_STOP_KEY          (1<<1)    //��ֹͣ��ֹͣ
    #define CLEAR_END_OF_PROTECT           (1<<2)    //�屣��ֹͣ 
    #define CLEAR_END_OF_CMD               (1<<3)    //������ֹͣ*/

    #define FPGA_AD_FIFO_REG               (0xF1)    //R,AD���ݵ�FIFO�Ĵ���

    #define FPGA_AD_CONFIG_CMD_REG         (0xF3)    //W,ADS1255��������Ĵ���
    #define FPGA_AD_CONFIG_DATA_REG        (0xF4)    //W,ADS1255�������ݼĴ���



    extern uint32_t ulFpgaCountFrequency;    //FPGA����ʱ�ӱ�׼Ƶ��


    void BSP_FPGA_Init(void);
    HAL_StatusTypeDef fpgaWriteBuffer(uint32_t ulWriteAddr, uint8_t *pucBuffer, uint32_t ulLen);
    HAL_StatusTypeDef fpgaReadBuffer(uint32_t ulReadAddr, uint8_t *pucBuffer, uint32_t ulLen);
    void fpgaWriteByte(uint32_t ulWriteAddr, uint8_t ucVal);
    uint8_t fpgaReadByte(uint32_t ulReadAddr);

    #ifdef __cplusplus
}
#endif

#endif /* __BSP_DRIVER_FPGA_H */