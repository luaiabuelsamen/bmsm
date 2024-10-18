#ifndef CHARGECONTROLTASK_H
#define CHARGECONTROLTASK_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"  // For STM32 HAL libraries
#include "cmsis_os.h"       // For FreeRTOS APIs
#include <stdio.h>          // For printf

/* Function prototypes -------------------------------------------------------*/

/**
 * @brief Task for controlling the charging and discharging process.
 * @param argument: Not used
 * @retval None
 */
void ChargeControlTask(void *argument);

/**
 * @brief Enables the charging process.
 */
void EnableCharging(void);

/**
 * @brief Disables the charging process.
 */
void DisableCharging(void);

/**
 * @brief Enables the discharging process.
 */
void EnableDischarging(void);

/**
 * @brief Disables the discharging process.
 */
void DisableDischarging(void);

/**
 * @brief Gets the current battery voltage.
 * @retval The current battery voltage in Volts.
 */
float GetBatteryVoltage(void);

/**
 * @brief Gets the current battery temperature.
 * @retval The current battery temperature in Celsius.
 */
float GetBatteryTemperature(void);

#endif /* CHARGECONTROLTASK_H */
