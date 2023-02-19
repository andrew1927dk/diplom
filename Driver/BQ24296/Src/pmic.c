/**
 * @file pmic.с
 * @brief source file for interaction with PMIC
 * @date 2022-12-27
 *
 *   (c) 2022
 */

#include "pmic.h"

// DPM Input Voltage Limit Set.
i2c_status_t PMIC_SetInputVoltageLimit(i2c_bus_t *i2c, uint8_t val) {
    pmic_inputSrcCtrl_t inputSrcCtrl = { 0 };
    i2c_status_t        ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_INPUT_SRC_CTRL_REG, (uint8_t *) &inputSrcCtrl, 1);
    if (ret == I2C_OK) {
        inputSrcCtrl.vindpm = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_INPUT_SRC_CTRL_REG, (uint8_t *) &inputSrcCtrl, 1);
    }
    return ret;
}

// DPM Input Current Limit Set.
i2c_status_t PMIC_SetInputCurrentLimitSet(i2c_bus_t *i2c, uint8_t val) {
    pmic_inputSrcCtrl_t inputSrcCtrl = { 0 };
    i2c_status_t        ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_INPUT_SRC_CTRL_REG, (uint8_t *) &inputSrcCtrl, 1);
    if (ret == I2C_OK) {
        inputSrcCtrl.iinlim = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_INPUT_SRC_CTRL_REG, (uint8_t *) &inputSrcCtrl, 1);
    }
    return ret;
}

i2c_status_t PMIC_EnableCharge(i2c_bus_t *i2c, bool val) {
    pmic_powerOnCfg_t powerOnCfg = { 0 };
    i2c_status_t      ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_CONFIG_REG, (uint8_t *) &powerOnCfg, 1);
    if (ret == I2C_OK) {
        powerOnCfg.chg_config = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_CONFIG_REG, (uint8_t *) &powerOnCfg, 1);
    }
    return ret;
}

i2c_status_t PMIC_ResetWatchdog(i2c_bus_t *i2c) {
    pmic_powerOnCfg_t powerOnCfg = { 0 };
    i2c_status_t      ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_CONFIG_REG, (uint8_t *) &powerOnCfg, 1);
    if (ret == I2C_OK) {
        powerOnCfg.wdg_reset = 1;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_CONFIG_REG, (uint8_t *) &powerOnCfg, 1);
    }
    return ret;
}

i2c_status_t PMIC_SetMinimumSystemVoltage(i2c_bus_t *i2c, uint8_t val) {
    pmic_powerOnCfg_t powerOnCfg = { 0 };
    i2c_status_t      ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_CONFIG_REG, (uint8_t *) &powerOnCfg, 1);
    if (ret == 0) {
        powerOnCfg.sys_min = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_CONFIG_REG, (uint8_t *) &powerOnCfg, (uint32_t) 1);
    }
    return ret;
}

i2c_status_t PMIC_SetBoostCurrentLimit(i2c_bus_t *i2c, uint8_t val) {
    pmic_powerOnCfg_t powerOnCfg = { 0 };
    i2c_status_t      ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_CONFIG_REG, (uint8_t *) &powerOnCfg, 1);
    if (ret == I2C_OK) {
        powerOnCfg.boost_lim = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_CONFIG_REG, (uint8_t *) &powerOnCfg, 1);
    }
    return ret;
}

i2c_status_t PMIC_SetChargeCurrent(i2c_bus_t *i2c, uint8_t val) {
    pmic_chgCurrentCtrl_t chgCurrentCtrl = { 0 };
    i2c_status_t          ret;
    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_ICHG_CTRL_REG, (uint8_t *) &chgCurrentCtrl, 1);
    if (ret == I2C_OK) {
        chgCurrentCtrl.ichg = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_ICHG_CTRL_REG, (uint8_t *) &chgCurrentCtrl, 1);
    }
    return ret;
}

