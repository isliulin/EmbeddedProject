/**
  ******************************************************************************
    @file
    @author
    @version
    @date
    @brief
  ******************************************************************************
    @history

  ******************************************************************************
*/

#include "include.h"

USART_TypeDef *USART_Port[USARTn] = {USART1, USART2};

uint32_t g_ulUSART1Count = 0;
//uint32_t g_ulUSART2Count = 0;
volatile uint8_t g_ucWIFIMsgValid, g_ucWIFIRecvState;
//volatile uint8_t g_ucGPSMsgValid, g_ucGPSRecvState;

uint8_t g_ucaUsart1RxBuf[USART1_RX_LEN] = {0}; //接收缓冲,最大USART_REC_LEN个字节.
//uint8_t g_ucUSART2_RX_BUF[USART2_RX_LEN] = {0};     //接收缓冲,最大USART_REC_LEN个字节.

uint8_t g_ucRxBuffer1[RXBUFFERSIZE] = {0}; //HAL库使用的串口接收缓冲
//uint8_t g_ucRxBuffer2[RXBUFFERSIZE] = {0};	//HAL库使用的串口接收缓冲

USART1TxMsg_T tWifiTxMsg;

UART_HandleTypeDef huart1;
//UART_HandleTypeDef huart2;
DMA_HandleTypeDef USART1RxDMA_Handler;
DMA_HandleTypeDef USART1TxDMA_Handler;



// SUPPLY_STATUS_E eSupplyStatus;
// WORK_MODE_E eWorkMode;


/*******************************************************
  * @brief  : Configures USART port.
  * @note   :
  * @param  : COMx: COM port to be configured.
  *          This parameter can be one of the following values:
  *            @arg  UARTPort1
  *            @arg  UARTPort2
  * @param  : ulBaudRate:BaudRate
  * @param  :
  * @retval :
*******************************************************/
void BSP_USART_Init(UART_HandleTypeDef *uartHandle, USARTPort_E COMx, uint32_t ulBaudRate)
{
    /* USART configuration */
    uartHandle->Instance          = USART_Port[COMx];
    uartHandle->Init.BaudRate     = ulBaudRate;               //波特率
    uartHandle->Init.WordLength   = UART_WORDLENGTH_8B;       //字长为8位数据格式
    uartHandle->Init.StopBits     = UART_STOPBITS_1;          //一个停止位
    uartHandle->Init.Parity       = UART_PARITY_NONE;         //无奇偶校验位
    uartHandle->Init.HwFlowCtl    = UART_HWCONTROL_NONE;      //无硬件流控
    uartHandle->Init.Mode         = UART_MODE_TX_RX;          //收发模式
    uartHandle->Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(uartHandle);
}

/*******************************************************
  * @brief  :此任务已不用，改为在需要的地方直接调用HAL_UART_Transmit_DMA
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @retval :
*******************************************************
void vUsart1TxTask(void *pvParameters)
{
    //USART1TxMsg_T tTxMessage;
    uint32_t ulSize;
    ulSize = sizeof(struct USART1TxMessage_T);

    for(;;)
    {
        //memset(&tTxMessage, 0, ulSize);

        ////从队列xUsart1TxQueue_Handle获取数据，队列项就是要发送的内容，ESP8266_SendAT里发送消息
        //xQueueReceive(xUsart1TxQueue_Handle, (void *)& tTxMessage, portMAX_DELAY);

        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //等待任务通知，在ESP8266_SendAT函数释放

        HAL_UART_Transmit_DMA(&huart1, tWifiTxMsg.ucTxBuf, tWifiTxMsg.usLength); //发送后要在USART1_DMA_TX_IRQHandler里清标志

        #ifdef DEBUG_TARGET
        SEGGER_RTT_SetTerminal(2);
        SEGGER_RTT_printf(0, "Send: %s\r\n", tWifiTxMsg.ucTxBuf);
        App_Printf("Send: %s\r\n", tWifiTxMsg.ucTxBuf);
        #endif

        //vTaskDelay(pdMS_TO_TICKS(50));
        Delay_ms(100);
        //		xSemaphoreTake(xUsart1TxSemaphore_Handle, portMAX_DELAY);	//USART1_DMA_TX_IRQHandler里释放
    }
}
*/
/*******************************************************
  * @brief  :
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @retval :
*******************************************************/
void vUsart1RxTask(void *pvParameters)
{
    char *ucSearch0, *ucSearch1, *ucSearch2; 
    const char *c_pcClientStr = "+IPD,";    //+IPD,0,n:xxxxxxxxxx
    const char *c_pcConnectStr = "CONNECT"; //n为ID号
    const char *c_pcCloseStr = "CLOSED";
 /*   const char *c_pcStr0 = "+IPD,0,";
    const char *c_pcStr1 = "+IPD,1,";
    const char *c_pcStr2 = "+IPD,2,";
    const char *c_pcStr3 = "+IPD,3,";
    const char *c_pcStr4 = "+IPD,4,";
    const char *c_pcStr5 = "0,CONNECT";
    const char *c_pcStr6 = "0,CLOSED";*/

    //(void)pvParameters;

    //在串口初始化调用的话会进入一次中断导致错误，在外部使能即可
    __HAL_UART_CLEAR_IT(&huart1, UART_CLEAR_IDLEF);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE); //使能空闲中断
    HAL_UART_Receive_DMA(&huart1, g_ucaUsart1RxBuf, USART1_RX_LEN);

    for(;;)
    {
        ////获取二值信号量xSemaphore,在Usart1_DMA_Rx_Data函数释放
        //xSemaphoreTake(xUsart1RxSemaphore_Handle, portMAX_DELAY);
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //等待任务通知，在Usart1_DMA_Rx_Data函数释放，代替上面注释的二进制信号量
        {            
            ucSearch0 = strstr((char *)g_ucaUsart1RxBuf, c_pcClientStr); //+IPD,0,n:xxxxxxxxxx
            ucSearch1 = strstr((char *)g_ucaUsart1RxBuf, c_pcConnectStr); //n,CONNECT//n为ID号
            ucSearch2 = strstr((char *)g_ucaUsart1RxBuf, c_pcCloseStr); //n,CLOSED

            if(ucSearch1 != NULL) //n,CONNECT//n为ID号
            {
                g_ucClientConncetStatus = 1;    //客户端连接状态，0：未连接，1：已连接     
                ONLINE_LED_ON(ONLINE_LED);
            }

            if(ucSearch2 != NULL) //n,CLOSED  客户端掉线
            {
                g_ucClientConncetStatus = 0;    //客户端连接状态，0：未连接，1：已连接
                ONLINE_LED_OFF(ONLINE_LED);
            }

            if(ucSearch0 != NULL) //接收到的是Client发过来的数据，+IPD,0,n:xxxxxxxxxx
            {
                uint8_t *pcTempPoint = NULL;
                uint8_t ucIDNum;
                g_ucClientID = 0;

                ONLINE_LED_ON(ONLINE_LED);
                pcTempPoint = (uint8_t *)strchr((char *)g_ucaUsart1RxBuf, ','); //逗号第一次出现的位置
                ucIDNum = *(pcTempPoint + 1);                             //逗号后第一个为ClientID
               
                if(ucIDNum == 0x30)
                {
                    g_ucClientID = 0;
                }
                else if(ucIDNum == 0x31)
                {
                    g_ucClientID = 1;
                }
                else if(ucIDNum == 0x32)
                {
                    g_ucClientID = 2;
                }
                else if(ucIDNum == 0x33)
                {
                    g_ucClientID = 3;
                }
                else if(ucIDNum == 0x34)
                {
                    g_ucClientID = 4;
                }             

                Usart1ReceiveAnalysis(g_ucaUsart1RxBuf, (uint8_t *)c_pcClientStr, AnalysisHandle); //解析             
            }      
            /*else //WIFI模块的回复信息
            {
                //把数据传给APRecvBuffer,给出信号量
                //strcpy(APRecvBuffer, (char *)g_ucaUsart1RxBuf);
                //memcpy(APRecvBuffer, g_ucaUsart1RxBuf, sizeof(APRecvBuffer));
                //xSemaphoreGive(xATCmdAckSemaphore_Handle); //WaitATCmdResponse里获取

                #ifdef DEBUG_TARGET
                SEGGER_RTT_SetTerminal(0);
                SEGGER_RTT_printf(0, "AT_echo(ASCII): %s\r\n", g_ucaUsart1RxBuf);
                App_Printf("AT_echo(ASCII): %s\r\n", g_ucaUsart1RxBuf);

                SEGGER_RTT_printf(0, "AT_echo(HEX): %#02x\r\n", g_ucaUsart1RxBuf);
                App_Printf("AT_echo(HEX): %#02x\r\n", g_ucaUsart1RxBuf);
                #endif
            }*/

            memset(g_ucaUsart1RxBuf, 0, USART1_RX_LEN); // 清零
        }
    }
}

