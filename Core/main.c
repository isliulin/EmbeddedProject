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



/**********************************************************************************
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
************************************************************************************/
TaskHandle_t xStartTask_Handle = NULL;
//TaskHandle_t xUsart1TxTask_Handle   = NULL;
TaskHandle_t xUsart1RxTask_Handle = NULL;
TaskHandle_t xInitHardwareTask_Handle = NULL;
TaskHandle_t xWriteToSDTask_Handle = NULL;
//TaskHandle_t xDataProcess_Handle    = NULL;
TaskHandle_t xLedTask_Handle = NULL;
TaskHandle_t xSaveDateTask_Handle = NULL;
TaskHandle_t xAdDataReceiveTask_Handle = NULL;
TaskHandle_t xFdipStartTask_Handle = NULL;
//TaskHandle_t xReadStartStopStatusRegTask_Handle = NULL;
//TaskHandle_t xQueryCurrentFile_Handle = NULL;
TaskHandle_t xReadIntStatusRegTask_Handle = NULL;
TaskHandle_t xReadDeviceStatusTask_Handle = NULL;
TaskHandle_t xSyncStartTask_Handle = NULL;
TaskHandle_t xNewFileTask_Handle = NULL;
TaskHandle_t xStopSendTask_Handle = NULL;
TaskHandle_t xReadSendStopFlagTask_Handle = NULL;
TaskHandle_t xTdipStartTask_Handle = NULL;
TaskHandle_t xReadSysInfTask_Handle = NULL;


#ifdef DEBUG_TARGET
TaskHandle_t xTouchTask_Handle = NULL;
TaskHandle_t xDebugTask_Handle = NULL;
TaskHandle_t xGUITask_Handle = NULL;
TaskHandle_t xFatfsTestTask_Handle = NULL;
#endif

/**********************************************************************************
 * 内核对象句柄
 * 信号量，消息队列，事件标志组，软件定时器这些都属于内核的对象，要想使用这些内核
 * 对象，必须先创建，创建成功之后会返回一个相应的句柄。实际上就是一个指针，
 * 可以通过这个句柄操作这些内核对象。
************************************************************************************/
QueueHandle_t xSDQueue_Handle = NULL;			                 //SD队列句柄
/*QueueHandle_t xUsart1TxQueue_Handle = NULL;		             //USART1发送队列句柄*/
QueueHandle_t xWriteToSDQueue_Handle = NULL;                     //写入SD卡的队列句柄

SemaphoreHandle_t xFatfsMscSemaphore_Handle = NULL;	             //FATFS和USB信号量句柄
SemaphoreHandle_t xATCmdAckSemaphore_Handle = NULL;	             //AT命令应答信号量句柄
SemaphoreHandle_t xUsart1RxSemaphore_Handle = NULL;	             //USART1接收信号量句柄
/*SemaphoreHandle_t xUsart1TxSemaphore_Handle = NULL;	         //USART1发送信号量句柄*/
/*SemaphoreHandle_t xAdDataReadableSemaphore_Handle = NULL;      //AD数据信号量句柄*/
SemaphoreHandle_t xSyncStartSemaphore_Handle = NULL;             //同步启动信号量句柄
SemaphoreHandle_t xGpsMsgReadableSemaphore_Handle = NULL;        //GPS信息信号量句柄
SemaphoreHandle_t xFdipSaveSemaphore_Handle = NULL;              //FDIP存储信号量句柄
SemaphoreHandle_t xStartSucessSemaphore_Handle = NULL;           //启动成功信号量句柄
SemaphoreHandle_t xFrequencyGroupCycleSemaphore_Handle = NULL;   //频组间隔信号量句柄
SemaphoreHandle_t xSendStopFlagSemaphore_Handle = NULL;          //发射停止标志信号量
SemaphoreHandle_t xWaveStartSemaphore_Handle = NULL;             //波形开始信号量


TimerHandle_t xGpsMsgReceiveTimer_Handle = NULL;    //GPS信息接收定时器句柄

#ifdef DEBUG_TARGET
SemaphoreHandle_t xFontSemaphore_Handle = NULL;        //字体读写完成信号量句柄
SemaphoreHandle_t xPrintfMutexSemaphore_Handle = NULL; //App_Printf互斥信号量句柄
#endif




void batteryTest(void)
{
    uint16_t usVal;
    uint8_t ucaBuf[50];
    float_t fTemp;
    int16_t sCurrent;


    usVal = batMode();
    sprintf((char *)ucaBuf, "模式：%#X", usVal);

    usVal = batStatus();
    sprintf((char *)ucaBuf, "状态：%#X", usVal);

    usVal = batTemperature(); //单位为0.1°K   开氏度＝摄氏度＋273.15 [°C] = [K] − 273.15          
    fTemp = (float)usVal / 10;
    fTemp = fTemp - 273.15;
    sprintf((char *)ucaBuf, "温度：%.2f", fTemp);

    usVal = batVoltage();
    fTemp = (float)usVal / 1000;
    sprintf((char *)ucaBuf, "电压：%.2f", fTemp);
    strcat((char *)ucaBuf, "V");

    sCurrent = batCurrent();
    fTemp = (float)sCurrent / 1000;
    sprintf((char *)ucaBuf, "电流：%.2f", fTemp);
    strcat((char *)ucaBuf, "A");

    usVal = batRelativeStateOfCharge();
    sprintf((char *)ucaBuf, "相对剩余：%d", usVal);
    strcat((char *)ucaBuf, "%");

    usVal = batAbsoluteStateOfCharge();
    sprintf((char *)ucaBuf, "绝对剩余：%d", usVal);
    strcat((char *)ucaBuf, "%");

    usVal = batRemainingCapacity();
    fTemp = (float)usVal / 1000;
    sprintf((char *)ucaBuf, "剩余电量：%.2f", fTemp);
    strcat((char *)ucaBuf, "Ah");

    usVal = batFullChargeCapacity();
    fTemp = (float)usVal / 1000;
    sprintf((char *)ucaBuf, "总容量：%.2f", fTemp);
    strcat((char *)ucaBuf, "Ah");

    usVal = batChargingCurrent();
    fTemp = (float)usVal / 1000;
    sprintf((char *)ucaBuf, "充电电流：%.2f", fTemp);
    strcat((char *)ucaBuf, "A");

    usVal = batChargingVoltage();
    fTemp = (float)usVal / 1000;
    sprintf((char *)ucaBuf, "充电电压：%.2f", fTemp);
    strcat((char *)ucaBuf, "V");

    usVal = batSerialNumber();
    sprintf((char *)ucaBuf, "序列号：%#X", usVal);




}

