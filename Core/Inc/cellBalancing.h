#ifndef CELL_BALANCING_H
#define CELL_BALANCING_H

#include "main.h"
#include "batteryManagement.h"

// Function declarations
void cellBalancingInit(void);
void balanceCells(void);
void activateBalancing(uint8_t cellIndex);
void deactivateBalancing(uint8_t cellIndex);

#endif // CELL_BALANCING_H
