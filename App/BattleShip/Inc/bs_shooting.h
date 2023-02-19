/**
 ******************************************************************************
 * @file           : bs_shooting.h
 * @date           : 2022-11-14
 * @brief          : This file contains all function prototyper and structures for
 *                   file bs_shooting.c
 ******************************************************************************
 */

#ifndef BS_SHOOTING_H
#define BS_SHOOTING_H

#include "bs_creator.h"

/**
 * @struct shootCord
 * @brief Structure that contains shot coordinates
 * @param fatal : for signaling if the latest shot for fatal for the ship
 * @param shotRes_x; : X Coordinate
 * @param shotRes_y : Y Coordinate
 */
struct shootCoord {
    uint8_t fatal;
    uint8_t shotResRadar;
    uint8_t shotRes_x;
    uint8_t shotRes_y;
};

/**
 * @brief This function is used to choose frame using ship's rotation parametr
 * @param fleetRadar : Radar grid
 */
void deathFrame(gridState_t fleetRadar[][TABLE_SIZE]);

/**
 * @brief This function is used to "send" information about the PLAYER choice for where to shoot
 * @param fleetTable : Fleet grid
 * @param fleetRadar : Radar grid
 */
gameStatus_t shootSend(gridState_t fleetTable[][TABLE_SIZE], gridState_t fleetRadar[][TABLE_SIZE], uint8_t x,
                       uint8_t y);

/**
 * @brief This function is used for PLAYER to "recieve" the information about the other PLAYER's choice for where to
 * shoot
 * @param fleetRadar : Radar grid
 */
gameStatus_t shootRecieve(gridState_t fleetRadar[][TABLE_SIZE]);
/**
 * @brief This function is a set of algorithms to detect and damage the enemy ship.
 * Just point and shoot.
 * @param shipPointer : Pointer to ship structure.
 * @param fleetTable : a recieveing PLAYER fleet grid
 */
gridState_t shipShoot(struct Ship *shipPointer, gridState_t fleetTable[][TABLE_SIZE]);

#endif    // BS_SHOOTING_H