void FdipTest1(void)
{
    uint32_t ulDelayTime;
    float fTempVal;
    uint8_t ucNumberOfCycles = 0, ucConut = 0;
    uint8_t ucSta;

    g_ucTriggerMode = 0x01;         //0x00表示软件触发,0x01表示GPS时钟触发
    g_ucSaveMode = 0x00;         //0x00表示周期存储,0x01表示全波形存储
    g_ucFrequencyGroup = 4;            //频组数
    g_ucFrequencyRatio = 0x0D;           //频比        
    g_usSendCycleNumber = 0x00;         //发射周期数高字节
    g_usSendCycleNumber |= 0x08;        //发射周期数低字节  
    g_ucFrequencyGroupCycle = 0x02;         //频组发射间隔
    g_ucNumberOfCycles = 0x00;         //循环次数

    g_usaMainFrequency[0] = 0x000A;
    g_usaMainFrequency[1] = 0x0004;
    g_usaMainFrequency[2] = 0x0002;
    g_usaMainFrequency[3] = 0x0001;

    g_ucWorkMode = FDIP_DOUBLE_FREQUENCY; //0xFF:待机, 0x00:TDIP，0x01:FDIP单频、0x02:FDIP双频 


    if(g_ucNumberOfCycles == 0)//循环次数为0时表示无限循环
    {
        for(;;)
        {
            for(ucConut = 0; ucConut < g_ucFrequencyGroup; ucConut++)
            {
                g_ucFrequencyGroupIndex++;

                FDIP_Init(g_usaMainFrequency[ucConut], g_ucFrequencyRatio, g_usSendCycleNumber);
                fpgaWriteByte(FPGA_SEND_CTRL_REG, SEND_START_CTRL);    //发射开始
                g_ucSendStopFlag = 0;

                //启动命令后第二个PPS来后开始发送，频组间的总间隔从这开始
                //xSemaphoreTake(xFDIPSyncStartSemaphore_Handle, portMAX_DELAY);//vGpsMsgReceiveCallback释放

                if(g_ucWorkMode == FDIP_SINGLE_FREQUENCY)//单频
                {
                    fTempVal = (float)1 / g_usaMainFrequency[ucConut];//高频周期
                }
                else if(g_ucWorkMode == FDIP_DOUBLE_FREQUENCY)//双频
                {
                    g_fLowFrequency = (float)g_usaMainFrequency[ucConut] / g_ucFrequencyRatio; //低频频率
                    fTempVal = (float)1 / g_fLowFrequency;//低频周期
                }

                fTempVal = fTempVal * g_usSendCycleNumber;
                ulDelayTime = (uint32_t)fTempVal;

                Delay_ms(fTempVal * 1000);//一个频组需要的时间

                for(;;)
                {
                    ucSta = fpgaReadByte(SEND_STOP_FLAG);
                    FMC_SRAM_ReadBuffer(&ucSta, 0x00000083, 1);

                    if((ucSta & END_OF_SEND_PERIOD) == END_OF_SEND_PERIOD) //发射周期结束
                    {
                        if(g_ucSendStopFlag == 0)
                        {
                            g_ucSendStopFlag = 1;
                            break;
                        }
                    }
                }
                //xSemaphoreTake(xSendStopFlagSemaphore_Handle, portMAX_DELAY);//vReadSendStopFlagTask释放,发射周期结束
                SyncStop();//一个频组完

                Delay_ms(g_ucFrequencyGroupCycle * 1000);//频组发射间隔
            }
        }
    }
    else//循环次数不为0
    {
        for(ucNumberOfCycles = 0; ucNumberOfCycles < g_ucNumberOfCycles; ucNumberOfCycles++)
        {
            for(uint8_t ucConut = 0; ucConut < g_ucFrequencyGroup; ucConut++)
            {
                g_ucFrequencyGroupIndex++;

                FDIP_Init(g_usaMainFrequency[ucConut], g_ucFrequencyRatio, g_usSendCycleNumber);
                fpgaWriteByte(FPGA_SEND_CTRL_REG, SEND_START_CTRL);    //发射开始
                g_ucSendStopFlag = 0;

                //启动命令后第二个PPS来后开始发送，频组间的总间隔从这开始
                //xSemaphoreTake(xFDIPSyncStartSemaphore_Handle, portMAX_DELAY);//vGpsMsgReceiveCallback释放

                if(g_ucWorkMode == FDIP_SINGLE_FREQUENCY)//单频
                {
                    fTempVal = (float)1 / g_usaMainFrequency[ucConut];//高频周期
                }
                else if(g_ucWorkMode == FDIP_DOUBLE_FREQUENCY)//双频
                {
                    g_fLowFrequency = (float)g_usaMainFrequency[ucConut] / g_ucFrequencyRatio; //低频频率
                    fTempVal = (float)1 / g_fLowFrequency;//低频周期
                }

                fTempVal = fTempVal * g_usSendCycleNumber;
                ulDelayTime = (uint32_t)fTempVal;

                Delay_ms(fTempVal * 1000);//一个频组需要的时间

                for(;;)
                {
                    ucSta = fpgaReadByte(SEND_STOP_FLAG);

                    if((ucSta & END_OF_SEND_PERIOD) == END_OF_SEND_PERIOD) //发射周期结束
                    {
                        if(g_ucSendStopFlag == 0)
                        {
                            g_ucSendStopFlag = 1;
                            break;
                        }
                    }
                }
                SyncStop();//一个频组完

                Delay_ms(g_ucFrequencyGroupCycle * 1000);//频组发射间隔
            }
        }
    }//循环次数不为0
}

