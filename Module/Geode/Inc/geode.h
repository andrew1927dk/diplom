/**
 * @file geode.h
 * @brief RFM point-to-multipoint protocol.
 *        This file provides functions to manage the following
 *        functionalities of GEODE protocol:
 *            + Initialization function
 *            + Nodes control functions
 *            + IO operation functions
 *        ### How to use GEODE protocol features ###
 *     GEODE is designed to provide bit streams between master and slaves
 *     and can be used as follows:
 *     + Initialize protocol by calling the GOD_Init() API:
 *         * This API initialize and configures also driver for RFM
 *             by calling RFM_Init() and RFM_Config() API
 *     + Start network node by calling GOD_Start() API:
 *         * Creates corresponding task to handle specific node behavior
 *         * Allocates buffer used for bitstreams
 *     + Process IO operations:
 *         * By calling next API:
 *             + GOD_Write()
 *             + GOD_Printf()
 *             + GOD_Read()
 *         * With possibility to operate:
 *             + Blocking or non-bloking modes
 *             + Read from specific node or any one
 *
 * @todo  Add next protocol functions and features:
 *          - de-initialization functions
 *          - Node closing (disconecting)
 *          - Protocol State functions
 *          * Slaves periodic synchronization
 *        Known:
 *          * Bugs:
 *              - read out from side node (pos flags does not cleared and |'ed so after 1 && 2 you get 3)
 *              - on master connect opened some msg to slave pos+1 can be acquired
 *          * Features:
 *              - GOD_Read() does not work in block mode if multiple calls present
 * @version 0.1
 * @date 2022-10-19
 *
 *  (c) 2022
 */

#ifndef GEODE_H
#define GEODE_H

#include "main.h"

#define GOD_THREAD_FLAG (0x80U)
#define GOD_MAX_CONN    (128U)
#define GOD_MBC         (0xFFU)

typedef enum {
    GOD_OK,
    GOD_ERROR = -1,
    GOD_BUF_EMPTY = -2,
    GOD_BUF_FULL = -3
} god_stat_t;

/**
 * @brief node type whether master or slave.
 *
 */
typedef enum {
    GOD_NODE_SLAVE = 0x01,
    GOD_NODE_MASTER = GOD_MAX_CONN
} god_node_t;

typedef enum {
    GOD_NON_BLOCK,
    GOD_BLOCK = GOD_MBC
} god_mode_t;

/**
 * @brief initialize stuff used by GOD
 * @note uses cmsis os flow control API
 * @return god_stat_t
 */
god_stat_t GOD_Init();

/**
 * @brief Function for collecting announcements from masters
 * @param time Ticks
 * @return Pointer to an array with announcements. The first byte of the array stores the number of announcements
 */
uint8_t *GOD_Scan(uint32_t time);

/**
 * @brief Function for transmission of announcements by the master
 * @param time Ticks
 * @param text Message (comment) to the announcement. (max_len = PKT_SIZE - (SYNC_WORD_LEN + 1))
 * @return Status
 */
god_stat_t GOD_Announce(uint32_t time, uint8_t *text);

/**
 * @brief Frees memory of the array with announcements
 * @return Status
 */
god_stat_t GOD_DelAnnBuf();

/**
 * @brief start radio node
 * @param nt created node type
 * @param args in case of GOD_NODE_SLAVE pass 3bytes of sync word and 1byte position to connect
 * @return god_stat_t
 */
god_stat_t GOD_Start(god_node_t nt, void *args);

/**
 * @brief Function for writing data to Tx buffer
 * @param data Pointer to data
 * @param size Data size
 * @return Status
 */
god_stat_t GOD_Write(uint8_t *data, uint16_t size);

/**
 * @brief Function writes a formatted string to the buffer
 */
god_stat_t GOD_Printf(char *data, ...);

/**
 * @brief Function for reading received data.
 * @param slot Option for sender selection (sender slot selection) can take values from 0 to GOD_MAX_CONN-1.
 * The value 0xFF (GOD_MBC) can also be used to read a message from a sender
 * who has already sent a required length message.
 * @note When using the value 0xFF, the function returns the sender's slot number.
 * @param data Array for reading data
 * @param size Data size
 * @param mode Function can work in two modes.
 * (GOD_BLOCK) if data of the required length has not been received,
 * the thread will wait for data to be received.
 * (GOD_NON_BLOCK) if no data has been received, return status.
 * @return Status or sender's slot number.
 */
god_stat_t GOD_Read(uint8_t slot, uint8_t *data, uint16_t size, god_mode_t mode);

void GOD_TIM_Callback(TIM_HandleTypeDef *htim);

#endif    // GEODE_H