/*******************************************************
  * @brief  :
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @retval :
*******************************************************/
void TransmitList(uint8_t ucFlag, uint8_t *pcData, uint16_t usDataLength, uint8_t ucId)
{
    uint8_t ucCheckSum = 0;
    uint8_t ucTxBuf[USART1_TX_LEN];
    uint16_t usLength = 0;
    const TickType_t xWaitTicks = pdMS_TO_TICKS(300); // 设置最大等待时间为 300ms

    ucTxBuf[0] = FRAME_HEADER; //帧头
    ucTxBuf[1] = ucFlag;       //回复命令
    ucTxBuf[2] = usDataLength; //数据长度

    for(int i = 0; i < usDataLength; i++)
    {
        ucTxBuf[i + 3] = pcData[i]; //第三位开始是数据
    }

    for(int i = 0; i < (3 + usDataLength); i++)
    {
        ucCheckSum = ucCheckSum + ucTxBuf[i];
    }
    ucCheckSum = (~ucCheckSum) + 1;

    ucTxBuf[3 + usDataLength] = ucCheckSum; //最后一位校验和

    usLength = 3 + usDataLength + 1; //总长度

    //if(1 == g_ucClientConncetStatus)//在联机状态才发送
    //{
        ATCmd_SendData((char *)(ucTxBuf), usLength, ucId); //往WIFI模块发送数据
    //}    
}

/** 
 * @brief : 
 * @note  :
 * @param : *pcStr-将被发送的数据块的指针
 * @param :  
 * @param : btf->0: 传输调用(将被发送的字节数)。 0: 检测调用
 * @return:返回已发送字节数或流状态
 */
UINT out_data(const BYTE *pcStr, UINT btf)
{
    UINT cnt = 0;

    if(btf != 0)
    { 
        /* 传输调用 */
        //TransmitList(REPLY_FLAG, pcStr, btf, g_ucClientID);
        ATCmd_SendData(pcStr, btf, g_ucClientID);

        cnt = btf;
    }
    else
    { 
        /* 检测调用
        返回流状态(0: 忙， 1: 就绪) 
        当检测调用时，一旦它返回就绪，那么在后续的传输调用时，它必须接收至少一个字节，或者
        f_forward 将以 FR_INT_ERROR 而失败。 */
        cnt = 1;
    }
    return cnt;
}

