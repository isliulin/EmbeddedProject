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
/* ˽�к궨�� ----------------------------------------------------------------*/
#define BLOCK_SIZE            512         // SD�����С     
#define NUMBER_OF_BLOCKS      8           // ���Կ�����(С��15)
#define WRITE_READ_ADDRESS    0x00002000  // ���Զ�д��ַ

/* ˽�б��� ------------------------------------------------------------------*/
uint32_t Buffer_Block_Tx[BLOCK_SIZE * NUMBER_OF_BLOCKS]; // д���ݻ���
uint32_t Buffer_Block_Rx[BLOCK_SIZE * NUMBER_OF_BLOCKS]; // �����ݻ���
HAL_StatusTypeDef sd_status;    // HAL�⺯������SD����������ֵ���������
TestStatus test_status;           // ���ݲ��Խ��


void SD_EraseTest( void );
void SD_Write_Read_Test( void );
void FATFS_SD_Test( void );

/**
    ��������: ��黺�����������Ƿ�Ϊ0xff��0
    �������: pBuffer��Ҫ�ȽϵĻ�������ָ��
              BufferLength������������
    �� �� ֵ: PASSED��������������ȫΪ0xff��0
              FAIL��������������������һ����Ϊ0xff��0
    ˵    ��: ��
*/
TestStatus eBuffercmp( uint32_t *pBuffer, uint32_t BufferLength )
{
    while ( BufferLength-- )
    {
        /* SD��������Ŀ���ֵΪ0xff��0 */
        if ( ( *pBuffer != 0xFFFFFFFF ) && ( *pBuffer != 0 ) )
        {
            return FAIL;
        }

        pBuffer++;
    }

    return PASSED;
}

/**
    ��������: SD����������
    �������: ��
    �� �� ֵ: ��
    ˵    ��: ��
*/
void SD_EraseTest( void )
{
    /* ��1������ΪSD���������2������Ϊ������ʼ��ַ����3������Ϊ����������ַ */
    sd_status = HAL_SD_Erase( &uSdHandle, WRITE_READ_ADDRESS, WRITE_READ_ADDRESS + NUMBER_OF_BLOCKS * 4 );
    SEGGER_RTT_printf(0, 0, "erase status:%d\r\n", sd_status );	

    HAL_Delay( 500 );

    if ( sd_status == HAL_OK )
    {
        /* ��ȡ�ող��������� */
        sd_status = HAL_SD_ReadBlocks( &uSdHandle, ( uint8_t * )Buffer_Block_Rx, WRITE_READ_ADDRESS, NUMBER_OF_BLOCKS, 0xffff );
        SEGGER_RTT_printf(0, 0, "erase read status:%d\r\n", sd_status );
        /* �Ѳ�������������Ա� */
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
    ��������: �ڻ���������д����
    �������: pBuffer��Ҫ���Ļ�����
              BufferLength��Ҫ���Ĵ�С
              Offset�����ڻ������ĵ�һ��ֵ
    �� �� ֵ: ��
    ˵    ��: ��
*/
void Fill_Buffer( uint32_t *pBuffer, uint32_t BufferLength, uint32_t Offset )
{
    uint32_t index = 0;

    /* ������� */
    for ( index = 0; index < BufferLength; index++ )
    {
        pBuffer[index] = index + Offset;
    }
}

/**
    ��������: �Ƚ������������е������Ƿ����
    �������: pBuffer1��Ҫ�ȽϵĻ�����1��ָ��
              pBuffer2��Ҫ�ȽϵĻ�����2��ָ��
              BufferLength������������
    �� �� ֵ: PASSED�����
              FAIL������
    ˵    ��: ��
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
    ��������: SD����д����
    �������: ��
    �� �� ֵ: ��
    ˵    ��: ��
*/
void SD_Write_Read_Test( void )
{
    int i, j = 0;
    /* ������ݵ�д���� */
    Fill_Buffer( Buffer_Block_Tx, BLOCK_SIZE * NUMBER_OF_BLOCKS, 0x6666 );

    /* ��SD��д������ */
    sd_status = HAL_SD_WriteBlocks_DMA( &uSdHandle, ( uint8_t * )Buffer_Block_Tx, WRITE_READ_ADDRESS, NUMBER_OF_BLOCKS );
    SEGGER_RTT_printf(0, 0, "write status:%d\r\n", sd_status );

    HAL_Delay( 500 );
    /* ��SD����ȡ���� */
    sd_status = HAL_SD_ReadBlocks_DMA( &uSdHandle, ( uint8_t * )Buffer_Block_Rx, WRITE_READ_ADDRESS, NUMBER_OF_BLOCKS );
    SEGGER_RTT_printf(0, 0, "read status:%d\r\n", sd_status );

    /* �Ƚ����� */
    test_status = Buffercmp( Buffer_Block_Tx, Buffer_Block_Rx, BLOCK_SIZE * NUMBER_OF_BLOCKS / 4 );	//�Ƚ�

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
	//Link the USB Host disk I/O driver  �ڿ�ʼ�������Ѿ�����
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

    retSD = f_open(&fil, caFoldPath, FA_WRITE | FA_OPEN_APPEND);//����һ�����ļ�������ļ��Ѵ��ڣ����ļ�׷��
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
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //�ȴ�����֪ͨ����vUsart1RxTask�����ͷ�
		if(pdTRUE == xSemaphoreTake(xFatfsMscSemaphore_Handle, portMAX_DELAY))//��ȡ�ź�������OTG_FS_IRQHandler�ͷ�
		{
			HAL_NVIC_DisableIRQ(OTG_FS_IRQn);
			Fatfs_RW_test();
			HAL_NVIC_EnableIRQ(OTG_FS_IRQn);
		}
		xSemaphoreGive(xFatfsMscSemaphore_Handle);	//�ͷ��ź�������OTG_FS_IRQHandler��ȡ
	}		
}