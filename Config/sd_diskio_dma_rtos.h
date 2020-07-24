/**
  ******************************************************************************
  * @file		include.h    
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
  
#ifndef __SD_DISKIO_H
#define __SD_DISKIO_H  

#ifdef __cplusplus
extern "C" {
#endif

//#include "include.h"
#include "ff_gen_drv.h"


#define BSP_SD_CardInfo HAL_SD_CardInfoTypeDef

extern const Diskio_drvTypeDef  tSD_Driver;

extern DSTATUS SD_initialize( BYTE lun );
extern DRESULT SD_read( BYTE, BYTE*, DWORD, UINT );
extern DRESULT SD_write( BYTE, const BYTE*, DWORD, UINT );



#ifdef __cplusplus
}
#endif

#endif /* __SD_DISKIO_H */

/****END OF FILE****/
