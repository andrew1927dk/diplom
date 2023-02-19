/**
 ******************************************************************************
 * @file           : ILI9341_comlist_lvl1.h
 * @date           : 2022-12-21
 * @brief          : This file contains level 1 command list for ILI9341
 ******************************************************************************
 */

#ifndef ILI9341_COMLIST_LVL1_H_
#define ILI9341_COMLIST_LVL1_H_

/**
 * @brief No operation
 */
#define ILI9341_NOP 0x00

/**
 * @brief When the Software Reset command is written, it causes a software reset. It resets the commands and parameters
 * to their S/W Reset default values. (See default tables in each command description.)
 */
#define ILI9341_RESET_SOFTWARE 0x01

/**
 * @brief This read byte returns 24 bits display identification information.
 */
#define ILI9341_READ_ID_DISPLAY 0x04

/**
 * @brief This command indicates the current status of the display
 */
#define ILI9341_READ_DISP_STATUS 0x09

/**
 * @brief This command indicates the current status of the display (power mode)
 */
#define ILI9341_READ_DISP_POWER_MODE 0x0A

/**
 * @brief This command indicates the current status of the display
 */
#define ILI9341_READ_DISP_MADCTL 0x0B

/**
 * @brief This command indicates the current status of the display
 */
#define ILI9341_READ_DISP_PIXEL_FORMAT 0x0C

/**
 * @brief This command indicates the current status of the display
 */
#define ILI9341_READ_DISP_IMAGE_FORMAT 0x0D

/**
 * @brief This command indicates the current status of the display
 */
#define ILI9341_READ_DISP_SIGNAL_MODE 0x0D

#define ILI9341_READ_DISP_SELF_TEST 0x0F

/**
 * @brief This command causes the LCD module to enter the minimum power consumption mode. In this mode e.g. the DC/DC
 * converter is stopped, Internal oscillator is stopped, and panel scanning is stopped.
 * MCU interface and memory are still working and the memory keeps its contents.
 */
#define ILI9341_ENABLE_SLEEP_MODE 0x10

/**
 * @brief This command turns off sleep mode. In this mode e.g. the DC/DC converter is enabled, Internal oscillator is
 * started, and panel scanning is started.
 */
#define ILI9341_DISABLE_SLEEP_MODE 0x11

/**
 * @brief This command turns on partial mode The partial mode window is described by the Partial Area command (30H).
 * To leave Partial mode, the Normal Display Mode On command (13H) should be written.
 */
#define ILI9341_ENABLE_PARTIAL_MODE 0x12

/**
 * @brief This command returns the display to normal mode. Normal display mode on means Partial mode off.
 */
#define ILI9341_ENABLE_NORMAL_MODE 0x13

/**
 * @brief This command is used to recover from display inversion mode. This command makes no change of the content of
 * frame memory. This command doesn’t change any other status.
 */
#define ILI9341_DISABLE_DISP_INVERSION 0x20

/**
 * @brief This command is used to enter into display inversion mode. This command makes no change of the content of
 * frame memory. Every bit is inverted from the frame memory to the display. This command doesn’t change any other
 * status. To exit Display inversion mode, the Display inversion OFF command (20h) should be written.
 */
#define ILI9341_ENABLE_DISP_INVERSION 0x21

/**
 * @brief This command is used to enter into display inversion mode. This command makes no change of the content of
 * frame memory. This command is used to select the desired Gamma curve for the current display. A maximum of 4 fixed
 * gamma curves can be selected. The curve is selected by setting the appropriate bit in the parameter as described in
 * the Table (see doc).
 */
#define ILI9341_SET_GAMMA 0x26

/**
 * @brief This command is used to enter into DISPLAY OFF mode. In this mode, the output from Frame Memory is disabled
 * and blank page inserted. This command makes no change of contents of frame memory. This command does not change any
 * other status. There will be no abnormal visible effect on the display.
 */
#define ILI9341_DISABLE_DISP_SCREEN 0x28

