/**
 * @file buzzer.h
 * @brief Audio Primitives Driver
 * @version Alpha 1.0
 * @date 2023-1-11
 *
 *  (c) 2023
 */

#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

#define BUZZER_WITH_DISPLACEMENT    TX_NO_WAIT
#define BUZZER_WITHOUT_DISPLACEMENT TX_WAIT_FOREVER

#define BUZZER_NOTE_C  130.82
#define BUZZER_NOTE_Cd 138.59
#define BUZZER_NOTE_D  147.83
#define BUZZER_NOTE_Dd 155.56
#define BUZZER_NOTE_E  164.81
#define BUZZER_NOTE_F  174.62
#define BUZZER_NOTE_Fd 185.0
#define BUZZER_NOTE_G  196.0
#define BUZZER_NOTE_Gd 207.0
#define BUZZER_NOTE_A  220.0
#define BUZZER_NOTE_B  233.08
#define BUZZER_NOTE_H  246.96

#define BUZZER_OCTAVE_BIG    0.5
#define BUZZER_OCTAVE_SMALL  1
#define BUZZER_OCTAVE_FIRST  2
#define BUZZER_OCTAVE_SECOND 4
#define BUZZER_OCTAVE_THIRD  8
#define BUZZER_OCTAVE_FOURTH 16

#define NOTE_TO_HZ(NOTE, OCTAVE) ((NOTE) * (OCTAVE))

typedef enum {
    BUZZER_MEANDER,
    BUZZER_SINE,
    BUZZER_TRIANGLE,
    BUZZER_SAWTOOTH,
    BUZZER_REV_SAWTOOTH,
    BUZZER_WAV
} buzzer_signal_t;

typedef enum {
    BUZZER_OK,
    BUZZER_ERR
} buzzer_state_t;

/** @brief Buzzer initialization
 *  @param None
 *  @return Zero on success, non-zero - on failure
 */
buzzer_state_t BUZZER_Init();

/** @brief Buzzer sound off
 *  @param None
 *  @return void
 */
void BUZZER_Off();

/** @brief Set a voluntary wave primitive
 *  @param signal wave primitive: @arg BUZZER_MEANDER, @arg BUZZER_SINE, @arg BUZZER_TRIANGLE,
 *  @arg BUZZER_SAWTOOTH, @arg BUZZER_REV_SAWTOOTH, @arg BUZZER_REV_SAWTOOTH, @arg BUZZER_CUSTOM
 *  @param sigArr custom sound sample array (WAV-file in HEX)
 *  @param sigArrsSize custom sound sample array size
 *  @param freq wave frequency
 *  @param volume sound volume from 0 to 255
 *  @param time sound length in milliseconds
 *  @param mode displacement mode: @arg BUZZER_WITH_DISPLACEMENT, @arg BUZZER_WITHOUT_DISPLACEMENT
 *  @return Zero on success, non-zero - on failure
 */
buzzer_state_t BUZZER_Sound(buzzer_signal_t signal, const uint8_t *sigArr, uint32_t sigArrsSize, float freq,
                            uint8_t volume, uint32_t time, uint32_t mode);

void BUZZER_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim);

/** @brief Set a square sound
 *  @param FREQ square wave frequency
 *  @param VOL sound volume from 0 to 255
 *  @param TIME sound length in milliseconds
 *  @param MODE displacement mode: @arg BUZZER_WITH_DISPLACEMENT, @arg BUZZER_WITHOUT_DISPLACEMENT
 *  @return Zero on success, non-zero - on failure
 */
#define BUZZER_SOUND_MEANDER(FREQ, VOL, TIME, MODE) \
    BUZZER_Sound(BUZZER_MEANDER, NULL, 0, (FREQ), (VOL), (TIME), (MODE));

/** @brief Set a sine sound
 *  @param FREQ wave frequency
 *  @param VOL sound volume from 0 to 255
 *  @param TIME sound length in milliseconds
 *  @param MODE displacement mode: @arg BUZZER_WITH_DISPLACEMENT, @arg BUZZER_WITHOUT_DISPLACEMENT
 *  @return Zero on success, non-zero - on failure
 */
#define BUZZER_SOUND_SINE(FREQ, VOL, TIME, MODE) BUZZER_Sound(BUZZER_SINE, NULL, 0, (FREQ), (VOL), (TIME), (MODE));

/** @brief Set a triangle sound
 *  @param FREQ wave frequency
 *  @param VOL sound volume from 0 to 255
 *  @param TIME sound length in milliseconds
 *  @param MODE displacement mode: @arg BUZZER_WITH_DISPLACEMENT, @arg BUZZER_WITHOUT_DISPLACEMENT
 *  @return Zero on success, non-zero - on failure
 */
#define BUZZER_SOUND_TRIANGLE(FREQ, VOL, TIME, MODE) \
    BUZZER_Sound(BUZZER_TRIANGLE, NULL, 0, (FREQ), (VOL), (TIME), (MODE));

/** @brief Set a sawtooth sound
 *  @param FREQ wave frequency
 *  @param VOL sound volume from 0 to 255
 *  @param TIME sound length in milliseconds
 *  @param MODE displacement mode: @arg BUZZER_WITH_DISPLACEMENT, @arg BUZZER_WITHOUT_DISPLACEMENT
 *  @return Zero on success, non-zero - on failure
 */
#define BUZZER_SOUND_SAWTOOTH(FREQ, VOL, TIME, MODE) \
    BUZZER_Sound(BUZZER_SAWTOOTH, NULL, 0, (FREQ), (VOL), (TIME), (MODE));

/** @brief Set a reverse sawtooth sound
 *  @param FREQ wave frequency
 *  @param VOL sound volume from 0 to 255
 *  @param TIME sound length in milliseconds
 *  @param MODE displacement mode: @arg BUZZER_WITH_DISPLACEMENT, @arg BUZZER_WITHOUT_DISPLACEMENT
 *  @return Zero on success, non-zero - on failure
 */
#define BUZZER_SOUND_REV_SAWTOOTH(FREQ, VOL, TIME, MODE) \
    BUZZER_Sound(BUZZER_REV_SAWTOOTH, NULL, 0, (FREQ), (VOL), (TIME), (MODE));

#endif