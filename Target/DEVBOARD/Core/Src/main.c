/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_threadx.h"
#include "adc.h"
#include "gpdma.h"
#include "i2c.h"
#include "icache.h"
#include "iwdg.h"
#include "usart.h"
#include "octospi.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "loglib.h"
#include "esp32.h"
#include "parser.h"
#include "buzzer.h"
#include "boot_reason.h"
#include "display.h"
#include "flash.h"
#include "rfm66a.h"
#include "geode.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void        SystemClock_Config(void);
static void SystemPower_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
    /* USER CODE BEGIN 1 */

    /* USER CODE END 1 */

    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* USER CODE BEGIN Init */

    /* USER CODE END Init */

    /* Configure the system clock */
    SystemClock_Config();

    /* Configure the System Power */
    SystemPower_Config();

    /* USER CODE BEGIN SysInit */
    GENERAL_GetResetStatus();

    HAL_PWREx_EnableVddA();
    /* USER CODE END SysInit */

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_GPDMA1_Init();
    MX_SPI2_Init();
    MX_TIM4_Init();
    MX_I2C1_Init();
    MX_I2C3_Init();
    MX_USART1_UART_Init();
    MX_SPI3_Init();
    MX_SPI1_Init();
    MX_USART2_UART_Init();
    MX_ADC1_Init();
    MX_LPUART1_UART_Init();
    MX_USART3_UART_Init();
    MX_TIM3_Init();
    MX_OCTOSPI1_Init();
    MX_TIM2_Init();
    MX_ICACHE_Init();
    MX_IWDG_Init();
    MX_TIM7_Init();
    /* USER CODE BEGIN 2 */

    /* USER CODE END 2 */

    MX_ThreadX_Init();

    /* We should never get here as control is now taken by the scheduler */
    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
    }
    /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
    RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

    /** Configure the main internal regulator output voltage
     */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.LSIDiv = RCC_LSI_DIV1;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV2;
    RCC_OscInitStruct.PLL.PLLM = 3;
    RCC_OscInitStruct.PLL.PLLN = 20;
    RCC_OscInitStruct.PLL.PLLP = 2;
    RCC_OscInitStruct.PLL.PLLQ = 2;
    RCC_OscInitStruct.PLL.PLLR = 1;
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLLVCIRANGE_1;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK3;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
        Error_Handler();
    }
}

/**
 * @brief Power Configuration
 * @retval None
 */
static void SystemPower_Config(void) {

    /*
     * Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
     */
    HAL_PWREx_DisableUCPDDeadBattery();
}

/* USER CODE BEGIN 4 */

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi == &DISP_SPI) {
        ILI9341_TxCpltCallback();
    }
    if (hspi == &RFM_SPI) {
        RFM_SPI_SemRelease();
    }
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi == &DISP_SPI) {
        ILI9341_RxCpltCallback();
    }
    if (hspi == &RFM_SPI) {
        RFM_SPI_SemRelease();
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    // if (huart == &MODEM_UART) {
    //     MOD_TxCpltCallback();
    // }
    if (huart == &LOG_UART) {
        LOG_TxCpltCallback();
    }
    if (huart == &EXT_UART) {
        ESP32_TxCallback();
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &LOG_UART) {
        PARSER_RxCpltCallback();
    }
    if (huart == &EXT_UART) {
        ESP32_RxCallback();
    }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        BUZZER_TIM_PWM_PulseFinishedCallback(htim);
    }
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == JOY_U_Pin) {
        LOG_DEBUG("UP BUTTON UNPRESSED");
    }
    if (GPIO_Pin == JOY_D_Pin) {
        LOG_DEBUG("DOWN BUTTON UNPRESSED");
    }
    if (GPIO_Pin == JOY_L_Pin) {
        LOG_DEBUG("LEFT BUTTON UNPRESSED");
    }
    if (GPIO_Pin == JOY_R_Pin) {
        LOG_DEBUG("RIGHT BUTTON UNPRESSED");
    }
    if (GPIO_Pin == JOY_OK_Pin) {
        LOG_DEBUG("OK BUTTON UNPRESSED");
    }
    if (GPIO_Pin == ISM_RDIO2_Pin) {
        RFM_SemRelease();
    }
}

void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == JOY_U_Pin) {
        LOG_DEBUG("UP BUTTON PRESSED");
    }
    if (GPIO_Pin == JOY_D_Pin) {
        LOG_DEBUG("DOWN BUTTON PRESSED");
    }
    if (GPIO_Pin == JOY_L_Pin) {
        LOG_DEBUG("LEFT BUTTON PRESSED");
    }
    if (GPIO_Pin == JOY_R_Pin) {
        LOG_DEBUG("RIGHT BUTTON PRESSED");
    }
    if (GPIO_Pin == JOY_OK_Pin) {
        LOG_DEBUG("OK BUTTON PRESSED");
    }
    if (GPIO_Pin == ISM_RDIO0_Pin || GPIO_Pin == ISM_RDIO4_Pin) {
        RFM_SemRelease();
    }
}

void HAL_OSPI_StatusMatchCallback(OSPI_HandleTypeDef *hospi) {
    if (hospi == &FLASH_QSPI) {
        IS25L_StatusMatchCallback();
    }
}

void HAL_OSPI_RxCpltCallback(OSPI_HandleTypeDef *hospi) {
    if (hospi == &FLASH_QSPI) {
        IS25L_RxCompleteCallback();
    }
}

void HAL_OSPI_TxCpltCallback(OSPI_HandleTypeDef *hospi) {
    if (hospi == &FLASH_QSPI) {
        IS25L_TxCompleteCallback();
    }
}

void HAL_OSPI_CmdCpltCallback(OSPI_HandleTypeDef *hospi) {
    if (hospi == &FLASH_QSPI) {
        IS25L_CmdCompleteCallback();
    }
}

/* USER CODE END 4 */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM17 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    /* USER CODE BEGIN Callback 0 */
    GOD_TIM_Callback(htim);
    /* USER CODE END Callback 0 */
    if (htim->Instance == TIM17) {
        HAL_IncTick();
    }
    /* USER CODE BEGIN Callback 1 */
    if (htim->Instance == TIM17) {
        lv_tick_inc(1);
    }

    /* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
