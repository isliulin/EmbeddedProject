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

SRAM_HandleTypeDef sramHandle;


//-------------------------------------------------------------

/*******************************************************
  * @brief  : 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void BSP_SRAM_Init(void)
{    
    FMC_NORSRAM_TimingTypeDef Timing;    
    FMC_NORSRAM_TimingTypeDef FSMC_ReadTim;
    FMC_NORSRAM_TimingTypeDef FSMC_WriteTim;

    /* SRAM device configuration */
    sramHandle.Instance = FMC_NORSRAM_DEVICE;
    sramHandle.Extended = FMC_NORSRAM_EXTENDED_DEVICE;
  
    /* Timing configuration derived from system clock (up to 216Mhz)
       for 108Mhz as SRAM clock frequency */
 /*   Timing.AddressSetupTime      = 2;
    Timing.AddressHoldTime       = 1;
    Timing.DataSetupTime         = 2;
    Timing.BusTurnAroundDuration = 1;
    Timing.CLKDivision           = 2;
    Timing.DataLatency           = 2;
    Timing.AccessMode            = FMC_ACCESS_MODE_A;*/
        
                                                            //FSMC读时序控制寄存器
    FSMC_ReadTim.AddressSetupTime = 0x11;                   //地址建立时间(ADDSET)为17个HCLK 1/216M=4.6ns*17=78ns
    FSMC_ReadTim.AddressHoldTime  = 0x00;
    FSMC_ReadTim.DataSetupTime    = 0x55;                   //数据保存时间(DATAST)为85个HCLK	=4.6*85=391ns
    FSMC_ReadTim.AccessMode       = FMC_ACCESS_MODE_A;      //模式A
    
                                                            //FSMC写时序控制寄存器
    FSMC_WriteTim.AddressSetupTime = 0x15;                  //地址建立时间(ADDSET)为21个HCLK=96ns
    FSMC_WriteTim.AddressHoldTime  = 0x00;
    FSMC_WriteTim.DataSetupTime    = 0x15;                  //数据保存时间(DATAST)为4.6ns*21个HCLK=96ns
    FSMC_WriteTim.AccessMode       = FMC_ACCESS_MODE_A;     //模式A
  
    sramHandle.Init.NSBank             = FMC_NORSRAM_BANK1;            //使用NE1
    sramHandle.Init.DataAddressMux     = FMC_DATA_ADDRESS_MUX_DISABLE; //不复用数据线
    sramHandle.Init.MemoryType         = FMC_MEMORY_TYPE_SRAM;         //SRAM
    sramHandle.Init.MemoryDataWidth    = FMC_NORSRAM_MEM_BUS_WIDTH_8;  //8位数据宽度
    sramHandle.Init.BurstAccessMode    = FMC_BURST_ACCESS_MODE_DISABLE;//是否使能突发访问,仅对同步突发存储器有效,此处未用到
    sramHandle.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW; //等待信号的极性,仅在突发模式访问下有用
    sramHandle.Init.WaitSignalActive   = FMC_WAIT_TIMING_BEFORE_WS;    //存储器是在等待周期之前的一个时钟周期还是等待周期期间使能NWAIT
    sramHandle.Init.WriteOperation     = FMC_WRITE_OPERATION_ENABLE;   //存储器写使能
    sramHandle.Init.WaitSignal         = FMC_WAIT_SIGNAL_DISABLE;      //等待使能位,此处未用到
    sramHandle.Init.ExtendedMode       = FMC_EXTENDED_MODE_DISABLE;    //读写使用不同的时序
    sramHandle.Init.AsynchronousWait   = FMC_ASYNCHRONOUS_WAIT_DISABLE;//是否使能同步传输模式下的等待信号,此处未用到
    sramHandle.Init.WriteBurst         = FMC_WRITE_BURST_DISABLE;      //禁止突发写
    sramHandle.Init.ContinuousClock    = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;

    /* Initialize the SRAM controller */
    if (HAL_SRAM_Init(&sramHandle, &FSMC_ReadTim, &FSMC_WriteTim) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }
}

/*******************************************************
  * @brief  : 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @return : 
*******************************************************/
//配置MPU的region(SRAM区域为透写模式)
void SRAM_MPU_Config(void)
{
    MPU_Region_InitTypeDef MPU_Initure;

    HAL_MPU_Disable();							                    //配置MPU之前先关闭MPU,配置完成以后在使能MPU	

    //外部SRAM为region0，此区域可读写
    MPU_Initure.Enable           = MPU_REGION_ENABLE;	            //使能region
    MPU_Initure.Number           = MPU_REGION_NUMBER0;		        //设置region，外部SRAM使用的region0
    MPU_Initure.BaseAddress      = SRAM_BANK_ADDR;	                //region基地址
    MPU_Initure.Size             = MPU_REGION_SIZE_256MB;			//region大小
    MPU_Initure.SubRegionDisable = 0x00;
    MPU_Initure.TypeExtField     = MPU_TEX_LEVEL0;
    MPU_Initure.AccessPermission = MPU_REGION_FULL_ACCESS;	        //此region可读写
    MPU_Initure.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;	//允许读取此区域中的指令
    MPU_Initure.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
    MPU_Initure.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;
    MPU_Initure.IsBufferable     = MPU_ACCESS_BUFFERABLE;
    HAL_MPU_ConfigRegion(&MPU_Initure);
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);                         //开启MPU
}

