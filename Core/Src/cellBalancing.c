#include "cellBalancing.h"
#include "main.h"

#define BALANCE_THRESHOLD 0.05f  // 50mV difference between cells

typedef struct {
    uint8_t isBalancing;
    GPIO_TypeDef* balancePort;
    uint16_t balancePin;
} CellBalancer;

CellBalancer cellBalancers[NUM_CELLS];  // Define balancers for each cell

extern BatteryPack batteryPack;  // Reference to battery pack data

// Initialize the cell balancing system
void cellBalancingInit(void) {
    // Assign GPIO ports and pins for each cell balancer
    cellBalancers[0].balancePort = Balance_Control_Port_Cell1;
    cellBalancers[0].balancePin = Balance_Control_Pin_Cell1;
    
    cellBalancers[1].balancePort = Balance_Control_Port_Cell2;
    cellBalancers[1].balancePin = Balance_Control_Pin_Cell2;

    cellBalancers[2].balancePort = Balance_Control_Port_Cell3;
    cellBalancers[2].balancePin = Balance_Control_Pin_Cell3;

    cellBalancers[3].balancePort = Balance_Control_Port_Cell4;
    cellBalancers[3].balancePin = Balance_Control_Pin_Cell4;

    cellBalancers[4].balancePort = Balance_Control_Port_Cell5;
    cellBalancers[4].balancePin = Balance_Control_Pin_Cell5;

    cellBalancers[5].balancePort = Balance_Control_Port_Cell6;
    cellBalancers[5].balancePin = Balance_Control_Pin_Cell6;

    for (uint8_t i = 0; i < NUM_CELLS; i++) {
        cellBalancers[i].isBalancing = 0;
    }
}

void activateBalancing(uint8_t cellIndex) {
    HAL_GPIO_WritePin(cellBalancers[cellIndex].balancePort, cellBalancers[cellIndex].balancePin, GPIO_PIN_SET);
    cellBalancers[cellIndex].isBalancing = 1;
}

void deactivateBalancing(uint8_t cellIndex) {
    HAL_GPIO_WritePin(cellBalancers[cellIndex].balancePort, cellBalancers[cellIndex].balancePin, GPIO_PIN_RESET);
    cellBalancers[cellIndex].isBalancing = 0;
}

void balanceCells(void) {
    float maxVoltage = batteryPack.cells[0].voltage;
    float minVoltage = batteryPack.cells[0].voltage;

    for (uint8_t i = 1; i < NUM_CELLS; i++) {
        if (batteryPack.cells[i].voltage > maxVoltage) {
            maxVoltage = batteryPack.cells[i].voltage;
        }
        if (batteryPack.cells[i].voltage < minVoltage) {
            minVoltage = batteryPack.cells[i].voltage;
        }
    }
    
    if (maxVoltage - minVoltage > BALANCE_THRESHOLD) {
        for (uint8_t i = 0; i < NUM_CELLS; i++) {
            if (batteryPack.cells[i].voltage > minVoltage) {
                activateBalancing(i);  // Balance cells that are higher than the minimum voltage
            } else {
                deactivateBalancing(i);  // Stop balancing if cell voltage is close to the minimum
            }
        }
    } else {
        for (uint8_t i = 0; i < NUM_CELLS; i++) {
            deactivateBalancing(i);
        }
    }
}
