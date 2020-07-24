/**
  ******************************************************************************
    @file    bsp_driver_sd.c
    @author
    @version
    @date
    @brief
  ******************************************************************************
    @history

  ******************************************************************************
*/


/* Includes ------------------------------------------------------------------*/
#include "include.h"

SD_HandleTypeDef uSdHandle;
static DMA_HandleTypeDef dmaSDMMC1RxHandle;
static DMA_HandleTypeDef dmaSDMMC1TxHandle;




/*******************************************************
  * @brief  :
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @retval :
*******************************************************/
void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
    GPIO_InitTypeDef GPIO_Init_Structure;

    /* Enable SDIO clock */
    __HAL_RCC_SDMMC1_CLK_ENABLE();

    /* Enable DMA2 clocks */
    __DMAx_TxRx_CLK_ENABLE();

    /* Enable GPIOs clock */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**SDMMC1 GPIO Configuration
    PC8     ------> SDMMC1_D0
    PC9     ------> SDMMC1_D1
    PC10     ------> SDMMC1_D2
    PC11     ------> SDMMC1_D3
    PC12     ------> SDMMC1_CK
    PD2     ------> SDMMC1_CMD
    */
    /* Common GPIO configuration */
    GPIO_Init_Structure.Mode = GPIO_MODE_AF_PP;
    GPIO_Init_Structure.Pull = GPIO_PULLUP;
    GPIO_Init_Structure.Speed = GPIO_SPEED_HIGH;
    GPIO_Init_Structure.Alternate = GPIO_AF12_SDMMC1;

    /* SD1_D0, SD1_D1, SD1_D2, SD1_D3 and SD1_CLK pins */
    GPIO_Init_Structure.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12;
    HAL_GPIO_Init(GPIOC, &GPIO_Init_Structure);

    /* SD1_CMD pin */
    GPIO_Init_Structure.Pin = GPIO_PIN_2;
    HAL_GPIO_Init(GPIOD, &GPIO_Init_Structure);

    /* NVIC configuration for SDIO interrupts */
    HAL_NVIC_SetPriority(SDMMC1_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(SDMMC1_IRQn);

    /* Configure DMA Rx parameters */
    dmaSDMMC1RxHandle.Init.Channel             = SD1_DMAx_Rx_CHANNEL;
    dmaSDMMC1RxHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    dmaSDMMC1RxHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
    dmaSDMMC1RxHandle.Init.MemInc              = DMA_MINC_ENABLE;
    dmaSDMMC1RxHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dmaSDMMC1RxHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    dmaSDMMC1RxHandle.Init.Mode                = DMA_PFCTRL;
    dmaSDMMC1RxHandle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
    dmaSDMMC1RxHandle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
    dmaSDMMC1RxHandle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    dmaSDMMC1RxHandle.Init.MemBurst            = DMA_MBURST_INC4;
    dmaSDMMC1RxHandle.Init.PeriphBurst         = DMA_PBURST_INC4;
    dmaSDMMC1RxHandle.Instance                 = SD1_DMAx_Rx_STREAM;

    /* Associate the DMA handle */
    __HAL_LINKDMA(hsd, hdmarx, dmaSDMMC1RxHandle);

    /* Deinitialize the stream for new transfer */
    HAL_DMA_DeInit(&dmaSDMMC1RxHandle);

    /* Configure the DMA stream */
    HAL_DMA_Init(&dmaSDMMC1RxHandle);

    /* Configure DMA Tx parameters */
    dmaSDMMC1TxHandle.Init.Channel             = SD1_DMAx_Tx_CHANNEL;
    dmaSDMMC1TxHandle.Init.Direction           = DMA_MEMORY_TO_PERIPH;
    dmaSDMMC1TxHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
    dmaSDMMC1TxHandle.Init.MemInc              = DMA_MINC_ENABLE;
    dmaSDMMC1TxHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    dmaSDMMC1TxHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_WORD;
    dmaSDMMC1TxHandle.Init.Mode                = DMA_PFCTRL;
    dmaSDMMC1TxHandle.Init.Priority            = DMA_PRIORITY_VERY_HIGH;
    dmaSDMMC1TxHandle.Init.FIFOMode            = DMA_FIFOMODE_ENABLE;
    dmaSDMMC1TxHandle.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
    dmaSDMMC1TxHandle.Init.MemBurst            = DMA_MBURST_INC4;
    dmaSDMMC1TxHandle.Init.PeriphBurst         = DMA_PBURST_INC4;
    dmaSDMMC1TxHandle.Instance                 = SD1_DMAx_Tx_STREAM;

    /* Associate the DMA handle */
    __HAL_LINKDMA(hsd, hdmatx, dmaSDMMC1TxHandle);

    /* Deinitialize the stream for new transfer */
    HAL_DMA_DeInit(&dmaSDMMC1TxHandle);

    /* Configure the DMA stream */
    HAL_DMA_Init(&dmaSDMMC1TxHandle);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(SD1_DMAx_Rx_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(SD1_DMAx_Rx_IRQn);

    /* NVIC configuration for DMA transfer complete interrupt */
    HAL_NVIC_SetPriority(SD1_DMAx_Tx_IRQn, 7, 0);
    HAL_NVIC_EnableIRQ(SD1_DMAx_Tx_IRQn);

}



/**
    @brief  Initializes the SD card device.
    @retval SD status
*/
uint8_t BSP_SD_Init(void)
{
    uint8_t SD_state = MSD_OK;
    //SD device interface configuration

    uSdHandle.Instance                 = SDMMC1;
    uSdHandle.Init.ClockEdge           = SDMMC_CLOCK_EDGE_RISING;
    uSdHandle.Init.ClockBypass         = SDMMC_CLOCK_BYPASS_DISABLE;
    uSdHandle.Init.ClockPowerSave      = SDMMC_CLOCK_POWER_SAVE_DISABLE;
    uSdHandle.Init.BusWide             = SDMMC_BUS_WIDE_1B;
    uSdHandle.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
    uSdHandle.Init.ClockDiv            = SDMMC_TRANSFER_CLK_DIV;

//    //SD card插入检测，用的是翻盖式MicroSD卡座,没用检测引脚
//    if( BSP_SD_IsDetected() != SD_PRESENT )
//    {
//        return MSD_ERROR;
//    }

    /* HAL SD initialization */
    if(HAL_SD_Init(&uSdHandle) != HAL_OK)
    {
        SD_state = MSD_ERROR;
    }
    /* Configure SD Bus width (4 bits mode selected) */
    if(SD_state == MSD_OK)
    {
        /* Enable wide operation */
        if(HAL_SD_ConfigWideBusOperation(&uSdHandle, SDMMC_BUS_WIDE_4B) != HAL_OK)
        {
            SD_state = MSD_ERROR;
        }
    }

    return SD_state;
}


/**
    @brief  Configures Interrupt mode for SD detection pin.
    @retval Returns 0 in success otherwise 1.
*/
uint8_t BSP_SD_ITConfig(void)
{
    /* TBI: add user code here depending on the hardware configuration used */
    return (uint8_t)0;
}

/**
    @brief  Reads block(s) from a specified address in an SD card, in polling mode.
    @param  pData: Pointer to the buffer that will contain the data to transmit
    @param  ReadAddr: Address from where data is to be read
    @param  NumOfBlocks: Number of SD blocks to read
    @param  Timeout: Timeout for read operation
    @retval SD status
*/
uint8_t BSP_SD_ReadBlocks(uint8_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout)
{
    uint8_t sd_state = MSD_OK;

    if(HAL_SD_ReadBlocks(&uSdHandle, (uint8_t *)pData, ReadAddr, NumOfBlocks, Timeout) != HAL_OK)
    {
        sd_state = MSD_ERROR;
    }

    return sd_state;
}


/**
    @brief  Writes block(s) to a specified address in an SD card, in polling mode.
    @param  pData: Pointer to the buffer that will contain the data to transmit
    @param  WriteAddr: Address from where data is to be written
    @param  NumOfBlocks: Number of SD blocks to write
    @param  Timeout: Timeout for write operation
    @retval SD status
*/
uint8_t BSP_SD_WriteBlocks(uint8_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout)
{
    uint8_t sd_state = MSD_OK;

    if(HAL_SD_WriteBlocks(&uSdHandle, (uint8_t *)pData, WriteAddr, NumOfBlocks, Timeout) != HAL_OK)
    {
        sd_state = MSD_ERROR;
    }

    return sd_state;
}

/**
    @brief  Reads block(s) from a specified address in an SD card, in DMA mode.
    @param  pData: Pointer to the buffer that will contain the data to transmit
    @param  ReadAddr: Address from where data is to be read
    @param  NumOfBlocks: Number of SD blocks to read
    @retval SD status
*/
uint8_t BSP_SD_ReadBlocks_DMA(uint8_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks)
{
    uint8_t sd_state = MSD_OK;

    /* Read block(s) in DMA transfer mode */
    if(HAL_SD_ReadBlocks_DMA(&uSdHandle, (uint8_t *)pData, ReadAddr, NumOfBlocks) != HAL_OK)
    {
        sd_state = MSD_ERROR;
    }

    return sd_state;
}


/**
    @brief  Writes block(s) to a specified address in an SD card, in DMA mode.
    @param  pData: Pointer to the buffer that will contain the data to transmit
    @param  WriteAddr: Address from where data is to be written
    @param  NumOfBlocks: Number of SD blocks to write
    @retval SD status
*/
uint8_t BSP_SD_WriteBlocks_DMA(uint8_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks)
{
    uint8_t sd_state = MSD_OK;

    /* Write block(s) in DMA transfer mode */
    if(HAL_SD_WriteBlocks_DMA(&uSdHandle, (uint8_t *)pData, WriteAddr, NumOfBlocks) != HAL_OK)
    {
        sd_state = MSD_ERROR;
    }

    return sd_state;
}


/**
    @brief  Erases the specified memory area of the given SD card.
    @param  StartAddr: Start byte address
    @param  EndAddr: End byte address
    @retval SD status
*/
uint8_t BSP_SD_Erase(uint32_t StartAddr, uint32_t EndAddr)
{
    uint8_t sd_state = MSD_OK;

    if(HAL_SD_Erase(&uSdHandle, StartAddr, EndAddr) != HAL_OK)
    {
        sd_state = MSD_ERROR;
    }

    return sd_state;
}


/**
    @brief  Gets the current SD card data status.
    @param  None
    @retval Data transfer state.
             This value can be one of the following values:
               @arg  SD_TRANSFER_OK: No data transfer is acting
               @arg  SD_TRANSFER_BUSY: Data transfer is acting
*/
uint8_t BSP_SD_GetCardState(void)
{
//    return ( ( HAL_SD_GetCardState( &uSdHandle ) == HAL_SD_CARD_TRANSFER ) ? SD_TRANSFER_OK : SD_TRANSFER_BUSY );
    HAL_SD_CardStateTypedef card_state;

    card_state = HAL_SD_GetCardState(&uSdHandle);

    if(card_state == HAL_SD_CARD_TRANSFER)
    {
        return (SD_TRANSFER_OK);
    }
    else if((card_state == HAL_SD_CARD_SENDING) ||
            (card_state == HAL_SD_CARD_RECEIVING) ||
            (card_state == HAL_SD_CARD_PROGRAMMING))
    {
        return (SD_TRANSFER_BUSY);
    }
    else
    {
        return(HAL_SD_CARD_ERROR);
    }

}

/**
    @brief  Get SD information about specific SD card.
    @param  CardInfo: Pointer to HAL_SD_CardInfoTypedef structure
    @retval None
*/
void BSP_SD_GetCardInfo(HAL_SD_CardInfoTypeDef *CardInfo)
{
    /* Get SD card Information */
    HAL_SD_GetCardInfo(&uSdHandle, CardInfo);
}

/**
  * @brief SD Abort callbacks
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_AbortCallback(SD_HandleTypeDef *hsd)
{
    BSP_SD_AbortCallback();
}

/**
  * @brief Tx Transfer completed callback
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
    BSP_SD_WriteCpltCallback();
}

/**
  * @brief Rx Transfer completed callback
  * @param hsd: SD handle
  * @retval None
  */
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
    BSP_SD_ReadCpltCallback();
}

/**
  * @brief BSP SD Abort callback
  * @retval None
  */
__weak void BSP_SD_AbortCallback(void)
{

}

/**
  * @brief BSP Tx Transfer completed callback
  * @retval None
  */
__weak void BSP_SD_WriteCpltCallback(void)
{
    //具体实现在sd_diskio_dma_rtos.c
}

/**
  * @brief BSP Rx Transfer completed callback
  * @retval None
  */
__weak void BSP_SD_ReadCpltCallback(void)
{
    //具体实现在sd_diskio_dma_rtos.c
}

/**
    @brief  Detects if SD card is correctly plugged in the memory slot or not.
    @param  None
    @retval Returns if SD is detected or not
SD card插入检测，用的是翻盖式MicroSD卡座,没用检测引脚
*/
uint8_t BSP_SD_IsDetected(void)
{
    __IO uint8_t status = SD_PRESENT;

    //SD卡插入是低电平，没有插入是高电平
//    if( HAL_GPIO_ReadPin( SD_CD_GPIO_Port, SD_CD_Pin ) == GPIO_PIN_RESET )
//    {
//        status = SD_PRESENT;
//    }

//    else
//    {
//        status = SD_NOT_PRESENT;
//    }

    return status;
}

//----------------END OF FILE---------------------
