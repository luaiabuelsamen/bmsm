#include "canCommunication.h"
#include "main.h"

extern CAN_HandleTypeDef hcan1;

void canTransmitBmsData(void) {
    CAN_TxHeaderTypeDef txHeader;
    uint32_t txMailbox;
    uint8_t bmsData[8] = {0};

    float voltage = readBatteryVoltage();
    float current = readBatteryCurrent();
    float soc = estimateSoc();

    bmsData[0] = (uint8_t)(voltage * 10);
    bmsData[1] = (uint8_t)((int)(voltage * 10) >> 8);
    bmsData[2] = (uint8_t)(current * 10);
    bmsData[3] = (uint8_t)((int)(current * 10) >> 8);
    bmsData[4] = (uint8_t)(soc * 10);
    bmsData[5] = (uint8_t)((int)(soc * 10) >> 8);

    txHeader.StdId = 0x321;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.IDE = CAN_ID_STD;
    txHeader.DLC = 6;
    txHeader.TransmitGlobalTime = DISABLE;

    if (HAL_CAN_AddTxMessage(&hcan1, &txHeader, bmsData, &txMailbox) != HAL_OK) {
        Error_Handler();
    }
}

void canInit(void) {
    // CAN initialization function remains unchanged
    if (HAL_CAN_Start(&hcan1) != HAL_OK) {
        Error_Handler();
    }
}
