/**
 ******************************************************************************
  * @file    bsp_driver_sd.h
  * @brief   This file contains the common defines and functions prototypes for 
  *          the bsp_driver_sd.c driver.
  ******************************************************************************

  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BSP_DRIVER_SD_H
#define __BSP_DRIVER_SD_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/

//#include "include.h"


/* Exported types --------------------------------------------------------*/ 
/** 
  * @brief SD Card information structure 
  */
#define BSP_SD_CardInfo HAL_SD_CardInfoTypeDef

extern const Diskio_drvTypeDef  tSD_Driver;
/* Exported constants --------------------------------------------------------*/ 
/**
  * @brief  SD status structure definition  
  */     
#define MSD_OK                        ((uint8_t)0x00)
#define MSD_ERROR                     ((uint8_t)0x01)

/** 
  * @brief  SD transfer state definition  
  */     
#define SD_TRANSFER_OK                ((uint8_t)0x00)
#define SD_TRANSFER_BUSY              ((uint8_t)0x01)

#define SD_PRESENT               ((uint8_t)0x01)
#define SD_NOT_PRESENT           ((uint8_t)0x00)
#define SD_DATATIMEOUT           ((uint32_t)100000000)

/* DMA definitions for SD DMA transfer */
#define __DMAx_TxRx_CLK_ENABLE             __HAL_RCC_DMA2_CLK_ENABLE
#define SD1_DMAx_Tx_CHANNEL                DMA_CHANNEL_4
#define SD1_DMAx_Rx_CHANNEL                DMA_CHANNEL_4
#define SD1_DMAx_Tx_STREAM                 DMA2_Stream6  
#define SD1_DMAx_Rx_STREAM                 DMA2_Stream3  
#define SD1_DMAx_Tx_IRQn                   DMA2_Stream6_IRQn
#define SD1_DMAx_Rx_IRQn                   DMA2_Stream3_IRQn
#define BSP_SDMMC1_DMA_Tx_IRQHandler       DMA2_Stream6_IRQHandler   
#define BSP_SDMMC1_DMA_Rx_IRQHandler       DMA2_Stream3_IRQHandler 
#define BSP_SDMMC1_IRQHandler              SDMMC1_IRQHandler

extern SD_HandleTypeDef uSdHandle;


/* Exported functions --------------------------------------------------------*/
uint8_t BSP_SD_Init(void);
uint8_t BSP_SD_ITConfig(void);
void BSP_SD_DetectIT(void);
void BSP_SD_DetectCallback(void);
uint8_t BSP_SD_ReadBlocks(uint8_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks, uint32_t Timeout);
uint8_t BSP_SD_WriteBlocks(uint8_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks, uint32_t Timeout);
uint8_t BSP_SD_ReadBlocks_DMA(uint8_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks);
uint8_t BSP_SD_WriteBlocks_DMA(uint8_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks);
uint8_t BSP_SD_Erase(uint32_t StartAddr, uint32_t EndAddr);
uint8_t BSP_SD_GetCardState(void);
void BSP_SD_GetCardInfo(HAL_SD_CardInfoTypeDef *CardInfo);
uint8_t BSP_SD_IsDetected(void);

/* These functions can be modified in case the current settings (e.g. DMA stream)
   need to be changed for specific application needs */
void    BSP_SD_AbortCallback(void);
void    BSP_SD_WriteCpltCallback(void);
void    BSP_SD_ReadCpltCallback(void);

HAL_StatusTypeDef SD_DMAConfigRx(SD_HandleTypeDef *hsd);
HAL_StatusTypeDef SD_DMAConfigTx(SD_HandleTypeDef *hsd);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_DRIVER_SD_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
