/* USER CODE BEGIN Header */
/**
  *******************************************************************************
  * @file           : main.c
  * @brief          : Main program body for Battery Management System (BMS)
  *******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "batteryManagement.h"
#include "canCommunication.h"

ADC_HandleTypeDef hadc1;
CAN_HandleTypeDef hcan1;
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart2;

osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_CAN1_Init(void);
static void MX_I2C1_Init(void);
static void MX_UART4_Init(void);
static void MX_USART2_UART_Init(void);
void StartDefaultTask(void *argument);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_ADC1_Init();
    MX_CAN1_Init();
    MX_I2C1_Init();
    MX_UART4_Init();
    MX_USART2_UART_Init();

    // Initialize charge control and CAN communication
    chargeControlInit();
    canInit();

    // Initialize FreeRTOS and create tasks
    osKernelInitialize();
    MX_FREERTOS_Init();  // Initialize FreeRTOS tasks and configuration
    osKernelStart();     // Start the FreeRTOS scheduler

    // Infinite loop removed, FreeRTOS handles task scheduling
    while (1) {
        // This should not execute as the FreeRTOS scheduler has taken over
    }
}

void processBmsData(void) {
    float soc = estimateSoc();
    float voltage = readBatteryVoltage();
    float current = readBatteryCurrent();
    float temperature = readBatteryTemperature();

    // Implementing overvoltage and overtemperature protection
    if (voltage > 4.2f) {
        activateOvervoltageProtection();
    }
    if (temperature > 60.0f) {
        activateOvertemperatureProtection();
    }

    if (soc < 20.0f) {
        enableCharging();
        disableDischarging();
    } else if (soc > 80.0f) {
        disableCharging();
        enableDischarging();
    }

    // Transmitting BMS data over CAN
    canTransmitBmsData();

    printf("SOC: %.2f%%, Voltage: %.2fV, Current: %.2fA, Temp: %.2f°C\n", soc, voltage, current, temperature);
}

void SystemClock_Config(void) {
    // System Clock Configuration remains unchanged
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}

void StartDefaultTask(void *argument) {
    for (;;) {
        osDelay(1);
    }
}

/* Additional initialization code for ADC, CAN, I2C, UART, GPIO remains unchanged */
/* Example for ADC initialization function */
static void MX_ADC1_Init(void) {
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }
}

void Error_Handler(void) {
    __disable_irq();
    while (1) {
        // Flash an LED or take necessary action
    }
}

