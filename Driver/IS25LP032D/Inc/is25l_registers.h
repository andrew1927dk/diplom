/**
 ***********************************************************************************
 * @file           : IS25L_registers.h
 * @date           : 2022-09-27
 * @brief          : This file contains all the description of the IS25L QSPI memory.
 ***********************************************************************************
 */

#ifndef CORE_INC_IS25L_REGISTERS_H_
#define CORE_INC_IS25L_REGISTERS_H_

/* Identification Operations */
#define IS25L_READ_ID_CMD 0x9F

/* Read Operations */
#define IS25L_INOUT_FAST_READ_CMD 0xEB

/* Write Operations */
#define IS25L_WRITE_ENABLE_CMD  0x06
#define IS25L_WRITE_DISABLE_CMD 0x04
#define IS25L_WRITE_PPQ_INP_CMD 0x32

/* Register Operations */
#define IS25L_READ_STATUS_REG_CMD  0x05
#define IS25L_WRITE_STATUS_REG_CMD 0x01

/* Chip Erase Operation */
#define IS25L_ERASE_CHIP_CMD 0x60

/* Sector Erase Operation */
#define IS25L_ERASE_SECTOR_CMD 0x20

#endif /* CORE_INC_IS25L_REGISTERS_H_ */