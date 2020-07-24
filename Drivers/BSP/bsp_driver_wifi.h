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
  
#ifndef __BSP_DRIVER_WIFI_H
#define __BSP_DRIVER_WIFI_H

#ifdef __cplusplus
extern "C" {
#endif
	
//#include "include.h"


#define ESP8266_RST_PORT_CLK_ENABLE()		__HAL_RCC_GPIOC_CLK_ENABLE()
//#define ESP8266_IO0_PORT_CLK_ENABLE()		__HAL_RCC_GPIOH_CLK_ENABLE()

// 定义ESP8266连接的GPIO端口 
#define ESP8266_RST_PORT		GPIOC			// ESP8266的复位端口
//#define ESP8266_IO0_PORT		GPIOB			// ESP8266的IO0端口
#define ESP8266_RST_PIN       	GPIO_PIN_1			// 连接到RST的GPIO
//#define ESP8266_IO0_PIN       	GPIO_PIN_11			// 连接到IO0的GPIO 

//硬件复位引脚
#define ESP8266_RST_H()     	HAL_GPIO_WritePin(ESP8266_RST_PORT, ESP8266_RST_PIN, GPIO_PIN_SET)     // 
#define ESP8266_RST_L()     	HAL_GPIO_WritePin(ESP8266_RST_PORT, ESP8266_RST_PIN, GPIO_PIN_RESET)   // 

//IO0
//#define ESP8266_IO0_H()			HAL_GPIO_WritePin(ESP8266_IO0_PORT, ESP8266_IO0_PIN, GPIO_PIN_SET)     // 
//#define ESP8266_IO0_L()			HAL_GPIO_WritePin(ESP8266_IO0_PORT, ESP8266_IO0_PIN, GPIO_PIN_RESET)   //

//0表示进入固件升级模式，1表示正常AT指令模式
//#define ESP8266_ENTER_ISP()		ESP8266_IO0_L()		//进入固件升级模式
//#define ESP8266_EXIT_ISP()		ESP8266_IO0_H()		//退出固件升级模式

extern const uint32_t gc_ulBaudrate;

//连接端口号
extern const char *c_PortNum;

//服务器超时时间，作为TCP Server时会踢掉一直不通信至超时时间的TCP Client，设为0表示不踢掉TCP Client
extern const char *c_TCPTimeout;

extern char *g_pcAPSSID;

extern char APRecvBuffer[USART1_RX_LEN];


/********************************************************************************************************
* 工作模式
********************************************************************************************************/
typedef enum{
	STA = 1,
	AP = 2,
	STA_AP = 3  
} ENUM_Net_ModeTypeDef;

/********************************************************************************************************
* 连接方式
********************************************************************************************************/
typedef enum{
	TCP = 1,
	UDP = 2,
	SSL = 3  
} ENUM_Net_ConnectTypeDef;

/********************************************************************************************************
* 加密方式
********************************************************************************************************/
typedef enum
{
	ECN_OPEN = 0,
	ECN_WEP = 1,
	ECN_WPA_PSK = 2,
	ECN_WPA2_PSK = 3,
	ECN_WPA_WPA2_PSK = 4,
}ENUM_AP_PsdMode_TypeDef;

/********************************************************************************************************
* 多连接时的ID号
********************************************************************************************************/
typedef enum{
	Multiple_ID_0 = 0,
	Multiple_ID_1 = 1,
	Multiple_ID_2 = 2,
	Multiple_ID_3 = 3,
	Multiple_ID_4 = 4,
	Single_ID_0 = 5,
} ENUM_ID_NO_TypeDef;


 
void BSP_ESP8266_Init(void);
void ESP8266_SendAT(char *cmd);
void ESP8266_RST(void);

/********************************************************************************************************
* AP 使用的 AT 命令
********************************************************************************************************/
void ATCmd_SetAPPara(char * pcSSID, char * pcPassWord, uint8_t ucChannel, ENUM_AP_PsdMode_TypeDef enumPsdMode);
void ATCmd_SetAPIP(char * pcApIp);
uint8_t ATCmd_ListIP(char * pcStaIp);
 

/********************************************************************************************************
* 共同使用的 AT 命令
********************************************************************************************************/
void ATCmd_SetUart(uint32_t ulBaudrate, uint8_t ucDatabit, uint8_t ucStopbit, uint8_t ucParity, uint8_t ucFlowControl);
void ATCmd_SetEcho(FunctionalState enumMode);
void ATCmd_SetMode(ENUM_Net_ModeTypeDef enumMode);
void ATCmd_RstCmd(void);
void ATCmd_MuxLink(FunctionalState enumEnUnvarnishTx);
void ATCmd_MaxConnect(uint8_t ucNum);
void ATCmd_TCPServer(FunctionalState enumMode, char * pcPortNum);
void ATCmd_SendData(char *pcStr, uint16_t usStrLength, ENUM_ID_NO_TypeDef ucId );
uint8_t WaitATCmdResponse(char *pcAckStr, uint16_t usTimeOut);
void ATCmd_TCPTimeout(char* pcTimeOver);
void ATCmd_TCPClose(uint8_t ucID);

#ifdef __cplusplus
}
#endif

#endif	//__BSP_DRIVER_WIFI_H