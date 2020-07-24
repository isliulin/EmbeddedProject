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

/* Includes ------------------------------------------------------------------*/
#include "include.h"

#define I2C2_TIMING     0x106069E2  //(80k Rise time = 0ns, Fall time = 0ns) */
#define I2C2_TIMEOUT    0x00008293  //25000000ns

SMBUS_HandleTypeDef smbus2Handle;

/*******************************************************
  * @brief  : 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void BSP_I2C_SMBUS_Init(void)
{
    smbus2Handle.Instance                  = I2C2;
    smbus2Handle.Init.Timing               = I2C2_TIMING;
    smbus2Handle.Init.AnalogFilter         = SMBUS_ANALOGFILTER_ENABLE;
    smbus2Handle.Init.OwnAddress1          = 0;
    smbus2Handle.Init.AddressingMode       = SMBUS_ADDRESSINGMODE_7BIT;
    smbus2Handle.Init.DualAddressMode      = SMBUS_DUALADDRESS_DISABLE;
    smbus2Handle.Init.OwnAddress2          = 0;
    smbus2Handle.Init.OwnAddress2Masks     = SMBUS_OA2_NOMASK;
    smbus2Handle.Init.GeneralCallMode      = SMBUS_GENERALCALL_DISABLE;
    smbus2Handle.Init.NoStretchMode        = SMBUS_NOSTRETCH_DISABLE;
    smbus2Handle.Init.PacketErrorCheckMode = SMBUS_PEC_ENABLE;
    smbus2Handle.Init.PeripheralMode       = SMBUS_PERIPHERAL_MODE_SMBUS_HOST;
    smbus2Handle.Init.SMBusTimeout         = I2C2_TIMEOUT;
    if (HAL_SMBUS_Init(&smbus2Handle) != HAL_OK)
    {
        Error_Handler();
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
void HAL_SMBUS_MspInit(SMBUS_HandleTypeDef *smbusHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (smbusHandle->Instance == I2C2)
    {
        __HAL_RCC_GPIOH_CLK_ENABLE();

        /**I2C2 GPIO Configuration    
        PH4     ------> I2C2_SCL
        PH5     ------> I2C2_SDA 
        */
        GPIO_InitStruct.Pin = I2C2_SCL_PIN | I2C2_SDA_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
        HAL_GPIO_Init(I2C2_GPIO_PORT, &GPIO_InitStruct);

        /* I2C2 clock enable */
        __HAL_RCC_I2C2_CLK_ENABLE();

        /* I2C2 interrupt Init */
        HAL_NVIC_SetPriority(I2C2_EV_IRQn, 8, 0);
        HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
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
void HAL_SMBUS_MspDeInit(SMBUS_HandleTypeDef *smbusHandle)
{

    if (smbusHandle->Instance == I2C2)
    {
        __HAL_RCC_I2C2_CLK_DISABLE();

        /**I2C2 GPIO Configuration    
    PH4     ------> I2C2_SCL
    PH5     ------> I2C2_SDA 
    */
        HAL_GPIO_DeInit(I2C2_GPIO_PORT, I2C2_SCL_PIN | I2C2_SDA_PIN);

        /* I2C2 interrupt Deinit */
        HAL_NVIC_DisableIRQ(I2C2_EV_IRQn);
    }
}




//-----------------------------------------------------------