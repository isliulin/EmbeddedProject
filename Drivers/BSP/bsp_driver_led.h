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


#ifndef __BSP_DRIVER_LED_H
#define __BSP_DRIVER_LED_H

#ifdef __cplusplus
extern "C" {
    #endif

    //#include "include.h"

    //呼吸灯（板载）
    //错误灯（板载）
    //连机灯（面板
    //过流灯（面板
    //发射灯（面板
    //故障灯（面板）
    typedef enum
    {
        BLN_LED         = 0,
        ERROR_LED       = 1,
        ONLINE_LED      = 2,
        OVERCURRENT_LED = 3,
        SEND_LED        = 4,
        FAULT_LED       = 5
    }LED_TypeDef_E;

    #define LEDn		((uint32_t)6)	//LED number

    //定义LED引脚连接的GPIO端口
    #define BLN_LED_PIN			         GPIO_PIN_0
    #define BLN_LED_GPIO_PORT	         GPIOB

    #define ERROR_LED_PIN		         GPIO_PIN_1
    #define ERROR_LED_GPIO_PORT          GPIOB

    #define ONLINE_LED_PIN               GPIO_PIN_5
    #define ONLINE_LED_GPIO_PORT         GPIOC

    #define OVERCURRENT_LED_PIN          GPIO_PIN_4
    #define OVERCURRENT_LED_GPIO_PORT    GPIOC

    #define SEND_LED_PIN                 GPIO_PIN_5
    #define SEND_LED_GPIO_PORT           GPIOA

    #define FAULT_LED_PIN                GPIO_PIN_4
    #define FAULT_LED_GPIO_PORT          GPIOA


    #define BLN_LED_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOB_CLK_ENABLE()
    #define BLN_LED_GPIO_CLK_DISABLE()            __HAL_RCC_GPIOB_CLK_DISABLE()

    #define ERROR_LED_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOB_CLK_ENABLE()
    #define ERROR_LED_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOB_CLK_DISABLE()

    #define ONLINE_LED_GPIO_CLK_ENABLE()          __HAL_RCC_GPIOC_CLK_ENABLE()
    #define ONLINE_LED_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOC_CLK_DISABLE()

    #define OVERCURRENT_LED_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOC_CLK_ENABLE()
    #define OVERCURRENT_LED_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOC_CLK_DISABLE()

    #define SEND_LED_GPIO_CLK_ENABLE()            __HAL_RCC_GPIOA_CLK_ENABLE()
    #define SEND_LED_GPIO_CLK_DISABLE()           __HAL_RCC_GPIOA_CLK_DISABLE()

    #define FAULT_LED_GPIO_CLK_ENABLE()           __HAL_RCC_GPIOA_CLK_ENABLE()
    #define FAULT_LED_GPIO_CLK_DISABLE()          __HAL_RCC_GPIOA_CLK_DISABLE()
          

    #define LEDx_GPIO_CLK_ENABLE(__INDEX__)  do{  \
     if((__INDEX__) == BLN_LED) BLN_LED_GPIO_CLK_ENABLE();  \
else if((__INDEX__) == ERROR_LED) ERROR_LED_GPIO_CLK_ENABLE();  \
else if((__INDEX__) == ONLINE_LED) ONLINE_LED_GPIO_CLK_ENABLE();  \
else if((__INDEX__) == OVERCURRENT_LED)  OVERCURRENT_LED_GPIO_CLK_ENABLE();  \
else if((__INDEX__) == SEND_LED) SEND_LED_GPIO_CLK_ENABLE();  \
else if((__INDEX__) == FAULT_LED) FAULT_LED_GPIO_CLK_ENABLE();  \
                                            }while(0)

    #define LEDx_GPIO_CLK_DISABLE(__INDEX__)  do{  \
     if((__INDEX__) == BLN_LED) BLN_LED_GPIO_CLK_DISABLE();  \
else if((__INDEX__) == ERROR_LED) ERROR_LED_GPIO_CLK_DISABLE();  \
else if((__INDEX__) == ONLINE_LED) ONLINE_LED_GPIO_CLK_DISABLE();  \
else if((__INDEX__) == OVERCURRENT_LED)  OVERCURRENT_LED_GPIO_CLK_DISABLE();  \
else if((__INDEX__) == SEND_LED) SEND_LED_GPIO_CLK_DISABLE();  \
else if((__INDEX__) == FAULT_LED) FAULT_LED_GPIO_CLK_DISABLE();  \
                                             }while(0)


    void BSP_LED_Init(LED_TypeDef_E Led);
    void BSP_LED_ON(LED_TypeDef_E Led);
    void BSP_LED_OFF(LED_TypeDef_E Led);
    void BSP_LED_TOGGLE(LED_TypeDef_E Led);

    void ONLINE_LED_ON(LED_TypeDef_E Led);
    void ONLINE_LED_OFF(LED_TypeDef_E Led);
    void OVERCURRENT_LED_ON(LED_TypeDef_E Led);
    void OVERCURRENT_LED_OFF(LED_TypeDef_E Led);
    void SEND_LED_ON(LED_TypeDef_E Led);
    void SEND_LED_OFF(LED_TypeDef_E Led);
    void FAULT_LED_ON(LED_TypeDef_E Led);
    void FAULT_LED_OFF(LED_TypeDef_E Led);

    #ifdef __cplusplus
}
#endif

#endif //__BSP_DRIVER_LED_H