/*******************************************************
  * @brief  : 在这里只打印解析的内容
  * @note   : 可以改成需要处理的任务
  * @param  :
  * @param  :
  * @param  :
  * @retval :
*******************************************************/
void AnalysisHandle(uint8_t ucCmd, uint8_t *pcStr, uint8_t ulRecLen)
{
    uint8_t ulSendLen = 0, ucSNLen = 0;
    uint8_t *pucSendDataBuf = (uint8_t *)malloc(USART1_TX_LEN);
    uint8_t *pucReceiveDataBuf = (uint8_t *)malloc(ulRecLen + 1);
    char caBuf[48] = {0};
    char *pcFileName = NULL;
    ENUM_ID_NO_TypeDef ucSendID;
    //SDInfo_T tSDInfo;

    union
    {
        float fValue;
        char caChar[4];
    } FtoC_U;

    if(ulRecLen != 0)
    {
        memcpy(pucReceiveDataBuf, pcStr, ulRecLen);
    }

    pucReceiveDataBuf[ulRecLen] = 0;
	
    /*#ifdef DEBUG_TARGET
    SEGGER_RTT_SetTerminal(0);
    SEGGER_RTT_printf(0, "Receive data: ");
    App_Printf("Receive data: ");
    for(int i = 0; i < ulRecLen; i++)
    {
        SEGGER_RTT_printf(0, "%#02X ", (uint8_t)pucReceiveDataBuf[i]);
        App_Printf("%#02X ", (uint8_t)pucReceiveDataBuf[i]);
    }
    SEGGER_RTT_printf(0, "\r\n");
    App_Printf("\r\n");
    #endif*/

    ucSendID = g_ucClientID;

    switch(ucCmd)
    {
        case DEVICE_HANDSHAKE: //设备联机(握手)
        {
            pucSendDataBuf[0] = DEVICE_HANDSHAKE;

            g_pcClientMac = ReadClientMac();

            for(int i = 0; i < 6; i++)
            {
                if(pucReceiveDataBuf[i] != g_pcClientMac[i])//比较接收到的MAC和存储在FLASH里的是否一样
                {
                    pucSendDataBuf[1] = FAILED;   //握手失败
                    break;
                }
                else
                {
                    pucSendDataBuf[1] = SUCCESS;   //握手成功
                }
            }           
            
            ulSendLen = 2;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
            break;
        }

        case DEVICE_PAIR: //设备配对
        {          
            g_pcClientMac = pucReceiveDataBuf;

            SaveClientMac(g_pcClientMac);//存储在外部FLASH里

            pucSendDataBuf[0] = DEVICE_PAIR;

            for (int i = 0; i < 6; i++)
            {
                pucSendDataBuf[i + 1] = g_pcClientMac[i];
            }

            ulSendLen = 7;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
            break;
        }

        case DEVICE_RESET://设备复位
        {
            pucSendDataBuf[0] = DEVICE_RESET;
            pucSendDataBuf[1] = SUCCESS;
            ulSendLen = 2;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);

            fpgaWriteByte(FPGA_SYS_RST_REG, SYS_RST);//向FPGA发设备复位 
            Delay_ms(2);
            fpgaWriteByte(FPGA_SYS_RST_REG, SYS_SET);//系统置位

            statusReset();

            break;
        }

        case DEVICE_PROTECT_RESET://设备保护复位
        {
            pucSendDataBuf[0] = DEVICE_PROTECT_RESET;
            pucSendDataBuf[1] = SUCCESS;
            ulSendLen = 2;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);

            fpgaWriteByte(FPGA_SYS_RST_REG, PROTECT_RST);//向FPGA发设备保护复位 
            Delay_ms(2);
            fpgaWriteByte(FPGA_SYS_RST_REG, PROTECT_SET);//保护置位

            break;
        }

        case QUERY_DEVICE_STATUS://查询设备状态
        {  
            pucSendDataBuf[0] = QUERY_DEVICE_STATUS;                       
            pucSendDataBuf[1] = g_ucDeviceStatusFirstByte;
            pucSendDataBuf[2] = g_ucDeviceStatusSecondByte;
            pucSendDataBuf[3] = g_ucDeviceStatusThirdByte; 
            pucSendDataBuf[4] = g_ucDeviceStatusFourthByte;
            ulSendLen        = 5;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
            break;
        }

        case QUERY_PARAMETER://查询当前发射参数
        {
            uint8_t ucTmp;

            pucSendDataBuf[0] = QUERY_PARAMETER;
            pucSendDataBuf[1] = g_ucTriggerMode;
            pucSendDataBuf[2] = g_ucSaveMode;

            if(g_ucWorkMode == TDIP_MODE)//TDIP_MODE
            {
                pucSendDataBuf[3] = g_ucPulseWidth;
                pucSendDataBuf[4] = g_ucDutyRatio;                
                ulSendLen = 5;
            }
            else if((g_ucWorkMode == FDIP_SINGLE_FREQUENCY) || (g_ucWorkMode == FDIP_DOUBLE_FREQUENCY))//FDIP
            {
                pucSendDataBuf[3] = g_ucFrequencyGroup;         //频组数
                pucSendDataBuf[4] = g_ucFrequencyRatio;         //频比
                pucSendDataBuf[5] = g_usSendCycleNumber >> 8;   //发射周期数高字节
                pucSendDataBuf[6] = g_usSendCycleNumber;        //发射周期数低字节 
                pucSendDataBuf[7] = g_ucFrequencyGroupCycle;    //频组发射间隔
                pucSendDataBuf[8] = g_ucNumberOfCycles;         //循环次数

                ucTmp = 9;
                for(uint8_t ucConut = 0; ucConut < g_ucFrequencyGroup; ucConut++)//主频
                {
                    pucSendDataBuf[ucTmp + ucConut] = g_usaMainFrequency[ucConut] >> 8;
                    pucSendDataBuf[ucTmp + 1 + ucConut] = g_usaMainFrequency[ucConut];
                    ucTmp++;
                }

                ulSendLen = 9 + g_ucFrequencyGroup * 2;
            }

            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
            break;            
        }

        case SET_TDIP_PARAMETER: //设置TDIP发射参数
        {//触发方式+存储方式+供电脉宽+占空比             
            pucSendDataBuf[0] = SET_TDIP_PARAMETER;       //回复移动端
            pucSendDataBuf[1] = SUCCESS;
            ulSendLen        = 2;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);

            g_ucWorkMode = TDIP_MODE;//0xFF:待机,0x00:TDIP，0x01:FDIP单频、0x02:FDIP双频                        
                                             
            g_ucTriggerMode  = pucReceiveDataBuf[0]; //0x00表示软件触发,0x01表示GPS时钟触发
            g_ucSaveMode     = pucReceiveDataBuf[1]; //0x00表示周期存储,0x01表示全波形存储
            g_ucSaveMode     = FULL_WAVEFORM_STORAGE;       //固定为全波形存储
            g_ucPulseWidth   = pucReceiveDataBuf[2]; //供电脉宽
            g_ucDutyRatio    = pucReceiveDataBuf[3]; //占空比

            break;
        }

        case SET_FDIP_PARAMETER://设置FDIP发射参数
        {//触发方式+存储方式+频组数+频比+发射周期数(两个字节)+频组发射间隔+循环次数+主频1+主频2+...主频N(两个字节,N为频组数)    注：发射周期数为0表示第一个频组无限循环，循环次数为0时表示频组无限循环，频比为0表示单频.0x00表示周期存储,0x01表示全波形存储
            pucSendDataBuf[0] = SET_FDIP_PARAMETER;
            pucSendDataBuf[1] = SUCCESS;         //    
            ulSendLen = 2;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);

			uint8_t ucTmp;
            g_usSendCycleNumber = 0;
		
            g_ucTriggerMode         = pucReceiveDataBuf[0];         //0x00表示软件触发,0x01表示GPS时钟触发
            g_ucSaveMode            = pucReceiveDataBuf[1];         //0x00表示周期存储,0x01表示全波形存储
            g_ucSaveMode            = FULL_WAVEFORM_STORAGE;       //固定为全波形存储
            g_ucFrequencyGroup      = pucReceiveDataBuf[2];         //频组数
            g_ucFrequencyRatio      = pucReceiveDataBuf[3];         //频比        
            g_usSendCycleNumber     = pucReceiveDataBuf[4] << 8;    //发射周期数高字节
            g_usSendCycleNumber     |= pucReceiveDataBuf[5];        //发射周期数低字节  
            g_ucFrequencyGroupCycle = pucReceiveDataBuf[6];         //频组发射间隔
            g_ucNumberOfCycles      = pucReceiveDataBuf[7];         //循环次数

			ucTmp = 8;
            for(uint8_t ucConut = 0; ucConut < g_ucFrequencyGroup; ucConut++)//主频 1~g_ucFrequencyGroup
            {				
				g_usaMainFrequency[ucConut] = 0;
                g_usaMainFrequency[ucConut] = pucReceiveDataBuf[ucTmp + ucConut] << 8;
                g_usaMainFrequency[ucConut] |= pucReceiveDataBuf[ucTmp + 1 + ucConut];
				ucTmp++;
            }   

            if(0 == g_ucFrequencyRatio)//单频
            {
                g_ucWorkMode = FDIP_SINGLE_FREQUENCY; //0xFF:待机, 0x00:TDIP，0x01:FDIP单频、0x02:FDIP双频               
            }
            else//双频
            {
                g_ucWorkMode = FDIP_DOUBLE_FREQUENCY; //0xFF:待机, 0x00:TDIP，0x01:FDIP单频、0x02:FDIP双频              
            }
            
			break;
        }

        case START_SEND://启动发射
        {            
            pucSendDataBuf[0] = START_SEND;       //回复移动端
            pucSendDataBuf[1] = SUCCESS;
            ulSendLen = 2;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);

            g_ucWorkStatus = fpgaReadByte(WORK_STATUS_REG);  //R,工作状态寄存器,0x00:待机,0x01:正供,0x02:正停供,0x03:负供,0x04:负停供
            if(g_ucWorkStatus != 0x00)//已经在工作状态
            {               
                g_ucDeviceStatusThirdByte &= (~WAIT_START_KEY);
                return;
            }
             
            if (g_ucFirstStart == 1)
            {
                g_ucSyncStart               = 1;//同步启动标志
                g_ucFirstStart              = 0;
                g_ucFirstSyncStart          = 1;
                g_ucSendToClientFlag        = 0;
                g_ucFrequencyGroupIndex     = 0;
                g_ucWaveStart               = 1;
                g_ucFrequencyGroupCycleFlag = 1;

                g_ucDeviceStatusFirstByte   = SEND_READY;
                g_ucDeviceStatusSecondByte  = g_ucWorkMode; //工作模式 0xFF:待机,0x00:TDIP，0x01:FDIP单频、0x02:FDIP双频
                g_ucDeviceStatusThirdByte  |= WAIT_START_KEY;
                
                g_ucDeviceStatusThirdByte &= ~STOP_CMD_TRIGGER_STOP;    //停止命令触发停止发射
                g_ucDeviceStatusThirdByte &= ~PROTECT_TRIGGER_STOP;     //保护触发停止发射
                g_ucDeviceStatusThirdByte &= ~STOP_KEY_TRIGGER_STOP;    //停止键触发停止发射

                if(g_ucWorkMode == TDIP_MODE)//TDIP_MODE
                {
                    xTaskCreate((TaskFunction_t)vTdipStartTask,
                                (const char *)"TdipStartTask",
                                (uint16_t)1024,
                                (void *)NULL,
                                TDIP_START_TASK_PRIO,
                                (TaskHandle_t *)&xTdipStartTask_Handle);   
                    
                }
                else if((g_ucWorkMode == FDIP_SINGLE_FREQUENCY) || (g_ucWorkMode == FDIP_DOUBLE_FREQUENCY))//FDIP
                {
                    //FDIP任务
                    xTaskCreate((TaskFunction_t)vFdipStartTask,
                        (const char *)"FdipStartTask",
                                (uint16_t)1024,
                                (void *)NULL,
                                FDIP_START_TASK_PRIO,
                                (TaskHandle_t *)&xFdipStartTask_Handle);
                }
            }
                                   
            break;
        }
  
        case STOP_SEND: //停止发射
        {    
            uint8_t ucSta1, ucSta2;

            pucSendDataBuf[0] = STOP_SEND;
            pucSendDataBuf[1] = SUCCESS;
            ulSendLen        = 2;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
            
            stopProcess();

            g_ucDeviceStatusThirdByte |= STOP_CMD_TRIGGER_STOP;//“停止”命令触发停止发射

            break;
        }    
  
        case CHARGE_CONTROL: //工作时充电控制
        {
            pucSendDataBuf[0] = CHARGE_CONTROL;
            pucSendDataBuf[1] = SUCCESS;
            ulSendLen        = 2;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);

            if(pucReceiveDataBuf[0] == 0x10) //0x10:可在工作时充电
            {
                HAL_GPIO_WritePin(EXT_POWER_CHARGE_CTRL_GPIO_PORT, EXT_POWER_CHARGE_CTRL_PIN, GPIO_PIN_SET);                
            }
            else //0x11:不可在工作时充电
            {
                HAL_GPIO_WritePin(EXT_POWER_CHARGE_CTRL_GPIO_PORT, EXT_POWER_CHARGE_CTRL_PIN, GPIO_PIN_RESET);               
            }
           
            break;
        }
               
        case GPS_SET_RTC: //GPS设置主机RTC时钟
        {
            pucSendDataBuf[0] = GPS_SET_RTC; //回复移动端
            pucSendDataBuf[1] = SUCCESS;
            ulSendLen = 2;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);

            rtcSetDate(ptGPSMsg->tUtc.ucYear, ptGPSMsg->tUtc.ucMonth, ptGPSMsg->tUtc.ucDate); //写入RTC
            rtcSetTime(ptGPSMsg->tUtc.ucHour, ptGPSMsg->tUtc.ucMinute, ptGPSMsg->tUtc.ucSecond);

            break;
        }

        case CLIENT_SET_RTC: //移动端设置主机RTC时钟
        {
            pucSendDataBuf[0] = CLIENT_SET_RTC;  //回复移动端
            pucSendDataBuf[1] = SUCCESS;
            ulSendLen = 2;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);

            rtcSetDate(pucReceiveDataBuf[0], pucReceiveDataBuf[1], pucReceiveDataBuf[2]); //写入RTC
            rtcSetTime(pucReceiveDataBuf[3], pucReceiveDataBuf[4], pucReceiveDataBuf[5]);
            
            break;
        }

        case SET_PROTECT_TEMPERATURE://设置主机保护温度
        {
            pucSendDataBuf[0] = SET_PROTECT_TEMPERATURE;  //回复移动端
            pucSendDataBuf[1] = SUCCESS;
            ulSendLen = 2;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);

            FtoC_U.caChar[0] = pucReceiveDataBuf[0];
            FtoC_U.caChar[1] = pucReceiveDataBuf[1];
            FtoC_U.caChar[2] = pucReceiveDataBuf[2];
            FtoC_U.caChar[3] = pucReceiveDataBuf[3];
            g_fProtectTemperature = FtoC_U.fValue;

            if(g_fProtectTemperature > 65)
            {
                g_fProtectTemperature = 65;
            }
            if(g_fProtectTemperature < 45)
            {
                g_fProtectTemperature = 45;
            }

            break;
        }

        case SET_BATTERY_ALARM_CAPACITY://设置电池报警剩余容量
        {
            pucSendDataBuf[0] = SET_BATTERY_ALARM_CAPACITY;  //回复移动端
            pucSendDataBuf[1] = SUCCESS;
            ulSendLen = 2;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
            
            g_ucBatteryAlarmCapacity = pucReceiveDataBuf[0];
            /*
            if(g_ucRelativeStateOfCharge < g_ucBatteryAlarmCapacity)
            {
                g_ucDeviceStatusFourthByte |= LOW_BATTERY;
            }
            else
            {
                g_ucDeviceStatusFourthByte &= ~LOW_BATTERY;
            }*/
            
            break;
        }  

        case SET_SD_ALARM_CAPACITY://设置MicroSD卡报警剩余容量
        {
            pucSendDataBuf[0] = SET_SD_ALARM_CAPACITY;  //回复移动端
            pucSendDataBuf[1] = SUCCESS;
            ulSendLen = 2;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);

            g_ucWorkStatus = fpgaReadByte(WORK_STATUS_REG);  //R,工作状态寄存器,0x00:待机,0x01:正供,0x02:正停供,0x03:负供,0x04:负停供
            if(g_ucWorkStatus != 0x00)//在工作状态
            {
                return;
            }

            //xSemaphoreTake(xFatfsMscSemaphore_Handle, portMAX_DELAY); //获取信号量，在OTG_FS_IRQHandler释放
            //HAL_NVIC_DisableIRQ(OTG_FS_IRQn);

            //tSDInfo = sdGetSDInfo();

            //xSemaphoreGive(xFatfsMscSemaphore_Handle); //释放信号量，在OTG_FS_IRQHandler获取
            //HAL_NVIC_EnableIRQ(OTG_FS_IRQn);

            FtoC_U.caChar[0] = pucReceiveDataBuf[0];
            FtoC_U.caChar[1] = pucReceiveDataBuf[1];
            FtoC_U.caChar[2] = pucReceiveDataBuf[2];
            FtoC_U.caChar[3] = pucReceiveDataBuf[3];
            g_fSDAlarmCapacity = FtoC_U.fValue;

            /*if(tSDInfo.fFreeCapacity < g_fSDAlarmCapacity)
            {
                g_ucDeviceStatusFourthByte |= SD_CAPACITY_NOT_ENOUGH;
            }
            else
            {
                g_ucDeviceStatusFourthByte &= ~SD_CAPACITY_NOT_ENOUGH;
            }*/

            break;
        }

        case QUERY_WORKING_INFO://查询当前发射信息
        {
            /******************************************************
            第1字节：
            0x00:待机，0x01: 发射准备状态，0x02:正供，0x03:正停供，0x04:负供，0x05:负停供
            第2字节：
            0x00:TDIP，0x01:FDIP单频、0x02:FDIP双频
            第3字节：
            D0:等待“启动”确认键，D1:等待同步启动触发，D2:“暂停”命令触发停止输出，D3:“停止”键触发停止发射，D4:“停止”命令触发停止发射，D5:“保护”触发停止发射
            第4字节：
            D0：保护， D1：过压， D2：过流， D3：IGBT保护， D4：过温， D5：电池通讯错误， D6：SD卡空间不足， D7：GPS定位有效标志*/

            //查询 工作状态寄存器、错误状态寄存器在vReadDeviceStatusTask任务里执行

            uint16_t ucTempVal;
                      
            pucSendDataBuf[0] = QUERY_WORKING_INFO;   
            pucSendDataBuf[1] = g_ucDeviceStatusFirstByte;
            pucSendDataBuf[2] = g_ucDeviceStatusSecondByte;
            pucSendDataBuf[3] = g_ucDeviceStatusThirdByte;
            pucSendDataBuf[4] = g_ucDeviceStatusFourthByte;
           
            if (g_ucWorkMode == TDIP_MODE)
            {
                pucSendDataBuf[5] = 0;
            }
            else if((g_ucWorkMode == FDIP_SINGLE_FREQUENCY) || (g_ucWorkMode == FDIP_DOUBLE_FREQUENCY))//FDIP
            {
                pucSendDataBuf[5] = g_ucFrequencyGroupIndex;//当前频组序号
            }

            FtoC_U.fValue    = g_fCurrent;
            pucSendDataBuf[6] = FtoC_U.caChar[0];
            pucSendDataBuf[7] = FtoC_U.caChar[1];
            pucSendDataBuf[8] = FtoC_U.caChar[2];
            pucSendDataBuf[9] = FtoC_U.caChar[3];

            g_fVoltage = readVoltage();

            FtoC_U.fValue     = g_fVoltage;
            pucSendDataBuf[10] = FtoC_U.caChar[0];
            pucSendDataBuf[11] = FtoC_U.caChar[1];
            pucSendDataBuf[12] = FtoC_U.caChar[2];
            pucSendDataBuf[13] = FtoC_U.caChar[3];
            
            ulSendLen = 14;
    
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);

            break;
        }

        case QUERY_SEND_START_TIME://查询发射启动时间
        {
            pucSendDataBuf[0] = QUERY_SEND_START_TIME;
            pucSendDataBuf[1] = tStartTime.ucHours;
            pucSendDataBuf[2] = tStartTime.ucMinutes;
            pucSendDataBuf[3] = tStartTime.ucSeconds;
            ulSendLen        = 4;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
            break;
        }
        
        case QUERY_CURRENT: //查询当前电流
        {    
            FtoC_U.fValue    = g_fCurrent;
            pucSendDataBuf[0] = QUERY_CURRENT; //回复移动端
            pucSendDataBuf[1] = FtoC_U.caChar[0];
            pucSendDataBuf[2] = FtoC_U.caChar[1];
            pucSendDataBuf[3] = FtoC_U.caChar[2];
            pucSendDataBuf[4] = FtoC_U.caChar[3];
            ulSendLen        = 5;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
            break;
        }

        case QUERY_VOLTAGE: //查询当前电压
        {
            readVoltage();
            FtoC_U.fValue    = g_fVoltage;
            pucSendDataBuf[0] = QUERY_VOLTAGE; //回复移动端
            pucSendDataBuf[1] = FtoC_U.caChar[0];
            pucSendDataBuf[2] = FtoC_U.caChar[1];
            pucSendDataBuf[3] = FtoC_U.caChar[2];
            pucSendDataBuf[4] = FtoC_U.caChar[3];
            ulSendLen        = 5;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
            break;
        }

        case QUERY_RTC_TIME: //查询主机时间
        {
            rtcGetCalendar();

            pucSendDataBuf[0] = QUERY_RTC_TIME;
            pucSendDataBuf[1] = tDateStructureget.Year;
            pucSendDataBuf[2] = tDateStructureget.Month;
            pucSendDataBuf[3] = tDateStructureget.Date;
            pucSendDataBuf[4] = tTimeStructureget.Hours;
            pucSendDataBuf[5] = tTimeStructureget.Minutes;
            pucSendDataBuf[6] = tTimeStructureget.Seconds;
            ulSendLen        = 7;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);

            break;
        }

        case QUERY_HOST_TEMPERATURE: //查询主机温度,在任务vReadSysInfTask里读取，这里直接发送信息
        {   
            //g_fTemperature = tempGetTemperature();//读取温度
            //读取温度放到任务vReadSysInfTask里读取
                                  
            FtoC_U.fValue    = g_fTemperature;
            pucSendDataBuf[0] = QUERY_HOST_TEMPERATURE; //回复移动端
            pucSendDataBuf[1] = FtoC_U.caChar[0];
            pucSendDataBuf[2] = FtoC_U.caChar[1];
            pucSendDataBuf[3] = FtoC_U.caChar[2];
            pucSendDataBuf[4] = FtoC_U.caChar[3];
            ulSendLen        = 5;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
            break;
        }

        case QUERY_GPS_STATUS: //查询GPS状态
        {
            /*#ifdef DEBUG_TARGET
            uint8_t pucaBuf[] = {"$GPRMC,083559.00,A,4717.112671,N,00833.914843,E,0.004,77.52,091202,,,A,V*57"};
            gpsNmeaAnalysis(ptGPSMsg, pucaBuf);
            #endif*/

            float fLatitude;
            float fLongitude;
            uint32_t ulInteger;
            uint32_t ulDecimals;
            float fDecimals;

            pucSendDataBuf[0] = QUERY_GPS_STATUS; //回复移动端  如:0x36+A+年+月+日+时+分+秒+纬度+N+经度+E

            pucSendDataBuf[1] = ptGPSMsg->ucGPSStatus;

            pucSendDataBuf[2] = ptGPSMsg->tUtc.ucYear;
            pucSendDataBuf[3] = ptGPSMsg->tUtc.ucMonth;
            pucSendDataBuf[4] = ptGPSMsg->tUtc.ucDate;
            pucSendDataBuf[5] = ptGPSMsg->tUtc.ucHour;
            pucSendDataBuf[6] = ptGPSMsg->tUtc.ucMinute;
            pucSendDataBuf[7] = ptGPSMsg->tUtc.ucSecond;

            //fLatitude        = (float)ptGPSMsg->ulLatitude / 10000;//纬度 ddmm.mmmm
            ulInteger          = ptGPSMsg->ulLatitude / 1000000;//dd
            ulDecimals         = ptGPSMsg->ulLatitude % 1000000;//mmmmmm          
            fDecimals          = (float)ulDecimals / 100;          
            fDecimals          = fDecimals / 60;
            fDecimals          = fDecimals / 100;
            fLatitude          = ulInteger + fDecimals;
            FtoC_U.fValue      = fLatitude;    //单位度
            pucSendDataBuf[8]   = FtoC_U.caChar[0];
            pucSendDataBuf[9]   = FtoC_U.caChar[1];
            pucSendDataBuf[10]  = FtoC_U.caChar[2];
            pucSendDataBuf[11]  = FtoC_U.caChar[3];

            pucSendDataBuf[12] = ptGPSMsg->ucSorN_Latitude;//北纬/南纬,N:北纬;S:南纬
                        
            //fLongitude          = (float)ptGPSMsg->ulLongitude / 10000;//经度 dddmm.mmmm
            ulInteger             = ptGPSMsg->ulLongitude / 1000000;//ddd
            ulDecimals            = ptGPSMsg->ulLongitude % 1000000;//mmmmmm            
            fDecimals             = (float)ulDecimals / 100;           
            fDecimals             = fDecimals / 60;
            fDecimals             = fDecimals / 100;
            fLongitude            = ulInteger + fDecimals;
            FtoC_U.fValue         = fLongitude;    //单位度
            pucSendDataBuf[13]     = FtoC_U.caChar[0];
            pucSendDataBuf[14]     = FtoC_U.caChar[1];
            pucSendDataBuf[15]     = FtoC_U.caChar[2];
            pucSendDataBuf[16]     = FtoC_U.caChar[3];

            pucSendDataBuf[17] = ptGPSMsg->ucEorW_Longitude;//东经/西经,E:东经;W:西经

            ulSendLen = 18;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);

            /*App_Printf("GPS data(HEX): ");
            for(int i = 0; i < tWifiTxMsg.usLength; i++)
            {
                App_Printf("%#02x ", (uint8_t)tWifiTxMsg.ucTxBuf[i]);
            }
            App_Printf("\r\n");
            */
            break;
        }

        case QUERY_SD_INFO: //查询MicroSD卡信息,在任务vReadSysInfTask里读取，这里直接发送信息
        {
            pucSendDataBuf[0] = QUERY_SD_INFO; //回复移动端

            //g_ucWorkStatus = fpgaReadByte(WORK_STATUS_REG);  //R,工作状态寄存器,0x00:待机,0x01:正供,0x02:正停供,0x03:负供,0x04:负停供
            //if(g_ucWorkStatus != 0x00)//在工作状态
            //{
            //    pucSendDataBuf[1] = 0;
            //    pucSendDataBuf[2] = 0;
            //    pucSendDataBuf[3] = 0;
            //    pucSendDataBuf[4] = 0;
            //    pucSendDataBuf[5] = 0;
            //    pucSendDataBuf[6] = 0;
            //    pucSendDataBuf[7] = 0;
            //    pucSendDataBuf[8] = 0;
            //    ulSendLen         = 9;
            //    TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
            //    return;
            //}
            //xSemaphoreTake(xFatfsMscSemaphore_Handle, portMAX_DELAY); //获取信号量，在操作完USB后释放
            //HAL_NVIC_DisableIRQ(OTG_FS_IRQn);
            //tSDInfo = sdGetSDInfo();
            //xSemaphoreGive(xFatfsMscSemaphore_Handle); //释放信号量，在需要操作USB的地方获取
            //HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
            
            FtoC_U.fValue = tSDInfo.fTotalCapacity;
            pucSendDataBuf[1] = FtoC_U.caChar[0];
            pucSendDataBuf[2] = FtoC_U.caChar[1];
            pucSendDataBuf[3] = FtoC_U.caChar[2];
            pucSendDataBuf[4] = FtoC_U.caChar[3];

            FtoC_U.fValue = tSDInfo.fFreeCapacity;
            pucSendDataBuf[5] = FtoC_U.caChar[0];
            pucSendDataBuf[6] = FtoC_U.caChar[1];
            pucSendDataBuf[7] = FtoC_U.caChar[2];
            pucSendDataBuf[8] = FtoC_U.caChar[3];

            /*if(tSDInfo.fFreeCapacity < g_fSDAlarmCapacity)
            {
                g_ucDeviceStatusFourthByte |= SD_CAPACITY_NOT_ENOUGH;
            }
            else
            {
                g_ucDeviceStatusFourthByte &= ~SD_CAPACITY_NOT_ENOUGH;
            }*/

            ulSendLen = 9;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
            break;
        }

        case QUERY_BAT_INFO: //查询电池信息,在任务vReadSysInfTask里读取，这里直接发送信息
        {
            pucSendDataBuf[0] = QUERY_BAT_INFO; //回复移动端
            if(g_ucExternalPower)
            {
                pucSendDataBuf[1] = 0x10; //外部供电
            }
            else
            {
                pucSendDataBuf[1] = 0x11; //电池供电
            }

            /*if((g_usBatFullChargeCapacity == 0xFFFF) || (g_usBatRelativeStateOfCharge == 0xFFFF) 
               || (g_usBatVoltage == 0xFFFF) || (g_usBatTemperature == 0xFFFF))
            {
                g_ucDeviceStatusFourthByte |= BATTERY_ERROR;
            }
            else
            {
                g_ucDeviceStatusFourthByte &= ~BATTERY_ERROR;
            }*/

            //g_usBatFullChargeCapacity = batFullChargeCapacity(); //总容量
            pucSendDataBuf[2] = g_usBatFullChargeCapacity >> 8;
            pucSendDataBuf[3] = g_usBatFullChargeCapacity;

            //g_usBatRelativeStateOfCharge = batRelativeStateOfCharge(); //相对剩余百分比
            pucSendDataBuf[4] = g_usBatRelativeStateOfCharge >> 8;
            pucSendDataBuf[5] = g_usBatRelativeStateOfCharge;
        
            //g_usBatVoltage = batVoltage(); //电池电压
            pucSendDataBuf[6] = g_usBatVoltage >> 8;
            pucSendDataBuf[7] = g_usBatVoltage;
           
            //g_usBatTemperature = batTemperature(); //电池温度
            pucSendDataBuf[8] = g_usBatTemperature >> 8;
            pucSendDataBuf[9] = g_usBatTemperature;
            
            ulSendLen = 10;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
            break;
        }

        case QUERY_CURRENT_LOG_LIST: //查询所有电流日志文件列表
        {
            static uint16_t usCount = 0, usLen;
            uint8_t ucSta;
            FRESULT eResult;

            g_ucWorkStatus = fpgaReadByte(WORK_STATUS_REG);  //R,工作状态寄存器,0x00:待机,0x01:正供,0x02:正停供,0x03:负供,0x04:负停供
            if(g_ucWorkStatus != 0x00)//在工作状态
            {
                pucSendDataBuf[0] = QUERY_CURRENT_LOG_LIST;
                pucSendDataBuf[1] = 0;          
                ulSendLen        = 2;
                TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
                return;
            }

            xSemaphoreTake(xFatfsMscSemaphore_Handle, portMAX_DELAY); //获取信号量，在OTG_FS_IRQHandler释放
            HAL_NVIC_DisableIRQ(OTG_FS_IRQn);

            //发送TDIP文件夹里的文件
            strcpy(caBuf, g_caSDPath);             //SD盘符 0:/
            strcat(caBuf, g_caTDIPFolder);         //文件夹 TDIP_MODE
            eResult = f_opendir(&tTDIPDir, caBuf); //打开目录

            if(eResult == FR_OK)
            {
                ucSta = 1;
                while(ucSta) //循环发送  序号 日志文件名
                {
                    ucSta = fatfsReadFileList(&tTDIPDir, pcFileName, &usCount);
                    if(ucSta == 0)
                    {
                        break;
                    }
                    usLen = strlen(pcFileName);

                    pucSendDataBuf[0] = usCount >> 8;
                    pucSendDataBuf[1] = usCount;
                    pucSendDataBuf[2] = ' ';

                    for(int i = 0; i < usLen; i++)
                    {
                        pucSendDataBuf[3 + i] = pcFileName[i];
                    }

                    strcpy(&pucSendDataBuf[3 + usLen], "\r");
                    strcpy(&pucSendDataBuf[3 + usLen + 1], "\n");

                    ulSendLen = 5 + usLen;

                    TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
                }
                f_closedir(&tTDIPDir);
            }
            else
            {
                f_closedir(&tTDIPDir);
                SDError();
            }

            //发送FDIP文件夹里的文件
            strcpy(caBuf, g_caSDPath);             //SD盘符 0:/
            strcat(caBuf, g_caFDIPFolder);         //文件夹 FDIP
            eResult = f_opendir(&tFDIPDir, caBuf); //打开目录

            if(eResult == FR_OK)
            {
                ucSta = 1;
                while(ucSta) //循环发送  序号 日志文件名\r\n
                {
                    ucSta = fatfsReadFileList(&tFDIPDir, pcFileName, &usCount);
                    if(ucSta == 0)
                    {
                        break;
                    }
                    usLen = strlen(pcFileName);

                    pucSendDataBuf[0] = usCount >> 8;
                    pucSendDataBuf[1] = usCount;
                    pucSendDataBuf[2] = ' ';

                    for(int i = 0; i < usLen; i++)
                    {
                        pucSendDataBuf[3 + i] = pcFileName[i];
                    }

                    ulSendLen = 3 + usLen;

                    TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
                }
                f_closedir(&tFDIPDir);
            }
            else
            {
                f_closedir(&tFDIPDir);
                SDError();
            }

            xSemaphoreGive(xFatfsMscSemaphore_Handle); //释放信号量，在OTG_FS_IRQHandler获取
            HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
            
            usCount = 0;

            pucSendDataBuf[0] = QUERY_CURRENT_LOG_LIST;
            pucSendDataBuf[1] = usCount >> 8; //文件总数
            pucSendDataBuf[2] = usCount;
            ulSendLen        = 3;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
            
            break;
        }

        case QUERY_CURRENT_LOG: //查询电流日志文件
        {
            FIL tReadFile;
            FRESULT eResult;
            UINT ByteFwd;
            uint32_t ulBytesRead;
            uint8_t *pucReadBuf = (uint8_t *)malloc(2048);

            g_ucWorkStatus = fpgaReadByte(WORK_STATUS_REG);  //R,工作状态寄存器,0x00:待机,0x01:正供,0x02:正停供,0x03:负供,0x04:负停供
            if(g_ucWorkStatus != 0x00)//在工作状态
            {
                pucSendDataBuf[0] = QUERY_CURRENT_LOG;
                pucSendDataBuf[1] = 0;
                ulSendLen        = 2;
                TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
                return;
            }

            pcFileName = pucReceiveDataBuf;

            xSemaphoreTake(xFatfsMscSemaphore_Handle, portMAX_DELAY); //获取信号量，在OTG_FS_IRQHandler释放
            HAL_NVIC_DisableIRQ(OTG_FS_IRQn);

            if(strstr(pcFileName, "TDIP") != NULL)
            {
                strcpy(caBuf, g_caSDPath);     //SD盘符 0:/
                strcat(caBuf, g_caTDIPFolder); //文件夹 TDIP_MODE
                strcat(caBuf, "/");
                strcat(caBuf, pcFileName); //文件名
            }
            else if(strstr(pcFileName, "FDIP") != NULL)
            {
                strcpy(caBuf, g_caSDPath);     //SD盘符 0:/
                strcat(caBuf, g_caFDIPFolder); //文件夹 FDIP
                strcat(caBuf, "/");
                strcat(caBuf, pcFileName); //文件名
            }
            eResult = f_open(&tReadFile, caBuf, FA_OPEN_EXISTING | FA_READ);
            if(eResult == FR_OK)
            {                
                //while(tReadFile.fptr < tReadFile.obj.objsize)
                while(!f_eof(&tReadFile))//读写指针没有到达文件末尾 
                {                    
                    //eResult = f_forward(&tReadFile, out_data, USART1_TX_LEN, &ByteFwd);//回调函数out_data传输数据
                    eResult = f_read(&tReadFile, pucReadBuf, sizeof(pucReadBuf), (UINT *)&ulBytesRead);
                    ATCmd_SendData(pucReadBuf, sizeof(pucReadBuf), g_ucClientID);
                }

                f_close(&tReadFile);
            }
            else
            {
                f_close(&tReadFile);
                SDError();
            }

            xSemaphoreGive(xFatfsMscSemaphore_Handle); //释放信号量，在OTG_FS_IRQHandler获取
            HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
                   
            pucSendDataBuf[0] = QUERY_CURRENT_LOG;
            pucSendDataBuf[1] = DATA_TRANSMIT_COMPLETE;
            ulSendLen = 2;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
            
            free(pucReadBuf);
            pucReadBuf = NULL;
            break;
        }
               
        case DELETE_ALL_FILE: //删除SD卡上所有文件
        {
            FRESULT eRES1, eRES2;

            pucSendDataBuf[0] = DELETE_ALL_FILE;
            pucSendDataBuf[1] = 0x10;
            ulSendLen        = 2;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);

            g_ucWorkStatus = fpgaReadByte(WORK_STATUS_REG);  //R,工作状态寄存器,0x00:待机,0x01:正供,0x02:正停供,0x03:负供,0x04:负停供
            if(g_ucWorkStatus != 0x00)//在工作状态
            {
                pucSendDataBuf[0] = DELETE_ALL_FILE;
                pucSendDataBuf[1] = 0;
                ulSendLen        = 2;
                TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
                return;
            }

            //这里后面改用新建任务删除所有文件？
            xSemaphoreTake(xFatfsMscSemaphore_Handle, portMAX_DELAY); //获取信号量，在OTG_FS_IRQHandler释放
            HAL_NVIC_DisableIRQ(OTG_FS_IRQn);

            //删除TDIP下面的所有文件
            memset(caBuf, 0, sizeof(caBuf));
            strcpy(caBuf, g_caSDPath);     //SD盘符 0:/
            strcat(caBuf, g_caTDIPFolder); //文件夹 TDIP_MODE
            eRES1 = fatfsDelDir(tTDIPDir, caBuf);
            if(eRES1 == FR_OK)
            {
                //删除FDIP下面的所有文件
                memset(caBuf, 0, sizeof(caBuf));
                strcpy(caBuf, g_caSDPath);     //SD盘符 0:/
                strcat(caBuf, g_caFDIPFolder); //文件夹 FDIP
                eRES2 = fatfsDelDir(tFDIPDir, caBuf);              
            }   
             
            xSemaphoreGive(xFatfsMscSemaphore_Handle); //释放信号量，在OTG_FS_IRQHandler获取
            HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
            break;
        }

        case DELETE_ASSIGN_FILE: //删除SD卡上指定文件
        {
            FIL tReadFile;
            FRESULT eResult;
            UINT ByteFwd;

            pucSendDataBuf[0] = DELETE_ASSIGN_FILE;
            pucSendDataBuf[1] = SUCCESS;
            ulSendLen        = 2;
            TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);

            g_ucWorkStatus = fpgaReadByte(WORK_STATUS_REG);  //R,工作状态寄存器,0x00:待机,0x01:正供,0x02:正停供,0x03:负供,0x04:负停供
            if(g_ucWorkStatus != 0x00)//在工作状态
            {
                pucSendDataBuf[0] = DELETE_ASSIGN_FILE;
                pucSendDataBuf[1] = 0;
                ulSendLen        = 2;
                TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);
                return;
            }

            pcFileName = pucReceiveDataBuf;

            //这里后面改用新建任务删除文件？
            xSemaphoreTake(xFatfsMscSemaphore_Handle, portMAX_DELAY); //获取信号量，在OTG_FS_IRQHandler释放
            HAL_NVIC_DisableIRQ(OTG_FS_IRQn);

            if(strstr(pcFileName, "TDIP") != NULL)
            {
                strcpy(caBuf, g_caSDPath);     //SD盘符 0:/
                strcat(caBuf, g_caTDIPFolder); //文件夹 TDIP_MODE
                strcat(caBuf, "/");
                strcat(caBuf, pcFileName); //文件名
            }
            else if(strstr(pcFileName, "FDIP") != NULL)
            {
                strcpy(caBuf, g_caSDPath);     //SD盘符 0:/
                strcat(caBuf, g_caFDIPFolder); //文件夹 FDIP
                strcat(caBuf, "/");
                strcat(caBuf, pcFileName); //文件名
            }
          
            eResult = f_unlink(caBuf);
            if(eResult != FR_OK)
            {
                SDError();              
                return;
            }

            xSemaphoreGive(xFatfsMscSemaphore_Handle); //释放信号量，在OTG_FS_IRQHandler获取
            HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
                       
            break;
        }

        default:
            break;
    }
    //TransmitList(REPLY_FLAG, pucSendDataBuf, ulSendLen, g_ucClientID);

    
    free(pucSendDataBuf);
    pucSendDataBuf = NULL;
    free(pucReceiveDataBuf);
    pucReceiveDataBuf = NULL;
}

