#include "FT6x36.h"

static ft6x36_touches_t FT6x36_touch = { 0, 0 };

uint8_t FT6x36_DetectTouch(i2c_bus_t *i2c_read) {
    uint8_t nbTouch = 0;
    uint8_t read_data = 0;

    I2C_Read_IT(i2c_read, TOUCH_DEVICE_ADDRESS, FT6x36_REG_TD_STATUS, &read_data, 1);

    nbTouch = read_data;
    nbTouch &= FT6x36_TD_STAT_MASK;

    if (nbTouch > FT6x36_MAX_DETECTABLE_TOUCH) {
        nbTouch = FT6x36_touch.touchesBeforeNumb;
    }

    FT6x36_touch.touchesCurrentNumb = nbTouch;

    return nbTouch;
}

void FT6x36_GetXY(i2c_bus_t *i2c_read, uint16_t *X, uint16_t *Y) {
    uint8_t regAddress = 0;
    uint8_t dataxy[4];

    if (FT6x36_touch.touchesBeforeNumb < FT6x36_touch.touchesCurrentNumb) {

        regAddress = FT6x36_REG_P1_XH;

        I2C_Read_IT(i2c_read, TOUCH_DEVICE_ADDRESS, regAddress, dataxy, sizeof(dataxy));

        *X = ((dataxy[0] & FT6x36_MSB_MASK) << 8) | (dataxy[1] & FT6x36_LSB_MASK);
        *Y = ((dataxy[2] & FT6x36_MSB_MASK) << 8) | (dataxy[3] & FT6x36_LSB_MASK);
    }
}
