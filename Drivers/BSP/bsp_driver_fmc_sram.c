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

SRAM_HandleTypeDef sramHandle;


//-------------------------------------------------------------

/*******************************************************
  * @brief  : 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void BSP_SRAM_Init(void)
{    
    FMC_NORSRAM_TimingTypeDef Timing;    
    FMC_NORSRAM_TimingTypeDef FSMC_ReadTim;
    FMC_NORSRAM_TimingTypeDef FSMC_WriteTim;

    /* SRAM device configuration */
    sramHandle.Instance = FMC_NORSRAM_DEVICE;
    sramHandle.Extended = FMC_NORSRAM_EXTENDED_DEVICE;
  
    /* Timing configuration derived from system clock (up to 216Mhz)
       for 108Mhz as SRAM clock frequency */
 /*   Timing.AddressSetupTime      = 2;
    Timing.AddressHoldTime       = 1;
    Timing.DataSetupTime         = 2;
    Timing.BusTurnAroundDuration = 1;
    Timing.CLKDivision           = 2;
    Timing.DataLatency           = 2;
    Timing.AccessMode            = FMC_ACCESS_MODE_A;*/
        
                                                            //FSMC��ʱ����ƼĴ���
    FSMC_ReadTim.AddressSetupTime = 0x11;                   //��ַ����ʱ��(ADDSET)Ϊ17��HCLK 1/216M=4.6ns*17=78ns
    FSMC_ReadTim.AddressHoldTime  = 0x00;
    FSMC_ReadTim.DataSetupTime    = 0x55;                   //���ݱ���ʱ��(DATAST)Ϊ85��HCLK	=4.6*85=391ns
    FSMC_ReadTim.AccessMode       = FMC_ACCESS_MODE_A;      //ģʽA
    
                                                            //FSMCдʱ����ƼĴ���
    FSMC_WriteTim.AddressSetupTime = 0x15;                  //��ַ����ʱ��(ADDSET)Ϊ21��HCLK=96ns
    FSMC_WriteTim.AddressHoldTime  = 0x00;
    FSMC_WriteTim.DataSetupTime    = 0x15;                  //���ݱ���ʱ��(DATAST)Ϊ4.6ns*21��HCLK=96ns
    FSMC_WriteTim.AccessMode       = FMC_ACCESS_MODE_A;     //ģʽA
  
    sramHandle.Init.NSBank             = FMC_NORSRAM_BANK1;            //ʹ��NE1
    sramHandle.Init.DataAddressMux     = FMC_DATA_ADDRESS_MUX_DISABLE; //������������
    sramHandle.Init.MemoryType         = FMC_MEMORY_TYPE_SRAM;         //SRAM
    sramHandle.Init.MemoryDataWidth    = FMC_NORSRAM_MEM_BUS_WIDTH_8;  //8λ���ݿ��
    sramHandle.Init.BurstAccessMode    = FMC_BURST_ACCESS_MODE_DISABLE;//�Ƿ�ʹ��ͻ������,����ͬ��ͻ���洢����Ч,�˴�δ�õ�
    sramHandle.Init.WaitSignalPolarity = FMC_WAIT_SIGNAL_POLARITY_LOW; //�ȴ��źŵļ���,����ͻ��ģʽ����������
    sramHandle.Init.WaitSignalActive   = FMC_WAIT_TIMING_BEFORE_WS;    //�洢�����ڵȴ�����֮ǰ��һ��ʱ�����ڻ��ǵȴ������ڼ�ʹ��NWAIT
    sramHandle.Init.WriteOperation     = FMC_WRITE_OPERATION_ENABLE;   //�洢��дʹ��
    sramHandle.Init.WaitSignal         = FMC_WAIT_SIGNAL_DISABLE;      //�ȴ�ʹ��λ,�˴�δ�õ�
    sramHandle.Init.ExtendedMode       = FMC_EXTENDED_MODE_DISABLE;    //��дʹ�ò�ͬ��ʱ��
    sramHandle.Init.AsynchronousWait   = FMC_ASYNCHRONOUS_WAIT_DISABLE;//�Ƿ�ʹ��ͬ������ģʽ�µĵȴ��ź�,�˴�δ�õ�
    sramHandle.Init.WriteBurst         = FMC_WRITE_BURST_DISABLE;      //��ֹͻ��д
    sramHandle.Init.ContinuousClock    = FMC_CONTINUOUS_CLOCK_SYNC_ONLY;

    /* Initialize the SRAM controller */
    if (HAL_SRAM_Init(&sramHandle, &FSMC_ReadTim, &FSMC_WriteTim) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }
}