/*******************************************************
  * @brief  : AT指令解析，+IPD,0,n:xxxxxxxxxx	//received n bytes,data=xxxxxxxxxxx
  * @note   : 举例子:Usart1ReceiveAnalysis("+IPD,0,n:xxxxxxxxxx", "+IPD,0,", AnalysisHandle);
  * @param  : pcSrcStr:需要解析的字符串
  * @param  : pcSubStr：子字符串
  * @param  : cb:回调函数指针，当找到内容后，去该函数
  * @retval :
*******************************************************/
void Usart1ReceiveAnalysis(uint8_t *pcSrcStr, uint8_t *pcSubStr, void (*cb)(uint8_t, uint8_t *, uint8_t))
{
    uint8_t *pcTempPoint = NULL;
    uint8_t ucFrameHead, ucRxCMD, ucDataLength = 0, ucRecCheckSum = 0;
    uint8_t ucLen = 0, ucCalCheckSum = 0, ucErr = 0;
    uint8_t *pucRecData = (uint8_t *)malloc(USART1_TX_LEN);

    /*    int len = strlen((char *)pcSubStr);

        pcTempPoint = (uint8_t *)strstr((char *)pcSrcStr, (char *)pcSubStr);
        //while (pcTempPoint != NULL)
        //{
            //pcSrcStr = pcTempPoint + len;
            //pcTempPoint = (uint8_t *)strstr((char *)pcSrcStr, (char *)pcSubStr);
        //}
        pcSrcStr = pcTempPoint + len; //去除+IPD,0,

       //数据总长度没用到，屏蔽
       //ucLen = *pcSrcStr; //接收到的数据总数，包括帧头、命令、长度、数据、校验和
        //冒号前的为数据总数
        uint8_t ucColonPos = 0;
        uint8_t ucaTempBuf[5] = {0};
        uint8_t ucLenCount = 0;

        for (int i = 0; i < sizeof(pcSrcStr); i++)
        {
            if (pcSrcStr[i] == ':')
            {
                ++ucColonPos;
                break;
            }
            if (ucColonPos == 0)
            {
                ucaTempBuf[ucLenCount] = pcSrcStr[i];
                ++ucLenCount;
            }
        }*/

    pcTempPoint = (uint8_t *)strchr((char *)pcSrcStr, ':'); //冒号第一次出现的位置
    pcSrcStr = pcTempPoint + 1;                             //冒号后第一个为帧头
    ucFrameHead = *pcSrcStr;                                //帧头

    #ifdef DEBUG_TARGET
    SEGGER_RTT_SetTerminal(0);
    SEGGER_RTT_printf(0, "%02d:%02d:%02d Receive CMD: ", ptGPSMsg->tUtc.ucHour, ptGPSMsg->tUtc.ucMinute, ptGPSMsg->tUtc.ucSecond);
    App_Printf("%02d:%02d:%02d Receive CMD: ", ptGPSMsg->tUtc.ucHour, ptGPSMsg->tUtc.ucMinute, ptGPSMsg->tUtc.ucSecond);
    for(int i = 0; i < 30; i++)
    {
        SEGGER_RTT_printf(0, "%#02X ", (uint8_t)pcSrcStr[i]);
        App_Printf("%#02X ", (uint8_t)pcSrcStr[i]);
    }
    SEGGER_RTT_printf(0, "\r\n");
    App_Printf("\r\n");
    #endif

    if(ucFrameHead == 0x33)
    {
        pcSrcStr++;
        ucRxCMD = *pcSrcStr; //命令
        pcSrcStr++;
        ucDataLength = *pcSrcStr; //命令帧里的数据个数
        pcSrcStr++;
        if(ucDataLength == 0) //如果数据长度为0，下一位是校验和，不为0则是数据
        {
            ucRecCheckSum = *pcSrcStr;
        }
        else
        {
            for(int i = 0; i < ucDataLength; i++)
            {
                pucRecData[i] = *pcSrcStr;
                pcSrcStr++;
            }
            ucRecCheckSum = *pcSrcStr;
        }

        ucCalCheckSum = ucFrameHead + ucRxCMD + ucDataLength;
        for(int i = 0; i < ucDataLength; i++)
        {
            ucCalCheckSum += pucRecData[i];
        }
        ucCalCheckSum = (~ucCalCheckSum) + 1;

        if(ucCalCheckSum == ucRecCheckSum)
        {
            ucErr = 0;
        }
        else
        {
            ucErr = 2; //接收到的校验和错误
        }
    } //end of if(ucFrameHead == 0x33)
    else
    {
        ucErr = 1; //接收的帧头错误
    }

    if(ucErr == 0) //接收正确
    {
        cb(ucRxCMD, pucRecData, ucDataLength);
    }
    else //接收错误，发送错误标识
    {
        uint8_t ucaSendBuf[1];

        ucaSendBuf[0] = RECEIVE_DATA_ERROR;
        ucDataLength = 1;
        
        TransmitList(REC_ERROR_FLAG, ucaSendBuf, ucDataLength, g_ucClientID);
    }

    free(pucRecData);
    pucRecData = NULL;
}

