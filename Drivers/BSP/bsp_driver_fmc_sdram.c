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

//-------------------------------------------------------------
SDRAM_HandleTypeDef SDRAM_Handler;   //SDRAM句柄
static FMC_SDRAM_TimingTypeDef SDRAM_Timing;
static FMC_SDRAM_CommandTypeDef SDRAM_Command;

uint8_t BSP_SDRAM_Init( void );
void    BSP_SDRAM_Initialization_sequence( uint32_t RefreshCount );
uint8_t BSP_SDRAM_ReadData( uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize );
uint8_t BSP_SDRAM_ReadData_DMA( uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize );
uint8_t BSP_SDRAM_WriteData( uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize );
uint8_t BSP_SDRAM_WriteData_DMA( uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize );
uint8_t BSP_SDRAM_Sendcmd( FMC_SDRAM_CommandTypeDef *SdramCmd );
void    BSP_SDRAM_DMA_IRQHandler( void );
void    BSP_SDRAM_MspInit( void );

uint8_t BSP_SDRAM_Init( void );
void SDRAM_MPU_Config( void );
uint8_t SDRAM_Send_Cmd( uint8_t bankx, uint8_t cmd, uint8_t refresh, uint16_t regval );
void FMC_SDRAM_WriteBuffer( uint8_t *pBuffer, uint32_t WriteAddr, uint32_t n );
void FMC_SDRAM_ReadBuffer( uint8_t *pBuffer, uint32_t ReadAddr, uint32_t n );

uint32_t bsp_TestExtSDRAM1(void);
uint32_t bsp_TestExtSDRAM2(void); 

/**
    @brief  Initializes the SDRAM device.
    @retval SDRAM status
*/
uint8_t BSP_SDRAM_Init( void )
{
    static uint8_t sdramstatus = SDRAM_ERROR;

    /* SDRAM device configuration */
    SDRAM_Handler.Instance 					= FMC_SDRAM_DEVICE;				//SDRAM在BANK5,6
    SDRAM_Handler.Init.SDBank             	= FMC_SDRAM_BANK1;
    SDRAM_Handler.Init.ColumnBitsNumber   	= FMC_SDRAM_COLUMN_BITS_NUM_9;	//列数量
    SDRAM_Handler.Init.RowBitsNumber      	= FMC_SDRAM_ROW_BITS_NUM_13;	//行数量
    SDRAM_Handler.Init.MemoryDataWidth    	= SDRAM_MEMORY_WIDTH;			//数据宽度为16位
    SDRAM_Handler.Init.InternalBankNumber	= FMC_SDRAM_INTERN_BANKS_NUM_4;	//一共4个BANK
    SDRAM_Handler.Init.CASLatency         	= FMC_SDRAM_CAS_LATENCY_3;		//CAS为3
    SDRAM_Handler.Init.WriteProtection    	= FMC_SDRAM_WRITE_PROTECTION_DISABLE;	//失能写保护
    SDRAM_Handler.Init.SDClockPeriod      	= SDCLOCK_PERIOD;				//SDRAM时钟为HCLK/2=216M/2=108M=9.3ns
    SDRAM_Handler.Init.ReadBurst          	= FMC_SDRAM_RBURST_ENABLE;		//使能突
    SDRAM_Handler.Init.ReadPipeDelay      	= FMC_SDRAM_RPIPE_DELAY_1;		//读通道延时

    /* SDRAM_Timing configuration for 90Mhz as SD clock frequency (System clock is up to 180Mhz */
    SDRAM_Timing.LoadToActiveDelay    = 2;		//加载模式寄存器到激活时间的延迟为2个时钟周期
    SDRAM_Timing.ExitSelfRefreshDelay = 8;		//退出自刷新延迟为8个时钟周期
    SDRAM_Timing.SelfRefreshTime      = 6;		//自刷新时间为6个时钟周期
    SDRAM_Timing.RowCycleDelay        = 6;		//行循环延迟为6个时钟周期
    SDRAM_Timing.WriteRecoveryTime    = 2;		//恢复延迟为2个时钟周期
    SDRAM_Timing.RPDelay              = 2;		//行预充电延迟为2个时钟周期
    SDRAM_Timing.RCDDelay             = 2;		//行到列延迟为2个时钟周期

    /* SDRAM controller initialization */
    if ( HAL_SDRAM_Init( &SDRAM_Handler, &SDRAM_Timing ) != HAL_OK )
    {
        sdramstatus = SDRAM_ERROR;
    }

    else
    {
        sdramstatus = SDRAM_OK;
    }

    /* SDRAM initialization sequence */
    BSP_SDRAM_Initialization_sequence( REFRESH_COUNT );
    return sdramstatus;
}

