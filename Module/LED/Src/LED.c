/**
 ******************************************************************************
 * @file           : LED.c
 * @date           : 2022-09-16
 * @brief          : This file contains implementation of indication API
 ******************************************************************************
 */
#include "LED.h"
#include <malloc.h>
#include <string.h>
#include "tim.h"
#include "tx_api.h"

#ifdef TARGET_HUB
// #include "indication.h"    // Battery indication will be activated after transfer to ThreadX
#endif

#define INDIC_MAIN_STACK_SIZE  (512)
#define INDIC_EVENT_STACK_SIZE (512)

// Max queue size should be 10 + extra slots for tasks with higher priority, so it will change in future
#define MAX_QUEUE_PRIORITY_SIZE (11)
#define CLEAR_EVENT_FLAG        (0x00)
#define QUEUE_IS_NOT_EMPTY_FLAG (0x01)
#define INTERRUPTION_EVENT_FLAG (0x02)

static TX_THREAD            indicationTaskHandle;                // Definitions for main indication thread
static TX_THREAD            interruptionEventGeneratorHandle;    // Definitions for thread that will generate events
static TX_MUTEX             indicationMutexHandle;               // Definitions for LED mutex
static TX_EVENT_FLAGS_GROUP indicationEventHandle;               // Definitions for LED queue event

static uint8_t  isExit = 0;           // Flag for task with higher priority than we have right now
static uint8_t  currLen = 0;          // Length of priority queue
static uint8_t  currPriority = 10;    // Priority of current task. start value should be higher than max priority
static uint16_t delay = 10;           // Delay time(ms) for event generator thread

struct queue_struct_t {
    struct queue_struct_t *next;

    uint8_t           priority;
    uint8_t           arraySize;
    struct LED_task_t indicationArray[];    // Flexible array member
};

static struct queue_struct_t *queueStart = NULL;
static struct queue_struct_t *queueEnd = NULL;

static void addElem(struct queue_struct_t *element) {    // Add element to queue
    ++currLen;
    element->next = NULL;

    if (queueStart == NULL) {    // If queue is empty
        queueStart = element;
        queueEnd = element;
        return;
    }

    if (element->priority > queueStart->priority) {
        element->next = queueStart;
        queueStart = element;
        return;
    }

    struct queue_struct_t *prev = queueStart;
    struct queue_struct_t *curr = queueStart->next;

    while (curr != NULL) {    // Looking for place to put our element
        if (element->priority > curr->priority) {
            element->next = curr;
            prev->next = element;
            return;
        }
        prev = curr;
        curr = curr->next;
    }

    queueEnd->next = element;
    queueEnd = element;
}