i2c_status_t PMIC_SetPrechargeCurrent(i2c_bus_t *i2c, uint8_t val) {
    pmic_preChgTermCurrentCtrl_t preChgTermCurrent = { 0 };
    i2c_status_t                 ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_PRECHG_CTRL_REG, (uint8_t *) &preChgTermCurrent, 1);
    if (ret == I2C_OK) {
        preChgTermCurrent.iprechg = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_PRECHG_CTRL_REG, (uint8_t *) &preChgTermCurrent, 1);
    }
    return ret;
}

i2c_status_t PMIC_SetTerminationCurrent(i2c_bus_t *i2c, uint8_t val) {
    pmic_preChgTermCurrentCtrl_t preChgTermCurrent = { 0 };
    i2c_status_t                 ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_PRECHG_CTRL_REG, (uint8_t *) &preChgTermCurrent, 1);
    if (ret == I2C_OK) {
        preChgTermCurrent.iterm = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_PRECHG_CTRL_REG, (uint8_t *) &preChgTermCurrent, 1);
    }
    return ret;
}

i2c_status_t PMIC_SetChargeVoltage(i2c_bus_t *i2c, uint8_t val) {
    pmic_chgVoltageVtrl_t chgVoltageCtrl = { 0 };
    i2c_status_t          ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_VREG_CTRL_REG, (uint8_t *) &chgVoltageCtrl, 1);
    if (ret == I2C_OK) {
        chgVoltageCtrl.vreg = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_VREG_CTRL_REG, (uint8_t *) &chgVoltageCtrl, 1);
    }
    return ret;
}

i2c_status_t PMIC_SetBattLowThreshold(i2c_bus_t *i2c, uint8_t val) {
    pmic_chgVoltageVtrl_t chgVoltageCtrl = { 0 };
    i2c_status_t          ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_VREG_CTRL_REG, (uint8_t *) &chgVoltageCtrl, 1);
    if (ret == I2C_OK) {
        chgVoltageCtrl.batlowv = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_VREG_CTRL_REG, (uint8_t *) &chgVoltageCtrl, 1);
    }
    return ret;
}

i2c_status_t PMIC_SetRechargeVoltage(i2c_bus_t *i2c, uint8_t val) {
    pmic_chgVoltageVtrl_t chgVoltageCtrl = { 0 };
    i2c_status_t          ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_VREG_CTRL_REG, (uint8_t *) &chgVoltageCtrl, 1);
    if (ret == I2C_OK) {
        chgVoltageCtrl.vrechg = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_VREG_CTRL_REG, (uint8_t *) &chgVoltageCtrl, 1);
    }
    return ret;
}

// Enable Battery Termination.
i2c_status_t PMIC_EnableTermination(i2c_bus_t *i2c, bool val) {
    pmic_chgTermTimerCtrl_t chgTermTimerCtrl = { 0 };
    i2c_status_t            ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_TIMER_CTRL_REG, (uint8_t *) &chgTermTimerCtrl, 1);
    if (ret == I2C_OK) {
        chgTermTimerCtrl.en_term = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_TIMER_CTRL_REG, (uint8_t *) &chgTermTimerCtrl, 1);
    }
    return ret;
}

i2c_status_t PMIC_SetWatchdogTimer(i2c_bus_t *i2c, uint8_t val) {
    pmic_chgTermTimerCtrl_t chgTermTimerCtrl = { 0 };
    i2c_status_t            ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_TIMER_CTRL_REG, (uint8_t *) &chgTermTimerCtrl, 1);
    if (ret == I2C_OK) {
        chgTermTimerCtrl.watchdog = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_TIMER_CTRL_REG, (uint8_t *) &chgTermTimerCtrl, 1);
    }
    return ret;
}

i2c_status_t PMIC_EnableSafetyTimer(i2c_bus_t *i2c, bool val) {
    pmic_chgTermTimerCtrl_t chgTermTimerCtrl = { 0 };
    i2c_status_t            ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_TIMER_CTRL_REG, (uint8_t *) &chgTermTimerCtrl, 1);
    if (ret == I2C_OK) {
        chgTermTimerCtrl.en_timer = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_TIMER_CTRL_REG, (uint8_t *) &chgTermTimerCtrl, 1);
    }
    return ret;
}

