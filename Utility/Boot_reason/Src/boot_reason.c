/**
 * @file boot_reason.c
 * @brief boot reason realization
 * @date 2023-01-16
 *
 *   (c) 2023
 */
#include "boot_reason.h"

uint32_t GENERAL_GetResetStatus() {
    static uint32_t resetReg = 0;
    static uint8_t  isInit = 0;

    if (!isInit) {
        resetReg = RCC->CSR;
        __HAL_RCC_CLEAR_RESET_FLAGS();

        isInit = 1;
    }

    return resetReg;
}