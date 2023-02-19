/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    app_threadx.c
 * @brief   ThreadX applicative file
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
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "wizchip.h"
#include "iwdg.h"
#include "usart.h"
#include "ethernet_adapter.h"
#include "wiznet_w5500.h"
#include "main.h"
#include "loglib.h"
#include "parser.h"
#include "LED.h"
#include "flash.h"
#include "platform_i2c.h"
#include "buzzer.h"
#include "bmp390.h"
#include "boot_reason.h"
#include "lis3dh.h"
#include "adc.h"
#include "iwdg.h"
#include "sht40.h"
#include "display.h"
#include "Touch.h"
#include "rfm66a.h"
#include "stm32u5xx_ll_utils.h"
#include "esp32.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define INIT_THREAD_STACK_SIZE 1024

#define LOG_INIT_OK (1 << 0)
#define DIS_INIT_OK (1 << 1)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
i2c_bus_t            i2c;
TX_MUTEX             i2c_mux;
TX_EVENT_FLAGS_GROUP i2c_ef;
static TX_THREAD     INIT_InitThread;
static TX_THREAD     IWDG_Thread;
static uint8_t       general_outputInitStatus;

/**
 * @brief Array for storing ADC values between 0 and (16 * (2^14-1)). Just read it
 * @param sensors_adcData[0] - Left ADC
 * @param sensors_adcData[1] - Right ADC
 */
static uint32_t  sensors_adcData[2] = { 0 };
static uint8_t   general_outputInitStatus = 0;
static TX_THREAD INIT_InitThread;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static void GENERAL_OutputMessage(char *text, log_type_t logType, log_module_t logModule);
static void GENERAL_BootReason();

void INIT_StartInitThread(ULONG arg) {
    TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL *) arg;

    if (LOG_Init(byte_pool) == LOG_S_OK) {
        general_outputInitStatus |= LOG_INIT_OK;
    }

    GENERAL_BootReason();

    LOG_INFO("---INITIALIZATION STARTED---");

    if (I2C_Init(&i2c, &I2C, &i2c_mux, &i2c_ef) != I2C_OK) {
        GENERAL_OutputMessage("I2C bus not inited", LOG_T_WARN, LOG_M_I2C);
    } else {
        GENERAL_OutputMessage("I2C bus init OK", LOG_T_DEBUG, LOG_M_I2C);
    }

    if (DISPLAY_Init(byte_pool, &i2c) != DISPLAY_OK) {
        GENERAL_OutputMessage("Display not inited", LOG_T_WARN, LOG_M_DISPLAY);
    } else {
        general_outputInitStatus |= DIS_INIT_OK;
        GENERAL_OutputMessage("Display inited", LOG_T_DEBUG, LOG_M_DISPLAY);
    }

    if (LED_Init(byte_pool) != LED_S_OK) {
        GENERAL_OutputMessage("LED not inited", LOG_T_WARN, LOG_M_INDICATION);
    } else {
        GENERAL_OutputMessage("LED init OK", LOG_T_DEBUG, LOG_M_INDICATION);
    }

    if (ETH_AdapterInit(byte_pool) != WEB_STATUS_OK) {
        GENERAL_OutputMessage("W5500 not inited", LOG_T_WARN, LOG_M_W5500);
    } else {
        GENERAL_OutputMessage("W5500 init OK", LOG_T_DEBUG, LOG_M_W5500);
    }

    if (BARO_Init(&i2c) != BARO_S_OK) {
        GENERAL_OutputMessage("BMP390 not inited", LOG_T_WARN, LOG_M_BMP390);
    } else {
        GENERAL_OutputMessage("BMP390 init OK", LOG_T_DEBUG, LOG_M_BMP390);
    }

    if (ESP32_Init(byte_pool) != ESP32_S_OK) {
        GENERAL_OutputMessage("ESP32 not inited", LOG_T_WARN, LOG_M_ESP32);
    } else {
        GENERAL_OutputMessage("ESP32 init OK", LOG_T_DEBUG, LOG_M_ESP32);
    }

    if (LIS3DH_Init(&i2c) != LIS3DH_S_OK) {
        GENERAL_OutputMessage("LIS3DH not inited", LOG_T_WARN, LOG_M_LIS3DH);
    } else {
        GENERAL_OutputMessage("LIS3DH init OK", LOG_T_DEBUG, LOG_M_LIS3DH);
    }

    if (BUZZER_Init() != BUZZER_OK) {
        GENERAL_OutputMessage("Buzzer not inited", LOG_T_WARN, LOG_M_BUZZER);
    } else {
        GENERAL_OutputMessage("Buzzer init OK", LOG_T_DEBUG, LOG_M_BUZZER);
        BUZZER_SOUND_SINE(3500, 10, 80, BUZZER_WITH_DISPLACEMENT);
    }

    if (HUMID_Init() != HUMID_S_OK) {
        GENERAL_OutputMessage("SHT40 not inited", LOG_T_WARN, LOG_M_SHT40);
    } else {
        GENERAL_OutputMessage("SHT40 init OK", LOG_T_DEBUG, LOG_M_SHT40);
    }

    if (RFM_Init() != RFM_OK) {
        GENERAL_OutputMessage("RFM not inited", LOG_T_WARN, LOG_M_RFM);
    } else {
        GENERAL_OutputMessage("RFM init OK", LOG_T_DEBUG, LOG_M_RFM);
    }

    if (FLASH_Init() != FLASH_OK) {
        GENERAL_OutputMessage("FLASH not inited", LOG_T_WARN, LOG_M_FLASH);
    } else {
        GENERAL_OutputMessage("FLASH init OK", LOG_T_DEBUG, LOG_M_FLASH);
    }
    LOG_INFO("----INITIALIZATION ENDED----");

    HAL_ADC_Start_DMA(&hadc1, sensors_adcData, 2);
}

