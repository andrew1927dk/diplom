/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define I2C                          hi2c1
#define EXT_I2C                      hi2c3
#define LOG_UART                     hlpuart1
#define FLASH_QSPI                   hospi1
#define ETH_SPI                      hspi1
#define DISP_SPI                     hspi2
#define RFM_SPI                      hspi3
#define EXT_UART                     huart1
#define WIFI_UART                    huart2
#define MODEM_UART                   huart3
#define LED_R_TIM                    htim3
#define LED_R_CH                     TIM_CHANNEL_1
#define LED_G_TIM                    htim3
#define LED_G_CH                     TIM_CHANNEL_2
#define LED_B_TIM                    htim3
#define LED_B_CH                     TIM_CHANNEL_3
#define ISM_RDIO2_Pin                GPIO_PIN_2
#define ISM_RDIO2_GPIO_Port          GPIOE
#define ISM_RDIO2_EXTI_IRQn          EXTI2_IRQn
#define ISM_RFSW_Pin                 GPIO_PIN_3
#define ISM_RFSW_GPIO_Port           GPIOE
#define ISM_RDIO4_Pin                GPIO_PIN_4
#define ISM_RDIO4_GPIO_Port          GPIOE
#define ISM_RDIO4_EXTI_IRQn          EXTI4_IRQn
#define ISM_RDIO5_Pin                GPIO_PIN_5
#define ISM_RDIO5_GPIO_Port          GPIOE
#define PMIC_IRQ_Pin                 GPIO_PIN_6
#define PMIC_IRQ_GPIO_Port           GPIOE
#define PMIC_IRQ_EXTI_IRQn           EXTI6_IRQn
#define DISP_RESET_Pin               GPIO_PIN_13
#define DISP_RESET_GPIO_Port         GPIOC
#define EXT_SCL_Pin                  GPIO_PIN_0
#define EXT_SCL_GPIO_Port            GPIOC
#define EXT_SDA_Pin                  GPIO_PIN_1
#define EXT_SDA_GPIO_Port            GPIOC
#define KNOB1_MEASURE_Pin            GPIO_PIN_2
#define KNOB1_MEASURE_GPIO_Port      GPIOC
#define KNOB2_MEASURE_Pin            GPIO_PIN_3
#define KNOB2_MEASURE_GPIO_Port      GPIOC
#define BUZZER_PWM_Pin               GPIO_PIN_0
#define BUZZER_PWM_GPIO_Port         GPIOA
#define DISP_DCX_Pin                 GPIO_PIN_1
#define DISP_DCX_GPIO_Port           GPIOA
#define DEBUG_TXD_Pin                GPIO_PIN_2
#define DEBUG_TXD_GPIO_Port          GPIOA
#define DEBUG_RXD_Pin                GPIO_PIN_3
#define DEBUG_RXD_GPIO_Port          GPIOA
#define QSPI_CS_Pin                  GPIO_PIN_4
#define QSPI_CS_GPIO_Port            GPIOA
#define QSPI_DATA3_Pin               GPIO_PIN_6
#define QSPI_DATA3_GPIO_Port         GPIOA
#define QSPI_DATA2_Pin               GPIO_PIN_7
#define QSPI_DATA2_GPIO_Port         GPIOA
#define TX_TO_GSM_Pin                GPIO_PIN_4
#define TX_TO_GSM_GPIO_Port          GPIOC
#define RX_FROM_GSM_Pin              GPIO_PIN_5
#define RX_FROM_GSM_GPIO_Port        GPIOC
#define QSPI_DATA1_Pin               GPIO_PIN_0
#define QSPI_DATA1_GPIO_Port         GPIOB
#define QSPI_DATA0_Pin               GPIO_PIN_1
#define QSPI_DATA0_GPIO_Port         GPIOB
#define EXT_SMBA_Pin                 GPIO_PIN_2
#define EXT_SMBA_GPIO_Port           GPIOB
#define ETH_IRQ_Pin                  GPIO_PIN_7
#define ETH_IRQ_GPIO_Port            GPIOE
#define ETH_IRQ_EXTI_IRQn            EXTI7_IRQn
#define ACCEL_IRQ1_Pin               GPIO_PIN_8
#define ACCEL_IRQ1_GPIO_Port         GPIOE
#define ACCEL_IRQ1_EXTI_IRQn         EXTI8_IRQn
#define ACCEL_IRQ2_Pin               GPIO_PIN_9
#define ACCEL_IRQ2_GPIO_Port         GPIOE
#define DISP_TS_IRQ_Pin              GPIO_PIN_10
#define DISP_TS_IRQ_GPIO_Port        GPIOE
#define DISP_TS_IRQ_EXTI_IRQn        EXTI10_IRQn
#define PP3V1_DISP_ON_Pin            GPIO_PIN_11
#define PP3V1_DISP_ON_GPIO_Port      GPIOE
#define ETH_NSS_Pin                  GPIO_PIN_12
#define ETH_NSS_GPIO_Port            GPIOE
#define ETH_SCK_Pin                  GPIO_PIN_13
#define ETH_SCK_GPIO_Port            GPIOE
#define ETH_MISO_Pin                 GPIO_PIN_14
#define ETH_MISO_GPIO_Port           GPIOE
#define ETH_MOSI_Pin                 GPIO_PIN_15
#define ETH_MOSI_GPIO_Port           GPIOE
#define QSPI_CLK_Pin                 GPIO_PIN_10
#define QSPI_CLK_GPIO_Port           GPIOB
#define PMIC_CE_Pin                  GPIO_PIN_12
#define PMIC_CE_GPIO_Port            GPIOB
#define PP4VD_ON_Pin                 GPIO_PIN_13
#define PP4VD_ON_GPIO_Port           GPIOB
#define PMIC_STAT_Pin                GPIO_PIN_14
#define PMIC_STAT_GPIO_Port          GPIOB
#define ETH_RESET_Pin                GPIO_PIN_15
#define ETH_RESET_GPIO_Port          GPIOB
#define BMP_CS_Pin                   GPIO_PIN_8
#define BMP_CS_GPIO_Port             GPIOD
#define BMP_IRQ_Pin                  GPIO_PIN_9
#define BMP_IRQ_GPIO_Port            GPIOD
#define BMP_IRQ_EXTI_IRQn            EXTI9_IRQn
#define GSM_STATUS_Pin               GPIO_PIN_10
#define GSM_STATUS_GPIO_Port         GPIOD
#define JOY_U_Pin                    GPIO_PIN_11
#define JOY_U_GPIO_Port              GPIOD
#define JOY_U_EXTI_IRQn              EXTI11_IRQn
#define JOY_L_Pin                    GPIO_PIN_12
#define JOY_L_GPIO_Port              GPIOD
#define JOY_L_EXTI_IRQn              EXTI12_IRQn
#define JOY_OK_Pin                   GPIO_PIN_13
#define JOY_OK_GPIO_Port             GPIOD
#define JOY_OK_EXTI_IRQn             EXTI13_IRQn
#define JOY_R_Pin                    GPIO_PIN_14
#define JOY_R_GPIO_Port              GPIOD
#define JOY_R_EXTI_IRQn              EXTI14_IRQn
#define JOY_D_Pin                    GPIO_PIN_15
#define JOY_D_GPIO_Port              GPIOD
#define JOY_D_EXTI_IRQn              EXTI15_IRQn
#define LED_R_Pin                    GPIO_PIN_6
#define LED_R_GPIO_Port              GPIOC
#define LED_G_Pin                    GPIO_PIN_7
#define LED_G_GPIO_Port              GPIOC
#define LED_B_Pin                    GPIO_PIN_8
#define LED_B_GPIO_Port              GPIOC
#define EXT_IO0_Pin                  GPIO_PIN_9
#define EXT_IO0_GPIO_Port            GPIOC
#define EXT_IO1_Pin                  GPIO_PIN_8
#define EXT_IO1_GPIO_Port            GPIOA
#define EXT_TXD_Pin                  GPIO_PIN_9
#define EXT_TXD_GPIO_Port            GPIOA
#define EXT_RXD_Pin                  GPIO_PIN_10
#define EXT_RXD_GPIO_Port            GPIOA
#define EXT_IO2_Pin                  GPIO_PIN_11
#define EXT_IO2_GPIO_Port            GPIOA
#define EXT_IO3_Pin                  GPIO_PIN_12
#define EXT_IO3_GPIO_Port            GPIOA
#define ISM_NSS_Pin                  GPIO_PIN_15
#define ISM_NSS_GPIO_Port            GPIOA
#define ISM_SCK_Pin                  GPIO_PIN_10
#define ISM_SCK_GPIO_Port            GPIOC
#define ISM_MISO_Pin                 GPIO_PIN_11
#define ISM_MISO_GPIO_Port           GPIOC
#define ISM_MOSI_Pin                 GPIO_PIN_12
#define ISM_MOSI_GPIO_Port           GPIOC
#define DISP_NSS_Pin                 GPIO_PIN_0
#define DISP_NSS_GPIO_Port           GPIOD
#define DISP_SCK_Pin                 GPIO_PIN_1
#define DISP_SCK_GPIO_Port           GPIOD
#define GSM_ON_Pin                   GPIO_PIN_2
#define GSM_ON_GPIO_Port             GPIOD
#define DISP_MISO_Pin                GPIO_PIN_3
#define DISP_MISO_GPIO_Port          GPIOD
#define DISP_MOSI_Pin                GPIO_PIN_4
#define DISP_MOSI_GPIO_Port          GPIOD
#define WIFI_TXD_Pin                 GPIO_PIN_5
#define WIFI_TXD_GPIO_Port           GPIOD
#define WIFI_RXD_Pin                 GPIO_PIN_6
#define WIFI_RXD_GPIO_Port           GPIOD
#define GSM_DTR_Pin                  GPIO_PIN_7
#define GSM_DTR_GPIO_Port            GPIOD
#define ISM_RST_Pin                  GPIO_PIN_4
#define ISM_RST_GPIO_Port            GPIOB
#define DISP_TS_RESET_Pin            GPIO_PIN_5
#define DISP_TS_RESET_GPIO_Port      GPIOB
#define SCL_Pin                      GPIO_PIN_6
#define SCL_GPIO_Port                GPIOB
#define SDA_Pin                      GPIO_PIN_7
#define SDA_GPIO_Port                GPIOB
#define PP3V1_EHT_ON_Pin             GPIO_PIN_8
#define PP3V1_EHT_ON_GPIO_Port       GPIOB
#define DISP_BACKLIGHT_PWM_Pin       GPIO_PIN_9
#define DISP_BACKLIGHT_PWM_GPIO_Port GPIOB
#define ISM_RDIO0_Pin                GPIO_PIN_0
#define ISM_RDIO0_GPIO_Port          GPIOE
#define ISM_RDIO0_EXTI_IRQn          EXTI0_IRQn
#define PP3V1_WIFI_ON_Pin            GPIO_PIN_1
#define PP3V1_WIFI_ON_GPIO_Port      GPIOE

/* USER CODE BEGIN Private defines */
#define PIN_SET(_pin)   _pin##_GPIO_Port->BSRR = _pin##_Pin;
#define PIN_RESET(_pin) _pin##_GPIO_Port->BRR = _pin##_Pin;
#define PIN_READ(_pin)  HAL_GPIO_ReadPin(_pin##_GPIO_Port, _pin##_Pin)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
