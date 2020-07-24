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


#if defined(SDMMC_DATATIMEOUT)
#define SD_TIMEOUT SDMMC_DATATIMEOUT
#elif defined(SD_DATATIMEOUT)
//#define SD_TIMEOUT SD_DATATIMEOUT
//#else
//#define SD_TIMEOUT 30 * 1000
#endif

#define SD_DEFAULT_BLOCK_SIZE	512

//SD_ReadDisk/SD_WriteDisk����ר��buf,�����������������ݻ�������ַ����4�ֽڶ����ʱ��,
//��Ҫ�õ�������,ȷ�����ݻ�������ַ��4�ֽڶ����.
__attribute__((aligned(4))) uint8_t SDIO_DATA_BUFFER[512];

SD_HandleTypeDef hsd;


HAL_SD_CardInfoTypeDef  SDCardInfo;     //SD����Ϣ�ṹ��              

static volatile DSTATUS Stat = STA_NOINIT;

static volatile  UINT  WriteStatus = 0, ReadStatus = 0;

/* Private function prototypes -----------------------------------------------*/
static DSTATUS SD_CheckStatus(BYTE lun);
DSTATUS SD_initialize(BYTE);
DSTATUS SD_status(BYTE);
DRESULT SD_read(BYTE, BYTE *, DWORD, UINT);
DRESULT SD_write(BYTE, const BYTE *, DWORD, UINT);
DRESULT SD_ioctl(BYTE, BYTE, void *);

//---------------------------------------
const Diskio_drvTypeDef tSD_Driver =
    {
        SD_initialize,
        SD_status,
        SD_read,
        SD_write,
        SD_ioctl,
};

/*******************************************************
  * @brief  : 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @return : 
*******************************************************/
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

	if(BSP_SD_Init() == MSD_OK)
	{
		Stat = SD_CheckStatus(lun);
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
	DRESULT res = RES_ERROR;
	uint32_t n;

	if((uint32_t)buff % 4 != 0)
	{
		for(n = 0; n < count; n++)
		{
			#if (SD_DMA_MODE == 1) 
			res = BSP_SD_ReadBlocks_DMA((uint8_t *)SDIO_DATA_BUFFER, sector + 512 * n, 1);
			if(res != MSD_OK)
			{
				BSP_SD_Init();
				res = BSP_SD_ReadBlocks_DMA((uint8_t *)SDIO_DATA_BUFFER, sector + 512 * n, 1);
			}

			#else
			INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)

			if(BSP_SD_ReadBlocks((uint8_t *)SDIO_DATA_BUFFER,
			   sector + 512 * n, 1, SD_TIMEOUT) == MSD_OK)
			{
				while(BSP_SD_GetCardState() != MSD_OK)
				{
				}
				res = RES_OK;
			}
			else
			{
				if(BSP_SD_ReadBlocks((uint8_t *)SDIO_DATA_BUFFER, sector + 512 * n, 1, SD_TIMEOUT) == MSD_OK)
				{
					while(BSP_SD_GetCardState() != MSD_OK)
					{
					}
					res = RES_OK;
				}
			}
			INTX_ENABLE();//�������ж�			
			#endif			
			memcpy(buff, SDIO_DATA_BUFFER, 512);
			buff += 512;
		}
	}
	else
	{
		#if (SD_DMA_MODE == 1) 
		res = BSP_SD_ReadBlocks_DMA(buff, sector, count);
		if(res != MSD_OK)
		{
			BSP_SD_Init();
			res = BSP_SD_ReadBlocks_DMA(buff, sector, count);
		}
		#else
		INTX_DISABLE();//�ر����ж�

		if(BSP_SD_ReadBlocks(buff, sector, count, SD_TIMEOUT) == MSD_OK)
		{
			while(BSP_SD_GetCardState() != MSD_OK)
			{
			}
			res = RES_OK;
		}
		else
		{
			if(BSP_SD_ReadBlocks(buff, sector, count, SD_TIMEOUT) == MSD_OK)
			{
				while(BSP_SD_GetCardState() != MSD_OK)
				{
				}
				res = RES_OK;
			}
		}
		INTX_ENABLE();//�������ж�		
		#endif
	}

	//	#if (SD_DMA_MODE == 0) 
	//	INTX_ENABLE();//�������ж�
	//	#endif

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
	DRESULT res = RES_ERROR;
	uint32_t n;

	if((uint32_t)buff % 4 != 0)
	{
		for(n = 0; n < count; n++)
		{
			memcpy((uint8_t *)buff, SDIO_DATA_BUFFER, 512);
			#if (SD_DMA_MODE == 1) 

			if(BSP_SD_WriteBlocks_DMA((uint32_t *)SDIO_DATA_BUFFER, sector + 512 * n, 1) != MSD_OK)
			{
				BSP_SD_Init();
				BSP_SD_WriteBlocks_DMA((uint32_t *)SDIO_DATA_BUFFER, sector + 512 * n, 1);
			}
			#else
			INTX_DISABLE();//�ر����ж�

			if(BSP_SD_WriteBlocks((uint8_t *)SDIO_DATA_BUFFER, sector + 512 * n, 1, SD_TIMEOUT) == MSD_OK)
			{
				while(BSP_SD_GetCardState() != MSD_OK)
				{
				}
				res = RES_OK;
			}
			else
			{
				BSP_SD_WriteBlocks((uint8_t *)SDIO_DATA_BUFFER, sector + 512 * n, 1, SD_TIMEOUT);
				if(res == MSD_OK)
				{
					while(BSP_SD_GetCardState() != MSD_OK)
					{
					}
					res = RES_OK;
				}
			}
			INTX_ENABLE();//�������ж�	
			#endif
			buff += 512;
		}
	}
	else
	{
		#if (SD_DMA_MODE == 1) 

		if(BSP_SD_WriteBlocks_DMA((uint8_t *)buff, sector, count) != MSD_OK)
		{
			BSP_SD_Init();
			res = BSP_SD_WriteBlocks_DMA((uint8_t *)buff, sector, count);
		}
		#else
		INTX_DISABLE();//�ر����ж�

		if(BSP_SD_WriteBlocks((uint8_t *)buff, sector, count, SD_TIMEOUT) == MSD_OK)
		{
			while(BSP_SD_GetCardState() != MSD_OK)
			{
			}
			res = RES_OK;
		}
		else
		{
			BSP_SD_WriteBlocks((uint8_t *)buff, sector, count, SD_TIMEOUT);
			if(res == MSD_OK)
			{
				while(BSP_SD_GetCardState() != MSD_OK)
				{
				}
				res = RES_OK;
			}
		}
		INTX_ENABLE();//�������ж�	
		#endif
	}

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
			BSP_SD_GetCardInfo(&SDCardInfo);
			*(DWORD *)buff = SDCardInfo.LogBlockNbr;
			res = RES_OK;
			break;

			/* Get R/W sector size (WORD) */
		case GET_SECTOR_SIZE:
			BSP_SD_GetCardInfo(&SDCardInfo);
			*(WORD *)buff = SDCardInfo.LogBlockSize;
			res = RES_OK;
			break;

			/* Get erase block size in unit of sector (DWORD) */
		case GET_BLOCK_SIZE:
			BSP_SD_GetCardInfo(&SDCardInfo);
			*(DWORD *)buff = SDCardInfo.LogBlockSize / SD_DEFAULT_BLOCK_SIZE;
			res = RES_OK;
			break;

		default:
			res = RES_PARERR;
	}

	return res;
}

//--------------------------------------------------------------------