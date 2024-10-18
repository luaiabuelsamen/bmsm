#include "batteryManagement.h"
#include "main.h"
#include <stdint.h>

extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;

void chargeControlInit(void) {
    // GPIOs for charge/discharge are already initialized
}

void enableCharging(void) {
    HAL_GPIO_WritePin(Charge_Control_Port, Charge_Control_Pin, GPIO_PIN_SET);
}

void disableCharging(void) {
    HAL_GPIO_WritePin(Charge_Control_Port, Charge_Control_Pin, GPIO_PIN_RESET);
}

void enableDischarging(void) {
    HAL_GPIO_WritePin(Discharge_Control_Port, Discharge_Control_Pin, GPIO_PIN_SET);
}

void disableDischarging(void) {
    HAL_GPIO_WritePin(Discharge_Control_Port, Discharge_Control_Pin, GPIO_PIN_RESET);
}

float readBatteryVoltage(void) {
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_0;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    uint32_t adcValue = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    return (adcValue * 3.3f) / 4096.0f; // Convert ADC value to voltage
}

float readBatteryCurrent(void) {
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_1;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    uint32_t adcValue = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    return (adcValue * 100.0f) / 4096.0f; // Convert ADC value to current
}

float readBatteryTemperature(void) {
    uint8_t tempRegister = 0x00;
    uint8_t tempData[2] = {0};

    HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)(TEMP_SENSOR_ADDRESS << 1), &tempRegister, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, (uint16_t)(TEMP_SENSOR_ADDRESS << 1), tempData, 2, HAL_MAX_DELAY);

    int16_t rawTemperature = (tempData[0] << 8) | tempData[1];
    rawTemperature >>= 4;

    return rawTemperature * 0.0625f; // Convert raw value to temperature
}

float estimateSoc(void) {
    float voltage = readBatteryVoltage();
    if (voltage >= 4.2f) return 100.0f;
    if (voltage <= 3.0f) return 0.0f;
    return ((voltage - 3.0f) / (4.2f - 3.0f)) * 100.0f;
}

// Safety management
void activateOvervoltageProtection(void) {
    HAL_GPIO_WritePin(Overvoltage_Protection_Port, Overvoltage_Protection_Pin, GPIO_PIN_SET);
}

void activateOvertemperatureProtection(void) {
    HAL_GPIO_WritePin(Overtemperature_Protection_Port, Overtemperature_Protection_Pin, GPIO_PIN_SET);
}
