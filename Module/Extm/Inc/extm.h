/**
 * @file extm.h
 * @brief
 * @version 0.1
 * @date 2022-11-30
 *
 *  (c) 2022
 */

#ifndef EXTM_H
#define EXTM_H

#define countof(_a) (sizeof(_a) / sizeof(*(_a)))

#ifndef clz
#define clz __builtin_clz
#endif    // clz

#define EXT_GET_REG_BYTE(_v, _n) (((_v) >> (_n << 3)) & 0xFFU)

#if defined(__BYTE_ORDER__)
#if (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
#define EXT_GET_BYTE(_v, _n) (((uint8_t *) &(_v))[(_n) ^ (sizeof(_v) - 1)])
#elif (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
#define EXT_GET_BYTE(_v, _n) (((uint8_t *) &(_v))[_n])
#else
#error "not suported byte order"
#endif
#else
#define EXT_GET_BYTE EXT_GET_REG_BYTE
#endif

/* minimal bit mask to cover _v */
#define EXT_VALUE_MASK(_v) ((1U << (32 - clz(_v))) - 1)

/* _v = variable, _n = bit position */
#define BIT_CHECK(_v, _n)  (((_v) >> _n) & 1)
#define BIT_SET(_v, _n)    ((_v) |= (1LLU << (_n)))
#define BIT_RESET(_v, _n)  ((_v) &= ~(1LLU << (_n)))
#define BIT_TOGGLE(_v, _n) ((_v) ^= (1LLU << (_n)))

/* _v = variable, _m = mask without side effects */
#define FLAGS_CHECK(_v, _m) ((_v) & (_m) == (_m))

#if 0
#define FLAGS_SET(_v, _m)    ((_v) |= (_m))
#define FLAGS_RESET(_v, _m)  ((_v) &= ~(_m))
#define FLAGS_TOGGLE(_v, _m) ((_v) ^= (_m))
#endif

/* no way of just including main.h and letting CubeMX do all includes stuff ... */
#if defined(STM32F413xx)
#define EXT_device_header   "stm32f4xx.h"
#define EXT_core_header     "core_cm4.h"
#define EXT_ll_utils_header "stm32f4xx_ll_utils.h"
#elif defined(STM32U585xx)
#define EXT_device_header   "stm32u5xx_hal.h"
#define EXT_core_header     "core_cm33.h"
#define EXT_ll_utils_header "stm32u5xx_ll_utils.h"
#elif
#error "Current MCU target not supported (see `extm.h' file)"
#endif

#ifdef EXT_DEBUG
#include EXT_core_header
#include "loglib.h"

#define DWT_CYCCNT         DWT->CYCCNT
#define EXT_DLOG(fmt...)   LOG_DEBUG(fmt)
#define DWT_CYCCNT_SET(_v) (DWT->CYCCNT = (_v))
#define EXT_DINIT()                                     \
    do {                                                \
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; \
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;            \
    } while (0)
#else
#define EXT_DLOG(fmt...)
#define DWT_CYCCNT_SET(_v)
#define EXT_DINIT()
#endif    // EXT_DEBUG

/* in most use cases of next macro `tx_api.h' already included */
#if 0
#include "tx_api.h"
#endif    // 0
#define EXT_MS_TO_TICKS(_ms) ((uint32_t) ((_ms) *TX_TIMER_TICKS_PER_SECOND / 1000))

#endif    // EXTM_H
