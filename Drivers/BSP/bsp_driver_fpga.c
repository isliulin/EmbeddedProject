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

uint32_t ulFpgaCountFrequency = 16368000L;    //FPGA����ʱ�ӱ�׼Ƶ��

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
    
    fpgaWriteByte(FPGA_SYS_RST_REG, SYS_RST | PROTECT_RST);             //ϵͳ��λ,������λ  
    Delay_ms(1000);
    fpgaWriteByte(FPGA_SYS_RST_REG, SYS_SET | PROTECT_SET);             //ϵͳ��λ,������λ 

    //uint8_t ucErrorStatus = fpgaReadByte(PROTECT_STATUS_REG);//R,����״̬�Ĵ���

    fpgaWriteByte(FPGA_SEND_CTRL_REG, SEND_STOP_CTRL);    //����ֹͣ
    // 
    fpgaWriteByte(FPGA_AD_RST_REG, SAMPLE_RST);           //AD��λ
    Delay_ms(2);
    fpgaWriteByte(FPGA_AD_RST_REG, SAMPLE_SET);           //AD��λ

    fpgaWriteByte(FPGA_SAMPLE_CMD_REG, SAMPLE_DISABLE);   //������ֹ
    fpgaWriteByte(FPGA_AD_CMD_REG, AD_CMD_DISABLE);       //AD�����ֹ

    fpgaWriteByte(FPGA_AD_FIFO_RST_REG, AD_FIFO_RST);     //AD��FIFO��λ
    Delay_ms(2);
    fpgaWriteByte(FPGA_AD_FIFO_RST_REG, AD_FIFO_SET);     //AD��FIFO��λ    
}


/*******************************************************
  * @brief  : ��ָ����ַ(WriteAddr+SRAM_BANK_ADDR)��ʼ,����д��ulLen���ֽ�.
  * @note   :
  * @param  : WriteAddr:Ҫд��ĵ�ַ
  * @param  : pBuffer:�ֽ�ָ�� Pointer to source buffer to write
  * @param  : ulLen:Ҫд����ֽ���
  * @return :
*******************************************************/
HAL_StatusTypeDef fpgaWriteBuffer(uint32_t ulWriteAddr, uint8_t *pucBuffer, uint32_t ulLen)
{
    HAL_StatusTypeDef eStatus;
    eStatus = HAL_SRAM_Write_8b(&sramHandle, (uint32_t *)(SRAM_BANK_ADDR + ulWriteAddr), pucBuffer, ulLen);
    return eStatus;
}

/*******************************************************
  * @brief  : ��ָ����ַ(WriteAddr+SRAM_BANK_ADDR)��ʼ,��������ulLen���ֽ�.Reads 8-bit buffer from SRAM memory.
  * @note   :
  * @param  : WriteAddr:Ҫд��ĵ�ַ
  * @param  : pBuffer:�ֽ�ָ�� Pointer to destination buffer
  * @param  : ulLen:Ҫ�������ֽ���
  * @return :
*******************************************************/
HAL_StatusTypeDef fpgaReadBuffer(uint32_t ulReadAddr, uint8_t *pucBuffer, uint32_t ulLen)
{
    HAL_StatusTypeDef eStatus;
    eStatus = HAL_SRAM_Read_8b(&sramHandle, (uint32_t *)(SRAM_BANK_ADDR + ulReadAddr), pucBuffer, ulLen);
    return eStatus;
}

/*******************************************************
  * @brief  : ��ָ����ַ(WriteAddr+SRAM_BANK_ADDR)д��1���ֽ�
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