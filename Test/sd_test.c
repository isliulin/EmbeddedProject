/**
  ******************************************************************************
    @file
    @author
    @version
    @date
    @brief
  ******************************************************************************
    @history

  ******************************************************************************
*/

#include "include.h"

typedef enum {FAIL = 0, PASSED = !FAIL} TestStatus;
/* 私有宏定义 ----------------------------------------------------------------*/
#define BLOCK_SIZE            512         // SD卡块大小     
#define NUMBER_OF_BLOCKS      8           // 测试块数量(小于15)
#define WRITE_READ_ADDRESS    0x00002000  // 测试读写地址

/* 私有变量 ------------------------------------------------------------------*/
uint32_t Buffer_Block_Tx[BLOCK_SIZE * NUMBER_OF_BLOCKS]; // 写数据缓存
uint32_t Buffer_Block_Rx[BLOCK_SIZE * NUMBER_OF_BLOCKS]; // 读数据缓存
HAL_StatusTypeDef sd_status;    // HAL库函数操作SD卡函数返回值：操作结果
TestStatus test_status;           // 数据测试结果


void SD_EraseTest( void );
void SD_Write_Read_Test( void );
void FATFS_SD_Test( void );

/**
    函数功能: 检查缓冲区的数据是否为0xff或0
    输入参数: pBuffer：要比较的缓冲区的指针
              BufferLength：缓冲区长度
    返 回 值: PASSED：缓冲区的数据全为0xff或0
              FAIL：缓冲区的数据至少有一个不为0xff或0
    说    明: 无
*/
TestStatus eBuffercmp( uint32_t *pBuffer, uint32_t BufferLength )
{
    while ( BufferLength-- )
    {
        /* SD卡擦除后的可能值为0xff或0 */
        if ( ( *pBuffer != 0xFFFFFFFF ) && ( *pBuffer != 0 ) )
        {
            return FAIL;
        }

        pBuffer++;
    }

    return PASSED;
}

/**
    函数功能: SD卡擦除测试
    输入参数: 无
    返 回 值: 无
    说    明: 无
*/
void SD_EraseTest( void )
{
    /* 第1个参数为SD卡句柄，第2个参数为擦除起始地址，第3个参数为擦除结束地址 */
    sd_status = HAL_SD_Erase( &uSdHandle, WRITE_READ_ADDRESS, WRITE_READ_ADDRESS + NUMBER_OF_BLOCKS * 4 );
    SEGGER_RTT_printf(0, 0, "erase status:%d\r\n", sd_status );	

    HAL_Delay( 500 );

    if ( sd_status == HAL_OK )
    {
        /* 读取刚刚擦除的区域 */
        sd_status = HAL_SD_ReadBlocks( &uSdHandle, ( uint8_t * )Buffer_Block_Rx, WRITE_READ_ADDRESS, NUMBER_OF_BLOCKS, 0xffff );
        SEGGER_RTT_printf(0, 0, "erase read status:%d\r\n", sd_status );
        /* 把擦除区域读出来对比 */
        test_status = eBuffercmp( Buffer_Block_Rx, BLOCK_SIZE * NUMBER_OF_BLOCKS );

        if ( test_status == PASSED )
        {
            SEGGER_RTT_printf(0, 0, ">Erase sucess!\r\n" );
        }

        else
        {
            SEGGER_RTT_printf(0, 0, ">Erase not sucess, data error!\r\n" );
        }
    }

    else
    {
        SEGGER_RTT_printf(0, 0, ">Erase test fail\r\n" );
    }
}

/**
    函数功能: 在缓冲区中填写数据
    输入参数: pBuffer：要填充的缓冲区
              BufferLength：要填充的大小
              Offset：填在缓冲区的第一个值
    返 回 值: 无
    说    明: 无
*/
void Fill_Buffer( uint32_t *pBuffer, uint32_t BufferLength, uint32_t Offset )
{
    uint32_t index = 0;

    /* 填充数据 */
    for ( index = 0; index < BufferLength; index++ )
    {
        pBuffer[index] = index + Offset;
    }
}

