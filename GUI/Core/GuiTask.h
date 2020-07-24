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

#ifndef __GUI_H
#define __GUI_H

#ifdef __cplusplus
extern "C" {
#endif

#define FONT_INFO_ADDRESS     0

#define XBF_FONT16_BASE_ADDRESS     0x00000000
#define XBF_FONT24_BASE_ADDRESS     0x00160FA6
#define XBF_FONT32_BASE_ADDRESS     0x003E0AAC

//字库信息结构体
typedef struct
{
    uint8_t ucFontInfo; //字库存在信息，0xAA，字库正常；其他，字库不存在
    uint32_t ulFont16Add;
    uint32_t ulFont24Add;
    uint32_t ulFont32Add;
} FontInfo_T;

extern FontInfo_T tFontInfo;



extern GUI_FONT XBF_Font16;
extern GUI_FONT XBF_Font24;
extern GUI_FONT XBF_Font32;




void vGUITask(void *pvParameters);
void GUI_SetXBF(void);


void guiCheckFont(void);

#ifdef __cplusplus
}
#endif

#endif //__GUI_H