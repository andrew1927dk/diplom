/**
 ******************************************************************************
 * @file           : display.c
 * @date           : 2022-12-21
 * @brief          :
 ******************************************************************************
 */

#include "display.h"
#include "tim.h"
#include "loglib.h"

#define DISPLAY_THREAD_STACK_SIZE 1024 * 5
static TX_THREAD thrDisplayHandle;

static lv_color_t buf_1[ILI9341_SCREEN_HEIGHT * 10];
static lv_color_t buf_2[ILI9341_SCREEN_HEIGHT * 10];
static i2c_bus_t *i2c_getter;

/* LVGL mutex */
static TX_MUTEX dispLVGLInit;

/* For testing purposes for the time being (to see if display works when flashed) */
static uint32_t btn_cnt = 1;

static void float_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t       *float_btn = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED) {
        lv_obj_t *list = lv_event_get_user_data(e);
        char      buf[32];
        lv_snprintf(buf, sizeof(buf), "Track %d", (int) btn_cnt);
        lv_obj_t *list_btn = lv_list_add_btn(list, LV_SYMBOL_AUDIO, buf);
        btn_cnt++;

        lv_obj_move_foreground(float_btn);

        lv_obj_scroll_to_view(list_btn, LV_ANIM_ON);
    }
}

void lv_example_scroll_3(void) {
    lv_obj_t *list = lv_list_create(lv_scr_act());
    lv_obj_set_size(list, 220, 320);
    lv_obj_center(list);

    for (btn_cnt = 1; btn_cnt <= 2; btn_cnt++) {
        char buf[32];
        lv_snprintf(buf, sizeof(buf), "Track %d", (int) btn_cnt);
        lv_list_add_btn(list, LV_SYMBOL_AUDIO, buf);
    }

    lv_obj_t *float_btn = lv_btn_create(list);
    lv_obj_set_size(float_btn, 50, 50);
    lv_obj_add_flag(float_btn, LV_OBJ_FLAG_FLOATING);
    lv_obj_align(float_btn, LV_ALIGN_BOTTOM_RIGHT, 0, -lv_obj_get_style_pad_right(list, LV_PART_MAIN));
    lv_obj_add_event_cb(float_btn, float_btn_event_cb, LV_EVENT_ALL, list);
    lv_obj_set_style_radius(float_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_img_src(float_btn, LV_SYMBOL_PLUS, 0);
    lv_obj_set_style_text_font(float_btn, lv_theme_get_font_large(float_btn), 0);
}

/**
 * @brief Flush function for LVGL with ILI9341 implemented
 * @param disp_drv : LVGL driver structure
 * @param area : Area of the screen
 * @param color_p : Colors
 */
static void DISPLAY_lvglFlush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p) {
    tx_mutex_get(&dispLVGLInit, TX_WAIT_FOREVER);
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;
    size_t   wh = w * h;

    ILI9341_START_WRITE;
    ILI9341_setWindow(area->x1, area->y1, w, h);
    ILI9341_START_DATA;
    ILI9341_sendData((uint8_t *) color_p, wh * 2);
    ILI9341_END_WRITE;

    lv_disp_flush_ready(disp_drv);
    tx_mutex_put(&dispLVGLInit);
}

static void TOUCH_lvglRead(lv_indev_drv_t *drv, lv_indev_data_t *data) {
    touch_state_t TOUCH_State = { 0 };
    TOUCH_GetState(i2c_getter, &TOUCH_State);
    if (TOUCH_State.touchDetected) {
        data->point.x = TOUCH_State.touchX;
        data->point.y = TOUCH_State.touchY;
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
    TOUCH_ResetTouchData(&TOUCH_State);
}

/**
 * @brief This function initializes ILI9341 and LVGL
 */
static void DISPLAY_lvTimerHandlerThread(ULONG argument) {
    lv_example_scroll_3();
    while (1) {
        tx_mutex_get(&dispLVGLInit, TX_WAIT_FOREVER);
        lv_timer_handler();
        tx_mutex_put(&dispLVGLInit);
        tx_thread_sleep(2);
    }
}

DISPLAY_state_t DISPLAY_Init(void *memoryPoolPtr, i2c_bus_t *i2c_read) {
    /* Mutex creation */
    if (tx_mutex_create(&dispLVGLInit, "dispLVGLInit", TX_NO_INHERIT) != TX_SUCCESS) {
        return DISPLAY_TX_ERR;
    }

    /* Backlight */
    PIN_SET(PP3V1_DISP_ON);
    tx_thread_sleep(10);
    PIN_SET(DISP_TS_RESET);
    tx_thread_sleep(10);

    /* Timers */
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
    __HAL_TIM_SetCompare(&htim4, TIM_CHANNEL_4, 100);

    touch_status_t TOUCH_status = TOUCH_Init(i2c_read);
    if (TOUCH_status == TOUCH_OK) {
        i2c_getter = i2c_read;
    }

    /* Initialize ILI9341 driver */
    if (ILI9341_init() != ILI9341_OK) {
        return DISPLAY_DRIVER_ERR;
    }

    /* LVGL Preparations part 1 */
    tx_mutex_get(&dispLVGLInit, TX_WAIT_FOREVER);

    lv_init();
    static lv_disp_draw_buf_t disp_buf;
    static lv_disp_drv_t      disp_drv;
    static lv_indev_drv_t     touch_drv;
    lv_disp_draw_buf_init(&disp_buf, buf_1, buf_2, ILI9341_SCREEN_WIDTH * 10);
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf;            /*Set an initialized buffer*/
    disp_drv.flush_cb = DISPLAY_lvglFlush;    /*Set a flush callback to draw to the display*/
    disp_drv.hor_res = ILI9341_SCREEN_WIDTH;  /*Set the horizontal resolution in pixels*/
    disp_drv.ver_res = ILI9341_SCREEN_HEIGHT; /*Set the vertical resolution in pixels*/
    disp_drv.rotated = 0;
    lv_disp_drv_register(&disp_drv);

    if (TOUCH_status == TOUCH_OK) {
        lv_indev_drv_init(&touch_drv);
        touch_drv.type = LV_INDEV_TYPE_POINTER;
        touch_drv.read_cb = TOUCH_lvglRead;
        lv_indev_drv_register(&touch_drv);
    }

    tx_mutex_put(&dispLVGLInit);

    /* Create thread with LVGL handler */
    TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL *) memoryPoolPtr;
    CHAR         *pointer = NULL;
    if (tx_byte_allocate(byte_pool, (void **) &pointer, DISPLAY_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS) {
        return DISPLAY_TX_ERR;
    }

    if (tx_thread_create(&thrDisplayHandle, "DISPLAY Thread", DISPLAY_lvTimerHandlerThread, 1, pointer,
                         DISPLAY_THREAD_STACK_SIZE, 10, 10, TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS) {
        return DISPLAY_TX_ERR;
    }

    return DISPLAY_OK;
}

void DISPLAY_Lock(void) {
    tx_mutex_get(&dispLVGLInit, TX_WAIT_FOREVER);
}

void DISPLAY_Unlock(void) {
    tx_mutex_put(&dispLVGLInit);
}