/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include <stdint.h>

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC

#define ADC_Voltage_Pin GPIO_PIN_0
#define ADC_Voltage_Port GPIOA
#define ADC_Current_Pin GPIO_PIN_1
#define ADC_Current_Port GPIOA

#define Charge_Control_Pin GPIO_PIN_4
#define Charge_Control_Port GPIOA
#define Discharge_Control_Pin GPIO_PIN_5
#define Discharge_Control_Port GPIOA

#define CAN1_TX_Pin GPIO_PIN_12
#define CAN1_TX_GPIO_Port GPIOA
#define CAN1_RX_Pin GPIO_PIN_11
#define CAN1_RX_GPIO_Port GPIOA

#define I2C1_SDA_Pin GPIO_PIN_7
#define I2C1_SDA_GPIO_Port GPIOB
#define I2C1_SCL_Pin GPIO_PIN_6
#define I2C1_SCL_GPIO_Port GPIOB

#define Overvoltage_Protection_Pin GPIO_PIN_8
#define Overvoltage_Protection_Port GPIOB
#define Overtemperature_Protection_Pin GPIO_PIN_9
#define Overtemperature_Protection_Port GPIOB

#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA

#define LD2_Pin GPIO_PIN_6
#define LD2_GPIO_Port GPIOA

#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define TEMP_SENSOR_ADDRESS 0x48  // Update to your sensor's actual I2C address

// Define the balancing control pins for each cell
#define Balance_Control_Pin_Cell1 GPIO_PIN_0
#define Balance_Control_Port_Cell1 GPIOB

#define Balance_Control_Pin_Cell2 GPIO_PIN_1
#define Balance_Control_Port_Cell2 GPIOB

#define Balance_Control_Pin_Cell3 GPIO_PIN_2
#define Balance_Control_Port_Cell3 GPIOB

#define Balance_Control_Pin_Cell4 GPIO_PIN_3
#define Balance_Control_Port_Cell4 GPIOB

#define Balance_Control_Pin_Cell5 GPIO_PIN_4
#define Balance_Control_Port_Cell5 GPIOB

#define Balance_Control_Pin_Cell6 GPIO_PIN_5
#define Balance_Control_Port_Cell6 GPIOB

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
