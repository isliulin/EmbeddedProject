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

#ifndef __BSP_DRIVER_USART_H
#define __BSP_DRIVER_USART_H

#ifdef __cplusplus
extern "C" {
    #endif

    //#include "include.h"


    ////////////////////////////////////////////////////////////////////////////////// 	
    #define USARTn		((uint8_t)2)

    #define USART1_TX_GPIO_PORT             GPIOB
    #define USART1_RX_GPIO_PORT             GPIOB
    #define USART1_TX_PIN                   GPIO_PIN_14
    #define USART1_RX_PIN                   GPIO_PIN_15

    #define USART2_TX_GPIO_PORT             GPIOA
    #define USART2_RX_GPIO_PORT             GPIOA
    #define USART2_TX_PIN                   GPIO_PIN_2
    #define USART2_RX_PIN                   GPIO_PIN_3

    #define USARTx_FORCE_RESET()             __USART1_FORCE_RESET()
    #define USARTx_RELEASE_RESET()           __USART1_RELEASE_RESET()

    #define USART1_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
    #define USART1_TX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOB_CLK_DISABLE()
    #define USART1_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
    #define USART1_RX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOB_CLK_DISABLE()

    #define USART2_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
    #define USART2_TX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()
    #define USART2_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
    #define USART2_RX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOA_CLK_DISABLE()

    #define USART1_TX_AF                    GPIO_AF4_USART1
    #define USART1_RX_AF                    GPIO_AF4_USART1

    #define USART2_TX_AF                    GPIO_AF7_USART2
    #define USART2_RX_AF                    GPIO_AF7_USART2

    /* Definition for USARTx's DMA */
    #define USART1_TX_DMA_STREAM              DMA2_Stream7
    #define USART1_RX_DMA_STREAM              DMA2_Stream2

    #define USART1_TX_DMA_CHANNEL             DMA_CHANNEL_4
    #define USART1_RX_DMA_CHANNEL             DMA_CHANNEL_4

    /* Definition for USARTx's NVIC */
    #define USART1_DMA_TX_IRQn                DMA2_Stream7_IRQn
    #define USART1_DMA_RX_IRQn                DMA2_Stream2_IRQn
    #define USART1_DMA_TX_IRQHandler          DMA2_Stream7_IRQHandler
    #define USART1_DMA_RX_IRQHandler          DMA2_Stream2_IRQHandler


    /******************************************************
    * 与移动客户端通信命令
    *******************************************************/
    #define FRAME_HEADER			      0x33	  //帧头
    #define REPLY_FLAG				      0xAA	  //回复标识(正确)
    #define REC_ERROR_FLAG			      0xBB	  //接收错误标识

    #define DEVICE_HANDSHAKE		      0x01	  //设备握手
    #define DEVICE_PAIR                   0x02    //设备配对
    #define DEVICE_RESET                  0x03    //设备复位
    #define DEVICE_PROTECT_RESET          0x04    //设备保护复位

    #define QUERY_DEVICE_STATUS           0X05    //查询设备状态
    #define QUERY_PARAMETER               0X06    //查询当前发射参数

    #define SET_TDIP_PARAMETER            0x10    //设置TDIP发射参数
    #define SET_FDIP_PARAMETER            0x11    //设置FDIP发射参数
    #define START_SEND			          0x12	  //启动发射
    #define PAUSE_SEND				      0x13	  //暂停发射
    #define CONTINUE_SEND                 0x14    //继续发射
    #define STOP_SEND				      0x15	  //停止发射

    #define SELF_TEST                     0X20    //自检
    #define STOP_SELF_TEST                0X21    //停止自检
    #define CHARGE_CONTROL                0x22    //工作时充电控制
    #define GPS_SET_RTC		              0x23	  //GPS设置主机RTC时钟
    #define CLIENT_SET_RTC		          0x24	  //移动端设置主机RTC时钟
    #define SET_PROTECT_TEMPERATURE       0x25	  //设置主机保护温度
    #define SET_BATTERY_ALARM_CAPACITY    0x26    //设置电池报警剩余容量
    #define SET_SD_ALARM_CAPACITY         0x27    //设置MicroSD卡报警剩余容量

    #define QUERY_WORKING_INFO            0x30    //查询当前发射信息
    #define QUERY_SEND_START_TIME         0x31    //查询发射启动时间
    #define QUERY_CURRENT			      0x32	  //查询当前电流
    #define QUERY_VOLTAGE			      0x33	  //查询当前电压
    #define QUERY_RTC_TIME			      0x34	  //查询主机RTC时钟
    #define QUERY_HOST_TEMPERATURE	      0x35	  //查询主机温度
    #define QUERY_GPS_STATUS		      0x36	  //查询GPS状态(定位状态\时间\经纬度等)
    #define QUERY_SD_INFO			      0x37	  //查询MicroSD卡信息（总容量\剩余容量）
    #define QUERY_BAT_INFO			      0x38	  //查询电池信息(外部供电or电池\剩余\电压)  
    #define QUERY_CURRENT_LOG_LIST	      0x39	  //查询所有电流日志文件列表
    #define QUERY_CURRENT_LOG		      0x3A	  //查询电流日志文件

    #define DELETE_ALL_FILE               0x40    //删除SD卡上所有文件
    #define DELETE_ASSIGN_FILE            0x41    //删除SD卡上指定文件

    /******************************************************
    第1字节：
    0x00:待机，0x01: 发射准备状态，0x02:正供，0x03:正停供，0x04:负供，0x05:负停供
    第2字节：
    0x00:TDIP，0x01:FDIP单频、0x02:FDIP双频
    第3字节：
    D0:等待“启动”确认键，D1:等待同步启动触发，D2:“暂停”命令触发停止输出，D3:“停止”键触发停止发射，D4:“停止”命令触发停止发射，D5:“保护”触发停止发射
    第4字节：
    D0：保护， D1：过压， D2：过流， D3：IGBT保护， D4：过温， D5：电池通讯错误， D6：SD卡空间不足， D7：GPS定位有效标志*/
    typedef enum
    {
        STANDBY              = 0x00,
        SEND_READY           = 0x01,
        POSITIVE_SUPPLY      = 0x02,
        POSITIVE_STOP_SUPPLY = 0x03,
        NEGATIVE_SUPPLY      = 0x04,
        NEGATIVE_STOP_SUPPLY = 0x05
    }SUPPLY_STATUS_E;

    typedef enum
    {
        TDIP_MODE = 0x00,
        FDIP_SINGLE_FREQUENCY = 0x01,
        FDIP_DOUBLE_FREQUENCY = 0x02
    }WORK_MODE_E;

    #define WAIT_START_KEY                 (1<<0)      //等待启动键
    #define WAIT_SYNC_START_TRIGGER        (1<<1)      //等待同步启动触发
    #define PAUSE_CMD_TRIGGER_STOP         (1<<2)      //暂停命令触发停止
    #define STOP_KEY_TRIGGER_STOP          (1<<3)      //停止键触发停止发射
    #define STOP_CMD_TRIGGER_STOP          (1<<4)      //停止命令触发停止发射
    #define PROTECT_TRIGGER_STOP           (1<<5)      //保护触发停止发射

    #define PROTECT                        (1<<0)      //保护
    #define OVERVOLTAGE                    (1<<1)      //过压
    #define OVERCURRENT                    (1<<2)      //过流
    #define IGBT_PROTECT                   (1<<3)      //IGBT保护
    #define OVER_TEMPERATURE               (1<<4)      //过温
    #define BATTERY_ERROR                  (1<<5)      //电池通讯错误
    #define SD_CAPACITY_NOT_ENOUGH         (1<<6)      //SD卡空间不足
    #define GPS_VALID                      (1<<7)      //GPS定位有效标志

    #define DATA_TRANSMIT_COMPLETE         0x99      //数据传输完成

    #define CYCLIC_STORAGE                 0x00      //周期存储
    #define FULL_WAVEFORM_STORAGE          0x01      //全波形存储

    #define SUCCESS		                   0x10	     //成功
    #define FAILED			               0x11	     //失败

    #define LOCATION_VALID			       0x10	     //定位有效
    #define LOCATION_INVALID		       0x11	     //定位无效

    #define EXTERNAL_POWER			       0x10	     //外部供电
    #define BATTERY_POWER			       0x11	     //电池供电

    #define RECHARGEABLE			       0x10	     //可在工作时充电RECHARGEABLE
    #define CAN_NOT_CHARGE			       0x11	     //不可在工作时充电CAN_NOT_CHARGE

                                                     //错误信息
    #define RECEIVE_DATA_ERROR             0X01      //接收数据错误
    #define SD_ERROR                       0X05      //MicroSD卡错误    


    #define USART1_TX_LEN  			512  	//定义最大发送字节数
    #define USART1_RX_LEN  			256  	//定义最大接收字节数
    //#define USART2_TX_LEN  			256
    //#define USART2_RX_LEN  			256

    #define RXBUFFERSIZE   1 //缓存大小

    typedef enum
    {
        USARTPort1 = 0,
        USARTPort2 = 1
    }USARTPort_E;

    typedef struct USART1TxMessage_T
    {
        uint16_t usLength;
        uint8_t ucTxBuf[USART1_TX_LEN];
    } USART1TxMsg_T;

    extern USART1TxMsg_T tWifiTxMsg;

    extern uint32_t g_ulUSART1Count;
    //extern uint32_t g_ulUSART2Count;
    extern volatile uint8_t g_ucWIFIMsgValid, g_ucWIFIRecvState;
    //extern volatile uint8_t g_ucGPSMsgValid, g_ucGPSRecvState;

    extern uint8_t g_ucaUsart1RxBuf[USART1_RX_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
    //extern uint8_t g_ucUSART2_RX_BUF[USART2_RX_LEN]};     //接收缓冲,最大USART_REC_LEN个字节.

    extern uint8_t g_ucRxBuffer1[RXBUFFERSIZE];	//HAL库使用的串口接收缓冲
    //extern uint8_t g_ucRxBuffer2[RXBUFFERSIZE];	//HAL库使用的串口接收缓冲


    extern UART_HandleTypeDef huart1;
    //extern UART_HandleTypeDef huart2;
    extern DMA_HandleTypeDef USART1RxDMA_Handler;
    extern DMA_HandleTypeDef USART1TxDMA_Handler;
      


    void BSP_USART_Init(UART_HandleTypeDef *uartHandle, USARTPort_E COMx, uint32_t ulBaudRate);
    void vUsart1TxTask(void *pvParameters);
    void vUsart1RxTask(void *pvParameters);
    void USART1_IdelCallback(void);

    void Usart1ReceiveAnalysis(uint8_t *pcSrcStr, uint8_t *pcSubStr, void (*cb)(uint8_t, uint8_t *, uint8_t));
    void AnalysisHandle(uint8_t ucCmd, uint8_t *pcStr, uint8_t ulRecLen);

    void TransmitList(uint8_t ucFlag, uint8_t *pcData, uint16_t usDataLength, uint8_t ucId);


    #ifdef __cplusplus
}
#endif

#endif	//__BSP_DRIVER_USART_H
