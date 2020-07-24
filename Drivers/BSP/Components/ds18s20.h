/**
  ******************************************************************************
  * @file       :
  * @author     :
  * @version    :
  * @date       :
  * @brief      :
  ******************************************************************************
    @history

  ******************************************************************************
*/


#ifndef __DS18S20_H
#define __DS18S20_H


#ifdef __cplusplus
extern "C" {
    #endif

    #define  SkipROM         0xCC     //����ROM
    #define  SearchROM       0xF0     //����ROM
    #define  ReadROM         0x33     //��ROM
    #define  MatchROM        0x55     //ƥ��ROM
    #define  AlarmROM        0xEC     //�澯ROM

    #define  StartConvert    0x44     //��ʼ�¶�ת�������¶�ת���ڼ����������0��ת�����������1
    #define  ReadScratchpad  0xBE     //���ݴ�����9���ֽ�
    #define  WriteScratchpad 0x4E     //д�ݴ������¶ȸ澯TH��TL
    #define  CopyScratchpad  0x48     //���ݴ������¶ȸ澯���Ƶ�EEPROM���ڸ����ڼ����������0������������1
    #define  RecallEEPROM    0xB8     //��EEPROM���¶ȸ澯���Ƶ��ݴ����У������ڼ����0��������ɺ����1
    #define  ReadPower       0xB4     //����Դ�Ĺ��緽ʽ��0Ϊ������Դ���磻1Ϊ�ⲿ��Դ����

    #define  DS_PRECISION    0x7f     //�������üĴ��� 1f=9λ; 3f=10λ; 5f=11λ; 7f=12λ;  
    #define  DS_AlarmTH      0x64
    #define  DS_AlarmTL      0x8a
    #define  DS_CONVERT_TICK 1000

        // �����¶ȴ�����DS18B20���ӵ�GPIO�˿�, ֻ��Ҫ�޸�������뼴�����ⵥ���ߵ�����
    #define ONE_WIRE_PORT			GPIOB			    // �����߶˿�
    #define ONE_WIRE_PIN       	    GPIO_PIN_12			// ���ӵ������ߵ�GPIO

    //ʹ��GPIOʱ��
    #define ONE_WIRE_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()

    #define	ONE_WIRE_OUT(n)      (n ? HAL_GPIO_WritePin(ONE_WIRE_PORT, ONE_WIRE_PIN, GPIO_PIN_SET) : HAL_GPIO_WritePin(ONE_WIRE_PORT, ONE_WIRE_PIN, GPIO_PIN_RESET))

    #define ONE_WIRE_READ()      HAL_GPIO_ReadPin(ONE_WIRE_PORT, ONE_WIRE_PIN)	//��������

    //IO��������
    //#define DS18B20_IO_IN()  {ONE_WIRE_PORT->MODER &= ~(3 << (12 * 2));ONE_WIRE_PORT->MODER |= 0 << 12 * 2;}	//PB12����ģʽ
    //#define DS18B20_IO_OUT() {ONE_WIRE_PORT->MODER &= ~(3 << (12 * 2));ONE_WIRE_PORT->MODER |= 1 << 12 * 2;} 	//PB12���ģʽ

    uint8_t DS18S20_Init(void);			    //��ʼ��DS18B20
    void DS18S20_StartConvert(void);		        //��ʼ�¶�ת��
    void DS18S20_WriteByte(uint8_t dat);   //д��һ���ֽ�
    uint8_t DS18S20_ReadByte(void);		//����һ���ֽ�
    static uint8_t DS18S20_ReadBit(void);		    //����һ��λ
    static uint8_t DS18S20_WaitAck(void);			//����Ƿ����DS18B20
    void DS18S20_Rst(void);			        //��λDS18B20 
    void DS18S20_ReadScratchpad(void);
    void DS18S20_ReadScratchpad(void);
    void DS18S20_StartConvert(void);
    void DS18B20_ReadId(uint8_t *pucId);
    void DS18S20_MatchRom(void);

    #ifdef __cplusplus
}
#endif

#endif
