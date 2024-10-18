#ifndef BATTERYMONITORINGTASK_H
#define BATTERYMONITORINGTASK_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal_conf.h"  // For STM32 HAL libraries
#include "cmsis_os.h"       // For FreeRTOS APIs
#include <stdio.h>          // For printf (optional)

/* Public defines ------------------------------------------------------------*/
#define MAX_VOLTAGE 4.2f       // Maximum allowable voltage per cell (Volts)
#define MIN_VOLTAGE 3.0f       // Minimum allowable voltage per cell (Volts)
#define MAX_TEMPERATURE 60.0f  // Maximum allowable temperature (Celsius)
#define MIN_TEMPERATURE 0.0f   // Minimum allowable temperature (Celsius)
#define NUM_CELLS 4            // Number of cells in the battery pack

/* Function prototypes -------------------------------------------------------*/

/**
 * @brief Task for monitoring battery voltage and temperature.
 * @param argument: Not used
 * @retval None
 */
void BatteryMonitoringTask(void *argument);

/**
 * @brief Reads the battery voltage using ADC.
 * @retval The measured battery voltage in Volts.
 */
float ReadVoltage(void);

/**
 * @brief Reads the battery temperature using ADC or other sensors.
 * @retval The measured battery temperature in Celsius.
 */
float ReadTemperature(void);

/**
 * @brief Handles the condition when the battery voltage exceeds the maximum limit.
 */
void HandleOverVoltage(void);

/**
 * @brief Handles the condition when the battery voltage falls below the minimum limit.
 */
void HandleUnderVoltage(void);

/**
 * @brief Handles the condition when the battery temperature exceeds the maximum limit.
 */
void HandleOverTemperature(void);

/**
 * @brief Handles the condition when the battery temperature falls below the minimum limit.
 */
void HandleUnderTemperature(void);

#endif /* BATTERYMONITORINGTASK_H */
