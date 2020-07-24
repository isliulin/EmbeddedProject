/**
  ******************************************************************************
    @file    sd_diskio_dma_rtos.c
    @author
    @brief   SD Disk I/O DMA with RTOS driver.
  ******************************************************************************
    @attention



  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "include.h"




#define SD_DEFAULT_BLOCK_SIZE 512

/*
    Depending on the usecase, the SD card initialization could be done at the
    application level, if it is the case define the flag below to disable
    the BSP_SD_Init() call in the SD_Initialize().
*/
//#define DISABLE_SD_INIT


/*
    when using cachable memory region, it may be needed to maintain the cache
    validity. Enable the define below to activate a cache maintenance at each
    read and write operation.
    Notice: This is applicable only for cortex M7 based platform.
*/
#define ENABLE_SD_DMA_CACHE_MAINTENANCE  1

/*
* Some DMA requires 4-Byte aligned address buffer to correctly read/wite data,
* in FatFs some accesses aren't thus we need a 4-byte aligned scratch buffer to correctly
* transfer data
*/
#define ENABLE_SCRATCH_BUFFER


#if defined(ENABLE_SCRATCH_BUFFER)
#if defined (ENABLE_SD_DMA_CACHE_MAINTENANCE)  
ALIGN_32BYTES(static uint8_t scratch[BLOCKSIZE]); // 32-Byte aligned for cache maintenance
#else
__ALIGN_BEGIN static uint8_t scratch[BLOCKSIZE] __ALIGN_END;
#endif
#endif

/* Disk status */
static volatile DSTATUS Stat = STA_NOINIT;

//SD_read/SD_Writ函数专用buf,当这两个函数的数据缓存区地址不是4字节对齐的时候,
//需要用到该数组,确保数据缓存区地址是4字节对齐的.
__attribute__((aligned(4))) uint8_t SDIO_DATA_BUFFER[512];

/* Private function prototypes -----------------------------------------------*/
static DSTATUS SD_CheckStatus(BYTE lun);
DSTATUS SD_initialize(BYTE);
DSTATUS SD_status(BYTE);
DRESULT SD_read(BYTE, BYTE *, DWORD, UINT);
DRESULT SD_write(BYTE, const BYTE *, DWORD, UINT);
DRESULT SD_ioctl(BYTE, BYTE, void *);


const Diskio_drvTypeDef  tSD_Driver =
{
    SD_initialize,
    SD_status,
    SD_read,
    SD_write,
    SD_ioctl,
};

/* Private functions ---------------------------------------------------------*/
static DSTATUS SD_CheckStatus(BYTE lun)
{
    Stat = STA_NOINIT;

    if(BSP_SD_GetCardState() == MSD_OK)
    {
        Stat &= ~STA_NOINIT;
    }

    return Stat;
}

/**
    @brief  Initializes a Drive
    @param  lun : not used
    @retval DSTATUS: Operation status
*/
DSTATUS SD_initialize(BYTE lun)
{
    Stat = STA_NOINIT;

    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)	//RTOS is started
    {
        #if !defined(DISABLE_SD_INIT)

        if(BSP_SD_Init() == MSD_OK)
        {
            Stat = SD_CheckStatus(lun);
        }

        #else
        Stat = SD_CheckStatus(lun);
        #endif


        //if the SD is correctly initialized, create the operation queue		
        if(Stat != STA_NOINIT)
        {
            xSDQueue_Handle = osMessageQueueNew(QUEUE_SIZE, sizeof(uint32_t), NULL);	//CMSIS-RTOS2封装过的函数
//			xSDQueue_Handle = xQueueCreate(QUEUE_SIZE, sizeof (uint32_t));	//创建队列
        }
    }

    return Stat;
}

/**
    @brief  Gets Disk Status
    @param  lun : not used
    @retval DSTATUS: Operation status
*/
DSTATUS SD_status(BYTE lun)
{
    return SD_CheckStatus(lun);
}

