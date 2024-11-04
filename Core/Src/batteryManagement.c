#include "batteryManagement.h"
#include "main.h"
#include "cellBalancing.h"
#include <stdint.h>

#define BUFFER_SIZE 10

typedef enum {
    STATUS_OK,
    STATUS_ERROR,
    STATUS_TIMEOUT,
    STATUS_INVALID_PARAM
} status_t;

typedef struct {
    float buffer[BUFFER_SIZE];
    uint8_t head;
    uint8_t count;
    float sum;
} CircularBuffer;

typedef struct {
    uint8_t channel;
    float voltage;
} Cell;

typedef struct {
    Cell cells[NUM_CELLS];
    float totalVoltage;
    float averageVoltage;
    float current;
    float temperature;
} BatteryPack;

typedef struct {
    uint8_t overVoltageProtection;
    uint8_t overTempProtection;
} BatterySafety;

CircularBuffer voltageBuffer = { .head = 0, .count = 0, .sum = 0.0f };
BatteryPack batteryPack;
BatterySafety batterySafety;

void batteryPackInit(void) {
    for (uint8_t i = 0; i < NUM_CELLS; i++) {
        batteryPack.cells[i].channel = ADC_CHANNEL_0 + i; // Assign ADC channels
        batteryPack.cells[i].voltage = 0.0f;              // Initialize voltages to 0
    }
    batteryPack.totalVoltage = 0.0f;
    batteryPack.averageVoltage = 0.0f;
    batteryPack.current = 0.0f;
    batteryPack.temperature = 0.0f;

    // Initialize safety flags
    batterySafety.overVoltageProtection = 0;
    batterySafety.overTempProtection = 0;
}

// Hardware Abstraction Layer for ADC Configuration
status_t configureADCChannel(uint8_t channel) {
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        return STATUS_ERROR;
    }
    return STATUS_OK;
}

// Function to read the voltage of a single cell
status_t readCellVoltage(uint8_t channel, float *voltage) {
    if (configureADCChannel(channel) != STATUS_OK) {
        return STATUS_ERROR;
    }

    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) != HAL_OK) {
        HAL_ADC_Stop(&hadc1);
        return STATUS_TIMEOUT;
    }

    uint32_t adcValue = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    // Convert ADC value to voltage (assuming 12-bit ADC)
    *voltage = (adcValue * 3.3f) / 4096.0f;
    return STATUS_OK;
}

void addVoltageToBuffer(CircularBuffer *cb, float newVoltage) {
    if (cb->count < BUFFER_SIZE) {
        cb->buffer[cb->head] = newVoltage;
        cb->sum += newVoltage;
        cb->count++;
    } else {
        cb->sum -= cb->buffer[cb->head];
        cb->buffer[cb->head] = newVoltage;
        cb->sum += newVoltage;
    }
    cb->head = (cb->head + 1) % BUFFER_SIZE;
}

float calculateAverageVoltage(CircularBuffer *cb) {
    return cb->count == 0 ? 0.0f : cb->sum / cb->count;
}

// Function to update battery pack voltages
status_t updateBatteryPackVoltages(void) {
    float totalVoltage = 0.0f;

    for (uint8_t i = 0; i < NUM_CELLS; i++) {
        float cellVoltage;
        if (readCellVoltage(batteryPack.cells[i].channel, &cellVoltage) != STATUS_OK) {
            return STATUS_ERROR;
        }

        batteryPack.cells[i].voltage = cellVoltage;
        totalVoltage += cellVoltage;
        addVoltageToBuffer(&voltageBuffer, cellVoltage);
    }

    batteryPack.totalVoltage = totalVoltage;
    batteryPack.averageVoltage = calculateAverageVoltage(&voltageBuffer);
    return STATUS_OK;
}

// Function to estimate State of Charge (SoC)
float estimateSoc(void) {
    if (updateBatteryPackVoltages() != STATUS_OK) {
        return -1.0f;  // Return an invalid SoC value if an error occurs
    }

    // Simple linear SoC estimation based on average voltage of all cells
    if (batteryPack.averageVoltage >= MAX_CELL_VOLTAGE) return 100.0f;
    if (batteryPack.averageVoltage <= MIN_CELL_VOLTAGE) return 0.0f;

    return ((batteryPack.averageVoltage - MIN_CELL_VOLTAGE) /
            (MAX_CELL_VOLTAGE - MIN_CELL_VOLTAGE)) * 100.0f;
}

// Function to read battery current from ADC (assuming current sense resistor)
status_t readBatteryCurrent(float *current) {
    if (configureADCChannel(ADC_CHANNEL_1) != STATUS_OK) {
        return STATUS_ERROR;
    }

    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) != HAL_OK) {
        HAL_ADC_Stop(&hadc1);
        return STATUS_TIMEOUT;
    }

    uint32_t adcValue = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);

    // Convert ADC value to current
    *current = (adcValue * 100.0f) / 4096.0f;
    batteryPack.current = *current;
    return STATUS_OK;
}

// Function to read battery temperature from an I2C sensor
status_t readBatteryTemperature(float *temperature) {
    uint8_t tempRegister = 0x00;
    uint8_t tempData[2] = {0};

    if (HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)(TEMP_SENSOR_ADDRESS << 1), &tempRegister, 1, HAL_MAX_DELAY) != HAL_OK) {
        return STATUS_ERROR;
    }

    if (HAL_I2C_Master_Receive(&hi2c1, (uint16_t)(TEMP_SENSOR_ADDRESS << 1), tempData, 2, HAL_MAX_DELAY) != HAL_OK) {
        return STATUS_ERROR;
    }

    int16_t rawTemperature = (tempData[0] << 8) | tempData[1];
    rawTemperature >>= 4;

    *temperature = rawTemperature * 0.0625f;
    batteryPack.temperature = *temperature;
    return STATUS_OK;
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
    float temperature;
    if (readBatteryTemperature(&temperature) == STATUS_OK && temperature > MAX_SAFE_TEMPERATURE) {
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
    if (updateBatteryPackVoltages() == STATUS_OK) {
        readBatteryCurrent(&batteryPack.current);
        checkSafety();
        controlCharging();
        balanceCells();
    } else {
        return
    }
}
