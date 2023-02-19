/**
 * @file buzzer.c
 * @brief Audio Primitives Driver
 * @version Alpha 1.0
 * @date 2023-1-11
 *
 *  (c) 2023
 */

#include "main.h"
#include "tim.h"
#include "buzzer.h"
#include "tx_api.h"

#define BUZZER_TIM htim2
#define CLOCK_FREQ 160000000U

#define WAV_DESCRIPTOR_SIZE        44
#define WAV_SAMPLE_RATE_INFO_SHIFT 24

#define BIT_DEPTH           256U
#define DEFAULT_SAMPLE_SIZE 256U
#define SAMPLING_FREQ       125000U
#define SAMPLING_THRESHOLD  1000U

#define FLAG_RELEASE 0x00000001U
static ULONG                BUZZER_actual_flags;
static TX_EVENT_FLAGS_GROUP BUZZER_evf;
static uint8_t              BUZZER_initState = 0;

typedef struct {
    buzzer_signal_t sigType;
    uint8_t         volume;
    uint32_t        sampleDelta;
    uint32_t        soundTime;
    uint32_t        CNT;
    uint32_t        indx;
    const uint8_t  *sigArr;
    uint32_t        sigArrSize;
} signal_t;
static signal_t BUZZER_sig;

uint8_t SINE_WAVE[DEFAULT_SAMPLE_SIZE] = {
    0x80, 0x83, 0x86, 0x89, 0x8c, 0x8f, 0x92, 0x95, 0x98, 0x9c, 0x9f, 0xa2, 0xa5, 0xa8, 0xab, 0xae, 0xb0, 0xb3, 0xb6,
    0xb9, 0xbc, 0xbf, 0xc1, 0xc4, 0xc7, 0xc9, 0xcc, 0xce, 0xd1, 0xd3, 0xd5, 0xd8, 0xda, 0xdc, 0xde, 0xe0, 0xe2, 0xe4,
    0xe6, 0xe8, 0xea, 0xec, 0xed, 0xef, 0xf0, 0xf2, 0xf3, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfc, 0xfd,
    0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfd, 0xfc, 0xfc, 0xfb,
    0xfa, 0xf9, 0xf8, 0xf7, 0xf6, 0xf5, 0xf3, 0xf2, 0xf0, 0xef, 0xed, 0xec, 0xea, 0xe8, 0xe6, 0xe4, 0xe2, 0xe0, 0xde,
    0xdc, 0xda, 0xd8, 0xd5, 0xd3, 0xd1, 0xce, 0xcc, 0xc9, 0xc7, 0xc4, 0xc1, 0xbf, 0xbc, 0xb9, 0xb6, 0xb3, 0xb0, 0xae,
    0xab, 0xa8, 0xa5, 0xa2, 0x9f, 0x9c, 0x98, 0x95, 0x92, 0x8f, 0x8c, 0x89, 0x86, 0x83, 0x80, 0x7c, 0x79, 0x76, 0x73,
    0x70, 0x6d, 0x6a, 0x67, 0x63, 0x60, 0x5d, 0x5a, 0x57, 0x54, 0x51, 0x4f, 0x4c, 0x49, 0x46, 0x43, 0x40, 0x3e, 0x3b,
    0x38, 0x36, 0x33, 0x31, 0x2e, 0x2c, 0x2a, 0x27, 0x25, 0x23, 0x21, 0x1f, 0x1d, 0x1b, 0x19, 0x17, 0x15, 0x13, 0x12,
    0x10, 0x0f, 0x0d, 0x0c, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x03, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x02, 0x03, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
    0x0a, 0x0c, 0x0d, 0x0f, 0x10, 0x12, 0x13, 0x15, 0x17, 0x19, 0x1b, 0x1d, 0x1f, 0x21, 0x23, 0x25, 0x27, 0x2a, 0x2c,
    0x2e, 0x31, 0x33, 0x36, 0x38, 0x3b, 0x3e, 0x40, 0x43, 0x46, 0x49, 0x4c, 0x4f, 0x51, 0x54, 0x57, 0x5a, 0x5d, 0x60,
    0x63, 0x67, 0x6a, 0x6d, 0x70, 0x73, 0x76, 0x79, 0x7c
};

static void setVolume(uint8_t volume) {
    __HAL_TIM_SET_COMPARE(&BUZZER_TIM, TIM_CHANNEL_1, __HAL_TIM_GET_AUTORELOAD(&BUZZER_TIM) * volume / UINT8_MAX);
}

static void startPWM(float freq, uint8_t volume) {
    __HAL_TIM_SET_AUTORELOAD(&BUZZER_TIM, CLOCK_FREQ / (BUZZER_TIM.Instance->PSC + 1) / freq - 1);
    setVolume(volume);
    HAL_TIM_PWM_Start_IT(&BUZZER_TIM, TIM_CHANNEL_1);
}