/**
    @brief  Reads Sector(s)
    @param  lun : not used
    @param  *buff: Data buffer to store read data
    @param  sector: Sector address (LBA)
    @param  count: Number of sectors to read (1..128)
    @retval DRESULT: Operation result
*/
DRESULT SD_read(BYTE lun, BYTE *buff, DWORD sector, UINT count)
{
    uint32_t ulTimer;
    uint32_t ulReceiveMessage = 0;
    DRESULT res = RES_ERROR;
    osStatus_t sta;
    uint32_t ulTimeOut, n;
    BaseType_t xTaskWokenByReceive = pdFALSE;

    #if (ENABLE_SD_DMA_CACHE_MAINTENANCE == 1)
    uint32_t alignedAddr;
    #endif

    if(IS_IRQ())
    {
        ulTimeOut = 0;
    }
    else
    {
        ulTimeOut = SD_TIMEOUT;
    }

    //if(Get_DCahceSta())SCB_CleanDCache(); //如果开启了D_Cache，一定要先清缓存!!!!!!

    if((uint32_t)buff % 4 != 0)
    {
        for(n = 0; n < count; n++)
        {           
            if(BSP_SD_ReadBlocks_DMA((uint8_t *)SDIO_DATA_BUFFER, sector + 512 * n, 1) == MSD_OK)
            {
                res = RES_ERROR;

                //这里用CMSIS-RTOS2封装过的函数,从队列接收信息
                sta = osMessageQueueGet(xSDQueue_Handle, &ulReceiveMessage, NULL, ulTimeOut);
                if(osOK == sta)
                {
                    if(ulReceiveMessage == READ_CPLT_MSG)
                    {
                        ulTimer = osKernelGetTickCount() + SD_TIMEOUT;

                        /* block until SDIO IP is ready or a timeout occur */
                        while(ulTimer > osKernelGetTickCount())
                        {
                            if(BSP_SD_GetCardState() == SD_TRANSFER_OK)
                            {
                                res = RES_OK;
                                #if (ENABLE_SD_DMA_CACHE_MAINTENANCE == 1)
                                /*
                                    the SCB_InvalidateDCache_by_Addr() requires a 32-Byte aligned address,
                                    adjust the address and the D-Cache size to invalidate accordingly.
                                */
                                alignedAddr = (uint32_t)buff & ~0x1F;
                                SCB_InvalidateDCache_by_Addr((uint32_t *)alignedAddr, count * BLOCKSIZE + ((uint32_t)buff - alignedAddr));
                                #endif
                                break;
                            }
                        }
                    }
                }
            }
		
            memcpy(buff, SDIO_DATA_BUFFER, 512);
            buff += 512;
        }
    }
    else
    {
        if(BSP_SD_ReadBlocks_DMA(buff, sector, count) == MSD_OK)
        {
            res = RES_ERROR;

            //这里用CMSIS-RTOS2封装过的函数,从队列接收信息
            sta = osMessageQueueGet(xSDQueue_Handle, &ulReceiveMessage, NULL, ulTimeOut);
            if(osOK == sta)
            {
                if(ulReceiveMessage == READ_CPLT_MSG)
                {
                    ulTimer = osKernelGetTickCount() + SD_TIMEOUT;

                    /* block until SDIO IP is ready or a timeout occur */
                    while(ulTimer > osKernelGetTickCount())
                    {
                        if(BSP_SD_GetCardState() == SD_TRANSFER_OK)
                        {
                            res = RES_OK;
                            #if (ENABLE_SD_DMA_CACHE_MAINTENANCE == 1)
                            /*
                                the SCB_InvalidateDCache_by_Addr() requires a 32-Byte aligned address,
                                adjust the address and the D-Cache size to invalidate accordingly.
                            */
                            alignedAddr = (uint32_t)buff & ~0x1F;
                            SCB_InvalidateDCache_by_Addr((uint32_t *)alignedAddr, count * BLOCKSIZE + ((uint32_t)buff - alignedAddr));
                            #endif
                            break;
                        }
                    }
                }
            }
        }     
    }

    //if(Get_DCahceSta())SCB_CleanDCache(); //DMA完成，清缓存

    return res;
}