/**
    @brief  Programs the SDRAM device.
    @param  RefreshCount: SDRAM refresh counter value
*/
void BSP_SDRAM_Initialization_sequence( uint32_t RefreshCount )
{
    __IO uint32_t tmpmrd = 0;

    /* Step 1: Configure a clock configuration enable command */
    SDRAM_Command.CommandMode            = FMC_SDRAM_CMD_CLK_ENABLE;
    SDRAM_Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
    SDRAM_Command.AutoRefreshNumber      = 1;
    SDRAM_Command.ModeRegisterDefinition = 0;
    HAL_SDRAM_SendCommand( &SDRAM_Handler, &SDRAM_Command, SDRAM_TIMEOUT ); /* Send the command */

    /* Step 2: Insert 100 us minimum delay */
    /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
    HAL_Delay( 1 );

    /* Step 3: Configure a PALL (precharge all) command */
    SDRAM_Command.CommandMode            = FMC_SDRAM_CMD_PALL;
    SDRAM_Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
    SDRAM_Command.AutoRefreshNumber      = 1;
    SDRAM_Command.ModeRegisterDefinition = 0;
    HAL_SDRAM_SendCommand( &SDRAM_Handler, &SDRAM_Command, SDRAM_TIMEOUT );

    /* Step 4: Configure an Auto Refresh command */
    SDRAM_Command.CommandMode            = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
    SDRAM_Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
    SDRAM_Command.AutoRefreshNumber      = 8;
    SDRAM_Command.ModeRegisterDefinition = 0;
    HAL_SDRAM_SendCommand( &SDRAM_Handler, &SDRAM_Command, SDRAM_TIMEOUT );

    /* Step 5: Program the external memory mode register */
    tmpmrd = ( uint32_t )SDRAM_MODEREG_BURST_LENGTH_1          | \
             SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   | \
             SDRAM_MODEREG_CAS_LATENCY_3           | \
             SDRAM_MODEREG_OPERATING_MODE_STANDARD | \
             SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
    SDRAM_Command.CommandMode            = FMC_SDRAM_CMD_LOAD_MODE;
    SDRAM_Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK1;
    SDRAM_Command.AutoRefreshNumber      = 1;
    SDRAM_Command.ModeRegisterDefinition = tmpmrd;
    HAL_SDRAM_SendCommand( &SDRAM_Handler, &SDRAM_Command, SDRAM_TIMEOUT );

    /* Step 6: Set the refresh rate counter */
    /* Set the device refresh rate */
    HAL_SDRAM_ProgramRefreshRate( &SDRAM_Handler, RefreshCount );
}

/**
    @brief  Reads an mount of data from the SDRAM memory in polling mode.
    @param  uwStartAddress: Read start address
    @param  pData: Pointer to data to be read
    @param  uwDataSize: Size of read data from the memory
    @retval SDRAM status
*/
uint8_t BSP_SDRAM_ReadData( uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize )
{
    if ( HAL_SDRAM_Read_32b( &SDRAM_Handler, ( uint32_t * )uwStartAddress, pData, uwDataSize ) != HAL_OK )
    {
        return SDRAM_ERROR;
    }

    else
    {
        return SDRAM_OK;
    }
}


/**
    @brief  Writes an mount of data to the SDRAM memory in polling mode.
    @param  uwStartAddress: Write start address
    @param  pData: Pointer to data to be written
    @param  uwDataSize: Size of written data from the memory
    @retval SDRAM status
*/
uint8_t BSP_SDRAM_WriteData( uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize )
{
    if ( HAL_SDRAM_Write_32b( &SDRAM_Handler, ( uint32_t * )uwStartAddress, pData, uwDataSize ) != HAL_OK )
    {
        return SDRAM_ERROR;
    }

    else
    {
        return SDRAM_OK;
    }
}


/**
    @brief  Sends command to the SDRAM bank.
    @param  SdramCmd: Pointer to SDRAM command structure
    @retval HAL status
*/
uint8_t BSP_SDRAM_Sendcmd( FMC_SDRAM_CommandTypeDef *SdramCmd )
{
    if ( HAL_SDRAM_SendCommand( &SDRAM_Handler, SdramCmd, SDRAM_TIMEOUT ) != HAL_OK )
    {
        return SDRAM_ERROR;
    }

    else
    {
        return SDRAM_OK;
    }
}

