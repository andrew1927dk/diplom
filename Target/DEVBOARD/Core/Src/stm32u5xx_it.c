/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    stm32u5xx_it.c
 * @brief   Interrupt Service Routines.
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
#include "stm32u5xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_NodeTypeDef    Node_GPDMA1_Channel5;
extern DMA_QListTypeDef   List_GPDMA1_Channel5;
extern DMA_HandleTypeDef  handle_GPDMA1_Channel5;
extern ADC_HandleTypeDef  hadc1;
extern DMA_HandleTypeDef  handle_GPDMA1_Channel2;
extern DMA_HandleTypeDef  handle_GPDMA1_Channel1;
extern DMA_HandleTypeDef  handle_GPDMA1_Channel4;
extern DMA_HandleTypeDef  handle_GPDMA1_Channel3;
extern I2C_HandleTypeDef  hi2c1;
extern I2C_HandleTypeDef  hi2c3;
extern IWDG_HandleTypeDef hiwdg;
extern DMA_HandleTypeDef  handle_GPDMA1_Channel0;
extern DMA_NodeTypeDef    Node_GPDMA1_Channel9;
extern DMA_QListTypeDef   List_GPDMA1_Channel9;
extern DMA_HandleTypeDef  handle_GPDMA1_Channel9;
extern UART_HandleTypeDef hlpuart1;
extern UART_HandleTypeDef huart1;
extern OSPI_HandleTypeDef hospi1;
extern DMA_HandleTypeDef  handle_GPDMA1_Channel8;
extern DMA_HandleTypeDef  handle_GPDMA1_Channel7;
extern DMA_HandleTypeDef  handle_GPDMA1_Channel6;
extern SPI_HandleTypeDef  hspi2;
extern SPI_HandleTypeDef  hspi3;
extern TIM_HandleTypeDef  htim2;
extern TIM_HandleTypeDef  htim7;
extern TIM_HandleTypeDef  htim17;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void) {
    /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

    /* USER CODE END NonMaskableInt_IRQn 0 */
    /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
    while (1) {
    }
    /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void) {
    /* USER CODE BEGIN HardFault_IRQn 0 */

    /* USER CODE END HardFault_IRQn 0 */
    while (1) {
        /* USER CODE BEGIN W1_HardFault_IRQn 0 */
        /* USER CODE END W1_HardFault_IRQn 0 */
    }
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void) {
    /* USER CODE BEGIN MemoryManagement_IRQn 0 */

    /* USER CODE END MemoryManagement_IRQn 0 */
    while (1) {
        /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
        /* USER CODE END W1_MemoryManagement_IRQn 0 */
    }
}

/**
 * @brief This function handles Prefetch fault, memory access fault.
 */
