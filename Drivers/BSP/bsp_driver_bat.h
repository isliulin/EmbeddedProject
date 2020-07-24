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

#ifndef __BSP_DRIVER_BATTERY_H
#define __BSP_DRIVER_BATTERY_H

#ifdef __cplusplus
extern "C" {
#endif

#define BAT_SLAVE_ADDRESS        0x16

#define BATTERY_MODE             0x03
#define TEMPERATURE              0x08
#define BAT_VOLTAGE              0x09
#define BAT_CURRENT              0x0A
#define RELATIVE_STATEOFCHARGE   0x0D
#define ABSOLUTE_STATEOFCHARGE   0x0E
#define REMAINING_CAPACITY       0x0F
#define FULLCHARGE_CAPACITY      0x10
#define CHARGING_CURRENT         0x14
#define CHARGING_VOLTAGE         0x15
#define BATTERY_STATUS           0x16
#define SPECIFICATION_INFO       0x1A
#define BAT_SERIAL_NUMBER        0x1C

#define SMBUS_WR                 0x00      
#define SMBUS_RD                 0x01      

#define ACK                      1
#define NACK                     0

uint16_t batMode(void);
uint16_t batStatus(void);
uint16_t batTemperature(void);
uint16_t batVoltage(void);
int16_t batCurrent(void);
uint16_t batRelativeStateOfCharge(void);
uint16_t batAbsoluteStateOfCharge(void);
uint16_t batRemainingCapacity(void);
uint16_t batFullChargeCapacity(void);
uint16_t batChargingCurrent(void);
uint16_t batChargingVoltage(void);
uint16_t batSpecificationInfo(void);
uint16_t batSerialNumber(void);
uint8_t batCheck(uint8_t ucAddress);

#ifdef __cplusplus
}
#endif

#endif //__BSP_DRIVER_BATTERY_H