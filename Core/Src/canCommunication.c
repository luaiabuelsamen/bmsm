#include "canCommunication.h"
#include "batteryManagement.h"
#include "main.h"

extern CAN_HandleTypeDef hcan1;

// Function to transmit BMS data via CAN
void canTransmitBmsData(void) {
    CAN_TxHeaderTypeDef txHeader;
    uint32_t txMailbox;
    uint8_t bmsData[8] = {0};

    // Read and scale BMS data
    float voltage = readBatteryVoltage();
    float current = readBatteryCurrent();
    float soc = estimateSoc();

    int voltage_scaled = (int)(voltage * 100);   // Scale to centivolts (2 decimal places)
    int current_scaled = (int)(current * 100);   // Scale to centiamps
    int soc_scaled = (int)(soc * 100);           // Scale to percentage (0–100%)

    // Fill CAN data bytes
    bmsData[0] = (uint8_t)(voltage_scaled & 0xFF);
    bmsData[1] = (uint8_t)((voltage_scaled >> 8) & 0xFF);
    bmsData[2] = (uint8_t)(current_scaled & 0xFF);
    bmsData[3] = (uint8_t)((current_scaled >> 8) & 0xFF);
    bmsData[4] = (uint8_t)(soc_scaled & 0xFF);
    bmsData[5] = (uint8_t)((soc_scaled >> 8) & 0xFF);

    // Set up CAN header
    txHeader.StdId = 0x321;                // Standard ID
    txHeader.RTR = CAN_RTR_DATA;           // Data frame
    txHeader.IDE = CAN_ID_STD;             // Standard identifier
    txHeader.DLC = 6;                      // Data length code (6 bytes)
    txHeader.TransmitGlobalTime = DISABLE; // No timestamp

    // Check if there is space in the mailbox
    if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) > 0) {
        // Attempt to transmit the CAN message
        if (HAL_CAN_AddTxMessage(&hcan1, &txHeader, bmsData, &txMailbox) != HAL_OK) {
            Error_Handler();  // Handle transmission error (could retry here)
        }
    } else {
        // Handle no available mailbox (could queue the message or retry later)
        // Optional: Implement retries or queueing here if needed
        Error_Handler();  // Could implement a more sophisticated retry mechanism
    }
}

// Function to initialize CAN communication
void canInit(void) {
    // Start the CAN peripheral
    if (HAL_CAN_Start(&hcan1) != HAL_OK) {
        Error_Handler();
    }

    // Optionally, activate CAN notifications (interrupts) for RX, etc.
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
        Error_Handler();
    }
}
