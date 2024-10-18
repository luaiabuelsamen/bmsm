#ifndef COMMUNICATIONTASK_H
#define COMMUNICATIONTASK_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"  // For STM32 HAL libraries
#include "cmsis_os.h"       // For FreeRTOS APIs
#include <stdio.h>          // For printf

/* Function prototypes -------------------------------------------------------*/

/**
 * @brief Task for handling communication via UART and CAN.
 * @param argument: Not used
 * @retval None
 */
void CommunicationTask(void *argument);

/**
 * @brief Sends the battery status (voltage, temperature, etc.) via UART and CAN.
 */
void SendBatteryStatus(void);

/**
 * @brief Sends a CAN message.
 * @param data: Pointer to the data array to be sent.
 * @param length: Length of the data array.
 */
void SendCANMessage(uint8_t *data, uint8_t length);

/**
 * @brief Receives and processes CAN messages.
 */
void ReceiveCANMessage(void);

/**
 * @brief Receives and processes commands via UART.
 */
void ReceiveCommand(void);

/**
 * @brief Processes a received command.
 * @param command: The received command string.
 */
void ProcessCommand(char* command);

#endif /* COMMUNICATIONTASK_H */
