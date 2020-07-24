/**
  ******************************************************************************
  * @file
  * @author
  * @version
  * @date
  * @brief      电池管理芯片BQ40Z50R1
  ******************************************************************************
  * @history

  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "include.h"

/*******************************************************
  * @brief  : Calculates the PEC of received bytes
  * @note   : C(x) = x8 + x2 + x + 1
  * @param  : ucaPec[]
  * @param  : 
  * @param  : 
  * @retval : pec[0]-this byte contains calculated uacCrc value
*******************************************************/
uint8_t batPECCalculation(uint8_t ucaPec[])
{
    uint8_t uacCrc[6];
    uint8_t ucBitPosition = 47;
    uint8_t shift;
    uint8_t i;
    uint8_t j;
    uint8_t temp;

    do
    {
        /*Load pattern value 0x000000000107 CRC-8*/
        uacCrc[5] = 0;
        uacCrc[4] = 0;
        uacCrc[3] = 0;
        uacCrc[2] = 0;
        uacCrc[1] = 0x01;
        uacCrc[0] = 0x07;
        /*Set maximum bit position at 47 ( six bytes byte5...byte0,MSbit=47)*/
        ucBitPosition = 47; //总共48位
        /*Set shift position at 0*/
        shift = 0;
        /*Find first "1" in the transmited message beginning from the MSByte byte5*/
        i = 5;
        j = 0;

        while ((ucaPec[i] & (0x80 >> j)) == 0 && i > 0)
        {
            ucBitPosition--;

            if (j < 7)
            {
                j++;
            }
            else
            {
                j = 0x00;
                i--;
            }
        } //End of while  找出48位中为“1”的最高位位置

        /*Get shift value for pattern value*/
        shift = ucBitPosition - 8;

        /*Shift pattern value */
        while (shift)
        {
            for (i = 5; i < 0xFF; i--)
            {
                if ((uacCrc[i - 1] & 0x80) && (i > 0))
                {
                    temp = 1;
                }
                else
                {
                    temp = 0;
                }

                uacCrc[i] <<= 1;
                uacCrc[i] += temp;
            } /*End of for*/

            shift--;
        } /*End of while*/

        /*Exclusive OR between pec and uacCrc*/
        for (i = 0; i <= 5; i++)
        {
            ucaPec[i] ^= uacCrc[i];
        } 
    } while (ucBitPosition > 8); /*End of do-while*/

    return ucaPec[0];
}