void FdipTest(void)
{
    uint8_t ucaWriteBuf[10] = {0x00, 0x3e, 0x70, 0x60, 0, 0xf9, 0xc1, 0x80, 0xaa, 9};
    uint8_t ucaReadBuf[600] = {0};
    uint8_t ucvBuf0[2000] = {0};
//    uint8_t ucvBuf1[600] = { 0 };	
//    uint8_t ucvBuf2[600] = { 0 };		
    uint32_t  i, j, index;
    uint32_t raddr, waddr, rnum, wnum, v_addr;
    uint8_t wvalue, rvalue;

    waddr = 0x00000060;
    wnum = 1;
    raddr = 0x00000060;
    rnum = 1;
    v_addr = 0x0000000d;

    //***********************************************
    //系统复位
    wvalue = 0x00;
    FMC_SRAM_WriteBuffer(&wvalue, 0x00, 1);
    //停止发射	
    FMC_SRAM_WriteBuffer(&wvalue, 0x20, 1);
    //保护复位
    FMC_SRAM_WriteBuffer(&wvalue, 0xf0, 1);
    //采集复位	
    FMC_SRAM_WriteBuffer(&wvalue, 0x53, 1);

    //采集命令禁止
    FMC_SRAM_WriteBuffer(&wvalue, 0x56, 1);
    //ADS1255命令禁止
    FMC_SRAM_WriteBuffer(&wvalue, 0x54, 1);
    //ADFIFO复位
    wvalue = 0x01;
    FMC_SRAM_WriteBuffer(&wvalue, 0x55, 1);
    //系统复位完成
    wvalue = 0x03;
    FMC_SRAM_WriteBuffer(&wvalue, 0x00, 1);
    //保护复位结束
    FMC_SRAM_WriteBuffer(&wvalue, 0xf0, 1);
    //采集复位结束	
    FMC_SRAM_WriteBuffer(&wvalue, 0x53, 1);
    //ADFIFO复位结束
    wvalue = 0x00;
    FMC_SRAM_WriteBuffer(&wvalue, 0x55, 1);

    wvalue = 0x00;
    FMC_SRAM_WriteBuffer(&wvalue, 0x04, 1);
//	wvalue = 0x55;
//        for(;;)	
//		{		
//			FMC_SRAM_WriteBuffer(&wvalue, 0x60, 1);	
//			FMC_SRAM_ReadBuffer(&rvalue, 0x60, 1); 
//		}	

    //******************************************
    /*
    //TDIP_MODE
    wvalue  = 0x00;
    //工作模式
    FMC_SRAM_WriteBuffer(&wvalue, 0x21, 1);
    //供电时间计数
    FMC_SRAM_WriteBuffer(ucaWriteBuf, 0x22, 4);
    //停供时间计数
    FMC_SRAM_WriteBuffer(ucaWriteBuf, 0x26, 4);
    */
    wvalue = 0x00;
    FMC_SRAM_WriteBuffer(&wvalue, 0xf5, 1);

    //FDIP2F
    wvalue = 0x02;
    FMC_SRAM_WriteBuffer(&wvalue, 0x21, 1);	 //工作模式
    ucaWriteBuf[0] = 0x00;
    ucaWriteBuf[1] = 0x0C;
    ucaWriteBuf[2] = 0x7C;
    ucaWriteBuf[3] = 0xE0; //10Hz 	
    FMC_SRAM_WriteBuffer(ucaWriteBuf, 0x2a, 4);	//高频半周期计数值
    ucaWriteBuf[0] = 0x00;
    ucaWriteBuf[1] = 0x00; 	 //不作校正
    FMC_SRAM_WriteBuffer(ucaWriteBuf, 0x2f, 2);	//高频校正半周期计数值
    ucaWriteBuf[0] = 0x00;
    ucaWriteBuf[1] = 0xD0; //连续发射	
    FMC_SRAM_WriteBuffer(ucaWriteBuf, 0x31, 2);	//高频发射半周期计数值		

    ucaWriteBuf[0] = 0x00;
    ucaWriteBuf[1] = 0xA2;
    ucaWriteBuf[2] = 0x57;
    ucaWriteBuf[3] = 0x60; 	//10/13Hz
    FMC_SRAM_WriteBuffer(ucaWriteBuf, 0x33, 4);	//低频半周期计数值
    ucaWriteBuf[0] = 0x00;
    ucaWriteBuf[1] = 0x00; 	 //不作校正
    FMC_SRAM_WriteBuffer(ucaWriteBuf, 0x37, 2);	//低频校正半周期计数值
    ucaWriteBuf[0] = 0x00;
    ucaWriteBuf[1] = 0x10; //连续发射	
    FMC_SRAM_WriteBuffer(ucaWriteBuf, 0x39, 2);	//低频发射半周期计数值			

    //***********************************************

    g_ucFrequencyGroup = 4;            //频组数
    g_ucFrequencyRatio = 0x0D;           //频比        
    g_usSendCycleNumber = 0x00;         //发射周期数高字节
    g_usSendCycleNumber |= 0x08;        //发射周期数低字节  
    g_ucFrequencyGroupCycle = 0x02;         //频组发射间隔
    g_ucNumberOfCycles = 0x00;         //循环次数

    g_usaMainFrequency[0] = 0x000A;
    g_usaMainFrequency[1] = 0x0004;
    g_usaMainFrequency[2] = 0x0002;
    g_usaMainFrequency[3] = 0x0001;


    FDIP_Init(g_usaMainFrequency[0], g_ucFrequencyRatio, g_usSendCycleNumber);

    wvalue = 0x01;
    //发射启动
    FMC_SRAM_WriteBuffer(&wvalue, 0x20, 1);


    rnum = 0x55;
    wvalue = 0x55;
    j = 1;
    //
    for(;;)
    {

        for(;;)
        {
            waddr = 0x00000083;  //发射停止状态
            FMC_SRAM_ReadBuffer(&rvalue, waddr, 1);
            if(rvalue) break;
        }

        for(i = 0; i < j; i++)
        {
            waddr = 0x00000020;//发射启停
            wvalue = 0x00;
            FMC_SRAM_WriteBuffer(ucaWriteBuf, waddr, 1);
        }
    }

    wvalue = 0x20;
    FMC_SRAM_WriteBuffer(&wvalue, 0x04, 1);
    //

    for(;;)
    {
        FMC_SRAM_ReadBuffer(&rvalue, 0x03, 1);
        if((rvalue & 0x20) == 0x00) break;
    }


    for(;;)
    {
        index = 0;
        for(j = 0; j < 19; j++)
        {
            do
            {
                FMC_SRAM_ReadBuffer(&rvalue, 0x03, 1); //读中断标志寄存器
                rvalue = rvalue & 0x01;
                FMC_SRAM_ReadBuffer(&rvalue, 0x82, 1);
            } while(rvalue != 0x01);

            for(i = 0; i < 96; i++)
            {
                FMC_SRAM_ReadBuffer(&ucvBuf0[j * 96 + i], 0xf1, 1); //读电流数据FIFO	
                //FMC_SRAM_WriteBuffer(&wvalue, 0x60, 1);	
            }
        }

        FMC_SRAM_ReadBuffer(&rvalue, 0x82, 1);
    }
}


void SRAMTest(void)
{
    uint8_t ucaWriteBuf[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0};
    uint8_t ucaReadBuf[10] = {0};

    Delay_ms(5);
    FMC_SRAM_WriteBuffer(ucaWriteBuf, 0x60, 1);
    Delay_ms(5);
    FMC_SRAM_ReadBuffer(ucaReadBuf, 0x60, 1);

}

float faBuf[20] = {0};
uint8_t ucCount = 0;

