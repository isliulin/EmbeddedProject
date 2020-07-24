/**
  ******************************************************************************
  * @file    bsp.h
  * @author
  * @brief
  ******************************************************************************
  * @attention
  *

  *
  ******************************************************************************
  */
  /* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SD_DISKIO_H
#define __SD_DISKIO_H

#ifdef __cplusplus
extern "C" {
	#endif


#include "ff_gen_drv.h"

#define SD_DMA_MODE				1   

#define BSP_SD_CardInfo HAL_SD_CardInfoTypeDef

extern const Diskio_drvTypeDef  tSD_Driver;

extern DSTATUS SD_initialize(BYTE lun);
extern DRESULT SD_read(BYTE, BYTE *, DWORD, UINT);
extern DRESULT SD_write(BYTE, const BYTE *, DWORD, UINT);
extern DSTATUS SD_status(BYTE);
extern DRESULT SD_ioctl(BYTE, BYTE, void *);

#ifdef __cplusplus
}
#endif

#endif /*__SD_DISKIO_H */

