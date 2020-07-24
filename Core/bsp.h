/**
  ******************************************************************************
  * @file    bsp.h
  * @author  
  * @brief   
  ******************************************************************************
  * @attention
  *

  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_H
#define __BSP_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
//#include "include.h"
#define STOP_SUPPLY (0)
#define POSITIVE_POWER_SUPPLY (1)
#define NEGATIVE_POWER_SUPPLY (2)
 
#define SN_ADDRESS          (QSPI_FLASH_SIZE - 4096)		//外部FLASH中序列号的存储地址
#define SAVE_MAC_ADDRESS    (QSPI_FLASH_SIZE - 8192)		//存储移动客户端的MAC地址

#define SD_TIMEOUT			(30 * 1000)

#define UART_TIMEOUT		(10 * 1000)

#define QUEUE_SIZE         (uint32_t) 10

#define READ_CPLT_MSG      (uint32_t) 1
#define WRITE_CPLT_MSG     (uint32_t) 2



typedef struct
{
    uint8_t ucHours;
    uint8_t ucMinutes;
    uint8_t ucSeconds;
} StartTime_T; 

/*typedef struct
{
    uint8_t ucYear;
    uint8_t ucMonth;
    uint8_t ucDate;
    uint8_t ucHour;
    uint8_t ucMinute;
    uint8_t ucSecond;
} FpgaTime_T;*/

typedef struct SerialNumber_T
{
	uint8_t ucInfo;     //仪器编号存在信息，0xAA表示外部FLASH里有编号信息存在
    uint8_t ucaSN[20];  //仪器编号
} SN_T;

extern SN_T tSerialNo;
//extern FpgaTime_T tFpgaTime;

extern char g_caTDIPFolder[5];
extern char g_caFDIPFolder[5];
     
extern uint8_t g_ucClientID;
extern float g_fVoltage;	                 //电压
extern float g_fCurrent;                     //电流
extern uint8_t g_ucPulseWidth;               //供电周期
extern uint8_t g_ucDutyRatio;                //占空比
extern uint16_t g_usaMainFrequency[128];     //主频
extern uint8_t g_ucFrequencyGroup;           //频组数
extern uint8_t g_ucFrequencyRatio;           //频比
extern uint16_t g_usSendCycleNumber;         //发射周期数,为0表示第一个频组无限循环
extern uint8_t g_ucFrequencyGroupCycle;      //频组发射间隔
extern uint8_t g_ucNumberOfCycles;           //循环次数
extern StartTime_T tStartTime;  	  	     //启动时间
extern uint8_t g_ucSupplyStatus;             //供电状态
extern uint8_t g_ucWorkMode;                 //工作模式 0xFF:待机,0x00:TDIP，0x01:FDIP单频、0x02:FDIP双频
extern uint8_t g_ucWorkStatus;               //工作状态 0x00:待机，0x01: 发射准备状态，0x02:正供，0x03:正停供，0x04:负供，0x05:负停供
extern uint8_t g_ucExternalPower;            //外部供电，0表示由电池供电，1为外部供电
extern float g_fTemperature;                 //温度
extern float g_fLowFrequency;                //低频频率
extern uint8_t g_ucFrequencyGroupIndex;      //频组序号
extern uint8_t g_ucFirstSyncStart;           //第一次同步启动
extern uint8_t g_ucFirstStart;               //第一次启动
extern uint8_t g_ucSyncStart;                //同步启动标志
extern uint8_t g_ucFrequencyGroupCycleFlag;  //频组间隔标志
extern uint8_t g_ucSaveMode;                 //存储方式，0：周期存储，1：全波形存储
extern uint8_t g_ucTriggerMode;              //触发方式：0x00表示软件触发,0x01表示GPS时钟触发
extern uint8_t g_ucClientConncetStatus;      //客户端连接状态，0：未连接，1：已连接
extern uint8_t g_ucSendToClientFlag;
extern char g_caQueryFileName[20];
extern char *g_pcClientMac;
extern uint8_t g_ucPauseStatus;              //暂停状态
extern float g_fProtectTemperature;          //保护温度
extern uint8_t g_ucBatteryAlarmCapacity;     //电池报警容量
extern float g_fSDAlarmCapacity;             //SD卡报警剩余容量
extern uint8_t g_ucTempFirstByte;
extern uint8_t g_ucDeviceStatusFirstByte;    //设备状态第一字节      
extern uint8_t g_ucDeviceStatusSecondByte;   //设备状态第二字节
extern uint8_t g_ucDeviceStatusThirdByte;    //设备状态第三字节   
extern uint8_t g_ucDeviceStatusFourthByte;   //设备状态第四字节
extern uint8_t g_ucSendStopFlag;
extern uint8_t g_ucWaveStart;
extern uint8_t g_ucGpsTiming;                //GPS授时标志
extern uint16_t g_usBatFullChargeCapacity;   //电池总容量
extern uint16_t g_usBatRelativeStateOfCharge;//电池相对剩余百分比
extern uint16_t g_usBatVoltage;              //电池电压
extern uint16_t g_usBatTemperature;          //电池温度
extern float g_fFrequencyGroupTime;          //频组时间

extern RTC_DateTypeDef tDateStructureget;
extern RTC_TimeTypeDef tTimeStructureget;


void BSP_Init(void);
char * GetLineStr( FIL *fp, char *pcStr, uint32_t ulLen );
uint8_t ReadSN(void);
void guiCheckFont(void);
void SaveClientMac(char *pcMac);
char *ReadClientMac(void);

#ifdef __cplusplus
}
#endif

#endif /*__BSP_H */

/****END OF FILE****/