void dataProcessTest(int32_t *piOriginData)
{
    int32_t iaDest[AD_FIFO_LEN / 3] = {0};
    int32_t iSum = 0;
    float fIvalue = 0;

    dataBubbleSortFilter(piOriginData, iaDest, AD_FIFO_LEN / 3, 10, 10); //排序，并丢弃最大和最小的10个数据

    for(int i = 0; i < (AD_FIFO_LEN / 3 - 20); i++)
    {
        iSum = iSum + iaDest[i];
    }
    fIvalue = (float)iSum / (AD_FIFO_LEN / 3 - 20);//取平均
    fIvalue = fIvalue * KAD;//得到电流值
    fIvalue = fIvalue / 2;//默认增益2倍

    faBuf[ucCount] = fIvalue;
    ucCount++;

/*
    if(fIvalue > g_fImax)
    {
        g_fImax = fIvalue;
    }
    if(fIvalue < g_fImin)
    {
        g_fImin = fIvalue;
    }
    g_fCurrent = (float)(g_fImax - g_fImin) / 2;*/

    #ifdef DEBUG_TARGET  
    printf("fIvalue    = %f\r\n", fIvalue);
    //printf("g_fCurrent = %f\r\n", g_fCurrent);
    //SEGGER_RTT_SetTerminal(2);
    SEGGER_RTT_printf(0, "I = %f\r\n", fIvalue);//不支持浮点数
    //printf("Imin=:%f\r\n", g_fImin);
    #endif

}

void TdipTest(void)
{
    uint8_t ucaWriteBuf[10] = {0x01, 0xf3, 0x83, 0x00, 0, 0xf9, 0xc1, 0x80, 0xaa, 9};
    uint8_t ucaReadBuf[500] = {0};
    uint8_t  i, raddr, waddr, rnum, wnum;
    uint8_t wvalue, rvalue;
    static uint32_t ulCount = 0;

    waddr = 0;
    wnum = 1;
    raddr = 0;
    rnum = 1;

    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn); //开 PA8：FPGA_PPS_PIN中断

    BSP_SRAM_Init();
/*
    //系统复位
    wvalue = 0x00;
    FMC_SRAM_WriteBuffer(&wvalue, 0x00, 1);
    //停止发射
    FMC_SRAM_WriteBuffer(&wvalue, 0x20, 1);
    //保护复位
    FMC_SRAM_WriteBuffer(&wvalue, 0xf0, 1);
       //采集复位
    FMC_SRAM_WriteBuffer(&wvalue, 0x53, 1);

    //采集命令禁止
    FMC_SRAM_WriteBuffer(&wvalue, 0x56, 1);
    //ADS1255命令禁止
    FMC_SRAM_WriteBuffer(&wvalue, 0x54, 1);
       //ADFIFO复位
    wvalue = 0x01;
    FMC_SRAM_WriteBuffer(&wvalue, 0x55, 1);

    for(i = 0; i < 10; i++)
        FMC_SRAM_ReadBuffer(ucaReadBuf, 0x00, 1);

    //系统复位完成
    wvalue = 0x01;
    FMC_SRAM_WriteBuffer(&wvalue, 0x00, 1);
    //保护复位结束
    FMC_SRAM_WriteBuffer(&wvalue, 0xf0, 1);
       //采集复位结束
    FMC_SRAM_WriteBuffer(&wvalue, 0x53, 1);
       //ADFIFO复位结束
    wvalue = 0x00;
    FMC_SRAM_WriteBuffer(&wvalue, 0x55, 1);

    FMC_SRAM_ReadBuffer(ucaReadBuf, 0xf1, 10);

    wvalue = 0x00;
       //工作模式
    FMC_SRAM_WriteBuffer(&wvalue, 0x21, 1);

       //供电时间计数
    FMC_SRAM_WriteBuffer(ucaWriteBuf, 0x22, 4);
       //停供时间计数
    FMC_SRAM_WriteBuffer(ucaWriteBuf, 0x26, 4);

    wvalue = 0x53;
    FMC_SRAM_WriteBuffer(&wvalue, 0xf3, 1);
    //wvalue = 0xf0;
    wvalue = 0xA1;
    FMC_SRAM_WriteBuffer(&wvalue, 0xf4, 1);
    wvalue = 0x01;
    FMC_SRAM_WriteBuffer(&wvalue, 0x54, 1);
    for(i = 0; i < 10; i++)
        FMC_SRAM_ReadBuffer(ucaReadBuf, 0x00, 1);
    wvalue = 0x00;
    FMC_SRAM_WriteBuffer(&wvalue, 0x54, 1);

    wvalue = 0xf0;
    FMC_SRAM_WriteBuffer(&wvalue, 0xf3, 1);
    wvalue = 0x00;
    FMC_SRAM_WriteBuffer(&wvalue, 0x54, 1);
    for(i = 0; i < 10; i++)
        FMC_SRAM_ReadBuffer(ucaReadBuf, 0x00, 1);
    wvalue = 0x00;
    FMC_SRAM_WriteBuffer(&wvalue, 0x54, 1);

    wvalue = 0x01;
       //发射启动
    FMC_SRAM_WriteBuffer(&wvalue, 0x20, 1);
       //采集启动
    FMC_SRAM_WriteBuffer(&wvalue, 0x56, 1);
    */

/*
    g_ucFrequencyGroup = 1;
    g_ucFrequencyRatio = 0;
    g_usSendCycleNumber = 0;
    g_usSendCycleNumber |= 0;
    g_ucFrequencyGroupCycle = 0;
    g_ucNumberOfCycles = 0;
    g_usaMainFrequency[0] = 1;
    FDIP_Init(g_usaMainFrequency[0], g_ucFrequencyRatio, g_usSendCycleNumber);
    */

    g_ucPulseWidth = 32;    //供电周期
    g_ucDutyRatio = 50;   //占空比

    TDIP_Init();


    fpgaWriteByte(FPGA_SEND_CTRL_REG, SEND_START_CTRL);    //发射开始
    fpgaWriteByte(FPGA_SAMPLE_CMD_REG, SAMPLE_ENABLE);     //采样使能

    rnum = 48;

    for(;;)
    //for(int k = 0; k < 5; k++)
    {
        do
        {
            //FMC_SRAM_ReadBuffer(&rvalue, 0x03, 1); //
            rvalue = fpgaReadByte(FPGA_INT_STATUS_REG);
            rvalue = rvalue & 0x01;
        } while(rvalue != 0x01);
        for(i = 0; i < 96; i++)
        {
            //ucaReadBuf[k * 96 + i] = fpgaReadByte(FPGA_AD_FIFO_REG);
            ucaReadBuf[i] = fpgaReadByte(FPGA_AD_FIFO_REG);
            /*printf("%#02X ", (uint8_t)ucaReadBuf[i]);
            SEGGER_RTT_printf(0, "%#02X ", ucaReadBuf[i]);*/
        }


        int iAdFifoCount = 0;
        int32_t iaCurrent[AD_FIFO_LEN / 3] = {0};

        //AD原始数据转换为电流数据
        for(int32_t iCurrentCount = 0; iCurrentCount < (AD_FIFO_LEN / 3); iCurrentCount++)
        {
            while(iAdFifoCount < AD_FIFO_LEN)
            {
                uIntData.tBytes.ucHighByte = ucaReadBuf[iAdFifoCount];
                uIntData.tBytes.ucMidByte = ucaReadBuf[iAdFifoCount + 1];
                uIntData.tBytes.ucLowByte = ucaReadBuf[iAdFifoCount + 2];

                if((ucaReadBuf[iAdFifoCount] & 0x80) == 0x80)
                {
                    uIntData.tBytes.ucSignByte = 0xFF;
                }
                else
                {
                    uIntData.tBytes.ucSignByte = 0x00;
                }
                iAdFifoCount = iAdFifoCount + 3;
                break;
            }
            iaCurrent[iCurrentCount] = uIntData.iResult;
        }
        dataProcessTest(iaCurrent);
        //Delay_ms(20);
        //FMC_SRAM_ReadBuffer(&rvalue, 0x03, 1);
    }
    FMC_SRAM_ReadBuffer(&rvalue, 0x03, 1);
}