/*******************************************************
  * @brief  :
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @retval :
*******************************************************/
void Usart1_DMA_Rx_Data(void)
{
    BaseType_t pxHigherPriorityTaskWoken;

    // 关闭DMA ，防止干扰
    __HAL_DMA_DISABLE(&USART1RxDMA_Handler);

    // 清DMA标志位
    __HAL_DMA_CLEAR_FLAG(&USART1RxDMA_Handler, DMA_FLAG_TCIF2_6);

    //  重新赋值计数值，必须大于等于最大可能接收到的数据帧数目
    WRITE_REG(((DMA_Stream_TypeDef *)USART1RxDMA_Handler.Instance)->NDTR, USART1_RX_LEN);

    __HAL_DMA_ENABLE(&USART1RxDMA_Handler);

    //        //给出二值信号量，发送接收到新数据标志，供vUsart1RxTask获取
    //        xSemaphoreGiveFromISR(xUsart1RxSemaphore_Handle, &pxHigherPriorityTaskWoken); //释放二值信号量
    vTaskNotifyGiveFromISR(xUsart1RxTask_Handle, &pxHigherPriorityTaskWoken); //发送任务通知给vUsart1RxTask，用来代替上面注释的二值信号量

    ////如果需要的话进行一次任务切换，系统会判断是否需要进行切换
    //portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
}