//SDRAM底层驱动，引脚配置，时钟使能
//此函数会被HAL_SDRAM_Init()调用
//hsdram:SDRAM句柄
void HAL_SDRAM_MspInit( SDRAM_HandleTypeDef *hsdram )
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_FMC_CLK_ENABLE();                 //使能FMC时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();               //使能GPIOC时钟
    __HAL_RCC_GPIOD_CLK_ENABLE();               //使能GPIOD时钟
    __HAL_RCC_GPIOE_CLK_ENABLE();               //使能GPIOE时钟
    __HAL_RCC_GPIOF_CLK_ENABLE();               //使能GPIOF时钟
    __HAL_RCC_GPIOG_CLK_ENABLE();               //使能GPIOG时钟
 
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
    //初始化PC0,2,3
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;        //推挽复用
    GPIO_Initure.Pull = GPIO_PULLUP;            //上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;       //高速
    GPIO_Initure.Alternate = GPIO_AF12_FMC;     //复用为FMC
    HAL_GPIO_Init( GPIOC, &GPIO_Initure );      //初始化

    //初始化PD0,1,8,9,10,14,15
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_8 | GPIO_PIN_9 | \
                       GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;        //推挽复用
    GPIO_Initure.Pull = GPIO_PULLUP;            //上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;       //高速
    GPIO_Initure.Alternate = GPIO_AF12_FMC;     //复用为FMC					   
    HAL_GPIO_Init( GPIOD, &GPIO_Initure ); //初始化

    //初始化PE0,1,7,8,9,10,11,12,13,14,15
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | \
                       GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;        //推挽复用
    GPIO_Initure.Pull = GPIO_PULLUP;            //上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;       //高速
    GPIO_Initure.Alternate = GPIO_AF12_FMC;     //复用为FMC					   
    HAL_GPIO_Init( GPIOE, &GPIO_Initure ); //初始化

    //初始化PF0,1,2,3,4,5,11,12,13,14,15
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | \
                       GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;        //推挽复用
    GPIO_Initure.Pull = GPIO_PULLUP;            //上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;       //高速
    GPIO_Initure.Alternate = GPIO_AF12_FMC;     //复用为FMC					   
    HAL_GPIO_Init( GPIOF, &GPIO_Initure ); //初始化

    //初始化PG0,1,2,4,5,8,15
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | \
                       GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15;
    GPIO_Initure.Mode = GPIO_MODE_AF_PP;        //推挽复用
    GPIO_Initure.Pull = GPIO_PULLUP;            //上拉
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;       //高速
    GPIO_Initure.Alternate = GPIO_AF12_FMC;     //复用为FMC					   
    HAL_GPIO_Init( GPIOG, &GPIO_Initure ); //初始化
}

//向SDRAM发送命令
//bankx:0,向BANK5上面的SDRAM发送指令
//      1,向BANK6上面的SDRAM发送指令
//cmd:指令(0,正常模式/1,时钟配置使能/2,预充电所有存储区/3,自动刷新/4,加载模式寄存器/5,自刷新/6,掉电)
//refresh:自刷新次数
//regval:模式寄存器的定义
//返回值:0,正常;1,失败.
uint8_t SDRAM_Send_Cmd( uint8_t bankx, uint8_t cmd, uint8_t refresh, uint16_t regval )
{
    uint32_t target_bank = 0;
    FMC_SDRAM_CommandTypeDef Command;

    if ( bankx == 0 )
    {
        target_bank = FMC_SDRAM_CMD_TARGET_BANK1;
    }

    else if ( bankx == 1 )
    {
        target_bank = FMC_SDRAM_CMD_TARGET_BANK2;
    }

    Command.CommandMode = cmd;              //命令
    Command.CommandTarget = target_bank;    //目标SDRAM存储区域
    Command.AutoRefreshNumber = refresh;    //自刷新次数
    Command.ModeRegisterDefinition = regval; //要写入模式寄存器的值

    if ( HAL_SDRAM_SendCommand( &SDRAM_Handler, &Command, 0X1000 ) == HAL_OK ) //向SDRAM发送命令
    {
        return 0;
    }

    else
    {
        return 1;
    }
}

//在指定地址(WriteAddr+EXT_SDRAM_ADDR)开始,连续写入n个字节.
//pBuffer:字节指针
//WriteAddr:要写入的地址
//n:要写入的字节数
void FMC_SDRAM_WriteBuffer( uint8_t *pBuffer, uint32_t WriteAddr, uint32_t n )
{
    for ( ; n != 0; n-- )
    {
        *( __IO uint8_t * )( EXT_SDRAM_ADDR + WriteAddr ) = *pBuffer;
        WriteAddr++;
        pBuffer++;
    }
}

//在指定地址((WriteAddr+EXT_SDRAM_ADDR))开始,连续读出n个字节.
//pBuffer:字节指针
//ReadAddr:要读出的起始地址
//n:要写入的字节数
void FMC_SDRAM_ReadBuffer( uint8_t *pBuffer, uint32_t ReadAddr, uint32_t n )
{
    for ( ; n != 0; n-- )
    {
        *pBuffer++ = *( __IO uint8_t * )( EXT_SDRAM_ADDR + ReadAddr );
        ReadAddr++;
    }
}

