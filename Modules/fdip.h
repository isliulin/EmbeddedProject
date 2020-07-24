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


#pragma once

#ifndef __TDIP_H
#define __TDIP_H

#ifdef __cplusplus
extern "C" {
#endif

    void FDIP_Init(uint16_t usMainFrequency, uint8_t ucFrequencyRatio, uint16_t usSendCycleNumber);
    void vFdipStartTask(void *pvParameters);


#ifdef __cplusplus
}
#endif

#endif	//__TDIP_H