/**
 * @brief This command is used to recover from DISPLAY OFF mode. Output from the Frame Memory is enabled.
 * This command makes no change of contents of frame memory. This command does not change any other status
 */
#define ILI9341_ENABLE_DISP_SCREEN 0x29

/**
 * @brief This command is used to define area of frame memory where MCU can access. This command makes no change on the
 * other driver status. The values of SC [15:0] and EC [15:0] are referred when RAMWR command comes.
 * Each value represents one column line in the Frame Memory.
 */
#define ILI9341_SET_COLUMN_ADDRESS 0x2A

/**
 * @brief This command is used to define area of frame memory where MCU can access. This command makes no change on the
 * other driver status. The values of SP [15:0] and EP [15:0] are referred when RAMWR command comes. Each value
 * represents one Page line in the Frame Memory.
 */
#define ILI9341_SET_PAGE_ADDRESS 0x2B

/**
 * @brief This command is used to transfer data from MCU to frame memory. This command makes no change to the other
 * driver status. When this command is accepted, the column register and the page register are reset to the Start
 * Column/Start
 */
#define ILI9341_WRITE_MEMORY 0x2C

/**
 * @brief This command is used to define the LUT for 16-bit to 18-bit color depth conversion.
 * 128 bytes must be written to the LUT regardless of the color mode. Only the values in Section 7.4 are referred.
 * This command has no effect on other commands, parameter and contents of frame memory. Visible change takes effect
 * next time the frame memory is written to.
 */
#define ILI9341_SET_COLOR 0x2D

/**
 * @brief This command transfers image data from ILI9341’s frame memory to the host processor starting at the pixel
 * location specified by preceding set_column_address and set_page_address commands.
 */
#define ILI9341_READ_MEMORY 0x2E

/**
 * @brief This command defines the partial mode’s display area. There are 2 parameters associated with this command, the
 * first defines the Start Row (SR) and the second the End Row (ER), as illustrated in the figures below. SR and ER
 * refer to the Frame Memory Line Pointer.
 */
#define ILI9341_SET_PARTIAL_AREA 0x30

/**
 * @brief This command defines the Vertical Scrolling Area of the display.
 */
#define ILI9341_SET_SCROLLING_VERTICAL 0x33

/**
 * @brief This command is used to turn OFF (Active Low) the Tearing Effect output signal from the TE signal line.
 */
#define ILI9341_DISABLE_TEARING_EFFECT 0x34

/**
 * @brief This command is used to turn ON the Tearing Effect output signal from the TE signal line.
 * This output is not affected by changing MADCTL bit B4.
 * The Tearing Effect Line On has one parameter which describes the mode of the Tearing Effect Output Line.
 */
#define ILI9341_ENABLE_TEARING_EFFECT 0x35

/**
 * @brief This command defines read/write scanning direction of frame memory.
 * This command makes no change on the other driver status.
 */
#define ILI9341_SET_MEMORY_ACCESS_CONTROL 0x36

/**
 * @brief This command is used together with Vertical Scrolling Definition (33h). These two commands describe the
 * scrolling area and the scrolling mode. The Vertical Scrolling Start Address command has one parameter which describes
 * the address of the line in the Frame Memory that will be written as the first line after the last line of the Top
 * Fixed Area
 */
#define ILI9341_SET_SCROLLING_VERTICAL_START_ADDRESS 0x37

/**
 * @brief This command is used to recover from Idle mode on.
 * In the idle off mode, LCD can display maximum 262,144 colors.
 */
#define ILI9341_DISABLE_IDLE_MODE 0x38

/**
 * @brief This command is used to enter into Idle mode on.
 * In the idle on mode, color expression is reduced. The primary and the secondary colors using MSB of each R, G and B
 * in the Frame Memory, 8 color depth data is displayed.
 */
#define ILI9341_ENABLE_IDLE_MODE 0x39

