#ifndef _INDICATION_H_
#define _INDICATION_H_

// Data type for validating indication
typedef enum {
    INDICATION_S_OK,
    INDICATION_S_ERROR
} indication_state_t;

/**
 * @brief Waiting for indication 5% flag
 *
 * @return void
 */
void INDICATION_FivePercFlagWait();

/**
 * @brief Initialize indication
 *
 * @return indication_state_t
 */
indication_state_t INDICATION_Init();

#endif /* _INDICATION_H_ */