/**
    函数功能: 比较两个缓冲区中的数据是否相等
    输入参数: pBuffer1：要比较的缓冲区1的指针
              pBuffer2：要比较的缓冲区2的指针
              BufferLength：缓冲区长度
    返 回 值: PASSED：相等
              FAIL：不等
    说    明: 无
*/
TestStatus Buffercmp( uint32_t *pBuffer1, uint32_t *pBuffer2, uint32_t BufferLength )
{
    while ( BufferLength-- )
    {
        if ( BufferLength % 50 == 0 )
        {
            SEGGER_RTT_printf(0, 0, "buf:0x%08X - 0x%08X\r\n", *pBuffer1, *pBuffer2 );
        }

        if ( *pBuffer1 != *pBuffer2 )
        {
            return FAIL;
        }

        pBuffer1++;
        pBuffer2++;
    }

    return PASSED;
}

/**
    函数功能: SD卡读写测试
    输入参数: 无
    返 回 值: 无
    说    明: 无
*/
void SD_Write_Read_Test( void )
{
    int i, j = 0;
    /* 填充数据到写缓存 */
    Fill_Buffer( Buffer_Block_Tx, BLOCK_SIZE * NUMBER_OF_BLOCKS, 0x6666 );

    /* 往SD卡写入数据 */
    sd_status = HAL_SD_WriteBlocks_DMA( &uSdHandle, ( uint8_t * )Buffer_Block_Tx, WRITE_READ_ADDRESS, NUMBER_OF_BLOCKS );
    SEGGER_RTT_printf(0, 0, "write status:%d\r\n", sd_status );

    HAL_Delay( 500 );
    /* 从SD卡读取数据 */
    sd_status = HAL_SD_ReadBlocks_DMA( &uSdHandle, ( uint8_t * )Buffer_Block_Rx, WRITE_READ_ADDRESS, NUMBER_OF_BLOCKS );
    SEGGER_RTT_printf(0, 0, "read status:%d\r\n", sd_status );

    /* 比较数据 */
    test_status = Buffercmp( Buffer_Block_Tx, Buffer_Block_Rx, BLOCK_SIZE * NUMBER_OF_BLOCKS / 4 );	//比较

    if ( test_status == PASSED )
    {
        SEGGER_RTT_printf(0, 0, ">>Read and write test sucessful!\r\n" );

        for ( i = 0; i < BLOCK_SIZE * NUMBER_OF_BLOCKS / 4; i++ )
        {
            if ( j == 8 )
            {
                SEGGER_RTT_printf(0, 0, "\r\n" );
                j = 0;
            }

            SEGGER_RTT_printf(0, 0, "%08x   ", Buffer_Block_Rx[i] );
            j++;
        }

        SEGGER_RTT_printf(0, 0, "\r\n" );
    }

    else
    {
        SEGGER_RTT_printf(0, 0, ">>Read and write test fail!\r\n " );
    }
}

