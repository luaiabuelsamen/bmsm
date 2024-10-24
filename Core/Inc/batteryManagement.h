#ifndef BATTERY_MANAGEMENT_H
#define BATTERY_MANAGEMENT_H

#include "main.h"

#define NUM_CELLS 6
#define MAX_CELL_VOLTAGE 4.2f
#define MIN_CELL_VOLTAGE 3.0f
#define MAX_SAFE_TEMPERATURE 60.0f


typedef struct {
    float voltage;
    uint8_t channel;  // ADC channel for each cell
} BatteryCell;

// Struct to represent the entire battery pack
typedef struct {
    BatteryCell cells[NUM_CELLS];  // Array of battery cells
    float totalVoltage;
    float averageVoltage;
    float current;
    float temperature;
} BatteryPack;

// Safety state
typedef struct {
    uint8_t overVoltageProtection;  // Flag for overvoltage protection
    uint8_t overTempProtection;     // Flag for overtemperature protection
} BatterySafety;

// Global variables for battery and safety states
BatteryPack batteryPack;
BatterySafety batterySafety;

extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;

// Function Prototypes
void chargeControlInit(void);
void enableCharging(void);
void disableCharging(void);
void enableDischarging(void);
void disableDischarging(void);
float readBatteryVoltage(void);
float readBatteryCurrent(void);
float readBatteryTemperature(void);
float estimateSoc(void);

#endif /* BATTERY_MANAGEMENT_H */