/*******************************************************
  * @brief  :
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @return :
*******************************************************/
void USART1_IdelCallback(void)
{
    Usart1_DMA_Rx_Data(); /* 释放一个信号量，表示数据已接收 */
}

/*******************************************************
  * @brief  : UART MSP Initialization
  * @note   :  This function configures the hardware resources :
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  *           - DMA configuration for transmission request by peripheral
  *           - NVIC configuration for DMA interrupt request enable
  * @param  : uartHandle: UART handle pointer
  * @param  :
  * @param  :
  * @retval :
*******************************************************/
void HAL_UART_MspInit(UART_HandleTypeDef *uartHandle)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;

    if(uartHandle->Instance == USART1)
    {
        USART1_TX_GPIO_CLK_ENABLE();
        USART1_RX_GPIO_CLK_ENABLE();

        /* Select SysClk as source of USART1 clocks */
        RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
        RCC_PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_SYSCLK;
        HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);

        /* USART1 clock enable */
        __HAL_RCC_USART1_CLK_ENABLE();

        /**USART1 GPIO Configuration
        PB14     ------> USART1_TX
        PB15     ------> USART1_RX
        */

        /* UART TX GPIO pin configuration  */
        GPIO_InitStruct.Pin = USART1_TX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = USART1_TX_AF;

        HAL_GPIO_Init(USART1_TX_GPIO_PORT, &GPIO_InitStruct);

        /* UART RX GPIO pin configuration  */
        GPIO_InitStruct.Pin = USART1_RX_PIN;
        GPIO_InitStruct.Alternate = USART1_RX_AF;

        HAL_GPIO_Init(USART1_RX_GPIO_PORT, &GPIO_InitStruct);

        // Enable DMA clock
        __DMA2_CLK_ENABLE();

        //Configure the DMA
        //Configure the DMA handler for Transmission process
        USART1TxDMA_Handler.Instance = USART1_TX_DMA_STREAM;
        USART1TxDMA_Handler.Init.Channel = USART1_TX_DMA_CHANNEL;
        USART1TxDMA_Handler.Init.Direction = DMA_MEMORY_TO_PERIPH;
        USART1TxDMA_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
        USART1TxDMA_Handler.Init.MemInc = DMA_MINC_ENABLE;
        USART1TxDMA_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        USART1TxDMA_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        USART1TxDMA_Handler.Init.Mode = DMA_NORMAL;
        USART1TxDMA_Handler.Init.Priority = DMA_PRIORITY_HIGH;
        USART1TxDMA_Handler.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        USART1TxDMA_Handler.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
        USART1TxDMA_Handler.Init.MemBurst = DMA_MBURST_INC4;
        USART1TxDMA_Handler.Init.PeriphBurst = DMA_PBURST_INC4;

        if(HAL_DMA_Init(&USART1TxDMA_Handler) != HAL_OK)
        {
            Error_Handler();
        }

        //Associate the initialized DMA handle to the UART handle
        __HAL_LINKDMA(uartHandle, hdmatx, USART1TxDMA_Handler);

        //Configure the DMA handler for reception process
        USART1RxDMA_Handler.Instance = USART1_RX_DMA_STREAM;
        USART1RxDMA_Handler.Init.Channel = USART1_RX_DMA_CHANNEL;
        USART1RxDMA_Handler.Init.Direction = DMA_PERIPH_TO_MEMORY;
        USART1RxDMA_Handler.Init.PeriphInc = DMA_PINC_DISABLE;
        USART1RxDMA_Handler.Init.MemInc = DMA_MINC_ENABLE;
        USART1RxDMA_Handler.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        USART1RxDMA_Handler.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        USART1RxDMA_Handler.Init.Mode = DMA_NORMAL;
        USART1RxDMA_Handler.Init.Priority = DMA_PRIORITY_VERY_HIGH;
        USART1RxDMA_Handler.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        USART1RxDMA_Handler.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
        USART1RxDMA_Handler.Init.MemBurst = DMA_MBURST_INC4;
        USART1RxDMA_Handler.Init.PeriphBurst = DMA_PBURST_INC4;

        if(HAL_DMA_Init(&USART1RxDMA_Handler) != HAL_OK)
        {
            Error_Handler();
        }

        //Associate the initialized DMA handle to the the UART handle
        __HAL_LINKDMA(uartHandle, hdmarx, USART1RxDMA_Handler);

        // NVIC configuration for DMA transfer complete interrupt (USARTx_TX)
        HAL_NVIC_SetPriority(USART1_DMA_TX_IRQn, 9, 0);
        HAL_NVIC_EnableIRQ(USART1_DMA_TX_IRQn);

        // NVIC configuration for DMA transfer complete interrupt (USARTx_RX)
        HAL_NVIC_SetPriority(USART1_DMA_RX_IRQn, 9, 0);
        HAL_NVIC_EnableIRQ(USART1_DMA_RX_IRQn);

        // NVIC configuration for USART
        HAL_NVIC_SetPriority(USART1_IRQn, 8, 0);
        HAL_NVIC_EnableIRQ(USART1_IRQn);
    }
    else if(uartHandle->Instance == USART2)
    {
        // USART2 clock enable
        __HAL_RCC_USART2_CLK_ENABLE();

        USART2_TX_GPIO_CLK_ENABLE();
        USART2_RX_GPIO_CLK_ENABLE();
        /*USART2 GPIO Configuration
        PA2     ------> USART2_TX
        PA3     ------> USART2_RX
        */

        // UART TX GPIO pin configuration
        GPIO_InitStruct.Pin = USART2_TX_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
        GPIO_InitStruct.Alternate = USART2_TX_AF;

        HAL_GPIO_Init(USART2_TX_GPIO_PORT, &GPIO_InitStruct);

        // UART RX GPIO pin configuration
        GPIO_InitStruct.Pin = USART2_RX_PIN;
        GPIO_InitStruct.Alternate = USART2_RX_AF;

        HAL_GPIO_Init(USART2_RX_GPIO_PORT, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(USART2_IRQn, 8, 0);
        HAL_NVIC_EnableIRQ(USART2_IRQn);
        __HAL_UART_ENABLE_IT(uartHandle, UART_IT_RXNE); //开启接收中断
    }
}