/*******************************************************
  * @brief  : 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @return : 
*******************************************************/
//����MPU��region(SRAM����Ϊ͸дģʽ)
void SRAM_MPU_Config(void)
{
    MPU_Region_InitTypeDef MPU_Initure;

    HAL_MPU_Disable();							                    //����MPU֮ǰ�ȹر�MPU,��������Ժ���ʹ��MPU	

    //�ⲿSRAMΪregion0��������ɶ�д
    MPU_Initure.Enable           = MPU_REGION_ENABLE;	            //ʹ��region
    MPU_Initure.Number           = MPU_REGION_NUMBER0;		        //����region���ⲿSRAMʹ�õ�region0
    MPU_Initure.BaseAddress      = SRAM_BANK_ADDR;	                //region����ַ
    MPU_Initure.Size             = MPU_REGION_SIZE_256MB;			//region��С
    MPU_Initure.SubRegionDisable = 0x00;
    MPU_Initure.TypeExtField     = MPU_TEX_LEVEL0;
    MPU_Initure.AccessPermission = MPU_REGION_FULL_ACCESS;	        //��region�ɶ�д
    MPU_Initure.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;	//�����ȡ�������е�ָ��
    MPU_Initure.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
    MPU_Initure.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;
    MPU_Initure.IsBufferable     = MPU_ACCESS_BUFFERABLE;
    HAL_MPU_ConfigRegion(&MPU_Initure);
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);                         //����MPU
}

/*******************************************************
  * @brief  : SRAM�ײ�������ʱ��ʹ�ܣ����ŷ���
  * @note   : �˺����ᱻHAL_SRAM_Init()����
  * @param  : hsram:SRAM���
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_FMC_CLK_ENABLE();                  //ʹ��FMCʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();                //ʹ��GPIOCʱ��
    __HAL_RCC_GPIOD_CLK_ENABLE();                //ʹ��GPIODʱ��
    __HAL_RCC_GPIOE_CLK_ENABLE();                //ʹ��GPIOEʱ��
    __HAL_RCC_GPIOF_CLK_ENABLE();                //ʹ��GPIOFʱ��
    __HAL_RCC_GPIOG_CLK_ENABLE();                //ʹ��GPIOGʱ��
 
    /** FMC GPIO Configuration 	
    PF0   ------> FMC_A0
    PF1   ------> FMC_A1
    PF2   ------> FMC_A2
    PF3   ------> FMC_A3
    PF4   ------> FMC_A4
    PF5   ------> FMC_A5
    PC0   ------> FMC_SDNWE
    PC2   ------> FMC_SDNE0
    PC3   ------> FMC_SDCKE0
    PF11   ------> FMC_SDNRAS
    PF12   ------> FMC_A6
    PF13   ------> FMC_A7
    PF14   ------> FMC_A8
    PF15   ------> FMC_A9
    PG0   ------> FMC_A10
    PG1   ------> FMC_A11
    PE7   ------> FMC_D4
    PE8   ------> FMC_D5
    PE9   ------> FMC_D6
    PE10   ------> FMC_D7
    PE11   ------> FMC_D8
    PE12   ------> FMC_D9
    PE13   ------> FMC_D10
    PE14   ------> FMC_D11
    PE15   ------> FMC_D12
    PD8   ------> FMC_D13
    PD9   ------> FMC_D14
    PD10   ------> FMC_D15
    PD14   ------> FMC_D0
    PD15   ------> FMC_D1
    PG2   ------> FMC_A12
    PG4   ------> FMC_BA0
    PG5   ------> FMC_BA1
    PG8   ------> FMC_SDCLK
    PD0   ------> FMC_D2
    PD1   ------> FMC_D3
    PD4   ------> FMC_NOE
    PD5   ------> FMC_NWE
    PD7   ------> FMC_NE1
    PG15   ------> FMC_SDNCAS
    PE0   ------> FMC_NBL0
    PE1   ------> FMC_NBL1
    */

    SRAM_MPU_Config();//ʹ��MPU����SRAM����

    /* Common GPIO configuration */
    GPIO_Initure.Mode      = GPIO_MODE_AF_PP;   //���츴��
    GPIO_Initure.Pull      = GPIO_PULLUP;       //����
    GPIO_Initure.Speed     = GPIO_SPEED_HIGH;   //����
    GPIO_Initure.Alternate = GPIO_AF12_FMC;     //����ΪFMC

    //��ʼ��PC0,2,3
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3;  
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);       //��ʼ��

    //��ʼ��PD0,1,4,5,7,8,9,10,14,15
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | \
                       GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15; 				   
    HAL_GPIO_Init(GPIOD, &GPIO_Initure);  //��ʼ��

    //��ʼ��PE0,1,7,8,9,10,11,12,13,14,15
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | \
                       GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;				   
    HAL_GPIO_Init(GPIOE, &GPIO_Initure);  //��ʼ��

    //��ʼ��PF0,1,2,3,4,5,11,12,13,14,15
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | \
                       GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;				   
    HAL_GPIO_Init(GPIOF, &GPIO_Initure);  //��ʼ��

    //��ʼ��PG0,1,2,4,5,8,15
    GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | \
                       GPIO_PIN_5 | GPIO_PIN_8 | GPIO_PIN_15;				   
    HAL_GPIO_Init(GPIOG, &GPIO_Initure);  //��ʼ��
}

