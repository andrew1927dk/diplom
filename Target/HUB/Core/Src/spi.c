/* USER CODE BEGIN Header */

/**
 ******************************************************************************
 * @file    spi.c
 * @brief   This file provides code for the configuration
 *          of the SPI instances.
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
#include "spi.h"

/* USER CODE BEGIN 0 */
#include "wiznet_w5500.h"
#include "rfm66.h"

/* USER CODE END 0 */

SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi4;
DMA_HandleTypeDef hdma_spi2_rx;
DMA_HandleTypeDef hdma_spi2_tx;
DMA_HandleTypeDef hdma_spi4_rx;
DMA_HandleTypeDef hdma_spi4_tx;

/* SPI2 init function */
void MX_SPI2_Init(void) {

    /* USER CODE BEGIN SPI2_Init 0 */

    /* USER CODE END SPI2_Init 0 */

    /* USER CODE BEGIN SPI2_Init 1 */

    /* USER CODE END SPI2_Init 1 */
    hspi2.Instance = SPI2;
    hspi2.Init.Mode = SPI_MODE_MASTER;
    hspi2.Init.Direction = SPI_DIRECTION_2LINES;
    hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi2.Init.NSS = SPI_NSS_SOFT;
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi2.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi2) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN SPI2_Init 2 */

    /* USER CODE END SPI2_Init 2 */
}
/* SPI4 init function */
void MX_SPI4_Init(void) {

    /* USER CODE BEGIN SPI4_Init 0 */

    /* USER CODE END SPI4_Init 0 */

    /* USER CODE BEGIN SPI4_Init 1 */

    /* USER CODE END SPI4_Init 1 */
    hspi4.Instance = SPI4;
    hspi4.Init.Mode = SPI_MODE_MASTER;
    hspi4.Init.Direction = SPI_DIRECTION_2LINES;
    hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi4.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi4.Init.NSS = SPI_NSS_SOFT;
    hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi4.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi4.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi4) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN SPI4_Init 2 */

    /* USER CODE END SPI4_Init 2 */
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *spiHandle) {

    GPIO_InitTypeDef GPIO_InitStruct = { 0 };
    if (spiHandle->Instance == SPI2) {
        /* USER CODE BEGIN SPI2_MspInit 0 */

        /* USER CODE END SPI2_MspInit 0 */
        /* SPI2 clock enable */
        __HAL_RCC_SPI2_CLK_ENABLE();

        __HAL_RCC_GPIOA_CLK_ENABLE();
        /**SPI2 GPIO Configuration
        PA9     ------> SPI2_SCK
        PA10     ------> SPI2_MOSI
        PA12     ------> SPI2_MISO
        */
        GPIO_InitStruct.Pin = RFM01_SCK_Pin | RFM01_MOSI_Pin | RFM01_MISO_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* SPI2 DMA Init */
        /* SPI2_RX Init */
        hdma_spi2_rx.Instance = DMA1_Stream3;
        hdma_spi2_rx.Init.Channel = DMA_CHANNEL_0;
        hdma_spi2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_spi2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_spi2_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_spi2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_spi2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_spi2_rx.Init.Mode = DMA_NORMAL;
        hdma_spi2_rx.Init.Priority = DMA_PRIORITY_HIGH;
        hdma_spi2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_spi2_rx) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(spiHandle, hdmarx, hdma_spi2_rx);

        /* SPI2_TX Init */
        hdma_spi2_tx.Instance = DMA1_Stream4;
        hdma_spi2_tx.Init.Channel = DMA_CHANNEL_0;
        hdma_spi2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_spi2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_spi2_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_spi2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_spi2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_spi2_tx.Init.Mode = DMA_NORMAL;
        hdma_spi2_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
        hdma_spi2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_spi2_tx) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(spiHandle, hdmatx, hdma_spi2_tx);

        /* SPI2 interrupt Init */
        HAL_NVIC_SetPriority(SPI2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(SPI2_IRQn);
        /* USER CODE BEGIN SPI2_MspInit 1 */

        /* USER CODE END SPI2_MspInit 1 */
    } else if (spiHandle->Instance == SPI4) {
        /* USER CODE BEGIN SPI4_MspInit 0 */

        /* USER CODE END SPI4_MspInit 0 */
        /* SPI4 clock enable */
        __HAL_RCC_SPI4_CLK_ENABLE();

        __HAL_RCC_GPIOE_CLK_ENABLE();
        /**SPI4 GPIO Configuration
        PE12     ------> SPI4_SCK
        PE13     ------> SPI4_MISO
        PE14     ------> SPI4_MOSI
        */
        GPIO_InitStruct.Pin = ETH_SCK_Pin | ETH_MISO_Pin | ETH_MOSI_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        /* SPI4 DMA Init */
        /* SPI4_RX Init */
        hdma_spi4_rx.Instance = DMA2_Stream0;
        hdma_spi4_rx.Init.Channel = DMA_CHANNEL_4;
        hdma_spi4_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        hdma_spi4_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_spi4_rx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_spi4_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_spi4_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_spi4_rx.Init.Mode = DMA_NORMAL;
        hdma_spi4_rx.Init.Priority = DMA_PRIORITY_HIGH;
        hdma_spi4_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_spi4_rx) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(spiHandle, hdmarx, hdma_spi4_rx);

        /* SPI4_TX Init */
        hdma_spi4_tx.Instance = DMA2_Stream1;
        hdma_spi4_tx.Init.Channel = DMA_CHANNEL_4;
        hdma_spi4_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        hdma_spi4_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        hdma_spi4_tx.Init.MemInc = DMA_MINC_ENABLE;
        hdma_spi4_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        hdma_spi4_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        hdma_spi4_tx.Init.Mode = DMA_NORMAL;
        hdma_spi4_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
        hdma_spi4_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&hdma_spi4_tx) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(spiHandle, hdmatx, hdma_spi4_tx);

        /* SPI4 interrupt Init */
        HAL_NVIC_SetPriority(SPI4_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(SPI4_IRQn);
        /* USER CODE BEGIN SPI4_MspInit 1 */

        /* USER CODE END SPI4_MspInit 1 */
    }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *spiHandle) {

    if (spiHandle->Instance == SPI2) {
        /* USER CODE BEGIN SPI2_MspDeInit 0 */

        /* USER CODE END SPI2_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_SPI2_CLK_DISABLE();

        /**SPI2 GPIO Configuration
        PA9     ------> SPI2_SCK
        PA10     ------> SPI2_MOSI
        PA12     ------> SPI2_MISO
        */
        HAL_GPIO_DeInit(GPIOA, RFM01_SCK_Pin | RFM01_MOSI_Pin | RFM01_MISO_Pin);

        /* SPI2 DMA DeInit */
        HAL_DMA_DeInit(spiHandle->hdmarx);
        HAL_DMA_DeInit(spiHandle->hdmatx);

        /* SPI2 interrupt Deinit */
        HAL_NVIC_DisableIRQ(SPI2_IRQn);
        /* USER CODE BEGIN SPI2_MspDeInit 1 */

        /* USER CODE END SPI2_MspDeInit 1 */
    } else if (spiHandle->Instance == SPI4) {
        /* USER CODE BEGIN SPI4_MspDeInit 0 */

        /* USER CODE END SPI4_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_SPI4_CLK_DISABLE();

        /**SPI4 GPIO Configuration
        PE12     ------> SPI4_SCK
        PE13     ------> SPI4_MISO
        PE14     ------> SPI4_MOSI
        */
        HAL_GPIO_DeInit(GPIOE, ETH_SCK_Pin | ETH_MISO_Pin | ETH_MOSI_Pin);

        /* SPI4 DMA DeInit */
        HAL_DMA_DeInit(spiHandle->hdmarx);
        HAL_DMA_DeInit(spiHandle->hdmatx);

        /* SPI4 interrupt Deinit */
        HAL_NVIC_DisableIRQ(SPI4_IRQn);
        /* USER CODE BEGIN SPI4_MspDeInit 1 */

        /* USER CODE END SPI4_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
    RFM_SPI_CpltCallback(hspi);

    // if (hspi == &ETH_SPI) {
    //     w5500_TxRxCpltCallback();
    // }
}

/* Disable code for native W5500 driver */
#if 0
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi == &ETH_SPI) {
        wizchipTxRxCpltCallback();
    }
}

void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
    if (hspi == &ETH_SPI) {
        wizchipTxRxCpltCallback();
    }
}
#endif

/* USER CODE END 1 */
