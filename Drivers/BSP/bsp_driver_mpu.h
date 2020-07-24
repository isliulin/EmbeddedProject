/**
  ******************************************************************************
  * @file    
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
**/

#ifndef __BSP_DRIVER_MPU_H
#define __BSP_DRIVER_MPU_H


#ifdef __cplusplus
extern "C" {
#endif
	
    
//#include "include.h"
//////////////////////////////////////////////////////////////////////////////////	 
	

uint8_t MPU_Set_Protection(uint32_t baseaddr,uint32_t size,uint32_t rnum,uint32_t ap,uint8_t sen,uint8_t cen,uint8_t ben);
void MPU_Memory_Protection(void);

#ifdef __cplusplus
}
#endif

#endif //__BSP_DRIVER_MPU_H
