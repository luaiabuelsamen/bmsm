#include "canCommunication.h"
#include "batteryManagement.h"
#include "main.h"

extern CAN_HandleTypeDef hcan1;

typedef enum {
    CAN_STATUS_OK,
    CAN_STATUS_ERROR,
    CAN_STATUS_NO_MAILBOX,
    CAN_STATUS_TIMEOUT,
} can_status_t;

// Function to initialize CAN communication
can_status_t canInit(void) {
    // Start the CAN peripheral
    if (HAL_CAN_Start(&hcan1) != HAL_OK) {
        return CAN_STATUS_ERROR;
    }

    // Optionally, activate CAN notifications (interrupts) for RX, etc.
    if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
        return CAN_STATUS_ERROR;
    }

    return CAN_STATUS_OK;
}

// Function to transmit data via CAN
can_status_t canTransmitMessage(uint32_t id, uint8_t *data, uint8_t length) {
    CAN_TxHeaderTypeDef txHeader;
    uint32_t txMailbox;

    // Set up CAN header
    txHeader.StdId = id;                  // Standard ID
    txHeader.RTR = CAN_RTR_DATA;          // Data frame
    txHeader.IDE = CAN_ID_STD;            // Standard identifier
    txHeader.DLC = length;                // Data length code
    txHeader.TransmitGlobalTime = DISABLE; // No timestamp

    // Check if there is space in the mailbox
    if (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) > 0) {
        // Attempt to transmit the CAN message
        if (HAL_CAN_AddTxMessage(&hcan1, &txHeader, data, &txMailbox) != HAL_OK) {
            return CAN_STATUS_ERROR;  // Transmission error
        }
    } else {
        // No mailbox available
        return CAN_STATUS_NO_MAILBOX;
    }

    return CAN_STATUS_OK;
}

// Function to prepare BMS data to transmit via CAN
can_status_t canTransmitBmsData(void) {
    uint8_t bmsData[8] = {0};
    float voltage = 0.0f, current = 0.0f, soc = 0.0f;

    // Read and scale BMS data
    if (readBatteryVoltage(&voltage) != STATUS_OK ||
        readBatteryCurrent(&current) != STATUS_OK ||
        estimateSoc(&soc) != STATUS_OK) {
        return CAN_STATUS_ERROR;  // Return if unable to read required data
    }

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

    // Attempt to transmit the CAN message with BMS data
    can_status_t status = canTransmitMessage(0x321, bmsData, 6);
    if (status != CAN_STATUS_OK) {
        // Handle error: logging, retrying, or other actions can be implemented here
        return status;
    }

    return CAN_STATUS_OK;
}

// Main CAN communication loop (example usage)
void canCommunicationLoop(void) {
    // Initialize CAN communication
    can_status_t initStatus = canInit();
    if (initStatus != CAN_STATUS_OK) {
        // Handle initialization error (e.g., log error, retry initialization, etc.)
        Error_Handler();
    }

    // Transmit BMS data
    can_status_t transmitStatus = canTransmitBmsData();
    if (transmitStatus != CAN_STATUS_OK) {
        // Implement proper error handling based on the returned status
        // For example, log error or retry based on CAN_STATUS_NO_MAILBOX
        if (transmitStatus == CAN_STATUS_NO_MAILBOX) {
            // Retry logic or queue for later transmission
        } else {
            Error_Handler();
        }
    }
}
