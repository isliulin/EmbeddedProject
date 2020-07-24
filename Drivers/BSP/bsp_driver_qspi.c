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

QSPI_HandleTypeDef QSPI_Handler;    //QSPI句柄

static uint8_t QSPI_ResetMemory          (QSPI_HandleTypeDef *hqspi);
static uint8_t QSPI_EnterFourBytesAddress(QSPI_HandleTypeDef *hqspi);
static uint8_t QSPI_DummyCyclesCfg       (QSPI_HandleTypeDef *hqspi);
static uint8_t QSPI_WriteEnable          (QSPI_HandleTypeDef *hqspi);
static uint8_t QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *hqspi, uint32_t Timeout);

/*******************************************************
    @brief  : QSPI初始化
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
uint8_t BSP_QSPI_Init( void )
{
    QSPI_Handler.Instance = QUADSPI; 		//QSPI
    QSPI_Handler.Init.ClockPrescaler = 3; 	 //QPSI分频比，此处为2，QSPI频率就为216/(2+1)=72MHZ
    QSPI_Handler.Init.FifoThreshold = 4;                    //FIFO阈值为4个字节
    QSPI_Handler.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_HALFCYCLE; //采样移位半个周期(DDR模式下,必须设置为0)
    QSPI_Handler.Init.FlashSize = POSITION_VAL( MX25L256_FLASH_SIZE ) - 1; //SPI FLASH大小，mx25l256大小为32M字节
    QSPI_Handler.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_4_CYCLE; //片选高电平时间为4个时钟(13.8*4=55.2ns),即手册里面的tSHSL参数
    QSPI_Handler.Init.ClockMode = QSPI_CLOCK_MODE_0;        //模式0
    QSPI_Handler.Init.FlashID = QSPI_FLASH_ID_1;            //第一片flash
    QSPI_Handler.Init.DualFlash = QSPI_DUALFLASH_DISABLE;   //禁止双闪存模式

    if ( HAL_QSPI_Init( &QSPI_Handler ) != HAL_OK )
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/*******************************************************
    @brief  : QSPI底层驱动,引脚配置,时钟使能,此函数会被HAL_QSPI_Init()调用
    @note   :
    @param  : hqspi:QSPI句柄
    @param  :
    @param  :
    @retval :
*******************************************************/
void HAL_QSPI_MspInit( QSPI_HandleTypeDef *hqspi )
{
    GPIO_InitTypeDef gpio_init_structure;

    //##-1- Enable peripherals and GPIO Clocks #################################
    // Enable the QuadSPI memory interface clock 
    QSPI_CLK_ENABLE();

    // Reset the QuadSPI memory interface 
    QSPI_FORCE_RESET();
    QSPI_RELEASE_RESET();

    // Enable GPIO clocks 
    QSPI_CS_GPIO_CLK_ENABLE();
    QSPI_CLK_GPIO_CLK_ENABLE();
    QSPI_D0_GPIO_CLK_ENABLE();
    QSPI_D1_GPIO_CLK_ENABLE();
    QSPI_D2_GPIO_CLK_ENABLE();
    QSPI_D3_GPIO_CLK_ENABLE();

    // QUADSPI GPIO Configuration
//    	PF6     ------> QUADSPI_BK1_IO3
//    	PF7     ------> QUADSPI_BK1_IO2
//    	PF8     ------> QUADSPI_BK1_IO0
//    	PF9     ------> QUADSPI_BK1_IO1
//    	PB2     ------> QUADSPI_CLK
//    	PB6     ------> QUADSPI_BK1_NCS
    
    //##-2- Configure peripheral GPIO ##
    // QSPI CS GPIO pin configuration  
    gpio_init_structure.Pin       = QSPI_CS_PIN;
    gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull      = GPIO_PULLUP;
    gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_structure.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init( QSPI_CS_GPIO_PORT, &gpio_init_structure );

    // QSPI CLK GPIO pin configuration  
    gpio_init_structure.Pin       = QSPI_CLK_PIN;
    gpio_init_structure.Pull      = GPIO_NOPULL;
    gpio_init_structure.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init( QSPI_CLK_GPIO_PORT, &gpio_init_structure );

    // QSPI D0 GPIO pin configuration  
    gpio_init_structure.Pin       = QSPI_D0_PIN;
    gpio_init_structure.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init( QSPI_D0_GPIO_PORT, &gpio_init_structure );

    // QSPI D1 GPIO pin configuration  
    gpio_init_structure.Pin       = QSPI_D1_PIN;
    gpio_init_structure.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init( QSPI_D1_GPIO_PORT, &gpio_init_structure );

    // QSPI D2 GPIO pin configuration  
    gpio_init_structure.Pin       = QSPI_D2_PIN;
    gpio_init_structure.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init( QSPI_D2_GPIO_PORT, &gpio_init_structure );

    // QSPI D3 GPIO pin configuration  
    gpio_init_structure.Pin       = QSPI_D3_PIN;
    gpio_init_structure.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init( QSPI_D3_GPIO_PORT, &gpio_init_structure );

    //##-3- Configure the NVIC for QSPI #########################################
    // NVIC configuration for QSPI interrupt 
    HAL_NVIC_SetPriority( QUADSPI_IRQn, 8, 0 );
    HAL_NVIC_EnableIRQ( QUADSPI_IRQn );	
}