i2c_status_t PMIC_SetChargeTimer(i2c_bus_t *i2c, uint8_t val) {
    pmic_chgTermTimerCtrl_t chgTermTimerCtrl = { 0 };
    i2c_status_t            ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_TIMER_CTRL_REG, (uint8_t *) &chgTermTimerCtrl, 1);
    if (ret == I2C_OK) {
        chgTermTimerCtrl.chg_timer = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_TIMER_CTRL_REG, (uint8_t *) &chgTermTimerCtrl, 1);
    }
    return ret;
}

i2c_status_t PMIC_SetBoostVoltage(i2c_bus_t *i2c, uint8_t val) {
    pmic_boostvThermalCtrl_t boostvThermalCtrl = { 0 };
    i2c_status_t             ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_BOOSTV_CTRL_REG, (uint8_t *) &boostvThermalCtrl, 1);
    if (ret == I2C_OK) {
        boostvThermalCtrl.boostv = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_BOOSTV_CTRL_REG, (uint8_t *) &boostvThermalCtrl, 1);
    }
    return ret;
}

i2c_status_t PMIC_SetBoostTempMonitor(i2c_bus_t *i2c, uint8_t val) {
    pmic_boostvThermalCtrl_t boostvThermalCtrl = { 0 };
    i2c_status_t             ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_BOOSTV_CTRL_REG, (uint8_t *) &boostvThermalCtrl, 1);
    if (ret == I2C_OK) {
        boostvThermalCtrl.bhot = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_BOOSTV_CTRL_REG, (uint8_t *) &boostvThermalCtrl, 1);
    }
    return ret;
}

i2c_status_t PMIC_SetThermalRegulationThreshold(i2c_bus_t *i2c, uint8_t val) {
    pmic_boostvThermalCtrl_t boostvThermalCtrl = { 0 };
    i2c_status_t             ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_BOOSTV_CTRL_REG, (uint8_t *) &boostvThermalCtrl, 1);
    if (ret == I2C_OK) {
        boostvThermalCtrl.treg = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_BOOSTV_CTRL_REG, (uint8_t *) &boostvThermalCtrl, 1);
    }
    return ret;
}

i2c_status_t PMIC_EnableBattFaultInterrupt(i2c_bus_t *i2c, uint8_t val) {
    pmic_miscCtrl_t miscCtrl = { 0 };
    i2c_status_t    ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_MISC_CTRL_REG, (uint8_t *) &miscCtrl, 1);
    if (ret == I2C_OK) {
        miscCtrl.int_mask0 = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_MISC_CTRL_REG, (uint8_t *) &miscCtrl, 1);
    }
    return ret;
}

i2c_status_t PMIC_EnableChgFaultInterrupt(i2c_bus_t *i2c, uint8_t val) {
    pmic_miscCtrl_t miscCtrl = { 0 };
    i2c_status_t    ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_MISC_CTRL_REG, (uint8_t *) &miscCtrl, 1);
    if (ret == I2C_OK) {
        miscCtrl.int_mask1 = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_MISC_CTRL_REG, (uint8_t *) &miscCtrl, 1);
    }
    return ret;
}

i2c_status_t PMIC_DisableBatfet(i2c_bus_t *i2c, uint8_t val) {
    pmic_miscCtrl_t miscCtrl = { 0 };
    i2c_status_t    ret;

    ret = I2C_Read_DMA(i2c, BQ24296_I2C_ADD, BQ24296_MISC_CTRL_REG, (uint8_t *) &miscCtrl, 1);
    if (ret == I2C_OK) {
        miscCtrl.batfet_disable = val;
        return I2C_Write_DMA(i2c, BQ24296_I2C_ADD, BQ24296_MISC_CTRL_REG, (uint8_t *) &miscCtrl, 1);
    }
    return ret;
}

i2c_status_t PMIC_GetRegister(i2c_bus_t *i2c, uint8_t reg, void *val) {
    return I2C_Read_DMA(i2c, BQ24296_I2C_ADD, reg, val, 1);
}