buzzer_state_t BUZZER_Sound(buzzer_signal_t signal, const uint8_t *sigArr, uint32_t sigArrSize, float freq,
                            uint8_t volume, uint32_t time, uint32_t mode) {
    if (BUZZER_initState != 1) {
        return BUZZER_ERR;
    }

    while (tx_event_flags_get(&BUZZER_evf, FLAG_RELEASE, TX_OR_CLEAR, &BUZZER_actual_flags, mode) == TX_NO_EVENTS) {
        BUZZER_Off();
    }

    if ((freq < 0.01 || freq > SAMPLING_FREQ)) {
        tx_event_flags_set(&BUZZER_evf, FLAG_RELEASE, TX_OR);
        return BUZZER_ERR;
    }

    BUZZER_sig.sigType = signal;
    if (signal == BUZZER_MEANDER) {
        BUZZER_sig.volume = volume / 2;
        BUZZER_sig.soundTime = time * freq / SAMPLING_THRESHOLD;
        startPWM(freq, BUZZER_sig.volume);
    } else {
        if (freq > SAMPLING_FREQ / 4) {
            tx_event_flags_set(&BUZZER_evf, FLAG_RELEASE, TX_OR);
            return BUZZER_ERR;
        }
        BUZZER_sig.volume = volume;
        BUZZER_sig.soundTime = time * (SAMPLING_FREQ / SAMPLING_THRESHOLD);
        if (signal == BUZZER_WAV) {
            BUZZER_sig.sigArr = sigArr;
            BUZZER_sig.indx = WAV_DESCRIPTOR_SIZE;
            BUZZER_sig.sigArrSize = sigArrSize - WAV_DESCRIPTOR_SIZE;
            BUZZER_sig.sampleDelta =
                (*(uint32_t *) (sigArr + WAV_SAMPLE_RATE_INFO_SHIFT) * SAMPLING_THRESHOLD) / (SAMPLING_FREQ / freq);
        } else {
            BUZZER_sig.sigArrSize = DEFAULT_SAMPLE_SIZE;
            BUZZER_sig.sampleDelta = (DEFAULT_SAMPLE_SIZE * SAMPLING_THRESHOLD) / (SAMPLING_FREQ / freq);
        }
        startPWM(SAMPLING_FREQ, 0);
    }

    return BUZZER_OK;
}

buzzer_state_t BUZZER_Init() {
    if (BUZZER_initState == 1) {
        return BUZZER_ERR;
    }

    if (tx_event_flags_create(&BUZZER_evf, "BUZZER event flags") != TX_SUCCESS) {
        return BUZZER_ERR;
    }
    tx_event_flags_set(&BUZZER_evf, FLAG_RELEASE, TX_OR);

    BUZZER_initState = 1;
    return BUZZER_OK;
}

void BUZZER_Off() {
    HAL_TIM_PWM_Stop(&BUZZER_TIM, TIM_CHANNEL_1);
    memset(&BUZZER_sig, 0, sizeof(signal_t));
    tx_event_flags_set(&BUZZER_evf, FLAG_RELEASE, TX_OR);
}

void BUZZER_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim) {
    BUZZER_sig.CNT += BUZZER_sig.sampleDelta;
    --BUZZER_sig.soundTime;
    if (BUZZER_sig.soundTime == 0) {
        BUZZER_Off();
        return;
    }
    if (BUZZER_sig.CNT < SAMPLING_THRESHOLD) {
        return;
    }
    BUZZER_sig.indx = (BUZZER_sig.indx + BUZZER_sig.CNT / SAMPLING_THRESHOLD) % BUZZER_sig.sigArrSize;
    BUZZER_sig.CNT %= SAMPLING_THRESHOLD;
    switch (BUZZER_sig.sigType) {
        case BUZZER_SINE:
            setVolume(SINE_WAVE[BUZZER_sig.indx] * BUZZER_sig.volume / BIT_DEPTH);
            break;

        case BUZZER_TRIANGLE:
            setVolume((BUZZER_sig.indx < DEFAULT_SAMPLE_SIZE / 2) ? (BUZZER_sig.indx * 2) :
                                                                    ((DEFAULT_SAMPLE_SIZE - 1 - BUZZER_sig.indx) * 2));
            break;

        case BUZZER_SAWTOOTH:
            setVolume(BUZZER_sig.indx * BUZZER_sig.volume / BIT_DEPTH);
            break;

        case BUZZER_REV_SAWTOOTH:
            setVolume((DEFAULT_SAMPLE_SIZE - BUZZER_sig.indx) * BUZZER_sig.volume / BIT_DEPTH);
            break;

        case BUZZER_WAV:
            setVolume(BUZZER_sig.sigArr[BUZZER_sig.indx + WAV_DESCRIPTOR_SIZE] * BUZZER_sig.volume / BIT_DEPTH);
            break;

        default:
            break;
    }
}