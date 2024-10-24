/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOSConfig.h"
#include "task.h"
#include "main.h"
#include "batteryManagement.h"
#include "canCommunication.h"
#include "cmsis_os2.h"

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId_t bmsTaskHandle;
const osThreadAttr_t bmsTask_attributes = {
  .name = "bmsTask",
  .stack_size = 128 * 4,   // Adjust stack size based on your requirements
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
void StartBmsTask(void *argument);

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* USER CODE BEGIN Application */

/* Function to start the BMS Task */
void StartBmsTask(void *argument) {
    for (;;) {
        processBmsData();  // Process BMS data periodically
        osDelay(1000);     // Delay for 1000ms (1 second) between iterations
    }
}

/* USER CODE END Application */

/* Hook to initialize FreeRTOS */
void MX_FREERTOS_Init(void) {
    /* Create the BMS task */
    bmsTaskHandle = osThreadNew(StartBmsTask, NULL, &bmsTask_attributes);
}