/*******************************************************
    @brief  :
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
int main(void)
{
    BSP_Init(); //Configure the hardware

    //配置通道 0，上行配置
    SEGGER_RTT_ConfigUpBuffer(0, "RTTUP", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);

    //配置通道 0，下行配置
    SEGGER_RTT_ConfigDownBuffer(0, "RTTDOWN", NULL, 0, SEGGER_RTT_MODE_NO_BLOCK_SKIP);



//        uint8_t pucaBuf[] = {"$GPRMC,083559.00,A,4717.112671,N,00833.914843,E,0.004,77.52,091202,,,A,V*57"};
//        gpsNmeaAnalysis(ptGPSMsg, pucaBuf);

    
    /*for (;;)
    {      
        g_fTemperature = tempGetTemperature();//读取温度
        Delay_ms(500);
    }*/
        

    /*for(;;)
    {
        TdipTest();
    }*/

 /*
    HAL_NVIC_DisableIRQ(EXTI3_IRQn);//PB3，EXTERNAL_POWER_PIN

    HAL_NVIC_DisableIRQ(EXTI9_5_IRQn); //开 PA8：FPGA_PPS_PIN中断
    for (;;)
    {
        FdipTest1();
    }
 */


    // 初始化 EventRecorder 并开启
    EventRecorderInitialize(EventRecordAll, 1U);
    EventRecorderStart();
//		EventStartA(0);
//		EventStopA(0);


    //创建开始任务
    xTaskCreate((TaskFunction_t)vStartTask,            //任务函数
                (const char *)"start_task",          //任务名称
                (uint16_t)256,        //任务堆栈大小
                (void *)NULL,                  //传递给任务函数的参数
                START_TASK_PRIO,       //任务优先级
                (TaskHandle_t *)&xStartTask_Handle);   //任务句柄  

   // xUsart1TxQueue_Handle = xQueueCreate(QUEUE_SIZE, sizeof(struct USART1TxMessage_T));	//USART1发送队列，队列是需要发送的数据	
    xWriteToSDQueue_Handle = xQueueCreate(QUEUE_SIZE, sizeof(struct WriteToSD_T));//SD卡写入队列

    xFatfsMscSemaphore_Handle            = xSemaphoreCreateBinary();		  //FATFS和USB MSC二进制信号量
    xATCmdAckSemaphore_Handle            = xSemaphoreCreateBinary();		  //AT命令应答信号量	
    xUsart1RxSemaphore_Handle            = xSemaphoreCreateBinary();		  //Usart1接收信号量
    xFontSemaphore_Handle                = xSemaphoreCreateBinary();          //字体读写完成信号量句柄
    xPrintfMutexSemaphore_Handle         = xSemaphoreCreateMutex();           //printf互斥信号量句柄
    xSyncStartSemaphore_Handle           = xSemaphoreCreateBinary();          //同步启动信号量
    xGpsMsgReadableSemaphore_Handle      = xSemaphoreCreateBinary();          //GPS信息信号量
    xFdipSaveSemaphore_Handle            = xSemaphoreCreateBinary();          //FDIP存储信号量
    xFrequencyGroupCycleSemaphore_Handle = xSemaphoreCreateBinary();          //频组间隔信号量
    xSendStopFlagSemaphore_Handle        = xSemaphoreCreateBinary();
    xWaveStartSemaphore_Handle           = xSemaphoreCreateBinary();

    xGpsMsgReceiveTimer_Handle = xTimerCreate((const char *)"GpsMsgReceiveTimer",
                                              pdMS_TO_TICKS(100),
                                              (UBaseType_t)pdTRUE,
                                              (void *)0,
                                              (TimerCallbackFunction_t)vGpsMsgReceiveCallback);   //GPS信息接收定时器
    if (xGpsMsgReceiveTimer_Handle != NULL)
    {
        xTimerStart(xGpsMsgReceiveTimer_Handle, 0);//启动GPS信息接收定时器
    }
            
    vTaskStartScheduler();          //开启任务调度
}

