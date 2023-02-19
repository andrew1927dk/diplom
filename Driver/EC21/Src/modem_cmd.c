/**
 * @file modem_cmd.c
 * @brief Modem command for terminal.
 * @version 0.1
 * @date 2022-11-22
 *
 *  (c) 2022
 */

#include <modem_cmd.h>
#include "modem.h"
#include "modem_adapter.h"
#define LOG_DEFAULT_MODULE LOG_M_MODEM
#include "loglib.h"

extern osEventFlagsId_t MODEM_flags;

static uint8_t isPoweredOn = 1;

void MOD_CMD_sendSMS(uint8_t argc, void *argv[argc]) {
    if (isPoweredOn == 0) {
        LOG_ERROR("The modem is off.");
        return;
    }
    if (argc != 2) {
        LOG_ERROR("You need to specify exactly two arguments: -n \"+380931231212\" and -t \"SMS Text.\".");
        return;
    }
    MOD_SMS_Send(argv[0], argv[1]);
}

void MOD_CMD_readSMS(uint8_t argc, void *argv[argc]) {
    if (isPoweredOn == 0) {
        LOG_ERROR("The modem is off.");
        return;
    }
    if (argc != 0) {
        LOG_ERROR("You need to specify exactly zero arguments.");
        return;
    }
    MOD_SMS_Recv();
}

void MOD_CMD_changeSIM(uint8_t argc, void *argv[argc]) {
    if (isPoweredOn == 0) {
        LOG_ERROR("The modem is off.");
        return;
    }
    if (atoi(argv[0]) == 1) {
        if (PIN_READ(SIM1_DET) == GPIO_PIN_RESET) {
            LOG_ERROR("SIM 1 not detected.");
            return;
        }
        PIN_RESET(SIMCARD_SEL);
        LOG_INFO("Changed to SIM 1.");
    } else if (atoi(argv[0]) == 2) {
        if (PIN_READ(SIM2_DET) == GPIO_PIN_RESET) {
            LOG_ERROR("SIM 2 not detected.");
            return;
        }
        PIN_SET(SIMCARD_SEL);
        LOG_INFO("Changed to SIM 2.");
    } else {
        LOG_ERROR("You can select only SIM 1 or 2.");
    }
}

void MOD_CMD_powerOn(uint8_t argc, void *argv[argc]) {
    if (isPoweredOn == 1) {
        LOG_ERROR("The modem is already on.");
        return;
    }
    MOD_PowerOn();
    osEventFlagsWait(MODEM_flags, MOD_FLAG_INIT, 0, 15000);
    osDelay(10000);
    MOD_GEN_Init();
    isPoweredOn = 1;
}

void MOD_CMD_powerOff(uint8_t argc, void *argv[argc]) {
    if (isPoweredOn == 0) {
        LOG_ERROR("The modem is already off.");
        return;
    }
    MOD_SendATCommand("AT+QPOWD\r");
    int i = 0;
    while (PIN_READ(GSM_STATUS) == GPIO_PIN_RESET && i < 10) {
        osDelay(1500);
        LOG_DEBUG("Wait Modem status DOWN");
        i++;
    }
    LOG_DEBUG("Modem off OK");
    PIN_RESET(GSM_VDD_ON);
    osDelay(150);
    PIN_SET(GSM_VDD_DISCHG);
    PIN_SET(GSM_ON);
    isPoweredOn = 0;
}

mod_status_t MOD_CMD_Init(void) {
    PARSER_AddCommand(MOD_CMD_sendSMS, "modem sendSMS -n 0 -t 0");
    PARSER_AddCommand(MOD_CMD_readSMS, "modem readSMS");
    PARSER_AddCommand(MOD_CMD_changeSIM, "modem changeSIM -sim 0");
    PARSER_AddCommand(MOD_CMD_powerOn, "modem powerOn");
    PARSER_AddCommand(MOD_CMD_powerOff, "modem powerOff");
    return MOD_STATUS_OK;
}