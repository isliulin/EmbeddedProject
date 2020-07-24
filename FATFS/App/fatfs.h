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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FATFS_H
#define __FATFS_H

#ifdef __cplusplus
extern "C" {
#endif

//#include "include.h"

#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

typedef struct
{
	float fTotalCapacity;
    float fFreeCapacity;
}SDInfo_T;

//д��SD���ṹ��
typedef struct WriteToSD_T
{
    uint8_t ucType;         //д������ͣ�0x01	:TDIP_MODE  0x02:FDIP
    __attribute__((aligned(4))) char caFileName[48];      //�ļ���
    __attribute__((aligned(4))) char caContent[1024];     //�ļ�����   
}WriteToSDTypeDef_T;

extern char g_caSDPath[4]; /* SD logical drive path */
extern FATFS tSDFatFS; /* File system object for SD logical drive */
extern FIL tWorkFile;
extern DIR tTDIPDir;
extern DIR tFDIPDir;
extern WriteToSDTypeDef_T tWriteToSD;
extern SDInfo_T tSDInfo;

uint8_t BSP_FATFS_Init(void);
DWORD get_fattime(void);
void SDError(void);
SDInfo_T sdGetSDInfo(void);
//void fatfsNewFile(char *pcFlieName);

void vWriteToSDTask(void *pvParameters);
uint8_t fatfsReadFileList(DIR *ptDir, char *pcFileName, uint16_t *pusNumber);
void StrToLower(char *pcStr);
uint8_t SeparateFileName(const char *pcSrcFile, char *pcDestName, char *pcFileExtension);
FRESULT fatfsDelDir(DIR tDir, char *pcPath);

#ifdef __cplusplus
}
#endif

#endif /* __FATFS_H */
