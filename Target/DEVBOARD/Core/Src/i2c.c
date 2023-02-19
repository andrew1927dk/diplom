/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    i2c.c
 * @brief   This file provides code for the configuration
 *          of the I2C instances.
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
#include "i2c.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c3;
DMA_HandleTypeDef handle_GPDMA1_Channel2;
DMA_HandleTypeDef handle_GPDMA1_Channel1;
DMA_HandleTypeDef handle_GPDMA1_Channel4;
DMA_HandleTypeDef handle_GPDMA1_Channel3;

/* I2C1 init function */
void MX_I2C1_Init(void) {

    /* USER CODE BEGIN I2C1_Init 0 */

    /* USER CODE END I2C1_Init 0 */

    /* USER CODE BEGIN I2C1_Init 1 */

    /* USER CODE END I2C1_Init 1 */
    hi2c1.Instance = I2C1;
    hi2c1.Init.Timing = 0x00F07BFF;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Analogue filter
     */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Digital filter
     */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN I2C1_Init 2 */

    /* USER CODE END I2C1_Init 2 */
}
/* I2C3 init function */
void MX_I2C3_Init(void) {

    /* USER CODE BEGIN I2C3_Init 0 */

    /* USER CODE END I2C3_Init 0 */

    /* USER CODE BEGIN I2C3_Init 1 */

    /* USER CODE END I2C3_Init 1 */
    hi2c3.Instance = I2C3;
    hi2c3.Init.Timing = 0x00F07BFF;
    hi2c3.Init.OwnAddress1 = 0;
    hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c3.Init.OwnAddress2 = 0;
    hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c3) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Analogue filter
     */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Digital filter
     */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN I2C3_Init 2 */

    /* USER CODE END I2C3_Init 2 */
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *i2cHandle) {

    GPIO_InitTypeDef         GPIO_InitStruct = { 0 };
    RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };
    if (i2cHandle->Instance == I2C1) {
        /* USER CODE BEGIN I2C1_MspInit 0 */

        /* USER CODE END I2C1_MspInit 0 */

        /** Initializes the peripherals clock
         */
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
        PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
            Error_Handler();
        }

        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**I2C1 GPIO Configuration
        PB6     ------> I2C1_SCL
        PB7     ------> I2C1_SDA
        */
        GPIO_InitStruct.Pin = SCL_Pin | SDA_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        /* I2C1 clock enable */
        __HAL_RCC_I2C1_CLK_ENABLE();

        /* I2C1 DMA Init */
        /* GPDMA1_REQUEST_I2C1_RX Init */
        handle_GPDMA1_Channel2.Instance = GPDMA1_Channel2;
        handle_GPDMA1_Channel2.Init.Request = GPDMA1_REQUEST_I2C1_RX;
        handle_GPDMA1_Channel2.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
        handle_GPDMA1_Channel2.Init.Direction = DMA_PERIPH_TO_MEMORY;
        handle_GPDMA1_Channel2.Init.SrcInc = DMA_SINC_FIXED;
        handle_GPDMA1_Channel2.Init.DestInc = DMA_DINC_INCREMENTED;
        handle_GPDMA1_Channel2.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
        handle_GPDMA1_Channel2.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
        handle_GPDMA1_Channel2.Init.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
        handle_GPDMA1_Channel2.Init.SrcBurstLength = 1;
        handle_GPDMA1_Channel2.Init.DestBurstLength = 1;
        handle_GPDMA1_Channel2.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
        handle_GPDMA1_Channel2.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
        handle_GPDMA1_Channel2.Init.Mode = DMA_NORMAL;
        if (HAL_DMA_Init(&handle_GPDMA1_Channel2) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(i2cHandle, hdmarx, handle_GPDMA1_Channel2);

        if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel2, DMA_CHANNEL_NPRIV) != HAL_OK) {
            Error_Handler();
        }

        /* GPDMA1_REQUEST_I2C1_TX Init */
        handle_GPDMA1_Channel1.Instance = GPDMA1_Channel1;
        handle_GPDMA1_Channel1.Init.Request = GPDMA1_REQUEST_I2C1_TX;
        handle_GPDMA1_Channel1.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
        handle_GPDMA1_Channel1.Init.Direction = DMA_MEMORY_TO_PERIPH;
        handle_GPDMA1_Channel1.Init.SrcInc = DMA_SINC_INCREMENTED;
        handle_GPDMA1_Channel1.Init.DestInc = DMA_DINC_FIXED;
        handle_GPDMA1_Channel1.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
        handle_GPDMA1_Channel1.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
        handle_GPDMA1_Channel1.Init.Priority = DMA_LOW_PRIORITY_MID_WEIGHT;
        handle_GPDMA1_Channel1.Init.SrcBurstLength = 1;
        handle_GPDMA1_Channel1.Init.DestBurstLength = 1;
        handle_GPDMA1_Channel1.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
        handle_GPDMA1_Channel1.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
        handle_GPDMA1_Channel1.Init.Mode = DMA_NORMAL;
        if (HAL_DMA_Init(&handle_GPDMA1_Channel1) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(i2cHandle, hdmatx, handle_GPDMA1_Channel1);

        if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel1, DMA_CHANNEL_NPRIV) != HAL_OK) {
            Error_Handler();
        }

        /* I2C1 interrupt Init */
        HAL_NVIC_SetPriority(I2C1_EV_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
        HAL_NVIC_SetPriority(I2C1_ER_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
        /* USER CODE BEGIN I2C1_MspInit 1 */

        /* USER CODE END I2C1_MspInit 1 */
    } else if (i2cHandle->Instance == I2C3) {
        /* USER CODE BEGIN I2C3_MspInit 0 */

        /* USER CODE END I2C3_MspInit 0 */

        /** Initializes the peripherals clock
         */
        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C3;
        PeriphClkInit.I2c3ClockSelection = RCC_I2C3CLKSOURCE_PCLK3;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
            Error_Handler();
        }

        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**I2C3 GPIO Configuration
        PC0     ------> I2C3_SCL
        PC1     ------> I2C3_SDA
        PB2     ------> I2C3_SMBA
        */
        GPIO_InitStruct.Pin = EXT_SCL_Pin | EXT_SDA_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = EXT_SMBA_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
        HAL_GPIO_Init(EXT_SMBA_GPIO_Port, &GPIO_InitStruct);

        /* I2C3 clock enable */
        __HAL_RCC_I2C3_CLK_ENABLE();

        /* I2C3 DMA Init */
        /* GPDMA1_REQUEST_I2C3_RX Init */
        handle_GPDMA1_Channel4.Instance = GPDMA1_Channel4;
        handle_GPDMA1_Channel4.Init.Request = GPDMA1_REQUEST_I2C3_RX;
        handle_GPDMA1_Channel4.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
        handle_GPDMA1_Channel4.Init.Direction = DMA_PERIPH_TO_MEMORY;
        handle_GPDMA1_Channel4.Init.SrcInc = DMA_SINC_FIXED;
        handle_GPDMA1_Channel4.Init.DestInc = DMA_DINC_INCREMENTED;
        handle_GPDMA1_Channel4.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
        handle_GPDMA1_Channel4.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
        handle_GPDMA1_Channel4.Init.Priority = DMA_LOW_PRIORITY_HIGH_WEIGHT;
        handle_GPDMA1_Channel4.Init.SrcBurstLength = 1;
        handle_GPDMA1_Channel4.Init.DestBurstLength = 1;
        handle_GPDMA1_Channel4.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
        handle_GPDMA1_Channel4.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
        handle_GPDMA1_Channel4.Init.Mode = DMA_NORMAL;
        if (HAL_DMA_Init(&handle_GPDMA1_Channel4) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(i2cHandle, hdmarx, handle_GPDMA1_Channel4);

        if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel4, DMA_CHANNEL_NPRIV) != HAL_OK) {
            Error_Handler();
        }

        /* GPDMA1_REQUEST_I2C3_TX Init */
        handle_GPDMA1_Channel3.Instance = GPDMA1_Channel3;
        handle_GPDMA1_Channel3.Init.Request = GPDMA1_REQUEST_I2C3_TX;
        handle_GPDMA1_Channel3.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
        handle_GPDMA1_Channel3.Init.Direction = DMA_MEMORY_TO_PERIPH;
        handle_GPDMA1_Channel3.Init.SrcInc = DMA_SINC_INCREMENTED;
        handle_GPDMA1_Channel3.Init.DestInc = DMA_DINC_FIXED;
        handle_GPDMA1_Channel3.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
        handle_GPDMA1_Channel3.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
        handle_GPDMA1_Channel3.Init.Priority = DMA_LOW_PRIORITY_MID_WEIGHT;
        handle_GPDMA1_Channel3.Init.SrcBurstLength = 1;
        handle_GPDMA1_Channel3.Init.DestBurstLength = 1;
        handle_GPDMA1_Channel3.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;
        handle_GPDMA1_Channel3.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
        handle_GPDMA1_Channel3.Init.Mode = DMA_NORMAL;
        if (HAL_DMA_Init(&handle_GPDMA1_Channel3) != HAL_OK) {
            Error_Handler();
        }

        __HAL_LINKDMA(i2cHandle, hdmatx, handle_GPDMA1_Channel3);

        if (HAL_DMA_ConfigChannelAttributes(&handle_GPDMA1_Channel3, DMA_CHANNEL_NPRIV) != HAL_OK) {
            Error_Handler();
        }

        /* I2C3 interrupt Init */
        HAL_NVIC_SetPriority(I2C3_EV_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(I2C3_EV_IRQn);
        HAL_NVIC_SetPriority(I2C3_ER_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(I2C3_ER_IRQn);
        /* USER CODE BEGIN I2C3_MspInit 1 */

        /* USER CODE END I2C3_MspInit 1 */
    }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef *i2cHandle) {

    if (i2cHandle->Instance == I2C1) {
        /* USER CODE BEGIN I2C1_MspDeInit 0 */

        /* USER CODE END I2C1_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_I2C1_CLK_DISABLE();

        /**I2C1 GPIO Configuration
        PB6     ------> I2C1_SCL
        PB7     ------> I2C1_SDA
        */
        HAL_GPIO_DeInit(SCL_GPIO_Port, SCL_Pin);

        HAL_GPIO_DeInit(SDA_GPIO_Port, SDA_Pin);

        /* I2C1 DMA DeInit */
        HAL_DMA_DeInit(i2cHandle->hdmarx);
        HAL_DMA_DeInit(i2cHandle->hdmatx);

        /* I2C1 interrupt Deinit */
        HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
        HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
        /* USER CODE BEGIN I2C1_MspDeInit 1 */

        /* USER CODE END I2C1_MspDeInit 1 */
    } else if (i2cHandle->Instance == I2C3) {
        /* USER CODE BEGIN I2C3_MspDeInit 0 */

        /* USER CODE END I2C3_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_I2C3_CLK_DISABLE();

        /**I2C3 GPIO Configuration
        PC0     ------> I2C3_SCL
        PC1     ------> I2C3_SDA
        PB2     ------> I2C3_SMBA
        */
        HAL_GPIO_DeInit(EXT_SCL_GPIO_Port, EXT_SCL_Pin);

        HAL_GPIO_DeInit(EXT_SDA_GPIO_Port, EXT_SDA_Pin);

        HAL_GPIO_DeInit(EXT_SMBA_GPIO_Port, EXT_SMBA_Pin);

        /* I2C3 DMA DeInit */
        HAL_DMA_DeInit(i2cHandle->hdmarx);
        HAL_DMA_DeInit(i2cHandle->hdmatx);

        /* I2C3 interrupt Deinit */
        HAL_NVIC_DisableIRQ(I2C3_EV_IRQn);
        HAL_NVIC_DisableIRQ(I2C3_ER_IRQn);
        /* USER CODE BEGIN I2C3_MspDeInit 1 */

        /* USER CODE END I2C3_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