/*******************************************************
    @brief  : QSPI发送命令
    @note   :
    @param  : instruction:要发送的指令
    @param  : address:发送到的目的地址
    @param  : dummyCycles:空指令周期数
    @param  : instructionMode:指令模式;QSPI_INSTRUCTION_NONE,QSPI_INSTRUCTION_1_LINE,QSPI_INSTRUCTION_2_LINE,QSPI_INSTRUCTION_4_LINE
    @param  : addressMode:地址模式; QSPI_ADDRESS_NONE,QSPI_ADDRESS_1_LINE,QSPI_ADDRESS_2_LINE,QSPI_ADDRESS_4_LINE
    @param  : addressSize:地址长度;QSPI_ADDRESS_8_BITS,QSPI_ADDRESS_16_BITS,QSPI_ADDRESS_24_BITS,QSPI_ADDRESS_32_BITS
    @param  : dataMode:数据模式; QSPI_DATA_NONE,QSPI_DATA_1_LINE,QSPI_DATA_2_LINE,QSPI_DATA_4_LINE
    @retval :
*******************************************************/
void QSPI_Send_CMD( uint32_t instruction,
                    uint32_t address,
                    uint32_t dummyCycles,
                    uint32_t instructionMode,
                    uint32_t addressMode,
                    uint32_t addressSize,
                    uint32_t dataMode )
{
    QSPI_CommandTypeDef Cmdhandler;

    Cmdhandler.Instruction = instruction;                 	//指令
    Cmdhandler.Address = address;                            	//地址
    Cmdhandler.DummyCycles = dummyCycles;                   //设置空指令周期数
    Cmdhandler.InstructionMode = instructionMode;				//指令模式
    Cmdhandler.AddressMode = addressMode;   					//地址模式
    Cmdhandler.AddressSize = addressSize;   					//地址长度
    Cmdhandler.DataMode = dataMode;             				//数据模式
    Cmdhandler.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;       	//每次都发送指令
    Cmdhandler.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE; //无交替字节
    Cmdhandler.DdrMode = QSPI_DDR_MODE_DISABLE;           	//关闭DDR模式
    Cmdhandler.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;

    HAL_QSPI_Command( &QSPI_Handler, &Cmdhandler, HAL_QPSI_TIMEOUT_DEFAULT_VALUE );
}

/*******************************************************
    @brief  : QSPI接收指定长度的数据
    @note   :
    @param  : buf:接收数据缓冲区首地址
    @param  : datalen:要传输的数据长度
    @param  :
    @retval : 返回值:0,正常,其他,错误代码
*******************************************************/
uint8_t QSPI_Receive( uint8_t *buf, uint32_t datalen )
{
    QSPI_Handler.Instance->DLR = datalen - 1;                       //配置数据长度

    if ( HAL_QSPI_Receive( &QSPI_Handler, buf, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) == HAL_OK )
    {
        return 0;    //接收数据
    }

    else
    {
        return 1;
    }
}