void IWDG_StartThread(ULONG arg) {
    while (1) {
        HAL_IWDG_Refresh(&hiwdg);
        tx_thread_sleep(250);
    }
}

/* USER CODE END PFP */

/**
 * @brief  Application ThreadX Initialization.
 * @param memory_ptr: memory pointer
 * @retval int
 */
UINT App_ThreadX_Init(VOID *memory_ptr) {
    UINT          ret = TX_SUCCESS;
    TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL *) memory_ptr;

    /* USER CODE BEGIN App_ThreadX_MEM_POOL */
    CHAR *INIT_InitThreadStackPointer;
    CHAR *IWDG_ThreadStackPointer;
    (void) byte_pool;
    /* USER CODE END App_ThreadX_MEM_POOL */

    /* USER CODE BEGIN App_ThreadX_Init */
    tx_byte_allocate(byte_pool, (void **) &IWDG_ThreadStackPointer, 512, TX_NO_WAIT);
    tx_thread_create(&IWDG_Thread, "IWDG Thread", IWDG_StartThread, (ULONG) byte_pool, IWDG_ThreadStackPointer, 512, 0,
                     0, TX_NO_TIME_SLICE, TX_AUTO_START);
    tx_byte_allocate(byte_pool, (void **) &INIT_InitThreadStackPointer, INIT_THREAD_STACK_SIZE, TX_NO_WAIT);
    tx_thread_create(&INIT_InitThread, "Init Thread", INIT_StartInitThread, (ULONG) byte_pool,
                     INIT_InitThreadStackPointer, INIT_THREAD_STACK_SIZE, 0, 0, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* USER CODE END App_ThreadX_Init */

    return ret;
}

/**
 * @brief  MX_ThreadX_Init
 * @param  None
 * @retval None
 */
void MX_ThreadX_Init(void) {
    /* USER CODE BEGIN  Before_Kernel_Start */

    /* USER CODE END  Before_Kernel_Start */

    tx_kernel_enter();

    /* USER CODE BEGIN  Kernel_Start_Error */

    /* USER CODE END  Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */
static void GENERAL_OutputMessage(char *text, log_type_t logType, log_module_t logModule) {
    if (strlen(text) == 0) {
        return;
    }

    if (general_outputInitStatus & LOG_INIT_OK) {
        LOG_Put(logType, logModule, text);
    }
    if (general_outputInitStatus & DIS_INIT_OK) {
#if 0
         DISPLAY_Put(logType, logModule, text);
#endif
    }
}

static void GENERAL_BootReason() {
    uint32_t resStatus = GENERAL_GetResetStatus();

    if (resStatus & RESET_S_OPTION_BYTE_LOADER) {
        GENERAL_OutputMessage("Option byte loader reset", LOG_T_DEBUG, LOB_M_BOOT);
    } else if (resStatus & RESET_S_LOW_POWER) {
        GENERAL_OutputMessage("Low power reset", LOG_T_DEBUG, LOB_M_BOOT);
    } else if (resStatus & RESET_S_POWER) {
        GENERAL_OutputMessage("Power on", LOG_T_DEBUG, LOB_M_BOOT);
    } else if (resStatus & RESET_S_SOFTWARE) {
        GENERAL_OutputMessage("Software reset", LOG_T_DEBUG, LOB_M_BOOT);
    } else if (resStatus & RESET_S_INDEPENDENT_WATCHDOG) {
        GENERAL_OutputMessage("Independent watchdog reset", LOG_T_WARN, LOB_M_BOOT);
    } else if (resStatus & RESET_S_WINDOW_WATCHDOG) {
        GENERAL_OutputMessage("Window watchdog reset", LOG_T_WARN, LOB_M_BOOT);
    } else if (resStatus & RESET_S_EXTERNAL) {
        GENERAL_OutputMessage("Target reset button pressed", LOG_T_DEBUG, LOB_M_BOOT);
    } else {
        GENERAL_OutputMessage("Unknown reason", LOG_T_WARN, LOB_M_BOOT);
    }
}
/* USER CODE END 1 */
