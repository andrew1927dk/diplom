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
#include "stm32f4xx_hal.h"

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
#define LOG_UART                 huart5
#define MODEM_UART               huart4
#define ETH_SPI                  hspi4
#define RFM_SPI                  hspi2
#define FLASH_QSPI               hqspi
#define LED_R_TIM                htim3
#define LED_R_CH                 TIM_CHANNEL_3
#define LED_G_TIM                htim4
#define LED_G_CH                 TIM_CHANNEL_3
#define LED_B_TIM                htim4
#define LED_B_CH                 TIM_CHANNEL_4
#define ON_DET_Pin               GPIO_PIN_13
#define ON_DET_GPIO_Port         GPIOC
#define VBAT_LOAD_Pin            GPIO_PIN_14
#define VBAT_LOAD_GPIO_Port      GPIOC
#define VBAT_SNS_Pin             GPIO_PIN_2
#define VBAT_SNS_GPIO_Port       GPIOC
#define VPSU_SNS_Pin             GPIO_PIN_3
#define VPSU_SNS_GPIO_Port       GPIOC
#define TX_TO_GSM_Pin            GPIO_PIN_0
#define TX_TO_GSM_GPIO_Port      GPIOA
#define RX_FROM_GSM_Pin          GPIO_PIN_1
#define RX_FROM_GSM_GPIO_Port    GPIOA
#define SIMCARD_SEL_Pin          GPIO_PIN_2
#define SIMCARD_SEL_GPIO_Port    GPIOA
#define ON_LATCH_Pin             GPIO_PIN_4
#define ON_LATCH_GPIO_Port       GPIOC
#define GSM_VDD_ON_Pin           GPIO_PIN_5
#define GSM_VDD_ON_GPIO_Port     GPIOC
#define LED_LOGO_R_Pin           GPIO_PIN_0
#define LED_LOGO_R_GPIO_Port     GPIOB
#define GSM_VDD_SNS_Pin          GPIO_PIN_1
#define GSM_VDD_SNS_GPIO_Port    GPIOB
#define GSM_VDD_DISCHG_Pin       GPIO_PIN_2
#define GSM_VDD_DISCHG_GPIO_Port GPIOB
#define ETH_NSS_Pin              GPIO_PIN_11
#define ETH_NSS_GPIO_Port        GPIOE
#define ETH_SCK_Pin              GPIO_PIN_12
#define ETH_SCK_GPIO_Port        GPIOE
#define ETH_MISO_Pin             GPIO_PIN_13
#define ETH_MISO_GPIO_Port       GPIOE
#define ETH_MOSI_Pin             GPIO_PIN_14
#define ETH_MOSI_GPIO_Port       GPIOE
#define ETH_RESET_Pin            GPIO_PIN_10
#define ETH_RESET_GPIO_Port      GPIOB
#define ETH_IRQ_Pin              GPIO_PIN_8
#define ETH_IRQ_GPIO_Port        GPIOD
#define ETH_IRQ_EXTI_IRQn        EXTI9_5_IRQn
#define ETH_ON_Pin               GPIO_PIN_11
#define ETH_ON_GPIO_Port         GPIOD
#define RFM01_RDIO5_Pin          GPIO_PIN_12
#define RFM01_RDIO5_GPIO_Port    GPIOD
#define FLASH_IO3_Pin            GPIO_PIN_13
#define FLASH_IO3_GPIO_Port      GPIOD
#define LED_LOGO_G_Pin           GPIO_PIN_14
#define LED_LOGO_G_GPIO_Port     GPIOD
#define LED_LOGO_B_Pin           GPIO_PIN_15
#define LED_LOGO_B_GPIO_Port     GPIOD
#define RFM01_RFSW_Pin           GPIO_PIN_6
#define RFM01_RFSW_GPIO_Port     GPIOC
#define FLASH_IO2_Pin            GPIO_PIN_8
#define FLASH_IO2_GPIO_Port      GPIOC
#define FLASH_IO0_Pin            GPIO_PIN_9
#define FLASH_IO0_GPIO_Port      GPIOC
#define RFM01_RST_Pin            GPIO_PIN_8
#define RFM01_RST_GPIO_Port      GPIOA
#define RFM01_SCK_Pin            GPIO_PIN_9
#define RFM01_SCK_GPIO_Port      GPIOA
#define RFM01_MOSI_Pin           GPIO_PIN_10
#define RFM01_MOSI_GPIO_Port     GPIOA
#define RFM01_NSS_Pin            GPIO_PIN_11
#define RFM01_NSS_GPIO_Port      GPIOA
#define RFM01_MISO_Pin           GPIO_PIN_12
#define RFM01_MISO_GPIO_Port     GPIOA
#define TAMPER_Pin               GPIO_PIN_15
#define TAMPER_GPIO_Port         GPIOA
#define TAMPER_EXTI_IRQn         EXTI15_10_IRQn
#define FLASH_IO1_Pin            GPIO_PIN_10
#define FLASH_IO1_GPIO_Port      GPIOC
#define SIM1_DET_Pin             GPIO_PIN_11
#define SIM1_DET_GPIO_Port       GPIOC
#define SIM2_DET_Pin             GPIO_PIN_12
#define SIM2_DET_GPIO_Port       GPIOC
#define RFM01_RDIO0_Pin          GPIO_PIN_0
#define RFM01_RDIO0_GPIO_Port    GPIOD
#define RFM01_RDIO0_EXTI_IRQn    EXTI0_IRQn
#define RFM01_RDIO2_Pin          GPIO_PIN_1
#define RFM01_RDIO2_GPIO_Port    GPIOD
#define RFM01_RDIO4_Pin          GPIO_PIN_2
#define RFM01_RDIO4_GPIO_Port    GPIOD
#define FLASH_CLK_Pin            GPIO_PIN_3
#define FLASH_CLK_GPIO_Port      GPIOD
#define GSM_ON_Pin               GPIO_PIN_4
#define GSM_ON_GPIO_Port         GPIOD
#define CHG_ON_Pin               GPIO_PIN_5
#define CHG_ON_GPIO_Port         GPIOD
#define GSM_STATUS_Pin           GPIO_PIN_6
#define GSM_STATUS_GPIO_Port     GPIOD
#define CHG_DET_Pin              GPIO_PIN_4
#define CHG_DET_GPIO_Port        GPIOB
#define CHG_DET_EXTI_IRQn        EXTI4_IRQn
#define CHG_STBY_Pin             GPIO_PIN_5
#define CHG_STBY_GPIO_Port       GPIOB
#define CHG_STBY_EXTI_IRQn       EXTI9_5_IRQn
#define FLASH_CS_Pin             GPIO_PIN_6
#define FLASH_CS_GPIO_Port       GPIOB

/* USER CODE BEGIN Private defines */

#define PIN_SET(_pin)                                                  \
    do {                                                               \
        HAL_GPIO_WritePin(_pin##_GPIO_Port, _pin##_Pin, GPIO_PIN_SET); \
    } while (0)
#define PIN_RESET(_pin)                                                  \
    do {                                                                 \
        HAL_GPIO_WritePin(_pin##_GPIO_Port, _pin##_Pin, GPIO_PIN_RESET); \
    } while (0)

#define PIN_READ(_pin) HAL_GPIO_ReadPin(_pin##_GPIO_Port, _pin##_Pin)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
