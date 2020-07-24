/**
  ******************************************************************************
  * @file		bsp_touch.c    
  * @author  
  * @version 
  * @date    
  * @brief		触摸屏驱动
  ******************************************************************************
  * @history

  ******************************************************************************
  */


//********************************************************************************

#include "include.h"

TS_DrvTypeDef *ts_driver = NULL;

void BSP_TS_Init(void);
	
/*******************************************************
  * @brief  :触摸屏初始化 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void BSP_TS_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

	if(lcddev.id == LCD_70_1024x600) //7寸1024*600 RGB屏
    {
		ts_driver = &ft5206_ts_drv;
		ts_driver->Init();		//FT5206初始化			
		
        ts_driver->ucTouchType |= 0x80;			//电容屏
        ts_driver->ucTouchType |= lcddev.dir & 0x01; //横屏还是竖屏        
    }	
	else  if(lcddev.id == LCD_70_800x480)//7寸800*480 RGB屏
	{		
		ts_driver = &gt911_ts_drv;
		ts_driver->Init();		//GT911初始化
		
        ts_driver->ucTouchType |= 0x80;			//电容屏
        ts_driver->ucTouchType |= lcddev.dir & 0x01; //横屏还是竖屏   		
	}
	else if(lcddev.id == LCD_70_1280x800)
	{
	
	}
}


//------------------------------------------------------------
