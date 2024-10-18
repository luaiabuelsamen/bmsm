#ifndef BATTERY_MANAGEMENT_H
#define BATTERY_MANAGEMENT_H

#include "main.h"

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
