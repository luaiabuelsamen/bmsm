#include "batteryManagement.h"
#include "main.h"
#include "cellBalancing.h"
#include <stdint.h>

// Initialize the battery pack structure
void batteryPackInit(void) {
    for (uint8_t i = 0; i < NUM_CELLS; i++) {
        batteryPack.cells[i].channel = ADC_CHANNEL_0 + i;  // Assign channels
        batteryPack.cells[i].voltage = 0.0f;               // Initialize voltages to 0
    }
    batteryPack.totalVoltage = 0.0f;
    batteryPack.averageVoltage = 0.0f;
    batteryPack.current = 0.0f;
    batteryPack.temperature = 0.0f;

    // Initialize safety flags
    batterySafety.overVoltageProtection = 0;
    batterySafety.overTempProtection = 0;
}

// Function to read the voltage of a single cell
float readCellVoltage(uint8_t channel) {
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    uint32_t adcValue = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    // Convert ADC value to voltage (assuming 12-bit ADC)
    return (adcValue * 3.3f) / 4096.0f;
}

// Function to update all cell voltages and calculate total and average voltages
void updateBatteryPackVoltages(void) {
    float totalVoltage = 0.0f;
    for (uint8_t i = 0; i < NUM_CELLS; i++) {
        batteryPack.cells[i].voltage = readCellVoltage(batteryPack.cells[i].channel);
        totalVoltage += batteryPack.cells[i].voltage;
    }
    batteryPack.totalVoltage = totalVoltage;
    batteryPack.averageVoltage = totalVoltage / NUM_CELLS;
}

// Function to estimate SoC based on the average cell voltage
float estimateSoc(void) {
    updateBatteryPackVoltages();

    // Simple linear SoC estimation based on average voltage of all cells
    if (batteryPack.averageVoltage >= MAX_CELL_VOLTAGE) return 100.0f;
    if (batteryPack.averageVoltage <= MIN_CELL_VOLTAGE) return 0.0f;

    return ((batteryPack.averageVoltage - MIN_CELL_VOLTAGE) / 
            (MAX_CELL_VOLTAGE - MIN_CELL_VOLTAGE)) * 100.0f;
}

// Function to read battery current from ADC (assuming current sense resistor)
float readBatteryCurrent(void) {
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_1;  // Assuming current is on ADC_CHANNEL_1
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    uint32_t adcValue = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    // Convert ADC value to current
    batteryPack.current = (adcValue * 100.0f) / 4096.0f;
    return batteryPack.current;
}

// Function to read battery temperature from an I2C sensor
float readBatteryTemperature(void) {
    uint8_t tempRegister = 0x00;
    uint8_t tempData[2] = {0};

    HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)(TEMP_SENSOR_ADDRESS << 1), &tempRegister, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, (uint16_t)(TEMP_SENSOR_ADDRESS << 1), tempData, 2, HAL_MAX_DELAY);

    int16_t rawTemperature = (tempData[0] << 8) | tempData[1];
    rawTemperature >>= 4;

    batteryPack.temperature = rawTemperature * 0.0625f;
    return batteryPack.temperature;
}

// Safety check function
void checkSafety(void) {
    // Check overvoltage for each cell
    for (uint8_t i = 0; i < NUM_CELLS; i++) {
        if (batteryPack.cells[i].voltage > MAX_CELL_VOLTAGE) {
            batterySafety.overVoltageProtection = 1;
            HAL_GPIO_WritePin(Overvoltage_Protection_Port, Overvoltage_Protection_Pin, GPIO_PIN_SET);
            return;
        }
    }

    // Check temperature
    readBatteryTemperature();
    if (batteryPack.temperature > MAX_SAFE_TEMPERATURE) {
        batterySafety.overTempProtection = 1;
        HAL_GPIO_WritePin(Overtemperature_Protection_Port, Overtemperature_Protection_Pin, GPIO_PIN_SET);
    }
}

// Function to enable or disable charging based on safety status
void controlCharging(void) {
    if (batterySafety.overVoltageProtection || batterySafety.overTempProtection) {
        disableCharging();
    } else {
        enableCharging();
    }
}

// Enable charging
void enableCharging(void) {
    HAL_GPIO_WritePin(Charge_Control_Port, Charge_Control_Pin, GPIO_PIN_SET);
}

// Disable charging
void disableCharging(void) {
    HAL_GPIO_WritePin(Charge_Control_Port, Charge_Control_Pin, GPIO_PIN_RESET);
}

// Main battery management loop
void batteryManagementLoop(void) {
    updateBatteryPackVoltages();
    readBatteryCurrent();
    checkSafety();
    controlCharging();
    balanceCells();
}