/*******************************************************
  * @brief  : 
  * @note   : S Slave_Address_Wr WaitAck Command WaitAck S Slave_Address_Rd WaitAck DataL Ack PEC NAck P
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
uint8_t batReadByte(uint8_t ucSlaveAddress, uint8_t ucCommand)
{
    uint8_t ucaBuf[6];
    uint8_t ucData;
    volatile uint8_t ucAck, ucAck2, ucAck3, ucPEC,ucPecReg;

    //S Slave_Address_Wr WaitAck Command WaitAck S Slave_Address_Rd WaitAck DataL Ack PEC NAck P  
    SMBusStart();                // 总线开始信号
    SMBusSendByte(ucSlaveAddress | SMBUS_WR); // 发送设备地址+写信号
    ucAck = SMBusWaitAck();

    SMBusSendByte(ucCommand);
    Delay_us(80);
    ucAck2 = SMBusWaitAck();

    SMBusStart(); // 总线开始信号

    SMBusSendByte(ucSlaveAddress | SMBUS_RD); // 发送设备地址+读信号
    ucAck3 = SMBusWaitAck();

    Delay_us(50);
    ucData = SMBusReadByte(); // 读出寄存器数据
    SMBusAck();

    Delay_us(50);
    ucPEC = SMBusReadByte();
    SMBusNAck();

    SMBusStop(); // 总线停止信号
    Delay_us(50);

    ucaBuf[5] = ucSlaveAddress;
    ucaBuf[4] = ucCommand;
    ucaBuf[3] = ucSlaveAddress + 1;
    ucaBuf[2] = ucData;
    ucaBuf[1] = 0;
    ucaBuf[0] = 0;
    ucPecReg = batPECCalculation(ucaBuf); //Calculate CRC
    if (ucPecReg != ucPEC)
    {
        ucData = 0;
    }
    else
    {
        return ucData;
    }
    return ucData;
}

/*******************************************************
  * @brief  : 
  * @note   : 格式：S Slave_Address_Wr WaitAck Command WaitAck S Slave_Address_Rd WaitAck DataL Ack DataH Ack PEC NAck P
  * @param  : 
  * @param  : 
  * @param  : 
  * @return : 
*******************************************************/
uint16_t batReadWord(uint8_t ucSlaveAddress, uint8_t ucCommand)
{
    uint16_t usData = 0;    
    uint8_t ucaBuf[6], ucDataL, ucDataH;
    volatile uint8_t ucAck, ucAck2, ucAck3, ucPEC, ucPecReg;
    HAL_StatusTypeDef sta = HAL_ERROR;
    uint8_t ucCount = 0;

    //S Slave_Address_Wr WaitAck Command WaitAck S Slave_Address_Rd WaitAck DataL Ack DataH Ack PEC NAck P
    do
    {
        SMBusStart();                  // 总线开始信号
        SMBusSendByte(ucSlaveAddress | SMBUS_WR); // 发送设备地址+写信号
        ucAck = SMBusWaitAck();

        SMBusSendByte(ucCommand);
        Delay_us(80);
        ucAck2 = SMBusWaitAck();

        SMBusStart(); // 总线开始信号

        SMBusSendByte(ucSlaveAddress | SMBUS_RD); // 发送设备地址+读信号
        ucAck3 = SMBusWaitAck();

        Delay_us(50);
        ucDataL = SMBusReadByte(); // 读出寄存器数据
        SMBusAck();

        Delay_us(50);
        ucDataH = SMBusReadByte();
        SMBusAck();

        Delay_us(50);
        ucPEC = SMBusReadByte();
        SMBusNAck();

        Delay_us(50);
        SMBusStop(); // 总线停止信号
                
        Delay_us(100);

        ucaBuf[5] = ucSlaveAddress;
        ucaBuf[4] = ucCommand;
        ucaBuf[3] = ucSlaveAddress + 1;
        ucaBuf[2] = ucDataL;
        ucaBuf[1] = ucDataH;
        ucaBuf[0] = 0;
        ucPecReg = batPECCalculation(ucaBuf); //Calculate CRC
        ucCount++;
    } while ((ucPecReg != ucPEC) && (ucCount < 3));//不正确时最多读取3次

    if (ucPecReg != ucPEC) 
    {
        usData = 0xFFFF;
    }
    else
    {
        usData = (ucDataH << 8) | ucDataL;
    }

    return usData;
}

/*******************************************************
  * @brief  : 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void batWriteByte(uint8_t ucRegAddr, uint8_t ucRegData)
{
    SMBusStart(); /* 总线开始信号 */

    SMBusSendByte(BAT_SLAVE_ADDRESS | SMBUS_WR); /* 发送设备地址+写信号 */
    SMBusWaitAck();

    SMBusSendByte(ucRegAddr); /* 内部寄存器地址 */
    SMBusWaitAck();

    SMBusSendByte(ucRegData); /* 内部寄存器数据 */
    SMBusWaitAck();

    SMBusStop(); /* 总线停止信号 */
}