/**
  * @brief UART MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO, DMA and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *uartHandle)
{
    if(uartHandle->Instance == USART1)
    {
        /*##-1- Reset peripherals ##################################################*/
        __USART1_FORCE_RESET();
        __USART1_RELEASE_RESET();
        /*##-2- Disable peripherals and GPIO Clocks ################################*/
        /* Configure UART Tx as alternate function  */
        HAL_GPIO_DeInit(USART1_TX_GPIO_PORT, USART1_TX_PIN);
        /* Configure UART Rx as alternate function  */
        HAL_GPIO_DeInit(USART1_RX_GPIO_PORT, USART1_RX_PIN);

        /*##-3- Disable the DMA Streams ############################################*/
        /* De-Initialize the DMA Stream associated to transmission process */
        HAL_DMA_DeInit(&USART1TxDMA_Handler);
        /* De-Initialize the DMA Stream associated to reception process */
        HAL_DMA_DeInit(&USART1RxDMA_Handler);

        /*##-4- Disable the NVIC for DMA ###########################################*/
        HAL_NVIC_DisableIRQ(USART1_DMA_TX_IRQn);
        HAL_NVIC_DisableIRQ(USART1_DMA_RX_IRQn);
        HAL_NVIC_DisableIRQ(USART1_IRQn);
    }
    else if(uartHandle->Instance == USART2)
    {
        __USART2_FORCE_RESET();
        __USART2_RELEASE_RESET();

        /*##-2- Disable peripherals and GPIO Clocks ################################*/
        /* Configure UART Tx as alternate function  */
        HAL_GPIO_DeInit(USART2_TX_GPIO_PORT, USART2_TX_PIN);
        /* Configure UART Rx as alternate function  */
        HAL_GPIO_DeInit(USART2_RX_GPIO_PORT, USART2_RX_PIN);

        /*##-3- Disable the NVIC for UART ##########################################*/
        __HAL_UART_DISABLE_IT(uartHandle, UART_IT_RXNE); //开启接收中断
        __HAL_UART_DISABLE_IT(uartHandle, UART_IT_TXE);
        HAL_NVIC_DisableIRQ(USART2_IRQn);
    }
}

//-----------------------------------------------------------------
