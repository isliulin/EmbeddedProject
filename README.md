
@par Hardware and Software environment
  - This application runs on STM32F767IITx devices.    
  - This application has been tested with STM32F767IITx core board.

  仪器编号SN_ADDRESS在外部FLASH中的存储地址为后4096字节开始的位置：QSPI_FLASH_SIZE(32 * 1024 * 1024) - 4096
  字库信息FONT_INFO_ADDRESS在外部FLASH中的存储地址为0开始，紧接着是字库,字库由三个xbf字库文件用工具软件合并而成（也可以三个分别写入），字体文件放在TF卡根目录，第一次时会把字体文件写入外部FLASH（可能会花些时间，此时不宜再进行和TF卡有关的操作）
  


  HAL_NVIC_SetPriority( OTG_FS_IRQn, 5, 0 );
  HAL_NVIC_SetPriority(SDMMC1_IRQn, 6, 0);
  HAL_NVIC_SetPriority(SD1_DMAx_Rx_IRQn, 7, 0);
  HAL_NVIC_SetPriority(SD1_DMAx_Tx_IRQn, 7, 0);
  HAL_NVIC_SetPriority(EXTI1_IRQn, 15, 0);//PA1，GPS_PPS_PIN
  HAL_NVIC_SetPriority(EXTI3_IRQn, 10, 0);//PB3，EXTERNAL_POWER_PIN
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 7, 0); //PB5，FPGA_IRQ_PIN。 PA8，FPGA_PPS_PIN
  HAL_NVIC_SetPriority( QUADSPI_IRQn, 8, 0 );
  HAL_NVIC_SetPriority( TIM3_IRQn, 10, 0 );
  HAL_NVIC_SetPriority( TIM4_IRQn, 10, 0 );
  HAL_NVIC_SetPriority(USART1_DMA_TX_IRQn, 9, 0);
  HAL_NVIC_SetPriority(USART1_DMA_RX_IRQn, 9, 0);
  HAL_NVIC_SetPriority(USART1_IRQn, 8, 0);
  HAL_NVIC_SetPriority(USART2_IRQn, 8, 0);
  HAL_NVIC_SetPriority(LTDC_IRQn, 0x0F, 0);

  ----------------------------------------------------------------------------------------------
  @history:
     

  20200724:
  更改vReadSysInfTask任务，加入读SD信息


  20200723:
  修改vUsart1RxTask任务的栈空间,把一些占用空间大的数组换成指针用malloc，free分配空间和释放，比如MX25Lxx_Write函数里的缓冲区


  20200721:
  段头0xA5后面加TDIP-DDF或FDIP2F-DDF

  20200715:
  修改BUG
  if(g_ucSyncStart == 0)//同步启动标志
                {
                    xSemaphoreGive(xSyncStartSemaphore_Handle);//vSyncStartTask里获取
                    if(g_ucWorkMode == TDIP_MODE)
                    {
                        g_ucSyncStart = 1;
                    }
                }
  stopChoise case STOP_SEND: //停止发射   
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

  20200713:
  面板上启动按键要在启动发射后才能有效，待机时按下些键应该不保留些状态

  20200711;
  FDIP状态第一字节，在一个频组完了后保存负停供状态
  SyncStop();//一个频组完
  g_ucDeviceStatusFirstByte = NEGATIVE_STOP_SUPPLY;
  vReadDeviceStatusTask里判断if((g_ucDeviceStatusThirdByte & WAIT_START_KEY) == WAIT_START_KEY)
                    {
                        g_ucDeviceStatusFirstByte = STANDBY;
                    } 


  20200710：
  电流幅值提取
  if (s_usCount % (AD_FIFO_LEN / 3) == 0)//32个数提取一个幅值
                        {
                            fAverage = 0;
                            int i = s_usCount - (AD_FIFO_LEN / 3);
                            for(int j = 0; j < (AD_FIFO_LEN / 3); j++)
                            {
                                fAverage = fAverage + g_faCurrent[i + j];
                            }
                            fAverage = fAverage / (AD_FIFO_LEN / 3);
                            if(fAverage > g_fImax)
                            {
                                g_fImax = fAverage;
                            }
                            if(fAverage < g_fImin)
                            {
                                g_fImin = fAverage;
                            }

                            //App_Printf("g_fImax = %fmA\r\n", g_fImax);
                            //App_Printf("g_fImin = %fmA\r\n", g_fImin);
                        }
                        
                        ulTCount++;

        if(g_ucWorkMode == TDIP)
        {
            if(ulTCount * 15 >= (g_ucPulseWidth * 4) * 1000)//一个周期取一个平均
            {
                //g_fCurrent = (g_fImax - g_fImin) / 2;//电流都是正的，不能用正负平均
                g_fCurrent = g_fImax;//
                ulTCount = 0;
                g_fImax = 0;
                g_fImin = 0;
                App_Printf("g_fCurrent = %fmA\r\n", g_fCurrent);
            }
        }
        else
        {
            if(ulTCount * 15 >= g_fFrequencyGroupTime * 1000)//一个频组取一个平均
            {
                //g_fCurrent = (g_fImax - g_fImin) / 2;//电流都是正的，不能用正负平均
                g_fCurrent = g_fImax;//
                ulTCount = 0;
                g_fImax = 0;
                g_fImin = 0;
                App_Printf("g_fCurrent = %fmA\r\n", g_fCurrent);
            }
        }

  20200709：
  void vWriteToSDTask(void *pvParameters)  configASSERT(pvParameters == NULL);//在此处这里不能去省，去掉后启动发射后会进入HardFault_Handler,目前没搞明白原因
  void vDebugTask(void *p_arg)uint8_t pcaWriteBuffer[1024];
  g_ucWorkStatus = fpgaReadByte(WORK_STATUS_REG);//R,工作状态寄存器,0x00:待机，0x01: 发射准备状态，0x02:正供，0x03:正停供，0x04:负供，0x05:负停供
        if (g_ucWorkStatus != 0)//FPGA里0x01:正供，0x02:正停供，0x03:负供，0x04:负停供，所以这里要加1
        {
            g_ucWorkStatus = g_ucWorkStatus + 1;
        }

    //传给FPGA的计数值
    double dCount;
    dCount = (double)ulFpgaCountFrequency * g_ucPulseWidth;//计数值=16368000*脉宽 
    ulSupplyCountVal = (uint32_t)dCount;
    //停 =（供 - 供 * 占）/ 占
    dCount = (double)ulFpgaCountFrequency * (g_ucPulseWidth - (g_ucPulseWidth * g_ucDutyRatio / 100) / (g_ucDutyRatio / 100));
    ulStopCountVal = (uint32_t)dCount;

    rtcSetDate(uint8_t ucYear, uint8_t ucMonth, uint8_t ucDate)tRTCDateStructure.WeekDay = 0;//这里必须要设置星期，否则读取年份不对

  20200707:
  设备状态第一字节改为：
  0x00:待机，0x01: 发射准备状态，0x02:正供，0x03:正停供，0x04:负供，0x05:负停供
  增加任务：读取温度和电池信息

  20200702:
  增加开机GPS有效时授时 
  供电周期改为供电脉宽  

  20200701:
  设备状态第四字节电池电量低改为电池通讯错误

  20200628:
  频组间隔改为固定

  20200602:
  app启动后发了条设备复位命令，而主机端只发复位，没发置位，引发GPS信息不更新

  20200508:
  存储方式改为只有全波形存储
  SD使用DMA

  20200309:
  设备状态改为4字节
    第1字节：
    0x00:待机，0x01:正供，0x02:正停供，0x03:负供，0x04:负停供
    第2字节：
    0x00:TDIP，0x01:FDIP单频、0x02:FDIP双频
    第3字节：
    D0:等待“启动”确认键，D1:等待同步启动触发，D2:“暂停”命令触发停止输出，D3:“停止”键触发停止发射，D4:“停止”命令触发停止发射，D5:“保护”触发停止发射
    第4字节：
    D0：保护， D1：过压， D2：过流， D3：IGBT保护， D4：过温， D5：电池电量不足， D6：SD卡空间不足， D7：GPS定位有效标志

  20200305:
  FPGA系统复位置位，AD复位置位放到FPGA初始化函数

  20200304:
  完善设备状态字，修复BUG

  20200302:
  以前是接到命令就进行处理，处理完成后才返回移动客户端命令，现修改为，接到命令成功后就返回成功，然后才进行处理。

  20200228:
  同步启动改为任务方式vSyncStartTask

  20191230:
  s根据DDF20-1 FPGA寄存器映射表，增加了一些寄存器，并增加相应的操作及函数

  20191030:
  更改很多，增加状态字等等

  20191014:
  简化WIFI的SSID，直接固定
  增加握手命令和获取控制权命令及相关功能,当客户端需要控制权时，发相应命令加MAC地址，主机则在外部FLASH上存储此MAC，当客户端发来握手信号命令加MAC时，
  比较存储的MAC和握手命令后的MAC，一样的话当前客户端就握手成功。

  20191012:
  完善周期存储

  20191010:
  启动停止状态寄存器

  20190929:
  存储方式必为周期存储和全波形存储可选

  20190927:
  改为二进制存储

  20190926:
  改为全波形存储，更改查询电流日志文件部分
  #define FF_USE_FORWARD	1
  #define FF_FS_TINY		1

  20190924:
  更正FDIP工作方式

  20190920:
  更改FDIP参数传输BUG

  20190919:
  更改vUsart1RxTask()

  20190918:
  增加 void ATCmd_TCPClose(uint8_t ucID)

  20190910:
  更改FDIP的参数部分。。。

  20190830:
  TDIP联调完成

  20190826:
  和FPGA联调TDIP发射过程

  20190823:
  vGpsMsgReceiveCallback里完成GPS信息读取
  AnalysisHandle函数查询GPS状态补全

  20190822:
  vGpsMsgReceiveCallback();

  20190821:
  sram初始化加入SRAM_MPU_Config(),否则读取不正常

  20190820:
  读取FPGA数据改为轮询

  20190819:
  tdip同步启动，加入dataprocess.c

  20190817:
  FPGA驱动，TDIP初始化

  20190815:
  更改WIFI模块的回复方式，加快WIFI传输速率

  20190812:
  增加sram驱动

  20190708:
  SD卡底层驱动更改，不用DMA。

  20190704:
  完成：删除主机SD上所有文件 
  
  20190703:
  增加命令删除主机SD上所有文件  
  
  20190628:
  调试完成查询电池信息

  20190626:
  调试完成查询电流日志文件
  调试完成查询MicroSD卡信息
  更改函数sdGetSDInfo

  20190624:
  给WIFI模块发送数据任务删除，改为在需要发送的地方直接调用HAL_UART_Transmit_DMA
  调试完成查询所有电流日志文件列表

  20190612：
  查询所有电流日志文件列表
  查询电流日志文件

  20190611:
  启动时间改为结构体
  在接到启动发射命令时，新建电流文件，文件名为TDIP（FDIP）+ 当前日期,并把相关参数写入SD卡，写入SD任务vWriteToSDTask，
  要写入SD的数据用队列传送
  
  20190610:
  TDIP的供电周期由浮点数改为uint8_t
  联机和发射状态灯
  在BSP_FATFS_Init里创建TDIP、FDIP文件夹

  20190606:
  调整各任务的堆栈大小，FatFS测试任务改成等待任务通知，在vUsart1RxTask函数释放
  修改configTICK_RATE_HZ的值，减少WIFI模块的等待时间

  20190605:
  修复BUG：SD卡驱动进行读写都OK，FATFS 单独读写文件也没有问题，接入USB后在PC资源管理器中能够正确读取U盘文件，
  但就是无法通过资源管理器改写文件，也无法copy文件到SD卡。在改写SD卡中文件时候，当时能够看到改写成功，
  但下次接入USB后发现SD卡中文件改写失败，类似的PC管理器拷贝的文件也没有写入SD卡。
  更改方法：
  USB接口函数  STORAGE_Read_FS 和  STORAGE_Write_FS里面的读写不用DMA方式。

  20190603:
  继续完善数据解析和发送,
  外部供电中断处理

  20190531:
  GPIO驱动和相应的GPIO中断

  20190528:
  移植Event Recorder

  20190527:
  移植SystemView
  
  20190525:
  增加客户端ID0掉线解决，
  客户端ID0掉线后关闭服务器，重新初始化，保证客户端重新连上后ID号为0
   
  20190524:
  guiCheckFont函数移到GuiTask.c,相关的全局变量和结构体也移动，增加软件定时器xAutoReloadTimer_Handle，定时周期1S，用于LED1和RTC显示
  原先的LED任务变成调试任务vDebugTask,  
  把能改成用任务通知书的地方改成用任务能知：
  1.vUsart1RxTask里xUsart1RxSemaphore_Handle换成任务通知
  2.vGUITask里xFontSemaphore_Handle改成任务通知，xFontSemaphore_Handle用来给AutoReloadCallback发送信号量
  RTC

  20190517:
  SMBus驱动
  电池驱动
  GPIO模拟SMBus要注意：
  发送命令后要注意延时。第一次地址位立即接受应答。后面命令位延时80us再接受应答。接受数据每一组间隔50us。

  20190509：
  GPS数据解析

  20190505:
  编写和移动端通信协议

  20190424:
  继续编写WIFI模块驱动
  过程中发现个BUG，在HAL_PCD_MspInit函数里，一定要使能USB引脚时钟__HAL_RCC_GPIOA_CLK_ENABLE();  

  20190423:
  串口1接收数据解析

  20190422:
  修复QFLASH  MX25L256驱动BUG

  20190417：
  接收完成，调试发送 

  20190408:
  编写WIFI模块ESP8266的驱动
  
  20190329:
  Usart驱动改用DMA方式,用DMA方式接收不定长数据，要用到空闲中断，在空闲中断到来时接收
  dma有两种模式，一种是normal模式，一种是circlular模式,
  选择使用normal模式在发送完成以后不会调用回调函数HAL_UART_TxCpltCallback（中断模式不存在这种情况，只要发送完成就会调用这个函数）,
  所以处理发送时要在中断服务程序USART1_DMA_TX_IRQHandler里进行
   
  20190325：
  修改SD卡驱动，由裸机方式改为FreeRTOS工作方式；
  裸机方式在传输时关闭所有中断，传输完成后再打开, 现改为DMA模式，
  在设置中断优先级时，特别注意优先级问题，
  其中USB和SD读写的优先级顺序为OTG_FS_IRQn > SDMMC1_IRQn > SD1_DMAx_T(R)x_IRQn 
 
  20190320:
  注释掉cmsis_os.c里的void SysTick_Handler(void)函数，此函数在stm32f7xx_it.c里实现；
  static osKernelState_t KernelState;去掉前面的static,变为全局可用，
  在include.c里加入下面语句，用来判断是否在中断里（在中断里超时时间不能太长）：  
  extern osKernelState_t KernelState;
  #ifndef IS_IRQ
  #define IS_IRQ_MASKED()           ((__get_PRIMASK() != 0U) || ((KernelState == osKernelRunning) && (__get_BASEPRI() != 0U)))
  #define IS_IRQ_MODE()             (__get_IPSR() != 0U)
  #define IS_IRQ()                  (IS_IRQ_MODE() || IS_IRQ_MASKED())
  #endif
    

















