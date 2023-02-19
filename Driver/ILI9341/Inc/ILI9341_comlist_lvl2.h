/**
 ******************************************************************************
 * @file           : ILI9341_comlist_lvl2.h
 * @date           : 2022-12-21
 * @brief          : This file contains level 2 command list for ILI9341
 ******************************************************************************
 */

#ifndef ILI9341_COMLIST_LVL2_H_
#define ILI9341_COMLIST_LVL2_H_

/**
 * @brief Sets the operation status of the display interface. The setting becomes effective as soon as the command is
 * received.
 */
#define ILI9341_CONTROL_RGB_INT_SIGNAL 0xB0

/**
 * @brief Sets the division ratio for internal clocks of Normal mode at MCU interface.
 */
#define ILI9341_CONTROL_FRAME_RATE_NORMAL_FULLCOLOR 0xB1

/**
 * @brief Sets the division ratio for internal clocks of Idle mode at MCU interface.
 */
#define ILI9341_CONTROL_FRAME_RATE_IDLE_8BITCOLOR 0xB2

/**
 * @brief Sets the division ratio for internal clocks of Partial mode (Idle mode off) at MCU interface.
 */
#define ILI9341_CONTROL_FRAME_RATE_PATRIAL_FULLCOLOR 0xB3

/**
 * @brief Display inversion mode set
 */
#define ILI9341_CONTROL_DISPLAY_INVERSION 0xB4

/**
 * @brief
 */
#define ILI9341_CONTROL_BLANKING_PORCH 0xB5

/**
 * @brief
 */
#define ILI9341_CONTROL_DISPLAY_FUNCTION 0xB6

/**
 * @brief
 */
#define ILI9341_CONTROL_SET_MODE_ENTRY 0xB7

/**
 * @brief These bits are used to set the percentage of grayscale data accumulate histogram value
 * in the user interface (UI) mode. This ratio of maximum number of pixels that makes display image
 * white (=data “255”) to the total of pixels by image processing.
 */
#define ILI9341_CONTROL_BACKLIGHT1 0xB8

/**
 * @brief These bits are used to set the percentage of grayscale data accumulate histogram value in the still picture
 * mode. This ratio of maximum number of pixels that makes display image white (=data “255”) to the total of pixels by
 * image processing.
 */
#define ILI9341_CONTROL_BACKLIGHT2 0xB9

/**
 * @briefThis parameter is used set the minimum limitation of grayscale threshold value in User Icon (UI) image mode.
 * This register setting will limit the minimum Dth value to prevent the display image from being too
 * white and the display quality is not acceptable.
 */
#define ILI9341_CONTROL_BACKLIGHT3 0xBA

/**
 * @brief This parameter is used set the minimum limitation of grayscale threshold value.
 * This register setting will limit the minimum Dth value to prevent the display image from being too
 * white and the display quality is not acceptable.
 */
#define ILI9341_CONTROL_BACKLIGHT4 0xBB

/**
 * @brief This parameter is used to set the transition time of brightness level to avoid the
 * sharp brightness transition on vision.
 */
#define ILI9341_CONTROL_BACKLIGHT5 0xBC

/**
 * @brief This command is used to adjust the PWM waveform frequency of PWM_OUT.
 * The PWM frequency can be calculated by using the following equation (see doc).
 */
#define ILI9341_CONTROL_BACKLIGHT7 0xBE

/**
 * @brief LEDPWMPOL: The bit is used to define polarity of LEDPWM signal.
 * LEDONPOL: This bit is used to control LEDON pin.
 * LEDONR: This bit is used to control LEDON pin.
 */
#define ILI9341_CONTROL_BACKLIGHT8 0xBF

/**
 * @brief Set the GVDD level, which is a reference level for the VCOM level and the grayscale voltage level.
 */
#define ILI9341_CONTROL_POWER1 0xC0

/**
 * @brief Sets the factor used in the step-up circuits.
 */
#define ILI9341_CONTROL_POWER2 0xC1

/**
 * @brief Set the VCOMH voltage.
 */
#define ILI9341_CONTROL_VCOM1 0xC5

/**
 * @brief Set the VCOM offset voltage.
 */
#define ILI9341_CONTROL_VCOM2 0xC7

/**
 * @brief This command is used to program the NV memory data.
 * After a successful MTP operation, the information of PGM_DATA [7:0] will programmed to NV memory.
 */
#define ILI9341_CONTROL_MEMORY_NV_WRITE 0xD0

/**
 * @brief NV memory programming protection key.
 */
#define ILI9341_CONTROL_MEMORY_NV_PROTECTION_KEY 0xD1

/**
 * @brief Read status of NV MEMORY
 */
#define ILI9341_CONTROL_MEMORY_NV_READ_STATUS 0xD2

/**
 * @brief Read IC device code.
 */
#define ILI9341_CONTROL_ID4_READ 0xD3

/**
 * @brief Set the gray scale voltage to adjust the gamma characteristics of the TFT panel.
 */
#define ILI9341_CONTROL_GAMMA_CORRECTION_POSITIVE 0xE0

/**
 * @brief Set the gray scale voltage to adjust the gamma characteristics of the TFT panel.
 */
#define ILI9341_CONTROL_GAMMA_CORRECTION_NEGATIVE 0xE1

/**
 * @brief Gamma Macro-adjustment registers for red and blue gamma curve.
 */
#define ILI9341_CONTROL_GAMMA_DIGITAL1 0xE2

/**
 * @brief Gamma Micro-adjustment register for red and blue gamma curve.
 */
#define ILI9341_CONTROL_GAMMA_DIGITAL2 0xE3

/**
 * @brief The set value of MADCTL is used in the IC is derived as exclusive OR between 1st Parameter of IFCTL and
 * MADCTLParameter.
 */
#define ILI9341_CONTROL_INTERFACE 0xF6

/**
 * @brief
 */
#define ILI9341_CONTROL_POWER_A 0xCB

/**
 * @brief
 */
#define ILI9341_CONTROL_POWER_B 0xCF

/**
 * @brief EQ timing for Internal clock
 */
#define ILI9341_CONTROL_DRIVER_TIMING_A1 0xE8

/**
 * @brief EQE timing for External clock
 */
#define ILI9341_CONTROL_DRIVER_TIMING_A2 0xE9

/**
 * @brief Gate driver timing control
 */
#define ILI9341_CONTROL_DRIVER_TIMING_B 0xEA

/**
 * @brief Soft start control
 */
#define ILI9341_CONTROL_POWER_ON_SEQUENCE 0xED

/**
 * @brief Enable 3 gamma control
 */
#define ILI9341_CONTROL_GAMMA_3GAMMA 0xF2

/**
 * @brief Ratio control
 */
#define ILI9341_CONTROL_PUMP_RATIO 0xF7

#endif /* INC_ILI9341_COMLIST_LVL2_H_ */