static void delElem(struct queue_struct_t *element) {
    struct queue_struct_t *curr = queueStart;
    struct queue_struct_t *prev = queueStart;

    while (curr != NULL) {    // Searching for element we need to delete
        if (curr == element) {
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    if (curr == NULL) {    // If element is missing just return
        return;
    }

    --currLen;
    if (curr != queueStart && curr != queueEnd) {
        prev->next = curr->next;
        curr->next = NULL;

        free(element);
        return;
    }

    if (curr == queueStart) {
        queueStart = queueStart->next;
        prev = NULL;
    }
    if (curr == queueEnd) {
        queueEnd = prev;

        if (queueEnd) {
            queueEnd->next = NULL;
        }
    }

    free(element);
}

static void set_RGB(const uint16_t *RGB) {
    __HAL_TIM_SET_COMPARE(&LED_R_TIM, LED_R_CH, RGB[0]);
    __HAL_TIM_SET_COMPARE(&LED_G_TIM, LED_G_CH, RGB[1]);
    __HAL_TIM_SET_COMPARE(&LED_B_TIM, LED_B_CH, RGB[2]);
}

static void checkValues(uint8_t *beg_br_, uint8_t *end_br_) {
    if (*beg_br_ > 100) {
        *beg_br_ = 100;
    }
    if (*end_br_ > 100) {
        *end_br_ = 100;
    }
}

void LED_FillElement(struct LED_task_t *array, uint8_t elementIndex, LED_RGB_t RGB, uint8_t br_beg, uint8_t br_end,
                     uint16_t dur_on, uint16_t dur_off, uint8_t switches) {
    memcpy(&array[elementIndex].RGB, &RGB, sizeof(LED_RGB_t));

    array[elementIndex].br_beg = br_beg;
    array[elementIndex].br_end = br_end;
    array[elementIndex].dur_on = dur_on;
    array[elementIndex].dur_off = dur_off;
    array[elementIndex].switches = switches;
}

void LED_TurnOff(void *indicQueueElem) {
    struct queue_struct_t *element = (struct queue_struct_t *) indicQueueElem;

    tx_mutex_get(&indicationMutexHandle, TX_WAIT_FOREVER);
    delElem(element);
    indicQueueElem = NULL;
    tx_mutex_put(&indicationMutexHandle);

    set_RGB((uint16_t[]){ 0, 0, 0 });
    isExit = 1;
}

void startEventGeneratorTask(ULONG argument) {
    for (;;) {
        tx_thread_sleep(delay);
        tx_event_flags_set(&indicationEventHandle, INTERRUPTION_EVENT_FLAG, TX_OR);
        if (currLen) {    // If queue is empty then we do nothing
            tx_event_flags_set(&indicationEventHandle, QUEUE_IS_NOT_EMPTY_FLAG, TX_OR);
        }
    }
}

void startIndicationTask(ULONG argument) {
    struct LED_task_t      param;
    struct queue_struct_t *element = NULL;
    uint8_t                arraySize = 0;

    int16_t  input_RGB[3] = { 0 };
    uint16_t start_RGB[3] = { 0 };
    uint16_t end_RGB[3] = { 0 };
    uint16_t tmpValue[3] = { 0 };

    ULONG actualFlags = 0;

    for (;;) {
        // Wait for interruption
        tx_event_flags_get(&indicationEventHandle, QUEUE_IS_NOT_EMPTY_FLAG, TX_OR_CLEAR, &actualFlags, TX_WAIT_FOREVER);

        tx_mutex_get(&indicationMutexHandle, TX_WAIT_FOREVER);
        element = queueStart;
        currPriority = element->priority;
        arraySize = element->arraySize;
        tx_mutex_put(&indicationMutexHandle);

        for (uint8_t count = 0; count < arraySize; ++count) {

            tx_mutex_get(&indicationMutexHandle, TX_WAIT_FOREVER);
            memcpy(&param, &element->indicationArray[count], sizeof(struct LED_task_t));
            tx_mutex_put(&indicationMutexHandle);

            checkValues(&param.br_beg, &param.br_end);
            if (param.dur_on == 0) {
                continue;
            }

            start_RGB[0] = param.RGB.R;
            start_RGB[1] = param.RGB.G;
            start_RGB[2] = param.RGB.B;

            for (uint8_t _switches = 0; _switches < param.switches; ++_switches) {
                for (uint8_t i = 0; i < 3; ++i) {    // Remember start and final brightness but with increased accuracy
                    input_RGB[i] = (int16_t) (start_RGB[i] * param.br_beg);
                    end_RGB[i] = start_RGB[i] * param.br_end;
                }

                // Return colors to normal accuracy, so we can set them [6555/100 = 65.55 = 65]
                for (uint8_t i = 0; i < 3; ++i) {
                    tmpValue[i] = input_RGB[i] / 100;
                }

                if (isExit) {
                    break;
                }
                set_RGB(tmpValue);    // Set start brightness

                int16_t  percentsDiff = (int16_t) (param.br_end - param.br_beg);
                uint8_t  percentStep = 1;    // Base percent step
                uint16_t steps = 1;          // Number of steps for brightness change
                int8_t   direction = 1;      // Direction of changing. Can be 1 for ascending and -1 for descending

                if (percentsDiff < 0) {
                    direction = -1;
                    percentsDiff = (int16_t) -percentsDiff;
                    // [100-60] -- percent difference is 40
                    // [60-100] -- percent difference is also 40
                }

                // Set number of steps based on percent diff or duration of time
                // And change interruption time if needed
                if ((percentsDiff < param.dur_on) && (percentsDiff != 0)) {
                    steps = percentsDiff;
                    delay *= param.dur_on / percentsDiff;
                } else {
                    steps = param.dur_on;
                    percentStep = direction * percentsDiff / param.dur_on;
                }

                for (uint16_t i = 0; i < steps; ++i) {
                    // Wait for interruption
                    tx_event_flags_get(&indicationEventHandle, INTERRUPTION_EVENT_FLAG, TX_OR_CLEAR, &actualFlags,
                                       TX_WAIT_FOREVER);

                    // Should abort current process if we receive a high priority task or turn off event
                    if (isExit) {
                        break;
                    }

                    for (uint8_t j = 0; j < 3; ++j) {
                        // If original color is 0 or current color is already equal to end color then we should skip it
                        if (start_RGB[j] == 0 || (input_RGB[j] == end_RGB[j])) {
                            continue;
                        }

                        input_RGB[j] += direction * (start_RGB[j] * (100 + percentStep)) / 100;

                        if (input_RGB[j] < 0) {    // Value sometimes can be lover then 0, so we correct it
                            input_RGB[j] = 0;
                        }
                    }

                    for (uint8_t j = 0; j < 3; ++j) {
                        tmpValue[j] = input_RGB[j] / 100;
                        // If one of colors has 0 brightness and it`s not the start value, then fill all colors with 0
                        if ((tmpValue[j] == 0) && (start_RGB[j] != 0)) {
                            memset(&tmpValue, 0, sizeof(tmpValue));
                            break;
                        }
                    }
                    set_RGB(tmpValue);
                }

                // Should abort current process if we receive a high priority task or turn off event
                if (isExit) {
                    break;
                }

                for (uint8_t i = 0; i < 3; ++i) {
                    end_RGB[i] /= 100;
                }
                set_RGB(end_RGB);

                delay = 10;

                // Duration off part
                if (param.dur_off == 0) {
                    continue;
                }

                set_RGB((uint16_t[]){ 0, 0, 0 });
                steps = param.dur_off;

                for (uint16_t i = 0; i < steps; ++i) {
                    // Wait for interruption
                    tx_event_flags_get(&indicationEventHandle, INTERRUPTION_EVENT_FLAG, TX_OR_CLEAR, &actualFlags,
                                       TX_WAIT_FOREVER);

                    if (isExit) {
                        break;
                    }
                }
            }

            // Should abort current process if we receive a high priority task or turn off event
            if (isExit) {
                isExit = 0;
                delay = 10;
                break;
            }
        }

        tx_thread_sleep(1);
        tx_event_flags_set(&indicationEventHandle, CLEAR_EVENT_FLAG, TX_AND);
    }
}

led_state_t LED_Init(void *memoryPoolPtr) {
    TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL *) memoryPoolPtr;
    CHAR         *pointer = NULL;

    HAL_TIM_PWM_Start(&LED_R_TIM, LED_R_CH);    // Red pin
    HAL_TIM_PWM_Start(&LED_G_TIM, LED_G_CH);    // Green pin
    HAL_TIM_PWM_Start(&LED_B_TIM, LED_B_CH);    // Blue pin

    if (tx_mutex_create(&indicationMutexHandle, "Indication Mutex", TX_NO_INHERIT) != TX_SUCCESS) {
        return LED_S_ERROR;
    }
    if (tx_event_flags_create(&indicationEventHandle, "IndicationEvent") != TX_SUCCESS) {
        return LED_S_ERROR;
    }

    // Allocate memory in byte pool for main indication task
    if (tx_byte_allocate(byte_pool, (void **) &pointer, INDIC_MAIN_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS) {
        return LED_S_ERROR;
    }
    if (tx_thread_create(&indicationTaskHandle, "IndicationTask", startIndicationTask, 0, pointer,
                         INDIC_MAIN_STACK_SIZE, 7, 7, TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS) {
        return LED_S_ERROR;
    }

    // Allocate memory in byte pool for  indication event generator task
    if (tx_byte_allocate(byte_pool, (void **) &pointer, INDIC_EVENT_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS) {
        return LED_S_ERROR;
    }
    if ((tx_thread_create(&interruptionEventGeneratorHandle, "IndicationEventGeneratorTask", startEventGeneratorTask, 0,
                          pointer, INDIC_EVENT_STACK_SIZE, 7, 7, TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)) {
        return LED_S_ERROR;
    }

#ifndef TARGET_HUB
// Battery indication will be activated after transfer to ThreadX
//    if (INDICATION_Init() != INDICATION_S_OK) {
//        return LED_S_ERROR;
//    }
#endif

    return LED_S_OK;
}

void LED_Deinit() {
    HAL_TIM_PWM_Stop(&LED_R_TIM, LED_R_CH);
    HAL_TIM_PWM_Stop(&LED_G_TIM, LED_G_CH);
    HAL_TIM_PWM_Stop(&LED_B_TIM, LED_B_CH);

    tx_thread_terminate(&indicationTaskHandle);
    tx_thread_terminate(&interruptionEventGeneratorHandle);
    tx_thread_delete(&indicationTaskHandle);
    tx_thread_delete(&interruptionEventGeneratorHandle);

    tx_mutex_delete(&indicationMutexHandle);
    tx_event_flags_delete(&indicationEventHandle);
}

void *LED_TurnOn(const struct LED_task_t *indicArray, uint8_t arraySize, uint8_t priority) {
    // Allocate memory for queue element with flexible array member
    struct queue_struct_t *element =
        (struct queue_struct_t *) malloc(sizeof(struct queue_struct_t) + arraySize * sizeof(struct LED_task_t));

    element->priority = priority;
    element->arraySize = arraySize;

    if (indicArray == NULL) {
        return NULL;
    }
    memcpy(element->indicationArray, indicArray, arraySize * sizeof(struct LED_task_t));

    uint8_t isHigher = 0;
    if (currLen == 0) {
        currPriority = 10;
    }
    if (element->priority > currPriority) {
        isHigher = 1;
    }

    if (currLen < MAX_QUEUE_PRIORITY_SIZE - 1 || isHigher) {
        tx_mutex_get(&indicationMutexHandle, TX_WAIT_FOREVER);
        addElem(element);
        tx_mutex_put(&indicationMutexHandle);

        if (isHigher) {
            isExit = 1;
        }
        return (void *) element;
    }

    return NULL;    // Return NULL if queue is full
}