/*******************************************************
    @brief  : 开始任务任务函数
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
void vStartTask(void *pvParameters)
{
    (void)pvParameters;

    taskENTER_CRITICAL(); //进入临界区

    //USART1 DMA 接收任务
    xTaskCreate((TaskFunction_t)vUsart1RxTask,
                (const char *)"Usart1RxTask",
                (uint16_t)4096,
                (void *)NULL,
                USART1_RX_TASK_PRIO,
                (TaskHandle_t *)&xUsart1RxTask_Handle);
/*
    ////USART1 DMA 发送任务
    //xTaskCreate((TaskFunction_t)vUsart1TxTask,
                //(const char *  )"Usart1TxTask",
                //(uint16_t      )768,
                //(void *        )NULL,
                //(UBaseType_t   )11,
                //(TaskHandle_t *)&xUsart1TxTask_Handle);

    //数据处理任务
    xTaskCreate((TaskFunction_t)vDataProcessTask,
        (const char *)"DataProcessTask",
                (uint16_t)1024,
                (void *)NULL,
                (UBaseType_t)11,
                (TaskHandle_t *)&xDataProcess_Handle);*/

   //初始化硬件任务，初始化后删除任务本身
    xTaskCreate((TaskFunction_t)vInitHardwareTask,
                (const char *)"InitHardwareTask",
                (uint16_t)1024,
                (void *)NULL,
                INIT_HARDWARE_TASK_PRIO,
                (TaskHandle_t *)&xInitHardwareTask_Handle);

    //SD卡写入任务
    xTaskCreate((TaskFunction_t)vWriteToSDTask,
                (const char *)"WriteToSDTask",
                (uint16_t)2048,
                (void *)NULL,
                (UBaseType_t)WRITE_TO_SD_TASK_PRIO,
                (TaskHandle_t *)&xWriteToSDTask_Handle);

    //读中断状态寄存器
    xTaskCreate((TaskFunction_t)vReadIntStatusRegTask,
                (const char *)"ReadIntStatusRegTask",
                (uint16_t)256,
                (void *)NULL,
                READ_INT_STATUS_REG_TASK_PRIO,
                (TaskHandle_t *)&xReadIntStatusRegTask_Handle);

    //读设备状态字
    xTaskCreate((TaskFunction_t)vReadDeviceStatusTask,
                (const char *)"ReadDeviceStatusWordTask",
                (uint16_t)256,
                (void *)NULL,
                READ_DEVICE_STATUS_TASK_PRIO,
                (TaskHandle_t *)&xReadDeviceStatusTask_Handle);

    //创建LED任务，用作心跳灯
    xTaskCreate((TaskFunction_t)vLedTask,
                (const char *)"LedTask",
                (uint16_t)64,
                (void *)NULL,
                LED_TASK_PRIO,
                (TaskHandle_t *)&xLedTask_Handle);

    //创建任务，读取系统信息
    xTaskCreate((TaskFunction_t)vReadSysInfTask,
                (const char *)"ReadSysInfTask",
                (uint16_t)256,
                (void *)NULL,
                READ_SYS_INF_TASK_PRIO,
                (TaskHandle_t *)&xReadSysInfTask_Handle);


    #ifdef DEBUG_TARGET
    /*
        //Fatfs测试任务
        xTaskCreate((TaskFunction_t)vFatfsTestTask,
                    (const char *  )"FatfsTestTask",
                    (uint16_t      )640,
                    (void *        )NULL,
                    (UBaseType_t   )4,
                    (TaskHandle_t *)&xFatfsTestTask_Handle);

        //创建触摸任务
        xTaskCreate((TaskFunction_t)vTouchTask,
                    (const char *  )"TouchTask",
                    (uint16_t      )256,
                    (void *        )NULL,
                    (UBaseType_t   )3,
                    (TaskHandle_t *)&xTouchTask_Handle);


        //创建GUI任务
        xTaskCreate((TaskFunction_t)vGUITask,
                    (const char *  )"GUITask",
                    (uint16_t      )2048,
                    (void *        )NULL,
                    (UBaseType_t   )1,
                    (TaskHandle_t *)&xGUITask_Handle);
                    (
 */
       
    //创建Debug任务
    xTaskCreate((TaskFunction_t)vDebugTask,
                    (const char *  )"DebugTask",
                    (uint16_t      )1024,
                    (void *        )NULL,
                    DEBUG_TASK_PRIO,
                    (TaskHandle_t *)&xDebugTask_Handle);


    #endif

    vTaskDelete(xStartTask_Handle); //删除开始任务

    taskEXIT_CRITICAL(); //退出临界区
}


/*******************************************************
  * @brief  :
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @retval :
*******************************************************/
void vInitHardwareTask(void *pvParameters)
{  
    (void)pvParameters;

    BSP_FATFS_Init();
        
    USB_DEVICE_Init();
 
    //HAL_NVIC_DisableIRQ(OTG_FS_IRQn);
    //tSDInfo = sdGetSDInfo();
    xSemaphoreGive(xFatfsMscSemaphore_Handle); //释放信号量，在需要操作USB的地方获取
    //HAL_NVIC_EnableIRQ(OTG_FS_IRQn);

    //ReadSN(); //仪器编号

    BSP_ESP8266_Init(); //WIFI

    // EXTI interrupt init
    HAL_NVIC_SetPriority(EXTI1_IRQn, 15, 0); //PA1，GPS_PPS_PIN
    //HAL_NVIC_EnableIRQ(EXTI1_IRQn);//开 PA1，GPS_PPS_PIN中断

    HAL_NVIC_SetPriority(EXTI3_IRQn, 10, 0); //PB3，EXTERNAL_POWER_PIN
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 7, 0); //PB5，FPGA_IRQ_PIN。 PA8，FPGA_PPS_PIN
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn); //开 PA8：FPGA_PPS_PIN中断

    /*
    g_fTemperature               = tempGetTemperature();       //读取温度
    g_usBatFullChargeCapacity    = batFullChargeCapacity();    //电池总容量
    g_usBatRelativeStateOfCharge = batRelativeStateOfCharge(); //电池相对剩余百分比
    g_usBatVoltage               = batVoltage();               //电池电压
    g_usBatTemperature           = batTemperature();           //电池温度

    App_Printf("First %f\r\n", g_fTemperature);
    */
    #ifdef DEBUG_TARGET

    //Fatfs_RW_test();

        //guiCheckFont();

    #endif

    vTaskDelete(xInitHardwareTask_Handle); //删除
}

