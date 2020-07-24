/**
  ******************************************************************************
  * @file    
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  * @history

  ******************************************************************************
  */
#ifndef __BSP_DRIVER_FMC_SDRAM_H
#define __BSP_DRIVER_FMC_SDRAM_H

#ifdef __cplusplus
 extern "C" {
#endif 



/* Includes ------------------------------------------------------------------*/
//#include "include.h"

//////////////////////////////////////////////////////////////////////////////////	 
#define   SDRAM_OK         0x00
#define   SDRAM_ERROR      0x01


#define EXT_SDRAM_ADDR  	((uint32_t)0xC0000000)	//SDRAM开始地址
#define EXT_SDRAM_SIZE		(32 * 1024 * 1024)		//SDRAM device size in Bytes

/* LCD显存,第1页, 分配2M字节 */
#define SDRAM_LCD_BUF1 		EXT_SDRAM_ADDR

/* LCD显存,第2页, 分配2M字节 */
#define SDRAM_LCD_BUF2		(EXT_SDRAM_ADDR + 4 * 1024 * 1024)

/* 12M字节，提供给应用程序使用 */
#define SDRAM_APP_BUF		(EXT_SDRAM_ADDR + 8 * 1024 * 1024)
#define SDRAM_APP_SIZE		(12 * 1024 * 1024)	 
	 
	 
	 
	 
#define SDRAM_MEMORY_WIDTH            FMC_SDRAM_MEM_BUS_WIDTH_16

#define SDCLOCK_PERIOD                   FMC_SDRAM_CLOCK_PERIOD_2


    //刷新频率计数器(以SDCLK频率计数),计算方法:
	//COUNT=SDRAM刷新周期/行数-20=SDRAM刷新周期(us)*SDCLK频率(Mhz)/行数
    //我们使用的SDRAM刷新周期为64ms,SDCLK=216/2=108Mhz,行数为8192(2^13).
	//所以,COUNT=64*1000*108/8192-20=823
#define REFRESH_COUNT                    823   // SDRAM refresh counter

   
#define SDRAM_TIMEOUT     ((uint32_t)0xFFFF) 

// DMA definitions for SDRAM DMA transfer
#define __DMAx_CLK_ENABLE                 __HAL_RCC_DMA2_CLK_ENABLE
#define SDRAM_DMAx_CHANNEL                DMA_CHANNEL_0
#define SDRAM_DMAx_STREAM                 DMA2_Stream0  
#define SDRAM_DMAx_IRQn                   DMA2_Stream0_IRQn
#define SDRAM_DMAx_IRQHandler             DMA2_Stream0_IRQHandler  


//SDRAM配置参数
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)

//extern SDRAM_HandleTypeDef SDRAM_Handler;//SDRAM句柄

extern uint8_t BSP_SDRAM_Init(void);
extern void    BSP_SDRAM_Initialization_sequence(uint32_t RefreshCount);
extern uint8_t BSP_SDRAM_ReadData(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize);
extern uint8_t BSP_SDRAM_ReadData_DMA(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize);
extern uint8_t BSP_SDRAM_WriteData(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize);
extern uint8_t BSP_SDRAM_WriteData_DMA(uint32_t uwStartAddress, uint32_t *pData, uint32_t uwDataSize);
extern uint8_t BSP_SDRAM_Sendcmd(FMC_SDRAM_CommandTypeDef *SdramCmd);
extern void    BSP_SDRAM_DMA_IRQHandler(void);  
extern void    BSP_SDRAM_MspInit(void);   

extern uint8_t BSP_SDRAM_Init(void);
extern void SDRAM_MPU_Config(void);
extern uint8_t SDRAM_Send_Cmd(uint8_t bankx,uint8_t cmd,uint8_t refresh,uint16_t regval);
extern void FMC_SDRAM_WriteBuffer(uint8_t *pBuffer,uint32_t WriteAddr,uint32_t n);
extern void FMC_SDRAM_ReadBuffer(uint8_t *pBuffer,uint32_t ReadAddr,uint32_t n);

extern uint32_t bsp_TestExtSDRAM1(void);
extern uint32_t bsp_TestExtSDRAM2(void);

#ifdef __cplusplus
}
#endif

#endif  //__BSP_DRIVER_FMC_SDRAM_H
