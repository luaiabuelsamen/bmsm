/* Includes ------------------------------------------------------------------*/
#include "CommunicationTask.h"
#include "cmsis_os.h"  // FreeRTOS APIs
#include <string.h>    // For string handling

/* Private defines -----------------------------------------------------------*/
#define UART_BUFFER_SIZE 64       // Size of UART receive buffer
#define CAN_TX_DATA_SIZE 8        // Size of CAN message data (in bytes)
#define TRANSMIT_INTERVAL 5000    // Interval to send data (in ms)
#define COMMAND_BUFFER_SIZE 32    // Buffer size for received commands

/* CAN-related variables -----------------------------------------------------*/
CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;
uint32_t TxMailbox;
uint8_t can_tx_data[CAN_TX_DATA_SIZE] = {0};  // Data to be transmitted via CAN
uint8_t can_rx_data[CAN_TX_DATA_SIZE] = {0};  // Data received via CAN

/* Function prototypes -------------------------------------------------------*/
void SendBatteryStatus(void);
void ReceiveCommand(void);
void ProcessCommand(char* command);
void SendCANMessage(uint8_t *data, uint8_t length);
void ReceiveCANMessage(void);

/* UART and CAN Handles (Externally defined) */
extern UART_HandleTypeDef huart4;  // UART4 for communication
extern CAN_HandleTypeDef hcan1;    // CAN1 for communication

/* Private variables ---------------------------------------------------------*/
char uart_buffer[UART_BUFFER_SIZE] = {0};
char command_buffer[COMMAND_BUFFER_SIZE] = {0};

/* Task implementation -------------------------------------------------------*/
void CommunicationTask(void *argument) {
    while(1) {
        // Send battery status over UART/CAN
        SendBatteryStatus();
        
        // Receive commands from an external system via UART
        ReceiveCommand();

        // Receive CAN messages if available
        ReceiveCANMessage();

        // Delay for a set interval before the next transmission
        osDelay(TRANSMIT_INTERVAL);
    }
}

/* Helper functions ----------------------------------------------------------*/

/**
 * @brief Sends the battery status (voltage, temperature, etc.) via UART and CAN.
 */
void SendBatteryStatus(void) {
    float voltage = GetBatteryVoltage();     // Assuming implemented elsewhere
    float temperature = GetBatteryTemperature();  // Assuming implemented elsewhere

    // Sending data via UART
    char message[UART_BUFFER_SIZE] = {0};
    snprintf(message, sizeof(message), "Voltage: %.2fV, Temperature: %.2fC\r\n", voltage, temperature);
    HAL_UART_Transmit(&huart4, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);

    // Sending data via CAN
    can_tx_data[0] = (uint8_t)(voltage * 100);  // Example conversion (scale to fit)
    can_tx_data[1] = (uint8_t)(temperature);    // Example: sending integer temperature
    SendCANMessage(can_tx_data, 2);
}

/**
 * @brief Sends a CAN message.
 * @param data: Pointer to the data array to be sent.
 * @param length: Length of the data array.
 */
void SendCANMessage(uint8_t *data, uint8_t length) {
    TxHeader.DLC = length;  // Data Length Code (number of bytes in the message)
    TxHeader.IDE = CAN_ID_STD;  // Standard Identifier
    TxHeader.StdId = 0x123;     // Example identifier
    TxHeader.RTR = CAN_RTR_DATA;  // Data frame

    // Transmit the CAN message
    if (HAL_CAN_AddTxMessage(&hcan1, &TxHeader, data, &TxMailbox) != HAL_OK) {
        // Transmission error
        printf("CAN Transmission Error\r\n");
    }
}

/**
 * @brief Receives a CAN message and processes it.
 */
void ReceiveCANMessage(void) {
    // Check if a CAN message has been received
    if (HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) > 0) {
        // Receive the CAN message
        if (HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, can_rx_data) == HAL_OK) {
            // Process the received data
            // Example: Print received data
            printf("Received CAN Message: 0x%X, Data: %d %d\r\n", RxHeader.StdId, can_rx_data[0], can_rx_data[1]);

            // Process the command if it matches expected criteria
            if (can_rx_data[0] == 0x01) {
                EnableCharging();  // Example action based on CAN data
            }
        }
    }
}

/**
 * @brief Receives and processes commands via UART.
 */
void ReceiveCommand(void) {
    // Poll UART for incoming command (non-blocking)
    if (HAL_UART_Receive(&huart4, (uint8_t*)command_buffer, COMMAND_BUFFER_SIZE, 100) == HAL_OK) {
        // Null-terminate the received command
        command_buffer[strcspn(command_buffer, "\r\n")] = '\0';

        // Process the command
        ProcessCommand(command_buffer);

        // Clear the command buffer after processing
        memset(command_buffer, 0, COMMAND_BUFFER_SIZE);
    }
}

/**
 * @brief Processes a received command.
 * @param command The received command string.
 */
void ProcessCommand(char* command) {
    if (strcmp(command, "START_CHARGING") == 0) {
        EnableCharging();  // Assuming implemented elsewhere
        HAL_UART_Transmit(&huart4, (uint8_t*)"Charging started.\r\n", 19, HAL_MAX_DELAY);
    } else if (strcmp(command, "STOP_CHARGING") == 0) {
        DisableCharging();  // Assuming implemented elsewhere
        HAL_UART_Transmit(&huart4, (uint8_t*)"Charging stopped.\r\n", 19, HAL_MAX_DELAY);
    } else {
        HAL_UART_Transmit(&huart4, (uint8_t*)"Unknown command.\r\n", 18, HAL_MAX_DELAY);
    }
}