/*
*********************************************************************************************************
*	函 数 名: bsp_TestExtSDRAM
*	功能说明: 扫描测试外部SRAM, 全部单元。
*	形    参: 无
*	返 回 值: 0 表示测试通过； 大于0表示错误单元的个数。
*********************************************************************************************************
*/
uint32_t bsp_TestExtSDRAM1(void)
{
	uint32_t i;
	uint32_t *pSDRAM;
	uint8_t *pBytes;
	uint32_t err;
	const uint8_t ByteBuf[4] = {0x55, 0xA5, 0x5A, 0xAA};

	/* 写SDRAM */
	pSDRAM = (uint32_t *)EXT_SDRAM_ADDR;
	for (i = 0; i < EXT_SDRAM_SIZE / 4; i++)
	{
		*pSDRAM++ = i;
	}

	/* 读SDRAM */
	err = 0;
	pSDRAM = (uint32_t *)EXT_SDRAM_ADDR;
	for (i = 0; i < EXT_SDRAM_SIZE / 4; i++)
	{
    	if (*pSDRAM++ != i)
		{
			err++;
		}
	}

	if (err >  0)
	{
		return  (4 * err);
	}

	/* 对SRAM 的数据求反并写入 */
    pSDRAM = (uint32_t *)EXT_SDRAM_ADDR;
	for (i = 0; i < EXT_SDRAM_SIZE / 4; i++)
	{
    	*pSDRAM = ~*pSDRAM;
    	pSDRAM++;
	}

	/* 再次比较SDRAM的数据 */
	err = 0;
    pSDRAM = (uint32_t *)EXT_SDRAM_ADDR;
	for (i = 0; i < EXT_SDRAM_SIZE / 4; i++)
	{
    	if (*pSDRAM++ != (~i))
		{
			err++;
		}
	}

	if (err >  0)
	{
		return (4 * err);
	}

	/* 测试按字节方式访问, 目的是验证 FSMC_NBL0 、 FSMC_NBL1 口线 */
	pBytes = (uint8_t *)EXT_SDRAM_ADDR;
	for (i = 0; i < sizeof(ByteBuf); i++)
	{
		*pBytes++ = ByteBuf[i];
	}

	/* 比较SDRAM的数据 */
	err = 0;
	pBytes = (uint8_t *)EXT_SDRAM_ADDR;
	for (i = 0; i < sizeof(ByteBuf); i++)
	{
		if (*pBytes++ != ByteBuf[i])
		{
			err++;
		}
	}
	if (err >  0)
	{
		return err;
	}
	return 0;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_TestExtSDRAM2
*	功能说明: 扫描测试外部SDRAM. 不扫描前面4M字节的显存。
*	形    参: 无
*	返 回 值: 0 表示测试通过； 大于0表示错误单元的个数。
*********************************************************************************************************
*/
uint32_t bsp_TestExtSDRAM2(void)
{
	uint32_t i;
	uint32_t *pSRAM;
	uint8_t *pBytes;
	uint32_t err;
	const uint8_t ByteBuf[4] = {0x55, 0xA5, 0x5A, 0xAA};

	/* 写SRAM */
	pSRAM = (uint32_t *)SDRAM_APP_BUF;
	for (i = 0; i < SDRAM_APP_SIZE / 4; i++)
	{
		*pSRAM++ = i;
	}

	/* 读SRAM */
	err = 0;
	pSRAM = (uint32_t *)SDRAM_APP_BUF;
	for (i = 0; i < SDRAM_APP_SIZE / 4; i++)
	{
		if (*pSRAM++ != i)
		{
			err++;
		}
	}

	if (err >  0)
	{
		return  (4 * err);
	}

#if 0
	/* 对SRAM 的数据求反并写入 */
	pSRAM = (uint32_t *)SDRAM_APP_BUF;
	for (i = 0; i < SDRAM_APP_SIZE / 4; i++)
	{
		*pSRAM = ~*pSRAM;
		pSRAM++;
	}

	/* 再次比较SDRAM的数据 */
	err = 0;
	pSRAM = (uint32_t *)SDRAM_APP_BUF;
	for (i = 0; i < SDRAM_APP_SIZE / 4; i++)
	{
		if (*pSRAM++ != (~i))
		{
			err++;
		}
	}

	if (err >  0)
	{
		return (4 * err);
	}
#endif	

	/* 测试按字节方式访问, 目的是验证 FSMC_NBL0 、 FSMC_NBL1 口线 */
	pBytes = (uint8_t *)SDRAM_APP_BUF;
	for (i = 0; i < sizeof(ByteBuf); i++)
	{
		*pBytes++ = ByteBuf[i];
	}

	/* 比较SDRAM的数据 */
	err = 0;
	pBytes = (uint8_t *)SDRAM_APP_BUF;
	for (i = 0; i < sizeof(ByteBuf); i++)
	{
		if (*pBytes++ != ByteBuf[i])
		{
			err++;
		}
	}
	if (err >  0)
	{
		return err;
	}
	return 0;
}