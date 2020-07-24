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
 * @brief : ��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:����1:������,����0:����
 */
uint8_t DS18S20_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    ONE_WIRE_GPIO_CLK_ENABLE();			//ʹ��GPIOʱ��

    GPIO_Initure.Pin   = ONE_WIRE_PIN;
    GPIO_Initure.Mode  = GPIO_MODE_OUTPUT_OD;      //��©ģʽ���
    GPIO_Initure.Pull  = GPIO_PULLUP;              //
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;       //
    HAL_GPIO_Init(ONE_WIRE_PORT, &GPIO_Initure); //��ʼ��

    DS18S20_Rst();
    return DS18S20_WaitAck();
}

/**
 * @brief : ��λDS18B20
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
void DS18S20_Rst(void)
{
    ONE_WIRE_OUT(0);    //����DQ
    Delay_us(500);      //����750us ��ʱ���ʱ�䷶Χ���Դ�480��960΢��
    ONE_WIRE_OUT(1);    //DQ=1 
    Delay_us(15);       //15us �ӻ����յ������ĸ�λ�źź󣬻���15~60us���������һ����������
}

/**
 * @brief : �ȴ�DS18B20�Ļ�Ӧ
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:����1:δ��⵽DS18B20�Ĵ���,����0:����
 */
static uint8_t DS18S20_WaitAck(void)
{
    uint8_t ucRetryTime = 0;

    //�ȴ���������ĵ�������������Ϊһ��60~240us�ĵ͵�ƽ�ź�,�����������û����������ʱ����
    while(ONE_WIRE_READ() && ucRetryTime < 200)
    {
        ucRetryTime++;
        Delay_us(1);
    }

    if(ucRetryTime >= 200)
    {
        return 1;
    }
    else
    {
        ucRetryTime = 0;
    }

    while(!ONE_WIRE_READ() && ucRetryTime < 240)//�������嵽�����Ҵ��ڵ�ʱ�䲻�ܳ���240us
    {
        ucRetryTime++;
        Delay_us(1);
    }

    if(ucRetryTime >= 240)
    {
        return 1;
    }

    return 0;
}

/**
 * @brief : ��DS18B20��ȡһ��λ
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:����ֵ��ucData:1/0
 */
static uint8_t DS18S20_ReadBit(void)
{
    uint8_t i, ucData;
    
    ONE_WIRE_OUT(0);//���ٱ���1us
    Delay_us(2);
    ONE_WIRE_OUT(1);
    Delay_us(12);
    if(ONE_WIRE_READ())//������������ŵ���ȡ����״̬�����ܳ��� 15us
    {
        ucData = 1;
    }
    else
    {
        ucData = 0;
    }
    Delay_us(50);

    return ucData;
}

/**
 * @brief : ��DS18B20��ȡһ���ֽ�
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:����ֵ������������
 */
uint8_t DS18S20_ReadByte(void)
{
    uint8_t i, ucRead, ucData;

    /*ucData = 0;
    for(i = 1; i <= 8; i++)
    {
        ucRead = DS18S20_ReadBit();
        ucData = (ucRead << 7) | (ucData >> 1);
    }
    return ucData;*/
    
    for(i = 8; i > 0; i--)
    {
        ucData >>= 1;
        ONE_WIRE_OUT(0);     //�Ӷ�ʱ��ʼ�������ź��߱�����15u�ڣ��Ҳ�������������15u�����  
        Delay_us(5);   //5us
        ONE_WIRE_OUT(1);
        Delay_us(5);   //5us
        if(ONE_WIRE_READ())
        {
            ucData |= 0x80;
        }
        else
        {
            ucData &= 0x7f;
        }
        Delay_us(50);   //65us
        ONE_WIRE_OUT(1);
    }
    return ucData;
}

/**
 * @brief : дһ���ֽڵ�DS18B20
 * @note  :
 * @param :ucData��Ҫд����ֽ�
 * @param :
 * @param :
 * @return:����ֵ������������
 */
void DS18S20_WriteByte(uint8_t ucData)
{
    uint8_t i;
    uint8_t ucTemp;

    for(i = 1; i <= 8; i++)
    {
        ucTemp = ucData & 0x01;
        ucData = ucData >> 1;
        if(ucTemp)       // д1
        {
            ONE_WIRE_OUT(0);
            Delay_us(2);
            ONE_WIRE_OUT(1);
            Delay_us(60);
        }
        else            //д0
        {
            ONE_WIRE_OUT(0);
            Delay_us(60);
            ONE_WIRE_OUT(1);
            Delay_us(2);
        }
    }

    /*for(i = 8; i > 0; i--)
    {
        ONE_WIRE_OUT(0);     //��15u���������������ϣ�DS18B20��15-60u���� 
        Delay_us(5);    //5us
        if(ucData & 0x01)
        {
            ONE_WIRE_OUT(1);
        }
        else
        {
            ONE_WIRE_OUT(0);
        }
        Delay_us(65);    //65us
        ONE_WIRE_OUT(1);
        Delay_us(2);    //������λ��Ӧ����1us
        ucData >>= 1;
    }*/
}

/**
 * @brief : ִ��ƥ�� DS18B20 ROM
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
void DS18S20_MatchRom(void)
{
    DS18S20_Rst();
    DS18S20_WaitAck();
    DS18S20_WriteByte(0X55);                // ƥ�� ROM
}

/**
 * @brief : ��ȡDS18B20���к� 
 * @note  :
 * @param :ds18b20_id���ڴ��DS18B20���кŵ�������׵�ַ
 * @param :
 * @param :
 * @return:
 */
void DS18B20_ReadId(uint8_t *pucId)
{
    uint8_t i;

    DS18S20_WriteByte(ReadROM);       //��ȡ���к�

    for(i = 0; i < 8; i++)
    {
        pucId[i] = DS18S20_ReadByte();
    }
}

/** 
 * @brief : ��ʼ�¶�ת��
 * @note  :
 * @param : 
 * @param : 
 * @param : 
 * @return:
 */
void DS18S20_StartConvert(void)
{
    DS18S20_Rst();
    DS18S20_WaitAck();
    DS18S20_WriteByte(SkipROM);   //���� ROM
    DS18S20_WriteByte(StartConvert);//��ʼ�¶�ת��
}

/**
 * @brief : ���ݴ�����9���ֽ�
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
void DS18S20_ReadScratchpad(void)
{
    DS18S20_Rst();
    DS18S20_WaitAck();
    DS18S20_WriteByte(SkipROM);    //���� ROM
    DS18S20_WriteByte(ReadScratchpad);//���ݴ�����9���ֽ�
}

//----------------------------------------------------------------------