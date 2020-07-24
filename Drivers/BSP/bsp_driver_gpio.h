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

#ifndef __BSP_DRIVER_GPIO_H
#define __BSP_DRIVER_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#define GPS_PPS_PIN                         GPIO_PIN_1
#define GPS_PPS_GPIO_PORT                   GPIOA
#define START_SEND_PIN                      GPIO_PIN_6
#define START_SEND_GPIO_PORT                GPIOA
#define STOP_SEND_PIN                       GPIO_PIN_7
#define STOP_SEND_GPIO_PORT                 GPIOA
#define FPGA_PPS_PIN                        GPIO_PIN_8
#define FPGA_PPS_GPIO_PORT                  GPIOA
#define EXTERNAL_POWER_PIN                  GPIO_PIN_3
#define EXTERNAL_POWER_GPIO_PORT            GPIOB
#define EXT_POWER_CHARGE_CTRL_PIN           GPIO_PIN_4
#define EXT_POWER_CHARGE_CTRL_GPIO_PORT     GPIOB
#define FPGA_IRQ_PIN                        GPIO_PIN_5
#define FPGA_IRQ_GPIO_PORT                  GPIOB



void BSP_GPIO_Init(void);

#ifdef __cplusplus
}
#endif

#endif //__BSP_DRIVER_GPIO_H