/*******************************************************
  * @brief  : This read/write word function sets various battery operating mode options
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
uint16_t batMode(void)
{
    uint16_t usMode;
    usMode = batReadWord(BAT_SLAVE_ADDRESS, BATTERY_MODE);
    return usMode;
}

/*******************************************************
  * @brief  : This read-word function returns various battery status information.
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
uint16_t batStatus(void)
{
    uint16_t usSta;
    usSta = batReadWord(BAT_SLAVE_ADDRESS, BATTERY_STATUS);
    return usSta;
}

/*******************************************************
  * @brief  : This read-word function returns the temperature in units 0.1°K
  * @note   : 开氏度＝摄氏度＋273.15
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
uint16_t batTemperature(void)
{
    uint16_t usSta;
    usSta = batReadWord(BAT_SLAVE_ADDRESS, TEMPERATURE);
    return usSta;
}

/*******************************************************
  * @brief  : This read-word function returns the sum of the measured cell voltages
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
uint16_t batVoltage(void)
{
    uint16_t usVoltage;
    usVoltage = batReadWord(BAT_SLAVE_ADDRESS, BAT_VOLTAGE);
    return usVoltage;
}
/*******************************************************
  * @brief  : This read-word function returns the measured current from the coulomb counter. If the input to the device
exceeds the maximum value, the value is clamped at the maximum and does not roll over.
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
int16_t batCurrent(void)
{
    int16_t sCurrent;
    sCurrent = batReadWord(BAT_SLAVE_ADDRESS, BAT_CURRENT);
    return sCurrent;
}

/*******************************************************
  * @brief  : This read-word function returns the predicted remaining battery capacity as a percentage of FullChargeCapacity().
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
uint16_t batRelativeStateOfCharge(void)
{
    uint16_t usBatPercent;
    usBatPercent = batReadWord(BAT_SLAVE_ADDRESS, RELATIVE_STATEOFCHARGE);
    return usBatPercent; 
}

/*******************************************************
  * @brief  : This read-word function returns the predicted remaining battery capacity as a percentage.
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
uint16_t batAbsoluteStateOfCharge(void)
{
    uint16_t usBatPercent;
    usBatPercent = batReadWord(BAT_SLAVE_ADDRESS, ABSOLUTE_STATEOFCHARGE);
    return usBatPercent;
}

/*******************************************************
  * @brief  :读取电池剩余容量 This read-word function returns the predicted remaining battery capacity
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
uint16_t batRemainingCapacity(void)
{
    uint16_t usRemaining;
    usRemaining = batReadWord(BAT_SLAVE_ADDRESS, REMAINING_CAPACITY);
    return usRemaining;
}

/*******************************************************
  * @brief  :读取电池总容量 This read-word function returns the predicted battery capacity when fully charged. The value returned will not be updated during charging
  * @note   :                
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
uint16_t batFullChargeCapacity(void)
{
    uint16_t usFullCapacity;
    usFullCapacity = batReadWord(BAT_SLAVE_ADDRESS, FULLCHARGE_CAPACITY);
    return usFullCapacity;
}

/*******************************************************
  * @brief  : This read-word function returns the desired charging current.
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
uint16_t batChargingCurrent(void)
{
    uint16_t usChargingCurrent;
    usChargingCurrent = batReadWord(BAT_SLAVE_ADDRESS, CHARGING_CURRENT);
    return usChargingCurrent;
}

/*******************************************************
  * @brief  : This read-word function returns the desired charging voltage.
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
uint16_t batChargingVoltage(void)
{
    uint16_t usChargingVoltage;
    usChargingVoltage = batReadWord(BAT_SLAVE_ADDRESS, CHARGING_VOLTAGE);
    return usChargingVoltage;
}

/*******************************************************
  * @brief  : 
  * @note   : 
Version (Bits 7–4): Version
0,0,0,1 = Version 1.0
0,0,1,1 = Version 1.1
0,0,1,1 = Version 1.1 with optional PEC support
Revision (Bits 3–0): Revision
0,0,0,1 = Version 1.0 and 1.1 (default)
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
uint16_t batSpecificationInfo(void)
{
    uint16_t usInfo;
    usInfo = batReadWord(BAT_SLAVE_ADDRESS, SPECIFICATION_INFO);
    return usInfo;  
}

/*******************************************************
  * @brief  : This read-word function returns the assigned pack serial number.
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
uint16_t batSerialNumber(void)
{
    uint16_t usSerialNumber;
    usSerialNumber = batReadWord(BAT_SLAVE_ADDRESS, BAT_SERIAL_NUMBER);
    return usSerialNumber;
}

/*******************************************************
  * @brief  : 检测SMBus总线设备，CPU发送设备地址，然后读取设备应答来判断该设备是否存在
  * @param  : ucAddress:设备的SMBus总线地址
  * @param  : 
  * @param  : 
  * @retval : 返回值 0 表示正确， 返回1表示未探测到
*******************************************************/
uint8_t batCheck(uint8_t ucAddress)
{
    uint8_t ucAck = 1;

    if (SMBUS_SDA_READ() && SMBUS_SCL_READ())
    {
        SMBusStart(); /* 发送启动信号 */
        /* 发送设备地址+读写控制bit（0 = w， 1 = r) bit7 先传 */
        SMBusSendByte(ucAddress | SMBUS_WR);
        ucAck = SMBusWaitAck(); /* 检测设备的ACK应答 */
        SMBusStop();            /* 发送停止信号 */
        return ucAck;
    }

    return 1; /* SMBus总线异常 */
}


//--------------------------------------------------