/*******************************************************
    @brief  : QSPI发送指定长度的数据
    @note   :
    @param  : buf:发送数据缓冲区首地址
    @param  : datalen:要传输的数据长度
    @param  :
    @retval : 返回值:0,正常,其他,错误代码
*******************************************************/
uint8_t QSPI_Transmit( uint8_t *buf, uint32_t datalen )
{
    QSPI_Handler.Instance->DLR = datalen - 1;                        //配置数据长度

    if ( HAL_QSPI_Transmit( &QSPI_Handler, buf, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) == HAL_OK )
    {
        return 0;    //发送数据
    }

    else
    {
        return 1;
    }
}


//---------------------------------------------------------



/**
    @brief  Initializes the QSPI interface.
    @retval QSPI memory status
*
uint8_t BSP_QSPI_Init( void )
{
    QSPI_Handler.Instance = QUADSPI;

    // Call the DeInit function to reset the driver 
    if ( HAL_QSPI_DeInit( &QSPI_Handler ) != HAL_OK )
    {
        return QSPI_ERROR;
    }

    // System level initialization 
    BSP_QSPI_MspInit( &QSPI_Handler, NULL );

    // QSPI initialization 
    // QSPI freq = SYSCLK /(1 + ClockPrescaler) = 216 MHz/(1+2) = 72 Mhz 
    QSPI_Handler.Init.ClockPrescaler     = 2;
    QSPI_Handler.Init.FifoThreshold      = 4;
    QSPI_Handler.Init.SampleShifting     = QSPI_SAMPLE_SHIFTING_HALFCYCLE;
    QSPI_Handler.Init.FlashSize          = POSITION_VAL( MX25L256_FLASH_SIZE ) - 1;
    QSPI_Handler.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_6_CYCLE; // Min 50ns for nonRead 
    QSPI_Handler.Init.ClockMode          = QSPI_CLOCK_MODE_0;
    QSPI_Handler.Init.FlashID            = QSPI_FLASH_ID_1;
    QSPI_Handler.Init.DualFlash          = QSPI_DUALFLASH_DISABLE;

    if ( HAL_QSPI_Init( &QSPI_Handler ) != HAL_OK )
    {
        return QSPI_ERROR;
    }

    // QSPI memory reset 
    if ( QSPI_ResetMemory( &QSPI_Handler ) != QSPI_OK )
    {
        return QSPI_NOT_SUPPORTED;
    }

    // Set the QSPI memory in 4-bytes address mode 
    if ( QSPI_EnterFourBytesAddress( &QSPI_Handler ) != QSPI_OK )
    {
        return QSPI_NOT_SUPPORTED;
    }

    // Configuration of the dummy cycles on QSPI memory side 
    if ( QSPI_DummyCyclesCfg( &QSPI_Handler ) != QSPI_OK )
    {
        return QSPI_NOT_SUPPORTED;
    }

    return QSPI_OK;
}
*/
/**
    @brief  De-Initializes the QSPI interface.
    @retval QSPI memory status
*/
uint8_t BSP_QSPI_DeInit( void )
{
    QSPI_Handler.Instance = QUADSPI;

    /* Call the DeInit function to reset the driver */
    if ( HAL_QSPI_DeInit( &QSPI_Handler ) != HAL_OK )
    {
        return QSPI_ERROR;
    }

    /* System level De-initialization */
    BSP_QSPI_MspDeInit( &QSPI_Handler, NULL );

    return QSPI_OK;
}