/*******************************************************
  * @brief  : ��ָ����ַ(WriteAddr+SRAM_BANK_ADDR)��ʼ,����д��ulLen���ֽ�.Writes 8-bit buffer to SRAM memory.
  * @note   : 
  * @param  : pBuffer:�ֽ�ָ�� Pointer to source buffer to write
  * @param  : WriteAddr:Ҫд��ĵ�ַ
  * @param  : ulLen:Ҫд����ֽ���
  * @return : 
*******************************************************/
HAL_StatusTypeDef FMC_SRAM_WriteBuffer(uint8_t *pucBuffer, uint32_t ulWriteAddr, uint32_t ulLen)
{
//    for (; ulLen != 0; ulLen--)
//    {
//        *(__IO uint8_t *)(SRAM_BANK_ADDR + ulWriteAddr) = *pucBuffer;
//        ulWriteAddr++;
//        pucBuffer++;
//    }
    HAL_StatusTypeDef eStatus;
    eStatus = HAL_SRAM_Write_8b(&sramHandle, (uint32_t *)(SRAM_BANK_ADDR + ulWriteAddr), pucBuffer, ulLen);
    return eStatus;
}

/*******************************************************
  * @brief  : ��ָ����ַ(WriteAddr+SRAM_BANK_ADDR)��ʼ,��������ulLen���ֽ�.Reads 8-bit buffer from SRAM memory.
  * @note   : 
  * @param  : pBuffer:�ֽ�ָ�� Pointer to destination buffer
  * @param  : WriteAddr:Ҫд��ĵ�ַ
  * @param  : ulLen:Ҫ�������ֽ���
  * @return : 
*******************************************************/
HAL_StatusTypeDef FMC_SRAM_ReadBuffer(uint8_t *pucBuffer, uint32_t ulReadAddr, uint32_t ulLen)
{
    //    for (; ulLen != 0; ulLen--)
    //    {
    //        *pucBuffer++ = *(__IO uint8_t *)(SRAM_BANK_ADDR + ulReadAddr);
    //        ulReadAddr++;
    //    }
    HAL_StatusTypeDef eStatus;
    eStatus = HAL_SRAM_Read_8b(&sramHandle, (uint32_t *)(SRAM_BANK_ADDR + ulReadAddr), pucBuffer, ulLen);
    return eStatus;
}

//---------------------------------------------------
