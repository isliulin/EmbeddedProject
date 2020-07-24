/**
  ******************************************************************************
    @file
    @author
    @version
    @date
    @brief   LED驱动代码
  ******************************************************************************
    @history

  ******************************************************************************
*/


#include "include.h"

//呼吸灯（板载）
//错误灯（板载）
//连机灯（面板
//过流灯（面板
//发射灯（面板
//故障灯（面板）
GPIO_TypeDef* LED_PORT[LEDn] = {BLN_LED_GPIO_PORT, ERROR_LED_GPIO_PORT, ONLINE_LED_GPIO_PORT, OVERCURRENT_LED_GPIO_PORT, SEND_LED_GPIO_PORT, FAULT_LED_GPIO_PORT};

uint32_t LED_PIN[LEDn] = {BLN_LED_PIN, ERROR_LED_PIN, ONLINE_LED_PIN, OVERCURRENT_LED_PIN, SEND_LED_PIN, FAULT_LED_PIN};


/*******************************************************
  * @brief  : Configures LED on GPIO and/or on MFX
  * @note   : 
  * @param  : Led: LED to be configured.
  * @param  : 
  * @param  : 
  * @retval : None
*******************************************************/
void BSP_LED_Init(LED_TypeDef_E Led)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/* Enable the GPIO_LED clock */
	LEDx_GPIO_CLK_ENABLE(Led);

	/* Configure the GPIO_LED pin */
	GPIO_InitStruct.Pin = LED_PIN[Led];
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;	//推挽输出
	GPIO_InitStruct.Pull = GPIO_PULLUP;			//上拉
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	HAL_GPIO_Init(LED_PORT[Led], &GPIO_InitStruct);

	/* By default, turn off LED */
	HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);    
}

/*******************************************************
  * @brief  : Turns selected LED On.
  * @note   : 
  * @param  : Led: LED to be set on
  *           This parameter can be one of the following values:
  *            @arg  BLN_LED
  *            @arg  ERROR_LED
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void BSP_LED_ON(LED_TypeDef_E Led)
{
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);
}

/*******************************************************
  * @brief  : Turns selected LED Off.
  * @note   : 
  * @param  : Led: LED to be set off   
  *           This parameter can be one of the following values:
  *            @arg  BLN_LED
  *            @arg  ERROR_LED
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void BSP_LED_OFF(LED_TypeDef_E Led)
{
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
}

/*******************************************************
  * @brief  : Toggles the selected LED.
  * @note   : 
  * @param  : Led: LED to be toggled
  *          This parameter can be one of the following values:
  *            @arg  BLN_LED
  *            @arg  ERROR_LED
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void BSP_LED_TOGGLE(LED_TypeDef_E Led)
{
  HAL_GPIO_TogglePin(LED_PORT[Led], LED_PIN[Led]);
}

/**
 * @brief :
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
void ONLINE_LED_ON(LED_TypeDef_E Led)
{
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
}

/**
 * @brief :
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
void ONLINE_LED_OFF(LED_TypeDef_E Led)
{
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);
}

/**
 * @brief :
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
void OVERCURRENT_LED_ON(LED_TypeDef_E Led)
{
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
}

/**
 * @brief :
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
void OVERCURRENT_LED_OFF(LED_TypeDef_E Led)
{
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);
}

/** 
 * @brief : 
 * @note  :
 * @param : 
 * @param : 
 * @param : 
 * @return:
 */
void SEND_LED_ON(LED_TypeDef_E Led)
{
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
}

/**
 * @brief :
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
void SEND_LED_OFF(LED_TypeDef_E Led)
{
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);
}

/**
 * @brief :
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
void FAULT_LED_ON(LED_TypeDef_E Led)
{
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET);
}

/**
 * @brief :
 * @note  :
 * @param :
 * @param :
 * @param :
 * @return:
 */
void FAULT_LED_OFF(LED_TypeDef_E Led)
{
    HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET);
}

//----------------------------------------------------------