/*******************************************************
  * @brief  :
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @return :
*******************************************************/
void vLedTask(void *pvParameters)
{
    (void)pvParameters;

    for(;;)
    {
        /*
        rtcGetCalendar();
        App_Printf("%d/%d/%d\r\n", tDateStructureget.Year, tDateStructureget.Month, tDateStructureget.Date);
        App_Printf("%d:%d:%d\r\n", tTimeStructureget.Hours, tTimeStructureget.Minutes, tTimeStructureget.Seconds);
        */

        BSP_LED_TOGGLE(BLN_LED);//用作系统正常运行指示灯 
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

/**
 * @brief : 清中断状态寄存器
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
void clearIntStatusReg(void)
{
    fpgaWriteByte(CLEAR_FPGA_INT_STATUS_REG, CLEAR_OVERCURRENT_STATUS);          //清过流中断
    fpgaWriteByte(CLEAR_FPGA_INT_STATUS_REG, CLEAR_OVERVOLTAGE_STATUS);          //清过压中断
    fpgaWriteByte(CLEAR_FPGA_INT_STATUS_REG, CLEAR_SEND_STOP_INT_STATUS);        //清发射停止中断
    fpgaWriteByte(CLEAR_FPGA_INT_STATUS_REG, CLEAR_TXCO_64S_LOCK_INT_STATUS);    //清GPS_PPS连续64秒锁定温补晶振计数有效中断状态
}

/**
 * @brief :
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
void stopProcess(void)
{
    uint8_t ucSta1, ucSta2;
      
    SEND_LED_OFF(SEND_LED);

    g_ucDeviceStatusFirstByte = STANDBY;

    ucSta1 = g_ucDeviceStatusThirdByte & WAIT_START_KEY;
    ucSta2 = g_ucDeviceStatusThirdByte & WAIT_SYNC_START_TRIGGER;
    if((ucSta1 == WAIT_START_KEY) || (ucSta2 == WAIT_SYNC_START_TRIGGER))
    {
        SyncStop();
        g_ucDeviceStatusThirdByte |= WAIT_START_KEY;
        g_ucDeviceStatusThirdByte |= WAIT_SYNC_START_TRIGGER;

        if((g_ucWorkMode == TDIP_MODE) && (eTaskGetState(xTdipStartTask_Handle) != eDeleted))
        {
            vTaskDelete(xTdipStartTask_Handle);//删除vTdipStartTask任务
            vTaskDelay(pdMS_TO_TICKS(100));
        }
        if(((g_ucWorkMode == FDIP_SINGLE_FREQUENCY) || (g_ucWorkMode == FDIP_DOUBLE_FREQUENCY)) && (eTaskGetState(xFdipStartTask_Handle) != eDeleted))
        {
            vTaskDelete(xFdipStartTask_Handle);//删除vFdipStartTask任务
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }

    //g_ucWorkStatus = fpgaReadByte(WORK_STATUS_REG);
    if((g_ucWorkStatus != 0) || (g_ucFirstSyncStart == 0))
    {
        StopSend();
        g_ucDeviceStatusThirdByte |= WAIT_START_KEY;
        g_ucDeviceStatusThirdByte |= WAIT_SYNC_START_TRIGGER;
    }

    statusReset();
}

/**
 * @brief : 读中断状态寄存器
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
void vReadIntStatusRegTask(void *pvParameters)
{
    uint8_t ucSta, ucSta1, ucSta2;

    (void)pvParameters;

    for(;;)
    {
        ucSta = fpgaReadByte(FPGA_INT_STATUS_REG);
        if((ucSta & OVERCURRENT_INT_STATUS) == OVERCURRENT_INT_STATUS) //过流中断
        {
            OVERCURRENT_LED_ON(OVERCURRENT_LED);
            FAULT_LED_ON(FAULT_LED);
            stopProcess();

            g_ucFirstStart = 0;
            g_ucDeviceStatusFourthByte |= OVERCURRENT;
            g_ucDeviceStatusThirdByte |= PROTECT_TRIGGER_STOP;//保护触发停止发射
        }
        else
        {
            g_ucDeviceStatusFourthByte &= ~OVERCURRENT;
            OVERCURRENT_LED_OFF(OVERCURRENT_LED);           
        }

        if((ucSta & OVERVOLTAGE_INT_STATUS) == OVERVOLTAGE_INT_STATUS) //过压中断
        {
            FAULT_LED_ON(FAULT_LED);
            stopProcess();

            g_ucFirstStart = 0;
            g_ucDeviceStatusFourthByte |= OVERVOLTAGE;
            g_ucDeviceStatusThirdByte |= PROTECT_TRIGGER_STOP;//保护触发停止发射
        }
        else
        {
            g_ucDeviceStatusFourthByte &= ~OVERVOLTAGE;           
        }

       /* if((ucSta & SEND_STOP_INT_STATUS) == SEND_STOP_INT_STATUS) //发射停止中断状态
        {
            if ((g_ucWorkMode == FDIP_SINGLE_FREQUENCY) || (g_ucWorkMode == FDIP_DOUBLE_FREQUENCY))
            {
                //xSemaphoreGive(xSendStopFlagSemaphore_Handle);  //vFdipStartTask接收
            }

            fpgaWriteByte(CLEAR_FPGA_INT_STATUS_REG, CLEAR_SEND_STOP_INT_STATUS);        //清发射停止中断
        }*/

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief :
 * @note  :
    第1字节：
    0x00:待机，0x01: 发射准备状态，0x02:正供，0x03:正停供，0x04:负供，0x05:负停供
    第2字节：
    0x00:TDIP，0x01:FDIP单频、0x02:FDIP双频
    第3字节：
    D0:等待“启动”确认键，D1:等待同步启动触发，D2:“暂停”命令触发停止输出，D3:“停止”键触发停止发射，D4:“停止”命令触发停止发射，D5:“保护”触发停止发射
    第4字节：
    D0：保护， D1：过压， D2：过流， D3：IGBT保护， D4：过温， D5：电池电量不足， D6：SD卡空间不足， D7：GPS定位有效标志
 * @param :
 * @param :
 * @param :
 * @return:
*/
void vReadDeviceStatusTask(void *pvParameters)
{
    uint8_t ucStartStopStatus = 0, ucWorkStatus = 0, ucErrorStatus = 0, ucTemp = 0, ucSta1, ucSta2;

    (void)pvParameters;

    for(;;)
    {
        ucStartStopStatus = fpgaReadByte(START_STOP_STATUS_REG);//R,启动停止状态寄存器         
        if((ucStartStopStatus & SEND_START_READY) == SEND_START_READY)//发射就绪
        {
            //
            if((g_ucSendToClientFlag == 0) && ((g_ucDeviceStatusFirstByte & SEND_READY) == SEND_READY)) //发送给客户端标志，为1时表示已经把启动键按下的状态发送给客户端了
            {
                g_ucSendToClientFlag = 1;
                g_ucDeviceStatusThirdByte &= (~WAIT_START_KEY);
                g_ucDeviceStatusThirdByte |= WAIT_SYNC_START_TRIGGER;
            }
        }

        if((ucStartStopStatus & SEND_STOP_STATUS) == SEND_STOP_STATUS)//发射停止,“停止”键，按下为“1”，弹起为“0”
        {
            stopProcess();

            g_ucDeviceStatusThirdByte |= STOP_KEY_TRIGGER_STOP;//停止键触发停止发射
        }

        g_ucWorkStatus = fpgaReadByte(WORK_STATUS_REG);//R,工作状态寄存器,0x00:待机，0x01: 发射准备状态，0x02:正供，0x03:正停供，0x04:负供，0x05:负停供
        if (g_ucWorkStatus != 0)//FPGA里0x01:正供，0x02:正停供，0x03:负供，0x04:负停供，所以这里要加1
        {
            g_ucWorkStatus = g_ucWorkStatus + 1;
        }
        switch(g_ucWorkStatus)
        {
            case STANDBY://待机                 
                if((g_ucDeviceStatusFirstByte & SEND_READY) == SEND_READY)
                {
                    g_ucDeviceStatusFirstByte = SEND_READY;
                }
                else
                {
                    if((g_ucDeviceStatusThirdByte & WAIT_START_KEY) == WAIT_START_KEY)
                    {
                        g_ucDeviceStatusFirstByte = STANDBY;
                    }                    
                }                
                break;

            case POSITIVE_SUPPLY://正供                                 
                g_ucDeviceStatusFirstByte = POSITIVE_SUPPLY;
                break;

            case POSITIVE_STOP_SUPPLY://正停供   
                g_ucDeviceStatusFirstByte = POSITIVE_STOP_SUPPLY;
                break;

            case NEGATIVE_SUPPLY://负供               
                g_ucDeviceStatusFirstByte = NEGATIVE_SUPPLY;
                break;

            case NEGATIVE_STOP_SUPPLY://负停供
                g_ucDeviceStatusFirstByte = NEGATIVE_STOP_SUPPLY;
                break;

            default:
                break;
        }

        ucErrorStatus = fpgaReadByte(PROTECT_STATUS_REG);//R,保护状态寄存器
        if((ucErrorStatus & PROTECT_STATUS) == PROTECT_STATUS) //保护
        {
            g_ucDeviceStatusFourthByte |= PROTECT;
            FAULT_LED_ON(FAULT_LED);
        }
        else
        {
            g_ucDeviceStatusFourthByte &= ~PROTECT;
            FAULT_LED_OFF(FAULT_LED);
        }

        if((ucErrorStatus & OVERVOLTAGE_STATUS) == OVERVOLTAGE_STATUS) //过压
        {
            g_ucDeviceStatusFourthByte |= OVERVOLTAGE;
            FAULT_LED_ON(FAULT_LED);
        }
        else
        {
            g_ucDeviceStatusFourthByte &= ~OVERVOLTAGE;
        }

        if((ucErrorStatus & OVERCURRENT_STATUS) == OVERCURRENT_STATUS) //过流
        {
            g_ucDeviceStatusFourthByte |= OVERCURRENT;
            FAULT_LED_ON(FAULT_LED);
            OVERCURRENT_LED_ON(OVERCURRENT_LED);
        }
        else
        {
            g_ucDeviceStatusFourthByte &= ~OVERCURRENT;
            OVERCURRENT_LED_OFF(OVERCURRENT_LED);
        }

        if((ucErrorStatus & IGBT_STATUS) == IGBT_STATUS) //IGBT
        {
            g_ucDeviceStatusFourthByte |= IGBT_PROTECT;
            FAULT_LED_ON(FAULT_LED);
        }
        else
        {
            g_ucDeviceStatusFourthByte &= ~IGBT_PROTECT;
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

/** 
 * @brief : 
 * @note  :
 * @param : 
 * @param : 
 * @param : 
 * @return:
 */
void vReadSysInfTask(void *pvParameters)
{
    (void)pvParameters;
    uint32_t ulCount = 0;

    for (;;)
    {
        g_fTemperature = tempGetTemperature();       //读取温度
        //App_Printf("%f\r\n", g_fTemperature);
        
        if (ulCount % 50 == 0)
        {
            g_usBatFullChargeCapacity    = batFullChargeCapacity();    //电池总容量
            g_usBatRelativeStateOfCharge = batRelativeStateOfCharge(); //电池相对剩余百分比
            g_usBatVoltage               = batVoltage();               //电池电压
            g_usBatTemperature           = batTemperature();           //电池温度
        }

        if(ulCount % 100 == 0)
        {
            g_ucWorkStatus = fpgaReadByte(WORK_STATUS_REG);  //R,工作状态寄存器,0x00:待机,0x01:正供,0x02:正停供,0x03:负供,0x04:负停供
            if(g_ucWorkStatus == 0x00)//不在工作状态才查询SD信息
            {
                //xSemaphoreTake(xFatfsMscSemaphore_Handle, portMAX_DELAY); //获取信号量，在操作完USB后释放
                //HAL_NVIC_DisableIRQ(OTG_FS_IRQn);
                tSDInfo = sdGetSDInfo();
                //xSemaphoreGive(xFatfsMscSemaphore_Handle); //释放信号量，在需要操作USB的地方获取
                //HAL_NVIC_EnableIRQ(OTG_FS_IRQn); 
            }
        }

        vTaskDelay(pdMS_TO_TICKS(6000));

        ulCount++;       
        if(ulCount == 10000)
        {
            ulCount = 0;
        }
    }
}


#ifdef DEBUG_TARGET
/*******************************************************
  * @brief  : 触摸任务的任务函数
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @retval :
*******************************************************/
void vTouchTask(void *pvParameters)
{
    (void)pvParameters;

    for(;;)
    {
        GUI_TOUCH_Exec();
        vTaskDelay(pdMS_TO_TICKS(100)); //延时50ms
    }
}

/*******************************************************
  * @brief  : Debug任务
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @retval :
*******************************************************/
void vDebugTask(void *p_arg)
{
    uint8_t pcaWriteBuffer[1024];

    for(;;)
    {
        //BSP_LED_TOGGLE(BLN_LED);

        SEGGER_RTT_SetTerminal(1);
        SEGGER_RTT_printf(0, "=================================================\r\n");
        SEGGER_RTT_printf(0, "Name                         Status Priority RemainingStack TaskNum\r\n");

        //App_Printf("=================================================\r\n");
        //App_Printf("Name                         Status Priority RemainingStack TaskNum\r\n");

        vTaskList((char *)&pcaWriteBuffer);
        SEGGER_RTT_printf(0, "%s\r\n", pcaWriteBuffer);
        //App_Printf("%s\r\n", pcaWriteBuffer);
                
        //SEGGER_RTT_printf(0, "\r\n Name                         Count	      Usage\r\n");
        //App_Printf("\r\n Name                         Count	      Usage\r\n");
        //vTaskGetRunTimeStats((char *)&pcaWriteBuffer);
        //SEGGER_RTT_printf(0, "%s\r\n", pcaWriteBuffer);
        //App_Printf("%s\r\n", pcaWriteBuffer);
        
        vTaskDelay(pdMS_TO_TICKS(5000)); //延时5000ms
    }
}

#endif



/*******************************************************
  * @brief  : This function is executed in case of error occurrence.
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @retval :
*******************************************************/
void Error_Handler(void)
{
    for(;;)
    {
        BSP_LED_ON(ERROR_LED);
        //vTaskDelay(pdMS_TO_TICKS(500));
    }
}

#ifdef USE_FULL_ASSERT
/*******************************************************
  * @brief  : ST库函数使用了C编译器的断言功能，如果定义了USE_FULL_ASSERT，那么所有的ST库函数将检查函数形参
    是否正确。如果不正确将调用 assert_failed() 函数，这个函数是一个死循环，便于用户检查代码。
  *	关键字__FILE__表示源代码文件名。关键字 __LINE__ 表示源代码行号。
  * @note   : 断言功能使能后将增大代码大小，推荐用户仅在调试时使能，在正式发布软件是禁止。
  * @param  : 源代码文件名称。关键字__FILE__表示源代码文件名。
  * @param  : 代码行号。关键字 __LINE__ 表示源代码行
  * @param  :
  * @retval :
*******************************************************/
void assert_failed(uint8_t *file, uint32_t line)
{
    //源代码文件名和代码行号，将错误文件和行号打印出来
    #ifdef DEBUG_TARGET
    App_Printf("Wrong parameters value: file %s on line %d\r\n", file, line);
    SEGGER_RTT_printf(0, "Wrong parameters value: file %s on line %d\r\n", file, line);
    #endif
}
#endif /* USE_FULL_ASSERT */


//---------------------------------------------------------------------------