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

// ����ESP8266���ӵ�GPIO�˿� 
#define ESP8266_RST_PORT		GPIOC			// ESP8266�ĸ�λ�˿�
//#define ESP8266_IO0_PORT		GPIOB			// ESP8266��IO0�˿�
#define ESP8266_RST_PIN       	GPIO_PIN_1			// ���ӵ�RST��GPIO
//#define ESP8266_IO0_PIN       	GPIO_PIN_11			// ���ӵ�IO0��GPIO 

//Ӳ����λ����
#define ESP8266_RST_H()     	HAL_GPIO_WritePin(ESP8266_RST_PORT, ESP8266_RST_PIN, GPIO_PIN_SET)     // 
#define ESP8266_RST_L()     	HAL_GPIO_WritePin(ESP8266_RST_PORT, ESP8266_RST_PIN, GPIO_PIN_RESET)   // 

//IO0
//#define ESP8266_IO0_H()			HAL_GPIO_WritePin(ESP8266_IO0_PORT, ESP8266_IO0_PIN, GPIO_PIN_SET)     // 
//#define ESP8266_IO0_L()			HAL_GPIO_WritePin(ESP8266_IO0_PORT, ESP8266_IO0_PIN, GPIO_PIN_RESET)   //

//0��ʾ����̼�����ģʽ��1��ʾ����ATָ��ģʽ
//#define ESP8266_ENTER_ISP()		ESP8266_IO0_L()		//����̼�����ģʽ
//#define ESP8266_EXIT_ISP()		ESP8266_IO0_H()		//�˳��̼�����ģʽ

extern const uint32_t gc_ulBaudrate;

//���Ӷ˿ں�
extern const char *c_PortNum;

//��������ʱʱ�䣬��ΪTCP Serverʱ���ߵ�һֱ��ͨ������ʱʱ���TCP Client����Ϊ0��ʾ���ߵ�TCP Client
extern const char *c_TCPTimeout;

extern char *g_pcAPSSID;

extern char APRecvBuffer[USART1_RX_LEN];


/********************************************************************************************************
* ����ģʽ
********************************************************************************************************/
typedef enum{
	STA = 1,
	AP = 2,
	STA_AP = 3  
} ENUM_Net_ModeTypeDef;

/********************************************************************************************************
* ���ӷ�ʽ
********************************************************************************************************/
typedef enum{
	TCP = 1,
	UDP = 2,
	SSL = 3  
} ENUM_Net_ConnectTypeDef;

/********************************************************************************************************
* ���ܷ�ʽ
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
* ������ʱ��ID��
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
* AP ʹ�õ� AT ����
********************************************************************************************************/
void ATCmd_SetAPPara(char * pcSSID, char * pcPassWord, uint8_t ucChannel, ENUM_AP_PsdMode_TypeDef enumPsdMode);
void ATCmd_SetAPIP(char * pcApIp);
uint8_t ATCmd_ListIP(char * pcStaIp);
 

/********************************************************************************************************
* ��ͬʹ�õ� AT ����
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