/*******************************************************
    @brief  :
    @note   :
    @param  :
    @param  :
    @param  :
    @retval :
*******************************************************/
void Fatfs_RW_test( void )
{
	FIL fil;                  // file objects
	uint32_t byteswritten;                /* File write counts */
	uint32_t bytesread;                   /* File read counts */
	uint8_t wtext[] = "A0B1C2D3E4F5\r\n"; /* File write buffer */
	uint8_t rtext[128] = {0};                     /* File read buffers */
	//char filename[] = "0:/TEST/test190705.txt";
    char filename[] = "test.txt";
    char caFoldPath[20] = {0};
    FRESULT retSD;

		
	SEGGER_RTT_SetTerminal(0);
	SEGGER_RTT_printf(0, "\r\n ****** FatFs Example ******\r\n\r\n");
	App_Printf("\r\n ****** FatFs Example ******\r\n\r\n");
 
    /*##-1- Register the file system object to the FatFs module ##############*/
	//Link the USB Host disk I/O driver  在开始任务里已经挂载
//    FATFS_LinkDriver(&tSD_Driver, g_caSDPath);
//	
//    retSD = f_mount(&tSDFatFS, (char const *)g_caSDPath, 1);	
//    if(retSD)
//    {
//        SEGGER_RTT_printf(0, " mount error : %d \r\n",retSD);
//        Error_Handler();
//    }
//    else
//        SEGGER_RTT_printf(0, " mount sucess!!! \r\n");
     
    /*##-2- Create and Open new text file objects with write access ######*/
    strcpy(caFoldPath, g_caSDPath);
    strcat(caFoldPath, filename);

    retSD = f_open(&fil, caFoldPath, FA_WRITE | FA_OPEN_APPEND);//创建一个新文件，如果文件已存在，打开文件追加
    if(retSD)
	{
		SEGGER_RTT_printf(0, " open file error : %d\r\n",retSD);
		App_Printf(" open file error : %d\r\n",retSD);
	}        
    else
	{
		SEGGER_RTT_printf(0, " open file sucess!!! \r\n");
		App_Printf(" open file sucess!!! \r\n");
	}        
     
    /*##-3- Write data to the text files ###############################*/	
    retSD = f_write(&fil, wtext, sizeof(wtext), (void *)&byteswritten);
	//retSD = f_write(&fil, g_ucUSART1_RX_BUF, strlen((char *)g_ucUSART1_RX_BUF), (void *)& byteswritten);
    if(retSD)
	{
		SEGGER_RTT_printf(0, " write file error : %d\r\n",retSD);
		App_Printf(" write file error : %d\r\n",retSD);
	}        
    else
    {
        SEGGER_RTT_printf(0, " write file sucess!!! \r\n");
        SEGGER_RTT_printf(0, " write Data : %s\r\n", wtext);
        App_Printf(" write file sucess!!! \r\n");
        App_Printf(" write Data : %s\r\n", wtext);		
    }
     
    /*##-4- Close the open text files ################################*/
    retSD = f_close(&fil);
    if(retSD)
	{
		SEGGER_RTT_printf(0, " close error : %d\r\n",retSD);
		App_Printf(" close error : %d\r\n",retSD);
	}        
    else
	{
		SEGGER_RTT_printf(0, " close sucess!!! \r\n");
		App_Printf(" close sucess!!! \r\n");
	}
       
    /*##-5- Open the text files object with read access ##############*/
    retSD = f_open(&fil, caFoldPath, FA_READ);
    if(retSD)
	{
		SEGGER_RTT_printf(0, " open file error : %d\r\n",retSD);
		App_Printf(" open file error : %d\r\n",retSD);
	}
    else
	{
		SEGGER_RTT_printf(0, " open file sucess!!! \r\n");
		App_Printf(" open file sucess!!! \r\n");
	}
        
    /*##-6- Read data from the text files ##########################*/
    retSD = f_read(&fil, rtext, sizeof(rtext), (UINT*)&bytesread);
    if(retSD)
	{
		SEGGER_RTT_printf(0, " read error!!! %d\r\n",retSD);
		App_Printf(" read error!!! %d\r\n",retSD);
	}        
    else
    {
        SEGGER_RTT_printf(0, " read sucess!!! \r\n");
        SEGGER_RTT_printf(0, " read Data : %s\r\n",rtext);
        App_Printf(" read sucess!!! \r\n");
        App_Printf(" read Data : %s\r\n",rtext);		
    }
     
    /*##-7- Close the open text files ############################*/
    retSD = f_close(&fil);
    if(retSD)  
	{
		SEGGER_RTT_printf(0, " close error!!! %d\r\n",retSD);
		App_Printf(" close error!!! %d\r\n",retSD);
	}        
    else
	{
		SEGGER_RTT_printf(0, " close sucess!!! \r\n");
		App_Printf(" close sucess!!! \r\n");
	}
             
    /*##-8- Compare read data with the expected data ############*/
    if(bytesread == byteswritten)
    { 
        SEGGER_RTT_printf(0, " FatFs is working well!!!\r\n");
		App_Printf(" FatFs is working well!!!\r\n");
    }
}

/*******************************************************
  * @brief  : 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void vFatfsTestTask(void *pvParameters)
{
    //(void)pvParameters;

	for(;;)
	{
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //等待任务通知，在vUsart1RxTask函数释放
		if(pdTRUE == xSemaphoreTake(xFatfsMscSemaphore_Handle, portMAX_DELAY))//获取信号量，在OTG_FS_IRQHandler释放
		{
			HAL_NVIC_DisableIRQ(OTG_FS_IRQn);
			Fatfs_RW_test();
			HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
		}
		xSemaphoreGive(xFatfsMscSemaphore_Handle);	//释放信号量，在OTG_FS_IRQHandler获取
	}		
}