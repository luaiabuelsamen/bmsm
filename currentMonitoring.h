/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : bms_current_monitoring.h
  * @brief          : Header file for Current Monitoring Logic
  ******************************************************************************
  */
/* USER CODE END Header */

#ifndef __BMS_CURRENT_MONITORING_H
#define __BMS_CURRENT_MONITORING_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Function Prototypes -------------------------------------------------------*/
void CurrentMonitoring_Init(void);
float Read_BatteryCurrent(void);

#ifdef __cplusplus
}
#endif

#endif /* __BMS_CURRENT_MONITORING_H */
