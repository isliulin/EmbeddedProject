/*********************************************************************
                 SEGGER Microcontroller GmbH & Co. KG
         Solutions for real time microcontroller applications
**********************************************************************
*                                                                    *
         (c) 1996 - 2017  SEGGER Microcontroller GmbH & Co. KG
*                                                                    *
         Internet: www.segger.com    Support:  support@segger.com
*                                                                    *
**********************************************************************

** emWin V5.46 - Graphical user interface for embedded applications **
    All  Intellectual Property rights  in the Software belongs to  SEGGER.
    emWin is protected by  international copyright laws.  Knowledge of the
    source code may not be used to write a similar product.  This file may
    only be used in accordance with the following terms:

    The software has been licensed to  ARM LIMITED whose registered office
    is situated at  110 Fulbourn Road,  Cambridge CB1 9NJ,  England solely
    for  the  purposes  of  creating  libraries  for  ARM7, ARM9, Cortex-M
    series,  and   Cortex-R4   processor-based  devices,  sublicensed  and
    distributed as part of the  MDK-ARM  Professional  under the terms and
    conditions  of  the   End  User  License  supplied  with  the  MDK-ARM
    Professional.
    Full source code is available at: www.segger.com

    We appreciate your understanding and fairness.
    ----------------------------------------------------------------------
    Licensing information
    Licensor:                 SEGGER Software GmbH
    Licensed to:              ARM Ltd, 110 Fulbourn Road, CB1 9NJ Cambridge, UK
    Licensed SEGGER software: emWin
    License number:           GUI-00181
    License model:            LES-SLA-20007, Agreement, effective since October 1st 2011
    Licensed product:         MDK-ARM Professional
    Licensed platform:        ARM7/9, Cortex-M/R4
    Licensed number of seats: -
    ----------------------------------------------------------------------
    File        : GUIConf.c
    Purpose     : Display controller initialization
    ---------------------------END-OF-HEADER------------------------------
*/


#include "include.h"

#define EX_SRAM  1	//使用外部RAM


#if EX_SRAM
#define GUI_NUMBYTES  (8 * 1024 * 1024)//设置EMWIN内存大小
#else
#define GUI_NUMBYTES  (100 * 1024)
#endif

#define GUI_BLOCKSIZE 0x80  //块大小


//GUI_X_Config
//初始化的时候调用,用来设置emwin所使用的内存
void GUI_X_Config( void )
{
#if EX_SRAM
	static U32 *aMemory;
	aMemory = (U32 *)SDRAM_APP_BUF;
		
	/*  Assign memory to emWin */
	GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
	GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE);
#else	
	/* 32 bit aligned memory area */
	static U32 aMemory[GUI_NUMBYTES / 4];
	
	/*  Assign memory to emWin */
	GUI_ALLOC_AssignMemory(aMemory, GUI_NUMBYTES);
	GUI_ALLOC_SetAvBlockSize(GUI_BLOCKSIZE);
#endif
}