/**
 * @brief This command sets the pixel format for the RGB image data used by the interface. DPI [2:0] is the pixel format
 * select of RGB interface and DBI [2:0] is the pixel format of MCU interface. If a particular interface, either RGB
 * interface or MCU interface, is not used then the corresponding bits in the parameter are ignored.
 */
#define ILI9341_SET_PIXEL_FORMAT 0x3A

/**
 * @brief This command transfers image data from the host processor to the display module’s frame memory continuing from
 * the pixel location following the previous write_memory_continue or write_memory_start command.
 */
#define ILI9341_WRITE_MEMORY_CONTINUE 0x3C

/**
 * @brief This command transfers image data from the display module’s frame memory to the host processor continuing from
 * the location following the previous read_memory_continue (3Eh) or read_memory_start (2Eh) command.
 */
#define ILI9341_READ_MEMORY_CONTINUE 0x3C

/**
 * @brief This command turns on the display Tearing Effect output signal on the TE signal line when the display reaches
 * line STS. The TE signal is not affected by changing set_address_mode bit B4. The Tearing Effect Line On has one
 * parameter that describes the Tearing Effect Output Line mode.
 */
#define ILI9341_SET_TEAR_SCANLINE 0x44

/**
 * @brief The display returns the current scan line, GTS, used to update the display device.
 * The total number of scan lines on a display device is defined as VSYNC + VBP + VACT + VFP.
 * The first scan line is defined as the first line of V-Sync and is denoted as Line 0.
 * When in Sleep Mode, the value returned by get_scanline is undefined.
 */
#define ILI9341_GET_SCANLINE 0x45

/**
 * @brief This command is used to adjust the brightness value of the display.
 */
#define ILI9341_WRITE_DISP_BRIGHTNESS 0x51

/**
 * @brief This command returns the brightness value of the display.
 */
#define ILI9341_READ_DISP_BRIGHTNESS 0x52

/**
 * @brief This command is used to control display brightness.
 */
#define ILI9341_SET_DISP_BRIGHTNESS_SETTINGS 0x53

/**
 * @brief This command is used to return brightness setting.
 */
#define ILI9341_READ_DISP_BRIGHTNESS_SETTINGS 0x54

/**
 * @brief This command is used to set parameters for image content based adaptive brightness control functionality.
 * There is possible to use 4 different modes for content adaptive image functionality
 */
#define ILI9341_SET_DISP_BRIGHTNESS_ADAPTIVE 0x55

/**
 * @brief This command is used to read the settings for image content based adaptive brightness control functionality.
 * It is possible to use 4 different modes for content adaptive image functionality
 */
#define ILI9341_READ_DISP_BRIGHTNESS_ADAPTIVE 0x56

/**
 * @brief This command is used to set the minimum brightness value of the display for CABC function.
 * CMB[7:0]: CABC minimum brightness control, this parameter is used to avoid too much brightness reduction.
 * When CABC is active, CABC cannot reduce the display brightness to less than CABC minimum brightness setting.
 * Image processing function is worked as normal, even if the brightness cannot be changed.
 */
#define ILI9341_SET_DISP_BRIGHTNESS_MIN 0x5E

/**
 * @brief This command returns the minimum brightness value of CABC function.
 */
#define ILI9341_READ_DISP_BRIGHTNESS_MIN 0x5F

/**
 * @brief This read byte identifies the LCD module’s manufacturer ID and it is specified by User.
 */
#define ILI9341_READ_ID_MNFCTR 0xDA

/**
 * @brief This read byte is used to track the LCD module/driver version. It is defined by display supplier
 * (with User’s agreement) and changes each time a revision is made to the display, material or construction
 * specifications.
 */
#define ILI9341_READ_ID_DRIVER_SUP 0xDB

/**
 * @brief This read byte identifies the LCD module/driver and It is specified by User.
 */
#define ILI9341_READ_ID_DRIVER_USER 0xDC

#endif /* ILI9341_COMLIST_LVL1_H_ */
