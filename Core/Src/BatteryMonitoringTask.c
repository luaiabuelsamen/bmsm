/* Includes ------------------------------------------------------------------*/
#include "BatteryMonitoringTask.h"
#include "stm32f4xx_hal_adc.h"
#include "cmsis_os.h"  // FreeRTOS APIs

/* Private defines -----------------------------------------------------------*/
#define MAX_VOLTAGE 4.2f       // Maximum allowable voltage per cell (in Volts)
#define MIN_VOLTAGE 3.0f       // Minimum allowable voltage per cell (in Volts)
#define MAX_TEMPERATURE 60.0f  // Maximum allowable temperature in Celsius
#define MIN_TEMPERATURE 0.0f   // Minimum allowable temperature in Celsius
#define NUM_CELLS 4            // Number of cells in the battery pack

/* Function prototypes -------------------------------------------------------*/
float ReadVoltage(void);
float ReadTemperature(void);
void HandleOverVoltage(void);
void HandleUnderVoltage(void);
void HandleOverTemperature(void);
void HandleUnderTemperature(void);

/* Task implementation -------------------------------------------------------*/

void BatteryMonitoringTask(void *argument) {
    while(1) {
        float voltage = 0;
        float temperature = 0;

        /* Read battery parameters */
        voltage = ReadVoltage();         // Read battery voltage from ADC
        temperature = ReadTemperature(); // Read temperature from ADC

        /* Check voltage levels */
        if (voltage > MAX_VOLTAGE) {
            HandleOverVoltage();
        } else if (voltage < MIN_VOLTAGE) {
            HandleUnderVoltage();
        }

        /* Check temperature levels */
        if (temperature > MAX_TEMPERATURE) {
            HandleOverTemperature();
        } else if (temperature < MIN_TEMPERATURE) {
            HandleUnderTemperature();
        }

        /* Delay for a fixed interval (e.g., check every 1 second) */
        osDelay(1000);
    }
}

/* Helper functions ----------------------------------------------------------*/

/**
 * @brief Reads the battery voltage using ADC.
 * @retval The measured battery voltage in Volts.
 */
float ReadVoltage(void) {
    uint32_t adc_value = 0;
    float voltage = 0.0f;

    /* Start ADC conversion */
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
        adc_value = HAL_ADC_GetValue(&hadc1); // Get the ADC value
    }
    HAL_ADC_Stop(&hadc1);

    /* Convert ADC value to voltage (based on your reference voltage and ADC resolution) */
    voltage = (adc_value / 4096.0f) * 3.3f * NUM_CELLS; // Assuming 12-bit ADC, 3.3V reference

    return voltage;
}

/**
 * @brief Reads the battery temperature using ADC or other sensor.
 * @retval The measured battery temperature in Celsius.
 */
float ReadTemperature(void) {
    uint32_t adc_value = 0;
    float temperature = 0.0f;

    /* Start ADC conversion */
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK) {
        adc_value = HAL_ADC_GetValue(&hadc1); // Get the ADC value
    }
    HAL_ADC_Stop(&hadc1);

    /* Convert ADC value to temperature (based on your sensor characteristics) */
    temperature = (adc_value / 4096.0f) * 100.0f; // Example conversion (depends on sensor)

    return temperature;
}

/**
 * @brief Handles the condition when the battery voltage exceeds the maximum limit.
 */
void HandleOverVoltage(void) {
    // Implement necessary actions like shutting down charging
    printf("Overvoltage detected! Taking action...\n");
    // Stop charging logic or send a signal to shut down the charge MOSFET
}

/**
 * @brief Handles the condition when the battery voltage falls below the minimum limit.
 */
void HandleUnderVoltage(void) {
    // Implement necessary actions like stopping the discharge
    printf("Undervoltage detected! Taking action...\n");
    // Stop discharging logic or send a signal to shut down the discharge MOSFET
}

/**
 * @brief Handles the condition when the battery temperature exceeds the maximum limit.
 */
void HandleOverTemperature(void) {
    // Implement necessary actions like shutting down the system
    printf("Overtemperature detected! Taking action...\n");
    // Trigger cooling system or stop charging/discharging
}

/**
 * @brief Handles the condition when the battery temperature falls below the minimum limit.
 */
void HandleUnderTemperature(void) {
    // Implement necessary actions like disabling charging
    printf("Undertemperature detected! Taking action...\n");
    // Avoid charging in too low temperatures to prevent damage
}