void BusFault_Handler(void) {
    /* USER CODE BEGIN BusFault_IRQn 0 */

    /* USER CODE END BusFault_IRQn 0 */
    while (1) {
        /* USER CODE BEGIN W1_BusFault_IRQn 0 */
        /* USER CODE END W1_BusFault_IRQn 0 */
    }
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void) {
    /* USER CODE BEGIN UsageFault_IRQn 0 */

    /* USER CODE END UsageFault_IRQn 0 */
    while (1) {
        /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
        /* USER CODE END W1_UsageFault_IRQn 0 */
    }
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void) {
    /* USER CODE BEGIN DebugMonitor_IRQn 0 */

    /* USER CODE END DebugMonitor_IRQn 0 */
    /* USER CODE BEGIN DebugMonitor_IRQn 1 */

    /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32U5xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32u5xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles EXTI Line0 interrupt.
 */
void EXTI0_IRQHandler(void) {
    /* USER CODE BEGIN EXTI0_IRQn 0 */

    /* USER CODE END EXTI0_IRQn 0 */
    HAL_GPIO_EXTI_IRQHandler(ISM_RDIO0_Pin);
    /* USER CODE BEGIN EXTI0_IRQn 1 */

    /* USER CODE END EXTI0_IRQn 1 */
}

/**
 * @brief This function handles EXTI Line2 interrupt.
 */
void EXTI2_IRQHandler(void) {
    /* USER CODE BEGIN EXTI2_IRQn 0 */

    /* USER CODE END EXTI2_IRQn 0 */
    HAL_GPIO_EXTI_IRQHandler(ISM_RDIO2_Pin);
    /* USER CODE BEGIN EXTI2_IRQn 1 */

    /* USER CODE END EXTI2_IRQn 1 */
}

/**
 * @brief This function handles EXTI Line4 interrupt.
 */
void EXTI4_IRQHandler(void) {
    /* USER CODE BEGIN EXTI4_IRQn 0 */

    /* USER CODE END EXTI4_IRQn 0 */
    HAL_GPIO_EXTI_IRQHandler(ISM_RDIO4_Pin);
    /* USER CODE BEGIN EXTI4_IRQn 1 */

    /* USER CODE END EXTI4_IRQn 1 */
}

/**
 * @brief This function handles EXTI Line6 interrupt.
 */
void EXTI6_IRQHandler(void) {
    /* USER CODE BEGIN EXTI6_IRQn 0 */

    /* USER CODE END EXTI6_IRQn 0 */
    HAL_GPIO_EXTI_IRQHandler(PMIC_IRQ_Pin);
    /* USER CODE BEGIN EXTI6_IRQn 1 */

    /* USER CODE END EXTI6_IRQn 1 */
}

/**
 * @brief This function handles EXTI Line7 interrupt.
 */
void EXTI7_IRQHandler(void) {
    /* USER CODE BEGIN EXTI7_IRQn 0 */

    /* USER CODE END EXTI7_IRQn 0 */
    HAL_GPIO_EXTI_IRQHandler(ETH_IRQ_Pin);
    /* USER CODE BEGIN EXTI7_IRQn 1 */

    /* USER CODE END EXTI7_IRQn 1 */
}

/**
 * @brief This function handles EXTI Line8 interrupt.
 */
void EXTI8_IRQHandler(void) {
    /* USER CODE BEGIN EXTI8_IRQn 0 */

    /* USER CODE END EXTI8_IRQn 0 */
    HAL_GPIO_EXTI_IRQHandler(ACCEL_IRQ1_Pin);
    /* USER CODE BEGIN EXTI8_IRQn 1 */

    /* USER CODE END EXTI8_IRQn 1 */
}

/**
 * @brief This function handles EXTI Line9 interrupt.
 */
void EXTI9_IRQHandler(void) {
    /* USER CODE BEGIN EXTI9_IRQn 0 */

    /* USER CODE END EXTI9_IRQn 0 */
    HAL_GPIO_EXTI_IRQHandler(BMP_IRQ_Pin);
    /* USER CODE BEGIN EXTI9_IRQn 1 */

    /* USER CODE END EXTI9_IRQn 1 */
}

/**
 * @brief This function handles EXTI Line10 interrupt.
 */
void EXTI10_IRQHandler(void) {
    /* USER CODE BEGIN EXTI10_IRQn 0 */

    /* USER CODE END EXTI10_IRQn 0 */
    HAL_GPIO_EXTI_IRQHandler(DISP_TS_IRQ_Pin);
    /* USER CODE BEGIN EXTI10_IRQn 1 */

    /* USER CODE END EXTI10_IRQn 1 */
}

/**
 * @brief This function handles EXTI Line11 interrupt.
 */
void EXTI11_IRQHandler(void) {
    /* USER CODE BEGIN EXTI11_IRQn 0 */

    /* USER CODE END EXTI11_IRQn 0 */
    HAL_GPIO_EXTI_IRQHandler(JOY_U_Pin);
    /* USER CODE BEGIN EXTI11_IRQn 1 */

    /* USER CODE END EXTI11_IRQn 1 */
}

/**
 * @brief This function handles EXTI Line12 interrupt.
 */
void EXTI12_IRQHandler(void) {
    /* USER CODE BEGIN EXTI12_IRQn 0 */

    /* USER CODE END EXTI12_IRQn 0 */
    HAL_GPIO_EXTI_IRQHandler(JOY_L_Pin);
    /* USER CODE BEGIN EXTI12_IRQn 1 */

    /* USER CODE END EXTI12_IRQn 1 */
}

/**
 * @brief This function handles EXTI Line13 interrupt.
 */
void EXTI13_IRQHandler(void) {
    /* USER CODE BEGIN EXTI13_IRQn 0 */

    /* USER CODE END EXTI13_IRQn 0 */
    HAL_GPIO_EXTI_IRQHandler(JOY_OK_Pin);
    /* USER CODE BEGIN EXTI13_IRQn 1 */

    /* USER CODE END EXTI13_IRQn 1 */
}

/**
 * @brief This function handles EXTI Line14 interrupt.
 */
void EXTI14_IRQHandler(void) {
    /* USER CODE BEGIN EXTI14_IRQn 0 */

    /* USER CODE END EXTI14_IRQn 0 */
    HAL_GPIO_EXTI_IRQHandler(JOY_R_Pin);
    /* USER CODE BEGIN EXTI14_IRQn 1 */

    /* USER CODE END EXTI14_IRQn 1 */
}

/**
 * @brief This function handles EXTI Line15 interrupt.
 */
void EXTI15_IRQHandler(void) {
    /* USER CODE BEGIN EXTI15_IRQn 0 */

    /* USER CODE END EXTI15_IRQn 0 */
    HAL_GPIO_EXTI_IRQHandler(JOY_D_Pin);
    /* USER CODE BEGIN EXTI15_IRQn 1 */

    /* USER CODE END EXTI15_IRQn 1 */
}

/**
 * @brief This function handles IWDG global interrupt.
 */
void IWDG_IRQHandler(void) {
    /* USER CODE BEGIN IWDG_IRQn 0 */

    /* USER CODE END IWDG_IRQn 0 */
    HAL_IWDG_IRQHandler(&hiwdg);
    /* USER CODE BEGIN IWDG_IRQn 1 */

    /* USER CODE END IWDG_IRQn 1 */
}

/**
 * @brief This function handles GPDMA1 Channel 0 global interrupt.
 */
void GPDMA1_Channel0_IRQHandler(void) {
    /* USER CODE BEGIN GPDMA1_Channel0_IRQn 0 */

    /* USER CODE END GPDMA1_Channel0_IRQn 0 */
    HAL_DMA_IRQHandler(&handle_GPDMA1_Channel0);
    /* USER CODE BEGIN GPDMA1_Channel0_IRQn 1 */

    /* USER CODE END GPDMA1_Channel0_IRQn 1 */
}

/**
 * @brief This function handles GPDMA1 Channel 1 global interrupt.
 */
void GPDMA1_Channel1_IRQHandler(void) {
    /* USER CODE BEGIN GPDMA1_Channel1_IRQn 0 */

    /* USER CODE END GPDMA1_Channel1_IRQn 0 */
    HAL_DMA_IRQHandler(&handle_GPDMA1_Channel1);
    /* USER CODE BEGIN GPDMA1_Channel1_IRQn 1 */

    /* USER CODE END GPDMA1_Channel1_IRQn 1 */
}

/**
 * @brief This function handles GPDMA1 Channel 2 global interrupt.
 */
void GPDMA1_Channel2_IRQHandler(void) {
    /* USER CODE BEGIN GPDMA1_Channel2_IRQn 0 */

    /* USER CODE END GPDMA1_Channel2_IRQn 0 */
    HAL_DMA_IRQHandler(&handle_GPDMA1_Channel2);
    /* USER CODE BEGIN GPDMA1_Channel2_IRQn 1 */

    /* USER CODE END GPDMA1_Channel2_IRQn 1 */
}

/**
 * @brief This function handles GPDMA1 Channel 3 global interrupt.
 */
void GPDMA1_Channel3_IRQHandler(void) {
    /* USER CODE BEGIN GPDMA1_Channel3_IRQn 0 */

    /* USER CODE END GPDMA1_Channel3_IRQn 0 */
    HAL_DMA_IRQHandler(&handle_GPDMA1_Channel3);
    /* USER CODE BEGIN GPDMA1_Channel3_IRQn 1 */

    /* USER CODE END GPDMA1_Channel3_IRQn 1 */
}

/**
 * @brief This function handles GPDMA1 Channel 4 global interrupt.
 */
void GPDMA1_Channel4_IRQHandler(void) {
    /* USER CODE BEGIN GPDMA1_Channel4_IRQn 0 */

    /* USER CODE END GPDMA1_Channel4_IRQn 0 */
    HAL_DMA_IRQHandler(&handle_GPDMA1_Channel4);
    /* USER CODE BEGIN GPDMA1_Channel4_IRQn 1 */

    /* USER CODE END GPDMA1_Channel4_IRQn 1 */
}

/**
 * @brief This function handles GPDMA1 Channel 5 global interrupt.
 */
void GPDMA1_Channel5_IRQHandler(void) {
    /* USER CODE BEGIN GPDMA1_Channel5_IRQn 0 */

    /* USER CODE END GPDMA1_Channel5_IRQn 0 */
    HAL_DMA_IRQHandler(&handle_GPDMA1_Channel5);
    /* USER CODE BEGIN GPDMA1_Channel5_IRQn 1 */

    /* USER CODE END GPDMA1_Channel5_IRQn 1 */
}

/**
 * @brief This function handles GPDMA1 Channel 6 global interrupt.
 */
void GPDMA1_Channel6_IRQHandler(void) {
    /* USER CODE BEGIN GPDMA1_Channel6_IRQn 0 */

    /* USER CODE END GPDMA1_Channel6_IRQn 0 */
    HAL_DMA_IRQHandler(&handle_GPDMA1_Channel6);
    /* USER CODE BEGIN GPDMA1_Channel6_IRQn 1 */

    /* USER CODE END GPDMA1_Channel6_IRQn 1 */
}

/**
 * @brief This function handles GPDMA1 Channel 7 global interrupt.
 */
void GPDMA1_Channel7_IRQHandler(void) {
    /* USER CODE BEGIN GPDMA1_Channel7_IRQn 0 */

    /* USER CODE END GPDMA1_Channel7_IRQn 0 */
    HAL_DMA_IRQHandler(&handle_GPDMA1_Channel7);
    /* USER CODE BEGIN GPDMA1_Channel7_IRQn 1 */

    /* USER CODE END GPDMA1_Channel7_IRQn 1 */
}

/**
 * @brief This function handles ADC1 global interrupt.
 */
void ADC1_IRQHandler(void) {
    /* USER CODE BEGIN ADC1_IRQn 0 */

    /* USER CODE END ADC1_IRQn 0 */
    HAL_ADC_IRQHandler(&hadc1);
    /* USER CODE BEGIN ADC1_IRQn 1 */

    /* USER CODE END ADC1_IRQn 1 */
}

/**
 * @brief This function handles TIM2 global interrupt.
 */
void TIM2_IRQHandler(void) {
    /* USER CODE BEGIN TIM2_IRQn 0 */

    /* USER CODE END TIM2_IRQn 0 */
    HAL_TIM_IRQHandler(&htim2);
    /* USER CODE BEGIN TIM2_IRQn 1 */

    /* USER CODE END TIM2_IRQn 1 */
}

/**
 * @brief This function handles TIM7 global interrupt.
 */
void TIM7_IRQHandler(void) {
    /* USER CODE BEGIN TIM7_IRQn 0 */

    /* USER CODE END TIM7_IRQn 0 */
    HAL_TIM_IRQHandler(&htim7);
    /* USER CODE BEGIN TIM7_IRQn 1 */

    /* USER CODE END TIM7_IRQn 1 */
}

/**
 * @brief This function handles I2C1 Event interrupt.
 */
void I2C1_EV_IRQHandler(void) {
    /* USER CODE BEGIN I2C1_EV_IRQn 0 */

    /* USER CODE END I2C1_EV_IRQn 0 */
    HAL_I2C_EV_IRQHandler(&hi2c1);
    /* USER CODE BEGIN I2C1_EV_IRQn 1 */

    /* USER CODE END I2C1_EV_IRQn 1 */
}

/**
 * @brief This function handles I2C1 Error interrupt.
 */
void I2C1_ER_IRQHandler(void) {
    /* USER CODE BEGIN I2C1_ER_IRQn 0 */

    /* USER CODE END I2C1_ER_IRQn 0 */
    HAL_I2C_ER_IRQHandler(&hi2c1);
    /* USER CODE BEGIN I2C1_ER_IRQn 1 */

    /* USER CODE END I2C1_ER_IRQn 1 */
}

/**
 * @brief This function handles SPI2 global interrupt.
 */
void SPI2_IRQHandler(void) {
    /* USER CODE BEGIN SPI2_IRQn 0 */

    /* USER CODE END SPI2_IRQn 0 */
    HAL_SPI_IRQHandler(&hspi2);
    /* USER CODE BEGIN SPI2_IRQn 1 */

    /* USER CODE END SPI2_IRQn 1 */
}

/**
 * @brief This function handles USART1 global interrupt.
 */
void USART1_IRQHandler(void) {
    /* USER CODE BEGIN USART1_IRQn 0 */

    /* USER CODE END USART1_IRQn 0 */
    HAL_UART_IRQHandler(&huart1);
    /* USER CODE BEGIN USART1_IRQn 1 */

    /* USER CODE END USART1_IRQn 1 */
}

/**
 * @brief This function handles LPUART1 global interrupt.
 */
void LPUART1_IRQHandler(void) {
    /* USER CODE BEGIN LPUART1_IRQn 0 */

    /* USER CODE END LPUART1_IRQn 0 */
    HAL_UART_IRQHandler(&hlpuart1);
    /* USER CODE BEGIN LPUART1_IRQn 1 */

    /* USER CODE END LPUART1_IRQn 1 */
}

/**
 * @brief This function handles TIM17 global interrupt.
 */
void TIM17_IRQHandler(void) {
    /* USER CODE BEGIN TIM17_IRQn 0 */

    /* USER CODE END TIM17_IRQn 0 */
    HAL_TIM_IRQHandler(&htim17);
    /* USER CODE BEGIN TIM17_IRQn 1 */

    /* USER CODE END TIM17_IRQn 1 */
}

/**
 * @brief This function handles OCTOSPI1 global interrupt.
 */
void OCTOSPI1_IRQHandler(void) {
    /* USER CODE BEGIN OCTOSPI1_IRQn 0 */

    /* USER CODE END OCTOSPI1_IRQn 0 */
    HAL_OSPI_IRQHandler(&hospi1);
    /* USER CODE BEGIN OCTOSPI1_IRQn 1 */

    /* USER CODE END OCTOSPI1_IRQn 1 */
}

/**
 * @brief This function handles GPDMA1 Channel 8 global interrupt.
 */
void GPDMA1_Channel8_IRQHandler(void) {
    /* USER CODE BEGIN GPDMA1_Channel8_IRQn 0 */

    /* USER CODE END GPDMA1_Channel8_IRQn 0 */
    HAL_DMA_IRQHandler(&handle_GPDMA1_Channel8);
    /* USER CODE BEGIN GPDMA1_Channel8_IRQn 1 */

    /* USER CODE END GPDMA1_Channel8_IRQn 1 */
}

/**
 * @brief This function handles GPDMA1 Channel 9 global interrupt.
 */
void GPDMA1_Channel9_IRQHandler(void) {
    /* USER CODE BEGIN GPDMA1_Channel9_IRQn 0 */

    /* USER CODE END GPDMA1_Channel9_IRQn 0 */
    HAL_DMA_IRQHandler(&handle_GPDMA1_Channel9);
    /* USER CODE BEGIN GPDMA1_Channel9_IRQn 1 */

    /* USER CODE END GPDMA1_Channel9_IRQn 1 */
}

/**
 * @brief This function handles I2C3 event interrupt.
 */
void I2C3_EV_IRQHandler(void) {
    /* USER CODE BEGIN I2C3_EV_IRQn 0 */

    /* USER CODE END I2C3_EV_IRQn 0 */
    HAL_I2C_EV_IRQHandler(&hi2c3);
    /* USER CODE BEGIN I2C3_EV_IRQn 1 */

    /* USER CODE END I2C3_EV_IRQn 1 */
}

/**
 * @brief This function handles I2C3 error interrupt.
 */
void I2C3_ER_IRQHandler(void) {
    /* USER CODE BEGIN I2C3_ER_IRQn 0 */

    /* USER CODE END I2C3_ER_IRQn 0 */
    HAL_I2C_ER_IRQHandler(&hi2c3);
    /* USER CODE BEGIN I2C3_ER_IRQn 1 */

    /* USER CODE END I2C3_ER_IRQn 1 */
}

/**
 * @brief This function handles SPI3 global interrupt.
 */
void SPI3_IRQHandler(void) {
    /* USER CODE BEGIN SPI3_IRQn 0 */

    /* USER CODE END SPI3_IRQn 0 */
    HAL_SPI_IRQHandler(&hspi3);
    /* USER CODE BEGIN SPI3_IRQn 1 */

    /* USER CODE END SPI3_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
