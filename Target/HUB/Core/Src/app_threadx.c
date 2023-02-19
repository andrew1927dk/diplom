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
#include "boot_reason.h"

// temporary solution (until writing a common API for both targets)
#include "HUBpower.h"

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
static TX_THREAD          INIT_InitThread;
extern IWDG_HandleTypeDef hiwdg;

static uint8_t general_outputInitStatus = 0;
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
    LOG_INFO("----INITIALIZATION ENDED----");

    while (1) {
        HAL_IWDG_Refresh(&hiwdg);
        tx_thread_sleep(500);
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
    (void) byte_pool;
    /* USER CODE END App_ThreadX_MEM_POOL */

    /* USER CODE BEGIN App_ThreadX_Init */
    tx_byte_allocate(byte_pool, (void **) &INIT_InitThreadStackPointer, INIT_THREAD_STACK_SIZE, TX_NO_WAIT);
    tx_thread_create(&INIT_InitThread, "Init Thread", INIT_StartInitThread, (ULONG) byte_pool,
                     INIT_InitThreadStackPointer, INIT_THREAD_STACK_SIZE, 0, 0, TX_NO_TIME_SLICE, TX_AUTO_START);

// temporary solution (until writing a common API for both targets)
#ifdef TARGET_HUB
    POWER_Init(memory_ptr);
#endif

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
}

static void GENERAL_BootReason() {
    uint32_t resStatus = GENERAL_GetResetStatus();

    if (resStatus & RESET_S_LOW_POWER) {
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
