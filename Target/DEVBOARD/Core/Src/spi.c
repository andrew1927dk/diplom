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

/* USER CODE END 0 */

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;
DMA_HandleTypeDef handle_GPDMA1_Channel8;
DMA_HandleTypeDef handle_GPDMA1_Channel7;
DMA_HandleTypeDef handle_GPDMA1_Channel6;

/* SPI1 init function */
void MX_SPI1_Init(void) {

    /* USER CODE BEGIN SPI1_Init 0 */

    /* USER CODE END SPI1_Init 0 */

    SPI_AutonomousModeConfTypeDef HAL_SPI_AutonomousMode_Cfg_Struct = { 0 };

    /* USER CODE BEGIN SPI1_Init 1 */

    /* USER CODE END SPI1_Init 1 */
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 0x7;
    hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
    hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
    hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
    hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
    hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
    hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
    hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
    hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
    hspi1.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
    hspi1.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
    if (HAL_SPI_Init(&hspi1) != HAL_OK) {
        Error_Handler();
    }
    HAL_SPI_AutonomousMode_Cfg_Struct.TriggerState = SPI_AUTO_MODE_DISABLE;
    HAL_SPI_AutonomousMode_Cfg_Struct.TriggerSelection = SPI_GRP1_GPDMA_CH0_TCF_TRG;
    HAL_SPI_AutonomousMode_Cfg_Struct.TriggerPolarity = SPI_TRIG_POLARITY_RISING;
    if (HAL_SPIEx_SetConfigAutonomousMode(&hspi1, &HAL_SPI_AutonomousMode_Cfg_Struct) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN SPI1_Init 2 */

    /* USER CODE END SPI1_Init 2 */
}
/* SPI2 init function */
void MX_SPI2_Init(void) {

    /* USER CODE BEGIN SPI2_Init 0 */

    /* USER CODE END SPI2_Init 0 */

    SPI_AutonomousModeConfTypeDef HAL_SPI_AutonomousMode_Cfg_Struct = { 0 };

    /* USER CODE BEGIN SPI2_Init 1 */

    /* USER CODE END SPI2_Init 1 */
    hspi2.Instance = SPI2;
    hspi2.Init.Mode = SPI_MODE_MASTER;
    hspi2.Init.Direction = SPI_DIRECTION_2LINES;
    hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi2.Init.NSS = SPI_NSS_SOFT;
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi2.Init.CRCPolynomial = 0x7;
    hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
    hspi2.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
    hspi2.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
    hspi2.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
    hspi2.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
    hspi2.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
    hspi2.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
    hspi2.Init.IOSwap = SPI_IO_SWAP_DISABLE;
    hspi2.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
    hspi2.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
    if (HAL_SPI_Init(&hspi2) != HAL_OK) {
        Error_Handler();
    }
    HAL_SPI_AutonomousMode_Cfg_Struct.TriggerState = SPI_AUTO_MODE_DISABLE;
    HAL_SPI_AutonomousMode_Cfg_Struct.TriggerSelection = SPI_GRP1_GPDMA_CH0_TCF_TRG;
    HAL_SPI_AutonomousMode_Cfg_Struct.TriggerPolarity = SPI_TRIG_POLARITY_RISING;
    if (HAL_SPIEx_SetConfigAutonomousMode(&hspi2, &HAL_SPI_AutonomousMode_Cfg_Struct) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN SPI2_Init 2 */

    /* USER CODE END SPI2_Init 2 */
}
/* SPI3 init function */
void MX_SPI3_Init(void) {

    /* USER CODE BEGIN SPI3_Init 0 */

    /* USER CODE END SPI3_Init 0 */

    SPI_AutonomousModeConfTypeDef HAL_SPI_AutonomousMode_Cfg_Struct = { 0 };

    /* USER CODE BEGIN SPI3_Init 1 */

    /* USER CODE END SPI3_Init 1 */
    hspi3.Instance = SPI3;
    hspi3.Init.Mode = SPI_MODE_MASTER;
    hspi3.Init.Direction = SPI_DIRECTION_2LINES;
    hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi3.Init.NSS = SPI_NSS_SOFT;
    hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
    hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi3.Init.CRCPolynomial = 0x7;
    hspi3.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
    hspi3.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
    hspi3.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
    hspi3.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
    hspi3.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
    hspi3.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
    hspi3.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
    hspi3.Init.IOSwap = SPI_IO_SWAP_DISABLE;
    hspi3.Init.ReadyMasterManagement = SPI_RDY_MASTER_MANAGEMENT_INTERNALLY;
    hspi3.Init.ReadyPolarity = SPI_RDY_POLARITY_HIGH;
    if (HAL_SPI_Init(&hspi3) != HAL_OK) {
        Error_Handler();
    }
    HAL_SPI_AutonomousMode_Cfg_Struct.TriggerState = SPI_AUTO_MODE_DISABLE;
    HAL_SPI_AutonomousMode_Cfg_Struct.TriggerSelection = SPI_GRP1_GPDMA_CH0_TCF_TRG;
    HAL_SPI_AutonomousMode_Cfg_Struct.TriggerPolarity = SPI_TRIG_POLARITY_RISING;
    if (HAL_SPIEx_SetConfigAutonomousMode(&hspi3, &HAL_SPI_AutonomousMode_Cfg_Struct) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN SPI3_Init 2 */

    /* USER CODE END SPI3_Init 2 */
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *spiHandle) {

    GPIO_InitTypeDef         GPIO_InitStruct = { 0 };
    RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };
    if (spiHandle->Instance == SPI1) {
        /* USER CODE BEGIN SPI1_MspInit 0 */

        /* USER CODE END SPI1_MspInit 0 */

        /** Initializes the peripherals clock
         */
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SPI1;
        PeriphClkInit.Spi1ClockSelection = RCC_SPI1CLKSOURCE_SYSCLK;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
            Error_Handler();
        }

        /* SPI1 clock enable */
        __HAL_RCC_SPI1_CLK_ENABLE();

        __HAL_RCC_GPIOE_CLK_ENABLE();
        /**SPI1 GPIO Configuration
        PE13     ------> SPI1_SCK
        PE14     ------> SPI1_MISO
        PE15     ------> SPI1_MOSI
        */
        GPIO_InitStruct.Pin = ETH_SCK_Pin | ETH_MISO_Pin | ETH_MOSI_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

        /* USER CODE BEGIN SPI1_MspInit 1 */

        /* USER CODE END SPI1_MspInit 1 */
    } else if (spiHandle->Instance == SPI2) {
        /* USER CODE BEGIN SPI2_MspInit 0 */

        /* USER CODE END SPI2_MspInit 0 */

        /** Initializes the peripherals clock
         */
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SPI2;
        PeriphClkInit.Spi2ClockSelection = RCC_SPI2CLKSOURCE_SYSCLK;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
            Error_Handler();
        }

        /* SPI2 clock enable */
        __HAL_RCC_SPI2_CLK_ENABLE();

        __HAL_RCC_GPIOD_CLK_ENABLE();
        /**SPI2 GPIO Configuration
        PD1     ------> SPI2_SCK
        PD3     ------> SPI2_MISO
        PD4     ------> SPI2_MOSI
        */
        GPIO_InitStruct.Pin = DISP_SCK_Pin | DISP_MISO_Pin | DISP_MOSI_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
        HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

        /* SPI2 DMA Init */
        /* GPDMA1_REQUEST_SPI2_TX Init */
        handle_GPDMA1_Channel8.Instance = GPDMA1_Channel8;
        handle_GPDMA1_Channel8.Init.Request = GPDMA1_REQUEST_SPI2_TX;
        handle_GPDMA1_Channel8.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
        handle_GPDMA1_Channel8.Init.Direction = DMA_PERIPH_TO_MEMORY;
        handle_GPDMA1_Channel8.Init.SrcInc = DMA_SINC_INCREMENTED;
        handle_GPDMA1_Channel8.Init.DestInc = DMA_DINC_FIXED;
        handle_GPDMA1_Channel8.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
        handle_GPDMA1_Channel8.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
        handle_GPDMA1_Channel8.Init.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
        handle_GPDMA1_Channel8.Init.SrcBurstLength = 1;
        handle_GPDMA1_Channel8.Init.DestBurstLength = 1;
        handle_GPDMA1_Channel8.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
        handle_GPDMA1_Channel8.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
        handle_GPDMA1_Channel8.Init.Mode = DMA_NORMAL;
        if (HAL_DMA_Init(&handle_GPDMA1_Channel8) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(spiHandle, hdmatx, handle_GPDMA1_Channel8);

        if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel8, DMA_CHANNEL_NPRIV) != HAL_OK) {
            Error_Handler();
        }

        /* SPI2 interrupt Init */
        HAL_NVIC_SetPriority(SPI2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(SPI2_IRQn);
        /* USER CODE BEGIN SPI2_MspInit 1 */

        /* USER CODE END SPI2_MspInit 1 */
    } else if (spiHandle->Instance == SPI3) {
        /* USER CODE BEGIN SPI3_MspInit 0 */

        /* USER CODE END SPI3_MspInit 0 */

        /** Initializes the peripherals clock
         */
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SPI3;
        PeriphClkInit.Spi3ClockSelection = RCC_SPI3CLKSOURCE_SYSCLK;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
            Error_Handler();
        }

        /* SPI3 clock enable */
        __HAL_RCC_SPI3_CLK_ENABLE();

        __HAL_RCC_GPIOC_CLK_ENABLE();
        /**SPI3 GPIO Configuration
        PC10     ------> SPI3_SCK
        PC11     ------> SPI3_MISO
        PC12     ------> SPI3_MOSI
        */
        GPIO_InitStruct.Pin = ISM_SCK_Pin | ISM_MISO_Pin | ISM_MOSI_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        /* SPI3 DMA Init */
        /* GPDMA1_REQUEST_SPI3_TX Init */
        handle_GPDMA1_Channel7.Instance = GPDMA1_Channel7;
        handle_GPDMA1_Channel7.Init.Request = GPDMA1_REQUEST_SPI3_TX;
        handle_GPDMA1_Channel7.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
        handle_GPDMA1_Channel7.Init.Direction = DMA_MEMORY_TO_PERIPH;
        handle_GPDMA1_Channel7.Init.SrcInc = DMA_SINC_INCREMENTED;
        handle_GPDMA1_Channel7.Init.DestInc = DMA_DINC_FIXED;
        handle_GPDMA1_Channel7.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
        handle_GPDMA1_Channel7.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
        handle_GPDMA1_Channel7.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
        handle_GPDMA1_Channel7.Init.SrcBurstLength = 1;
        handle_GPDMA1_Channel7.Init.DestBurstLength = 1;
        handle_GPDMA1_Channel7.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
        handle_GPDMA1_Channel7.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
        handle_GPDMA1_Channel7.Init.Mode = DMA_NORMAL;
        if (HAL_DMA_Init(&handle_GPDMA1_Channel7) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(spiHandle, hdmatx, handle_GPDMA1_Channel7);

        if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel7, DMA_CHANNEL_NPRIV) != HAL_OK) {
            Error_Handler();
        }

        /* GPDMA1_REQUEST_SPI3_RX Init */
        handle_GPDMA1_Channel6.Instance = GPDMA1_Channel6;
        handle_GPDMA1_Channel6.Init.Request = GPDMA1_REQUEST_SPI3_RX;
        handle_GPDMA1_Channel6.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
        handle_GPDMA1_Channel6.Init.Direction = DMA_PERIPH_TO_MEMORY;
        handle_GPDMA1_Channel6.Init.SrcInc = DMA_SINC_FIXED;
        handle_GPDMA1_Channel6.Init.DestInc = DMA_DINC_INCREMENTED;
        handle_GPDMA1_Channel6.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
        handle_GPDMA1_Channel6.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
        handle_GPDMA1_Channel6.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
        handle_GPDMA1_Channel6.Init.SrcBurstLength = 1;
        handle_GPDMA1_Channel6.Init.DestBurstLength = 1;
        handle_GPDMA1_Channel6.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
        handle_GPDMA1_Channel6.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
        handle_GPDMA1_Channel6.Init.Mode = DMA_NORMAL;
        if (HAL_DMA_Init(&handle_GPDMA1_Channel6) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(spiHandle, hdmarx, handle_GPDMA1_Channel6);

        if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel6, DMA_CHANNEL_NPRIV) != HAL_OK) {
            Error_Handler();
        }

        /* SPI3 interrupt Init */
        HAL_NVIC_SetPriority(SPI3_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(SPI3_IRQn);
        /* USER CODE BEGIN SPI3_MspInit 1 */

        /* USER CODE END SPI3_MspInit 1 */
    }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef *spiHandle) {

    if (spiHandle->Instance == SPI1) {
        /* USER CODE BEGIN SPI1_MspDeInit 0 */

        /* USER CODE END SPI1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_SPI1_CLK_DISABLE();

        /**SPI1 GPIO Configuration
        PE13     ------> SPI1_SCK
        PE14     ------> SPI1_MISO
        PE15     ------> SPI1_MOSI
        */
        HAL_GPIO_DeInit(GPIOE, ETH_SCK_Pin | ETH_MISO_Pin | ETH_MOSI_Pin);

        /* USER CODE BEGIN SPI1_MspDeInit 1 */

        /* USER CODE END SPI1_MspDeInit 1 */
    } else if (spiHandle->Instance == SPI2) {
        /* USER CODE BEGIN SPI2_MspDeInit 0 */

        /* USER CODE END SPI2_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_SPI2_CLK_DISABLE();

        /**SPI2 GPIO Configuration
        PD1     ------> SPI2_SCK
        PD3     ------> SPI2_MISO
        PD4     ------> SPI2_MOSI
        */
        HAL_GPIO_DeInit(GPIOD, DISP_SCK_Pin | DISP_MISO_Pin | DISP_MOSI_Pin);

        /* SPI2 DMA DeInit */
        HAL_DMA_DeInit(spiHandle->hdmatx);

        /* SPI2 interrupt Deinit */
        HAL_NVIC_DisableIRQ(SPI2_IRQn);
        /* USER CODE BEGIN SPI2_MspDeInit 1 */

        /* USER CODE END SPI2_MspDeInit 1 */
    } else if (spiHandle->Instance == SPI3) {
        /* USER CODE BEGIN SPI3_MspDeInit 0 */

        /* USER CODE END SPI3_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_SPI3_CLK_DISABLE();

        /**SPI3 GPIO Configuration
        PC10     ------> SPI3_SCK
        PC11     ------> SPI3_MISO
        PC12     ------> SPI3_MOSI
        */
        HAL_GPIO_DeInit(GPIOC, ISM_SCK_Pin | ISM_MISO_Pin | ISM_MOSI_Pin);

        /* SPI3 DMA DeInit */
        HAL_DMA_DeInit(spiHandle->hdmatx);
        HAL_DMA_DeInit(spiHandle->hdmarx);

        /* SPI3 interrupt Deinit */
        HAL_NVIC_DisableIRQ(SPI3_IRQn);
        /* USER CODE BEGIN SPI3_MspDeInit 1 */

        /* USER CODE END SPI3_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
