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

#include "include.h"

/** 
 * @brief : 
 * @note  :
 * @param : 
 * @param : 
 * @param : 
 * @return:
 */
void BSP_TEMP_Init(void)
{
    DS18S20_Init();

    /*while(DS18S20_Init());
           
    DS18S20_WriteByte(SkipROM);
    DS18S20_WriteByte(WriteScratchpad);
    DS18S20_WriteByte(DS_AlarmTL);
    DS18S20_WriteByte(DS_AlarmTH);
    DS18S20_WriteByte(DS_PRECISION);

    DS18S20_WriteByte(SkipROM);
    DS18S20_WriteByte(CopyScratchpad);
  
    while(!ONE_WIRE_READ());  //�ȴ��������
    */
}

/** 
 * @brief : ������ƥ�� ROM ����»�ȡ DS18B20 �¶�ֵ
 * @note  :�¶� = ����λ + ���� + С��*0.0625  �¶���16 λ�Ĵ�������չ�Ķ����Ʋ�����ʽ ��������12λ�ֱ���ʱ������5������λ��7������λ��4��С��λ
 * @param : 
 * @param : 
 * @param : 
 * @return:
 */
float tempGetTemperatureSkipRom(void)
{
    uint8_t ucTempHigh, ucTempLow;
    int16_t sTemperature;
    float fTemp;

    DS18S20_StartConvert();           //��ʼת��

    DS18S20_ReadScratchpad();

    ucTempLow = DS18S20_ReadByte();
    ucTempHigh = DS18S20_ReadByte();

    sTemperature = ucTempHigh << 8;
    sTemperature = sTemperature | ucTempLow;
 
    if(sTemperature < 0)                // ���¶�
    {
        fTemp = (~sTemperature + 1) * 0.0625;
    }
    else
    {
        fTemp = sTemperature * 0.0625;
    }

    return fTemp;
}

/** 
 * @brief : ��ƥ�� ROM ����»�ȡ DS18B20 �¶�ֵ
 * @note  :
 * @param : ds18b20_id����� DS18B20 ���кŵ�������׵�ַ
 * @param : 
 * @param : 
 * @return:
 */
float tempGetTemperatureMatchRom(uint8_t *pucId)
{
    uint8_t ucTempHigh, ucTempLow, i;
    int16_t sTemperature;
    float fTemp;

    DS18S20_MatchRom();            //ƥ��ROM

    for(i = 0; i < 8; i++)
    {
        DS18S20_WriteByte(pucId[i]);
    }

    DS18S20_WriteByte(0X44);                                /* ��ʼת�� */

    DS18S20_MatchRom();            //ƥ��ROM

    for(i = 0; i < 8; i++)
    {
        DS18S20_WriteByte(pucId[i]);
    }

    DS18S20_WriteByte(0XBE);                                /* ���¶�ֵ */

    ucTempLow = DS18S20_ReadByte();
    ucTempHigh = DS18S20_ReadByte();

    sTemperature = ucTempHigh << 8;
    sTemperature = sTemperature | ucTempLow;

    if(sTemperature < 0)                /* ���¶� */
    {
        fTemp = (~sTemperature + 1) * 0.0625;
    }
    else
    {
        fTemp = sTemperature * 0.0625;
    }
    
    return fTemp;
}

/**
 * @brief :��ds18b20�õ��¶�ֵ
 * @note  :���ȣ�0.1C  ��������12λ�ֱ���ʱ������5������λ��7������λ��4��С��λ �¶� = ����λ + ���� + С��*0.0625
 * @param :
 * @param :
 * @param :
 * @return:����ֵ���¶�ֵ ��-550~1250��
 */
float tempGetTemperature(void)
{
    uint8_t ucTmp;
    uint8_t ucTempLow, ucTempHigh;
    int16_t sTemperature;
    float fTemp;

    DS18S20_StartConvert();           //��ʼת�� \

    DS18S20_ReadScratchpad();

    ucTempLow = DS18S20_ReadByte();     // LSB   
    ucTempHigh = DS18S20_ReadByte();     // MSB  

    if(ucTempHigh & 0x80)//�¶�Ϊ��  
    {
        ucTempHigh = ~ucTempHigh;
        ucTempLow = ~ucTempLow + 1;        
        ucTmp = 0;//�¶�Ϊ�� 
    }
    else//�¶�Ϊ��
    {
        ucTmp = 1;//�¶�Ϊ��
    }

    sTemperature = ucTempHigh; //��ø߰�λ
    sTemperature <<= 8;
    sTemperature += ucTempLow;//��õװ�λ
    fTemp = (float)sTemperature * 0.5;//ת��     

    if(ucTmp)
    {
        return fTemp;//�����¶�ֵ
    }
    else
    {
        return -fTemp;
    }
}

/**
 * @brief :
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:����ֵ���¶�ֵ ��-550~1250��
 *
float tempGetTemperature(void)
{
    float fTemp;
  
    fTemp = tempGetTemperature_f();
    g_fTemperature = fTemp;
        
    return fTemp;
}
*/
//-----------------------------------------------------