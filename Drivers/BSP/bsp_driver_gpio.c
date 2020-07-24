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

#include "include.h"

/*******************************************************
  * @brief  : 
  * @note   : 
  * @param  : 
  * @param  : 
  * @param  : 
  * @retval : 
*******************************************************/
void BSP_GPIO_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // GPIO PORTs Clock Enable 
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE(); 
    __HAL_RCC_GPIOC_CLK_ENABLE();
//    __HAL_RCC_GPIOD_CLK_ENABLE();
//    __HAL_RCC_GPIOE_CLK_ENABLE();
//    __HAL_RCC_GPIOF_CLK_ENABLE();
//    __HAL_RCC_GPIOG_CLK_ENABLE();
//    __HAL_RCC_GPIOH_CLK_ENABLE();
//    __HAL_RCC_GPIOI_CLK_ENABLE();


    //HAL_GPIO_WritePin(GPIOA, GPS_PPS_PIN | FPGA_PPS_PIN, GPIO_PIN_RESET);

    //HAL_GPIO_WritePin(FPGA_IRQ_GPIO_PORT, FPGA_IRQ_PIN, GPIO_PIN_RESET);

    //Configure GPIO PIN Output Level 
    HAL_GPIO_WritePin(GPIOA, FAULT_LED_PIN | SEND_LED_PIN | START_SEND_PIN | STOP_SEND_PIN, GPIO_PIN_RESET);

    //Configure GPIO PIN Output Level 
    HAL_GPIO_WritePin(GPIOC, OVERCURRENT_LED_PIN | ONLINE_LED_PIN, GPIO_PIN_RESET);

    //Configure GPIO PIN Output Level 
    HAL_GPIO_WritePin(EXT_POWER_CHARGE_CTRL_GPIO_PORT, EXT_POWER_CHARGE_CTRL_PIN, GPIO_PIN_SET);

    //Configure GPIO PINs : PCPIN PCPIN 
    GPIO_InitStruct.Pin   = ONLINE_LED_PIN | OVERCURRENT_LED_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    //Configure GPIO PINs : PAPIN PAPIN 
    GPIO_InitStruct.Pin  = GPS_PPS_PIN | FPGA_PPS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    //Configure GPIO PINs : PAPIN PAPIN PAPIN 
    GPIO_InitStruct.Pin   = FAULT_LED_PIN | SEND_LED_PIN | START_SEND_PIN | STOP_SEND_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
 
    //Configure GPIO PIN : PtPIN   
    GPIO_InitStruct.Pin  = EXTERNAL_POWER_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(EXTERNAL_POWER_GPIO_PORT, &GPIO_InitStruct);
    
    //Configure GPIO PIN : PtPIN 
    GPIO_InitStruct.Pin   = EXT_POWER_CHARGE_CTRL_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(EXT_POWER_CHARGE_CTRL_GPIO_PORT, &GPIO_InitStruct);

    //Configure GPIO pin : FPGA_IRQ_PIN改为轮询,初始化为输入
 /*   GPIO_InitStruct.Pin  = FPGA_IRQ_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(FPGA_IRQ_GPIO_PORT, &GPIO_InitStruct);*/
    GPIO_InitStruct.Pin = FPGA_IRQ_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(FPGA_IRQ_GPIO_PORT, &GPIO_InitStruct);
/*
    // EXTI interrupt init
    HAL_NVIC_SetPriority(EXTI1_IRQn, 15, 0); //PA1，GPS_PPS_PIN
    //HAL_NVIC_EnableIRQ(EXTI1_IRQn);//开 PA1，GPS_PPS_PIN中断

    HAL_NVIC_SetPriority(EXTI3_IRQn, 10, 0); //PB3，EXTERNAL_POWER_PIN
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);

    HAL_NVIC_SetPriority(EXTI9_5_IRQn, 7, 0); //PB5，FPGA_IRQ_PIN。 PA8，FPGA_PPS_PIN
    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn); //开 PA8：FPGA_PPS_PIN中断*/
}

//----------------------------------------------------