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

#include "include.h"

uint32_t ulFpgaCountFrequency = 16368000L;    //FPGA计数时钟标准频率

/*******************************************************
  * @brief  : 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @return : 
*******************************************************/
void BSP_FPGA_Init(void)
{
    BSP_SRAM_Init();
    
    fpgaWriteByte(FPGA_SYS_RST_REG, SYS_RST | PROTECT_RST);             //系统复位,保护复位  
    Delay_ms(1000);
    fpgaWriteByte(FPGA_SYS_RST_REG, SYS_SET | PROTECT_SET);             //系统置位,保护置位 

    //uint8_t ucErrorStatus = fpgaReadByte(PROTECT_STATUS_REG);//R,保护状态寄存器

    fpgaWriteByte(FPGA_SEND_CTRL_REG, SEND_STOP_CTRL);    //发射停止
    // 
    fpgaWriteByte(FPGA_AD_RST_REG, SAMPLE_RST);           //AD复位
    Delay_ms(2);
    fpgaWriteByte(FPGA_AD_RST_REG, SAMPLE_SET);           //AD置位

    fpgaWriteByte(FPGA_SAMPLE_CMD_REG, SAMPLE_DISABLE);   //采样禁止
    fpgaWriteByte(FPGA_AD_CMD_REG, AD_CMD_DISABLE);       //AD命令禁止

    fpgaWriteByte(FPGA_AD_FIFO_RST_REG, AD_FIFO_RST);     //AD的FIFO复位
    Delay_ms(2);
    fpgaWriteByte(FPGA_AD_FIFO_RST_REG, AD_FIFO_SET);     //AD的FIFO置位    
}


/*******************************************************
  * @brief  : 在指定地址(WriteAddr+SRAM_BANK_ADDR)开始,连续写入ulLen个字节.
  * @note   :
  * @param  : WriteAddr:要写入的地址
  * @param  : pBuffer:字节指针 Pointer to source buffer to write
  * @param  : ulLen:要写入的字节数
  * @return :
*******************************************************/
HAL_StatusTypeDef fpgaWriteBuffer(uint32_t ulWriteAddr, uint8_t *pucBuffer, uint32_t ulLen)
{
    HAL_StatusTypeDef eStatus;
    eStatus = HAL_SRAM_Write_8b(&sramHandle, (uint32_t *)(SRAM_BANK_ADDR + ulWriteAddr), pucBuffer, ulLen);
    return eStatus;
}

/*******************************************************
  * @brief  : 在指定地址(WriteAddr+SRAM_BANK_ADDR)开始,连续读出ulLen个字节.Reads 8-bit buffer from SRAM memory.
  * @note   :
  * @param  : WriteAddr:要写入的地址
  * @param  : pBuffer:字节指针 Pointer to destination buffer
  * @param  : ulLen:要读出的字节数
  * @return :
*******************************************************/
HAL_StatusTypeDef fpgaReadBuffer(uint32_t ulReadAddr, uint8_t *pucBuffer, uint32_t ulLen)
{
    HAL_StatusTypeDef eStatus;
    eStatus = HAL_SRAM_Read_8b(&sramHandle, (uint32_t *)(SRAM_BANK_ADDR + ulReadAddr), pucBuffer, ulLen);
    return eStatus;
}

/*******************************************************
  * @brief  : 在指定地址(WriteAddr+SRAM_BANK_ADDR)写入1个字节
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @return : 
*******************************************************/
void fpgaWriteByte(uint32_t ulWriteAddr, uint8_t ucVal)
{
    HAL_SRAM_Write_8b(&sramHandle, (uint32_t *)(SRAM_BANK_ADDR + ulWriteAddr), &ucVal, 1);
}


/*******************************************************
  * @brief  :
  * @note   :
  * @param  :
  * @param  :
  * @param  :
  * @return :
*******************************************************/
uint8_t fpgaReadByte(uint32_t ulReadAddr)
{
    uint8_t ucReadVal = 0;
    HAL_SRAM_Read_8b(&sramHandle, (uint32_t *)(SRAM_BANK_ADDR + ulReadAddr), &ucReadVal, 1);
    return ucReadVal;
}

//----------------------------------------------------------------------------------