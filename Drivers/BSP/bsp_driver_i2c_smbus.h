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

#ifndef __BSP_DRIVER_I2C_SMBUS_H
#define __BSP_DRIVER_I2C_SMBUS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Definition for I2C2 clock resources */
#define I2C2_CLK_ENABLE()               __HAL_RCC_I2C2_CLK_ENABLE()
#define I2C2_SDA_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOH_CLK_ENABLE()
#define I2C2_SCL_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOH_CLK_ENABLE()

#define I2C2_FORCE_RESET()              __HAL_RCC_I2C2_FORCE_RESET()
#define I2C2_RELEASE_RESET()            __HAL_RCC_I2C2_RELEASE_RESET()

/* Definition for I2C2 Pins */
#define I2C2_SCL_PIN                    GPIO_PIN_4
#define I2C2_SCL_GPIO_PORT              GPIOH
#define I2C2_SDA_PIN                    GPIO_PIN_5
#define I2C2_SDA_GPIO_PORT              GPIOH
#define I2C2_GPIO_PORT                  GPIOH

extern SMBUS_HandleTypeDef smbus2Handle;

void BSP_I2C_SMBUS_Init(void);

#ifdef __cplusplus
}
#endif

#endif //__BSP_DRIVER_I2C_SMBUS_H