/* Includes ------------------------------------------------------------------*/
#include "ChargeControlTask.h"
#include "cmsis_os.h"  // FreeRTOS APIs
#include "main.h"

/* Private defines -----------------------------------------------------------*/
#define MAX_CHARGE_VOLTAGE 4.2f     // Maximum voltage allowed during charging (V)
#define MIN_DISCHARGE_VOLTAGE 3.0f  // Minimum voltage allowed during discharging (V)
#define MAX_CHARGE_TEMPERATURE 45.0f  // Max temperature for safe charging (C)
#define MAX_DISCHARGE_TEMPERATURE 60.0f // Max temperature for safe discharging (C)

/* Function prototypes -------------------------------------------------------*/
float GetBatteryVoltage(void);
float GetBatteryTemperature(void);
void EnableCharging(void);
void DisableCharging(void);
void EnableDischarging(void);
void DisableDischarging(void);

/* Task implementation -------------------------------------------------------*/
void ChargeControlTask(void *argument) {
    while(1) {
        float voltage = GetBatteryVoltage();
        float temperature = GetBatteryTemperature();

        /* Charge Control Logic */
        if (voltage < MAX_CHARGE_VOLTAGE && temperature < MAX_CHARGE_TEMPERATURE) {
            EnableCharging();
        } else {
            DisableCharging();
        }

        /* Discharge Control Logic */
        if (voltage > MIN_DISCHARGE_VOLTAGE && temperature < MAX_DISCHARGE_TEMPERATURE) {
            EnableDischarging();
        } else {
            DisableDischarging();
        }

        /* Delay to avoid frequent polling */
        osDelay(1000);  // Check every second
    }
}

/* Helper functions ----------------------------------------------------------*/

/**
 * @brief Gets the current battery voltage.
 * @retval The current battery voltage in Volts.
 */
float GetBatteryVoltage(void) {
    // Implement the function to read the current battery voltage.
    // This could be done using an ADC or external monitoring IC.
    return ReadVoltage();  // Assuming the ReadVoltage function is implemented in another module
}

/**
 * @brief Gets the current battery temperature.
 * @retval The current battery temperature in Celsius.
 */
float GetBatteryTemperature(void) {
    // Implement the function to read the current battery temperature.
    // This could be done using an ADC or a temperature sensor.
    return ReadTemperature();  // Assuming the ReadTemperature function is implemented in another module
}

/**
 * @brief Enables the charging process.
 */
void EnableCharging(void) {
    // Implement the logic to enable charging
    // This could be controlling a MOSFET or relay
    printf("Charging enabled.\n");
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);  // Example: Set GPIO pin to enable charging
}

/**
 * @brief Disables the charging process.
 */
void DisableCharging(void) {
    // Implement the logic to disable charging
    printf("Charging disabled.\n");
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);  // Example: Reset GPIO pin to disable charging
}

/**
 * @brief Enables the discharging process.
 */
void EnableDischarging(void) {
    // Implement the logic to enable discharging
    printf("Discharging enabled.\n");
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);  // Example: Set GPIO pin to enable discharging
}

/**
 * @brief Disables the discharging process.
 */
void DisableDischarging(void) {
    // Implement the logic to disable discharging
    printf("Discharging disabled.\n");
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);  // Example: Reset GPIO pin to disable discharging
}
