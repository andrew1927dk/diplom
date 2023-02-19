/**
 * @file boot_reason.h
 * @brief boot reason header
 * @date 2023-01-16
 *
 *   (c) 2023
 */
#ifndef BOOT_REASON_H_
#define BOOT_REASON_H_

#include "main.h"

#define RESET_S_LOW_POWER            (RCC_CSR_LPWRRSTF)
#define RESET_S_WINDOW_WATCHDOG      (RCC_CSR_WWDGRSTF)
#define RESET_S_INDEPENDENT_WATCHDOG (RCC_CSR_IWDGRSTF)
#define RESET_S_SOFTWARE             (RCC_CSR_SFTRSTF)
#define RESET_S_POWER                (RCC_CSR_BORRSTF)
#define RESET_S_EXTERNAL             (RCC_CSR_PINRSTF)

#ifdef TARGET_DEVBOARD
#define RESET_S_OPTION_BYTE_LOADER (RCC_CSR_OBLRSTF)
#endif

/**
 * @brief Get reset status of microcontroller
 * @attention First time it will read data from register for the next it just return previous value
 * @return Reset status
 */
uint32_t GENERAL_GetResetStatus();

#endif    // BOOT_REASON_H_