/**
    @brief  Writes Sector(s)
    @param  lun : not used
    @param  *buff: Data to be written
    @param  sector: Sector address (LBA)
    @param  count: Number of sectors to write (1..128)
    @retval DRESULT: Operation result
*/
DRESULT SD_write(BYTE lun, const BYTE *buff, DWORD sector, UINT count)
{
    uint32_t ulTimer;
    uint32_t ulReceiveMessage = 0;
    DRESULT res = RES_ERROR;
    osStatus_t sta;
    uint32_t ulTimeOut, n;

    if(IS_IRQ())
    {
        ulTimeOut = 0;
    }
    else
    {
        ulTimeOut = SD_TIMEOUT;
    }

    #if (ENABLE_SD_DMA_CACHE_MAINTENANCE == 1)
    uint32_t alignedAddr;
    /*
        the SCB_CleanDCache_by_Addr() requires a 32-Byte aligned address
        adjust the address and the D-Cache size to clean accordingly.
    */
    alignedAddr = (uint32_t)buff & ~0x1F;
    SCB_CleanDCache_by_Addr((uint32_t *)alignedAddr, count * BLOCKSIZE + ((uint32_t)buff - alignedAddr));
    #endif

    //if(Get_DCahceSta())SCB_CleanDCache(); //如果开启了D_Cache，一定要先清缓存!!!!!!

    if((uint32_t)buff % 4 != 0)
    {
        for(n = 0; n < count; n++)
        {
            memcpy((uint8_t *)buff, SDIO_DATA_BUFFER, 512);
            
            if(BSP_SD_WriteBlocks_DMA((uint8_t *)SDIO_DATA_BUFFER, sector + 512 * n, 1) == MSD_OK)
            {
                res = RES_ERROR;

                //这里用CMSIS-RTOS2封装过的函数,从队列接收信息
                sta = osMessageQueueGet(xSDQueue_Handle, &ulReceiveMessage, NULL, ulTimeOut);
                if(osOK == sta)
                {
                    if(ulReceiveMessage == WRITE_CPLT_MSG)
                    {
                        ulTimer = osKernelGetTickCount() + SD_TIMEOUT;

                        /* block until SDIO IP is ready or a timeout occur */
                        while(ulTimer > osKernelGetTickCount())
                        {
                            if(BSP_SD_GetCardState() == SD_TRANSFER_OK)
                            {
                                res = RES_OK;
                                break;
                            }
                        }
                    }
                }
            }            
            buff += 512;
        }
    }
    else
    {
        if(BSP_SD_WriteBlocks_DMA((uint8_t *)buff, sector, count) == MSD_OK)
        {
            res = RES_ERROR;

            //这里用CMSIS-RTOS2封装过的函数,从队列接收信息
            sta = osMessageQueueGet(xSDQueue_Handle, &ulReceiveMessage, NULL, ulTimeOut);
            if(osOK == sta)
            {
                if(ulReceiveMessage == WRITE_CPLT_MSG)
                {
                    ulTimer = osKernelGetTickCount() + SD_TIMEOUT;

                    /* block until SDIO IP is ready or a timeout occur */
                    while(ulTimer > osKernelGetTickCount())
                    {
                        if(BSP_SD_GetCardState() == SD_TRANSFER_OK)
                        {
                            res = RES_OK;
                            break;
                        }
                    }
                }
            }
        }    
    }

    //if(Get_DCahceSta())SCB_CleanDCache(); //DMA完成，清缓存

    return res;
}

/**
    @brief  I/O control operation
    @param  lun : not used
    @param  cmd: Control code
    @param  *buff: Buffer to send/receive control data
    @retval DRESULT: Operation result
*/
DRESULT SD_ioctl(BYTE lun, BYTE cmd, void *buff)
{
    DRESULT res = RES_ERROR;
    BSP_SD_CardInfo CardInfo;

    if(Stat & STA_NOINIT)
    {
        return RES_NOTRDY;
    }

    switch(cmd)
    {
        /* Make sure that no pending write process */
        case CTRL_SYNC:
            res = RES_OK;
            break;

        /* Get number of sectors on the disk (DWORD) */
        case GET_SECTOR_COUNT:
            BSP_SD_GetCardInfo(&CardInfo);
            *(DWORD *)buff = CardInfo.LogBlockNbr;
            res = RES_OK;
            break;

        /* Get R/W sector size (WORD) */
        case GET_SECTOR_SIZE:
            BSP_SD_GetCardInfo(&CardInfo);
            *(WORD *)buff = CardInfo.LogBlockSize;
            res = RES_OK;
            break;

        /* Get erase block size in unit of sector (DWORD) */
        case GET_BLOCK_SIZE:
            BSP_SD_GetCardInfo(&CardInfo);
            *(DWORD *)buff = CardInfo.LogBlockSize / SD_DEFAULT_BLOCK_SIZE;
            res = RES_OK;
            break;

        default:
            res = RES_PARERR;
    }

    return res;
}

/**
    @brief Tx Transfer completed callbacks
    @param
    @retval None
*/
void BSP_SD_WriteCpltCallback(void)
{
    uint32_t ulSendMessage = WRITE_CPLT_MSG;
    uint32_t ulTimeOut;

    if(IS_IRQ())
    {
        ulTimeOut = 0;
    }
    else
    {
        ulTimeOut = SD_TIMEOUT;
    }
    osMessageQueuePut(xSDQueue_Handle, &ulSendMessage, NULL, ulTimeOut);	//向队列中发送数据，SD_write里获取
    //osMessageQueuePut(xSDQueue_Handle, &ulSendMessage, NULL, osWaitForever);	//向队列中发送数据，SD_write里获取
}

/**
    @brief Rx Transfer completed callbacks
    @param
    @retval None
*/
void BSP_SD_ReadCpltCallback(void)
{
    uint32_t ulSendMessage = READ_CPLT_MSG;
    uint32_t ulTimeOut;

    if(IS_IRQ())
    {
        ulTimeOut = 0;
    }
    else
    {
        ulTimeOut = SD_TIMEOUT;
    }
    osMessageQueuePut(xSDQueue_Handle, &ulSendMessage, NULL, ulTimeOut);	//向队列中发送数据,SD_read里获取
    //osMessageQueuePut(xSDQueue_Handle, &ulSendMessage, NULL, osWaitForever);    //向队列中发送数据,SD_read里获取
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