/**
    @brief  Reads an amount of data from the QSPI memory.
    @param  pData: Pointer to data to be read
    @param  ReadAddr: Read start address
    @param  Size: Size of data to read
    @retval QSPI memory status
*/
uint8_t BSP_QSPI_Read( uint8_t *pData, uint32_t ReadAddr, uint32_t Size )
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the read command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = QUAD_OUT_FAST_READ_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
    s_command.Address           = ReadAddr;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = MX25L256_DUMMY_CYCLES_READ_QUAD;
    s_command.NbData            = Size;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if ( HAL_QSPI_Command( &QSPI_Handler, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
    {
        return QSPI_ERROR;
    }

    /* Set S# timing for Read command */
    MODIFY_REG( QSPI_Handler.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_3_CYCLE );

    /* Reception of the data */
    if ( HAL_QSPI_Receive( &QSPI_Handler, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
    {
        return QSPI_ERROR;
    }

    /* Restore S# timing for nonRead commands */
    MODIFY_REG( QSPI_Handler.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_6_CYCLE );

    return QSPI_OK;
}

/**
    @brief  Writes an amount of data to the QSPI memory.
    @param  pData: Pointer to data to be written
    @param  WriteAddr: Write start address
    @param  Size: Size of data to write
    @retval QSPI memory status
*/
uint8_t BSP_QSPI_Write( uint8_t *pData, uint32_t WriteAddr, uint32_t Size )
{
    QSPI_CommandTypeDef s_command;
    uint32_t end_addr, current_size, current_addr;

    /* Calculation of the size between the write address and the end of the page */
    current_size = MX25L256_PAGE_SIZE - ( WriteAddr % MX25L256_PAGE_SIZE );

    /* Check if the size of the data is less than the remaining place in the page */
    if ( current_size > Size )
    {
        current_size = Size;
    }

    /* Initialize the address variables */
    current_addr = WriteAddr;
    end_addr = WriteAddr + Size;

    /* Initialize the program command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = QUAD_IN_FAST_PROG_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Perform the write page by page */
    do
    {
        s_command.Address = current_addr;
        s_command.NbData  = current_size;

        /* Enable write operations */
        if ( QSPI_WriteEnable( &QSPI_Handler ) != QSPI_OK )
        {
            return QSPI_ERROR;
        }

        /* Configure the command */
        if ( HAL_QSPI_Command( &QSPI_Handler, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
        {
            return QSPI_ERROR;
        }

        /* Transmission of the data */
        if ( HAL_QSPI_Transmit( &QSPI_Handler, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
        {
            return QSPI_ERROR;
        }

        /* Configure automatic polling mode to wait for end of program */
        if ( QSPI_AutoPollingMemReady( &QSPI_Handler, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != QSPI_OK )
        {
            return QSPI_ERROR;
        }

        /* Update the address and size variables for next page programming */
        current_addr += current_size;
        pData += current_size;
        current_size = ( ( current_addr + MX25L256_PAGE_SIZE ) > end_addr ) ? ( end_addr - current_addr ) : MX25L256_PAGE_SIZE;
    } while ( current_addr < end_addr );

    return QSPI_OK;
}

/**
    @brief  Erases the specified block of the QSPI memory.
    @param  BlockAddress: Block address to erase
    @retval QSPI memory status
*/
uint8_t BSP_QSPI_Erase_Block( uint32_t BlockAddress )
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the erase command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = BLOCK_ERASE_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
    s_command.Address           = BlockAddress;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Enable write operations */
    if ( QSPI_WriteEnable( &QSPI_Handler ) != QSPI_OK )
    {
        return QSPI_ERROR;
    }

    /* Send the command */
    if ( HAL_QSPI_Command( &QSPI_Handler, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
    {
        return QSPI_ERROR;
    }

    /* Configure automatic polling mode to wait for end of erase */
    if ( QSPI_AutoPollingMemReady( &QSPI_Handler, MX25L256_SUBSECTOR_ERASE_MAX_TIME ) != QSPI_OK )
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
    @brief  Erases the entire QSPI memory.
    @retval QSPI memory status
*/
uint8_t BSP_QSPI_Erase_Chip( void )
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the erase command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = BULK_ERASE_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Enable write operations */
    if ( QSPI_WriteEnable( &QSPI_Handler ) != QSPI_OK )
    {
        return QSPI_ERROR;
    }

    /* Send the command */
    if ( HAL_QSPI_Command( &QSPI_Handler, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
    {
        return QSPI_ERROR;
    }

    /* Configure automatic polling mode to wait for end of erase */
    if ( QSPI_AutoPollingMemReady( &QSPI_Handler, MX25L256_BULK_ERASE_MAX_TIME ) != QSPI_OK )
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
    @brief  Reads current status of the QSPI memory.
    @retval QSPI memory status
*/
uint8_t BSP_QSPI_GetStatus( void )
{
    QSPI_CommandTypeDef s_command;
    uint8_t reg;

    /* Initialize the read flag status register command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = READ_STATUS_REG_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_1_LINE;
    s_command.DummyCycles       = 0;
    s_command.NbData            = 1;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if ( HAL_QSPI_Command( &QSPI_Handler, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
    {
        return QSPI_ERROR;
    }

    /* Reception of the data */
    if ( HAL_QSPI_Receive( &QSPI_Handler, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
    {
        return QSPI_ERROR;
    }
	return reg;
//    /* Check the value of the register */
//    if ( ( reg & ( N25Q512A_FSR_PRERR | N25Q512A_FSR_VPPERR | N25Q512A_FSR_PGERR | N25Q512A_FSR_ERERR ) ) != 0 )
//    {
//        return QSPI_ERROR;
//    }
//    else if ( ( reg & ( N25Q512A_FSR_PGSUS | N25Q512A_FSR_ERSUS ) ) != 0 )
//    {
//        return QSPI_SUSPENDED;
//    }
//    else if ( ( reg & N25Q512A_FSR_READY ) != 0 )
//    {
//        return QSPI_OK;
//    }
//    else
//    {
//        return QSPI_BUSY;
//    }
}

/**
    @brief  Return the configuration of the QSPI memory.
    @param  pInfo: pointer on the configuration structure
    @retval QSPI memory status
*/
uint8_t BSP_QSPI_GetInfo( QSPI_Info *pInfo )
{
    /* Configure the structure with the memory configuration */
    pInfo->FlashSize          = MX25L256_FLASH_SIZE;
    pInfo->EraseSectorSize    = MX25L256_SECTOR_SIZE;
    pInfo->EraseSectorsNumber = ( MX25L256_FLASH_SIZE / MX25L256_SECTOR_SIZE );
    pInfo->ProgPageSize       = MX25L256_PAGE_SIZE;
    pInfo->ProgPagesNumber    = ( MX25L256_FLASH_SIZE / MX25L256_PAGE_SIZE );

    return QSPI_OK;
}

/**
    @brief  Configure the QSPI in memory-mapped mode
    @retval QSPI memory status
*/
uint8_t BSP_QSPI_EnableMemoryMappedMode( void )
{
    QSPI_CommandTypeDef      s_command;
    QSPI_MemoryMappedTypeDef s_mem_mapped_cfg;

    /* Configure the command for the read instruction */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = QUAD_OUT_FAST_READ_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
    s_command.AddressSize       = QSPI_ADDRESS_32_BITS;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_4_LINES;
    s_command.DummyCycles       = MX25L256_DUMMY_CYCLES_READ_QUAD;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the memory mapped mode */
    s_mem_mapped_cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
    s_mem_mapped_cfg.TimeOutPeriod     = 0;

    if ( HAL_QSPI_MemoryMapped( &QSPI_Handler, &s_command, &s_mem_mapped_cfg ) != HAL_OK )
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}


/**
    @brief QSPI MSP Initialization
    @param hqspi: QSPI handle
    @param Params
           This function configures the hardware resources used in this example:
              - Peripheral's clock enable
              - Peripheral's GPIO Configuration
              - NVIC configuration for QSPI interrupt
    @retval None
*/
__weak void BSP_QSPI_MspInit( QSPI_HandleTypeDef *hqspi, void *Params )
{
    GPIO_InitTypeDef gpio_init_structure;

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable the QuadSPI memory interface clock */
    QSPI_CLK_ENABLE();
    /* Reset the QuadSPI memory interface */
    QSPI_FORCE_RESET();
    QSPI_RELEASE_RESET();
    /* Enable GPIO clocks */
    QSPI_CS_GPIO_CLK_ENABLE();
    QSPI_CLK_GPIO_CLK_ENABLE();
    QSPI_D0_GPIO_CLK_ENABLE();
    QSPI_D1_GPIO_CLK_ENABLE();
    QSPI_D2_GPIO_CLK_ENABLE();
    QSPI_D3_GPIO_CLK_ENABLE();
	
    /** QUADSPI GPIO Configuration
    	PF6     ------> QUADSPI_BK1_IO3
    	PF7     ------> QUADSPI_BK1_IO2
    	PF8     ------> QUADSPI_BK1_IO0
    	PF9     ------> QUADSPI_BK1_IO1
    	PB2     ------> QUADSPI_CLK
    	PB6     ------> QUADSPI_BK1_NCS
    */
    /*##-2- Configure peripheral GPIO ##########################################*/
    /* QSPI CS GPIO pin configuration  */
    gpio_init_structure.Pin       = QSPI_CS_PIN;
    gpio_init_structure.Mode      = GPIO_MODE_AF_PP;
    gpio_init_structure.Pull      = GPIO_PULLUP;
    gpio_init_structure.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_structure.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init( QSPI_CS_GPIO_PORT, &gpio_init_structure );

    /* QSPI CLK GPIO pin configuration  */
    gpio_init_structure.Pin       = QSPI_CLK_PIN;
    gpio_init_structure.Pull      = GPIO_NOPULL;
    gpio_init_structure.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init( QSPI_CLK_GPIO_PORT, &gpio_init_structure );

    /* QSPI D0 GPIO pin configuration  */
    gpio_init_structure.Pin       = QSPI_D0_PIN;
    gpio_init_structure.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init( QSPI_D0_GPIO_PORT, &gpio_init_structure );

    /* QSPI D1 GPIO pin configuration  */
    gpio_init_structure.Pin       = QSPI_D1_PIN;
    gpio_init_structure.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init( QSPI_D1_GPIO_PORT, &gpio_init_structure );

    /* QSPI D2 GPIO pin configuration  */
    gpio_init_structure.Pin       = QSPI_D2_PIN;
    gpio_init_structure.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init( QSPI_D2_GPIO_PORT, &gpio_init_structure );

    /* QSPI D3 GPIO pin configuration  */
    gpio_init_structure.Pin       = QSPI_D3_PIN;
    gpio_init_structure.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init( QSPI_D3_GPIO_PORT, &gpio_init_structure );

    /*##-3- Configure the NVIC for QSPI #########################################*/
    /* NVIC configuration for QSPI interrupt */
    HAL_NVIC_SetPriority( QUADSPI_IRQn, 8, 0 );
    HAL_NVIC_EnableIRQ( QUADSPI_IRQn );	
}

/**
    @brief QSPI MSP De-Initialization
    @param hqspi: QSPI handle
    @param Params
           This function frees the hardware resources used in this example:
             - Disable the Peripheral's clock
             - Revert GPIO and NVIC configuration to their default state
    @retval None
*/
__weak void BSP_QSPI_MspDeInit( QSPI_HandleTypeDef *hqspi, void *Params )
{
    /*##-1- Disable the NVIC for QSPI ###########################################*/
    HAL_NVIC_DisableIRQ( QUADSPI_IRQn );

    /*##-2- Disable peripherals and GPIO Clocks ################################*/
    /* De-Configure QSPI pins */
    HAL_GPIO_DeInit( QSPI_CS_GPIO_PORT, QSPI_CS_PIN );
    HAL_GPIO_DeInit( QSPI_CLK_GPIO_PORT, QSPI_CLK_PIN );
    HAL_GPIO_DeInit( QSPI_D0_GPIO_PORT, QSPI_D0_PIN );
    HAL_GPIO_DeInit( QSPI_D1_GPIO_PORT, QSPI_D1_PIN );
    HAL_GPIO_DeInit( QSPI_D2_GPIO_PORT, QSPI_D2_PIN );
    HAL_GPIO_DeInit( QSPI_D3_GPIO_PORT, QSPI_D3_PIN );

    /*##-3- Reset peripherals ##################################################*/
    /* Reset the QuadSPI memory interface */
    QSPI_FORCE_RESET();
    QSPI_RELEASE_RESET();

    /* Disable the QuadSPI memory interface clock */
    QSPI_CLK_DISABLE();
}

/**
    @brief  This function reset the QSPI memory.
    @param  hqspi: QSPI handle
    @retval None
*/
static uint8_t QSPI_ResetMemory( QSPI_HandleTypeDef *hqspi )
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the reset enable command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = RESET_ENABLE_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Send the command */
    if ( HAL_QSPI_Command( hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
    {
        return QSPI_ERROR;
    }

    /* Send the reset memory command */
    s_command.Instruction = RESET_MEMORY_CMD;

    if ( HAL_QSPI_Command( hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
    {
        return QSPI_ERROR;
    }

    /* Configure automatic polling mode to wait the memory is ready */
    if ( QSPI_AutoPollingMemReady( hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != QSPI_OK )
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
    @brief  This function set the QSPI memory in 4-byte address mode
    @param  hqspi: QSPI handle
    @retval None
*/
static uint8_t QSPI_EnterFourBytesAddress( QSPI_HandleTypeDef *hqspi )
{
    QSPI_CommandTypeDef s_command;

    /* Initialize the command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = ENTER_4_BYTE_ADDR_MODE_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Enable write operations */
    if ( QSPI_WriteEnable( hqspi ) != QSPI_OK )
    {
        return QSPI_ERROR;
    }

    /* Send the command */
    if ( HAL_QSPI_Command( hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
    {
        return QSPI_ERROR;
    }

    /* Configure automatic polling mode to wait the memory is ready */
    if ( QSPI_AutoPollingMemReady( hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != QSPI_OK )
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
    @brief  This function configure the dummy cycles on memory side.
    @param  hqspi: QSPI handle
    @retval None
*/
static uint8_t QSPI_DummyCyclesCfg( QSPI_HandleTypeDef *hqspi )
{
    QSPI_CommandTypeDef s_command;
    uint8_t reg;

    /* Initialize the read volatile configuration register command */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = READ_CFG_REG_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_1_LINE;
    s_command.DummyCycles       = 0;
    s_command.NbData            = 1;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if ( HAL_QSPI_Command( hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
    {
        return QSPI_ERROR;
    }

    /* Reception of the data */
    if ( HAL_QSPI_Receive( hqspi, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
    {
        return QSPI_ERROR;
    }

    /* Enable write operations */
    if ( QSPI_WriteEnable( hqspi ) != QSPI_OK )
    {
        return QSPI_ERROR;
    }

    /* Update volatile configuration register (with new dummy cycles) */
    s_command.Instruction = WRITE_STATUS_CFG_REG_CMD;
    MODIFY_REG( reg, MX25L256_CR_NB_DUMMY, ( MX25L256_DUMMY_CYCLES_READ_QUAD << POSITION_VAL( MX25L256_CR_NB_DUMMY ) ) );

    /* Configure the write volatile configuration register command */
    if ( HAL_QSPI_Command( hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
    {
        return QSPI_ERROR;
    }

    /* Transmission of the data */
    if ( HAL_QSPI_Transmit( hqspi, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
    @brief  This function send a Write Enable and wait it is effective.
    @param  hqspi: QSPI handle
    @retval None
*/
static uint8_t QSPI_WriteEnable( QSPI_HandleTypeDef *hqspi )
{
    QSPI_CommandTypeDef     s_command;
    QSPI_AutoPollingTypeDef s_config;

    /* Enable write operations */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = WRITE_ENABLE_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_NONE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    if ( HAL_QSPI_Command( hqspi, &s_command, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
    {
        return QSPI_ERROR;
    }

    /* Configure automatic polling mode to wait for write enabling */
    s_config.Match           = MX25L256_SR_WREN;
    s_config.Mask            = MX25L256_SR_WREN;
    s_config.MatchMode       = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval        = 0x10;
    s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

    s_command.Instruction    = READ_STATUS_REG_CMD;
    s_command.DataMode       = QSPI_DATA_1_LINE;

    if ( HAL_QSPI_AutoPolling( hqspi, &s_command, &s_config, HAL_QPSI_TIMEOUT_DEFAULT_VALUE ) != HAL_OK )
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

/**
    @brief  This function read the SR of the memory and wait the EOP.
    @param  hqspi: QSPI handle
    @param  Timeout
    @retval None
*/
static uint8_t QSPI_AutoPollingMemReady( QSPI_HandleTypeDef *hqspi, uint32_t Timeout )
{
    QSPI_CommandTypeDef     s_command;
    QSPI_AutoPollingTypeDef s_config;

    /* Configure automatic polling mode to wait for memory ready */
    s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction       = READ_STATUS_REG_CMD;
    s_command.AddressMode       = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode          = QSPI_DATA_1_LINE;
    s_command.DummyCycles       = 0;
    s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

    s_config.Match           = 0;
    s_config.Mask            = MX25L256_SR_WIP;
    s_config.MatchMode       = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval        = 0x10;
    s_config.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

    if ( HAL_QSPI_AutoPolling( hqspi, &s_command, &s_config, Timeout ) != HAL_OK )
    {
        return QSPI_ERROR;
    }

    return QSPI_OK;
}

//------------------------------------------------------