/*******************************************************
  * @brief  : SRAM底层驱动，时钟使能，引脚分配
  * @note   : 此函数会被HAL_SRAM_Init()调用
  * @param  : hsram:SRAM句柄
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_FMC_CLK_ENABLE();                  //使能FMC时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();                //使能GPIOC时钟
    __HAL_RCC_GPIOD_CLK_ENABLE();                //使能GPIOD时钟
    __HAL_RCC_GPIOE_CLK_ENABLE();                //使能GPIOE时钟
    __HAL_RCC_GPIOF_CLK_ENABLE();                //使能GPIOF时钟
    __HAL_RCC_GPIOG_CLK_ENABLE();                //使能GPIOG时钟
 
    /** FMC GPIO Configuration 	
    PF0   ------> FMC_A0
    PF1   ------> FMC_A1
    PF2   ------> FMC_A2
    PF3   ------> FMC_A3
    PF4   ------> FMC_A4
    PF5   ------> FMC_A5
    PC0   ------> FMC_SDNWE
    PC2   ------> FMC_SDNE0
    PC3   ------> FMC_SDCKE0
    PF11   ------> FMC_SDNRAS
    PF12   ------> FMC_A6
    PF13   ------> FMC_A7
    PF14   ------> FMC_A8
    PF15   ------> FMC_A9
    PG0   ------> FMC_A10
    PG1   ------> FMC_A11
    PE7   ------> FMC_D4
    PE8   ------> FMC_D5
    PE9   ------> FMC_D6
    PE10   ------> FMC_D7
    PE11   ------> FMC_D8
    PE12   ------> FMC_D9
    PE13   ------> FMC_D10
    PE14   ------> FMC_D11
    PE15   ------> FMC_D12
    PD8   ------> FMC_D13
    PD9   ------> FMC_D14
    PD10   ------> FMC_D15
    PD14   ------> FMC_D0
    PD15   ------> FMC_D1
    PG2   ------> FMC_A12
    PG4   ------> FMC_BA0
    PG5   ------> FMC_BA1
    PG8   ------> FMC_SDCLK
    PD0   ------> FMC_D2
    PD1   ------> FMC_D3
    PD4   ------> FMC_NOE
    PD5   ------> FMC_NWE
    PD7   ------> FMC_NE1
    PG15   ------> FMC_SDNCAS
    PE0   ------> FMC_NBL0
    PE1   ------> FMC_NBL1
    */

    SRAM_MPU_Config();//使能MPU保护SRAM区域

    /* Common GPIO configuration */
    GPIO_Initure.Mode      = GPIO_MODE_AF_PP;   //推挽复用
    GPIO_Initure.Pull      = GPIO_PULLUP;       //上拉
    GPIO_Initure.Speed     = GPIO_SPEED_HIGH;   //高速
    GPIO_Initure.Alternate = GPIO_AF12_FMC;     //复用为FMC

    //初始化PC0,2,3
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3;  
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);       //初始化

    //初始化PD0,1,4,5,7,8,9,10,14,15
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | \
                       GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15; 				   
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);  //初始化

    //初始化PE0,1,7,8,9,10,11,12,13,14,15
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | \
                       GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;				   
    HAL_GPIO_Init(GPIOE, &GPIO_Initure);  //初始化

    //初始化PF0,1,2,3,4,5,11,12,13,14,15
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | \
                       GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;				   
    HAL_GPIO_Init(GPIOF, &GPIO_Initure);  //初始化

    //初始化PG0,1,2,4,5,8,15
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | \
                       GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15;				   
    HAL_GPIO_Init(GPIOG, &GPIO_Initure);  //初始化
}

/*******************************************************
  * @brief  : 在指定地址(WriteAddr+SRAM_BANK_ADDR)开始,连续写入ulLen个字节.Writes 8-bit buffer to SRAM memory.
  * @note   : 
  * @param  : pBuffer:字节指针 Pointer to source buffer to write
  * @param  : WriteAddr:要写入的地址
  * @param  : ulLen:要写入的字节数
  * @return : 
*******************************************************/
HAL_StatusTypeDef FMC_SRAM_WriteBuffer(uint8_t *pucBuffer, uint32_t ulWriteAddr, uint32_t ulLen)
{
//    for (; ulLen != 0; ulLen--)
//    {
//        *(__IO uint8_t *)(SRAM_BANK_ADDR + ulWriteAddr) = *pucBuffer;
//        ulWriteAddr++;
//        pucBuffer++;
//    }
    HAL_StatusTypeDef eStatus;
    eStatus = HAL_SRAM_Write_8b(&sramHandle, (uint32_t *)(SRAM_BANK_ADDR + ulWriteAddr), pucBuffer, ulLen);
    return eStatus;
}

/*******************************************************
  * @brief  : 在指定地址(WriteAddr+SRAM_BANK_ADDR)开始,连续读出ulLen个字节.Reads 8-bit buffer from SRAM memory.
  * @note   : 
  * @param  : pBuffer:字节指针 Pointer to destination buffer
  * @param  : WriteAddr:要写入的地址
  * @param  : ulLen:要读出的字节数
  * @return : 
*******************************************************/
HAL_StatusTypeDef FMC_SRAM_ReadBuffer(uint8_t *pucBuffer, uint32_t ulReadAddr, uint32_t ulLen)
{
    //    for (; ulLen != 0; ulLen--)
    //    {
    //        *pucBuffer++ = *(__IO uint8_t *)(SRAM_BANK_ADDR + ulReadAddr);
    //        ulReadAddr++;
    //    }
    HAL_StatusTypeDef eStatus;
    eStatus = HAL_SRAM_Read_8b(&sramHandle, (uint32_t *)(SRAM_BANK_ADDR + ulReadAddr), pucBuffer, ulLen);
    return eStatus;
}

//---------------------------------------------------
