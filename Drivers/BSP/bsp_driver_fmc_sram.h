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
  
#ifndef __BSP_DRIVER_FMC_SRAM_H
#define __BSP_DRIVER_FMC_SRAM_H

#ifdef __cplusplus
extern "C" {
#endif
	
//#include "include.h"
    
#define SRAM_BANK_ADDR                 ((uint32_t)0x60000000)  

extern SRAM_HandleTypeDef sramHandle;
    
void BSP_SRAM_Init(void);   
HAL_StatusTypeDef FMC_SRAM_WriteBuffer(uint8_t *pucBuffer, uint32_t ulWriteAddr, uint32_t ulLen);
HAL_StatusTypeDef FMC_SRAM_ReadBuffer(uint8_t *pucBuffer, uint32_t ulReadAddr, uint32_t ulLen);

#ifdef __cplusplus
}
#endif

#endif	//__BSP_DRIVER_FMC_SRAM_H    