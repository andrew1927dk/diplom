/**
 ******************************************************************************
 * @file           : bs_creator.h
 * @date           : 2022-11-11
 * @brief          : This file contains all function prototyper and structures for
 *                   file bs_creator.c
 ******************************************************************************
 */

#ifndef CREATOR_H
#define CREATOR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#include "LED.h"
#include "loglib.h"
#include "modem_adapter.h"
#include "parser.h"
#include "flash.h"
#include "rfm.h"

#define TABLE_SIZE 10
#define FLEET_SIZE 10

extern uint8_t playerOneHealth;
extern FILE   *shotPosition;
extern FILE   *shotResult;
extern FILE   *statPlayers;
extern FILE   *statShip;

/**
 * @enum gameStatus_t
 * @brief Status enum to be used as return values. For internal processes.
 */
typedef enum {
    GAME_OK,
    GAME_PLACEMENT_ERROR,
    GAME_X_ERROR,
    GAME_Y_ERROR,
    GAME_SHOT_DENIED,
    GAME_SHOT_SHIP,
    GAME_SHOT_REGISTERED,
    GAME_SHOT_MISS,
} gameStatus_t;

typedef enum {
    TURN_PLAYER1,
    TURN_PLAYER2,
    TURN_PLAYER1_GAMEOVER,
    TURN_PLAYER2_GAMEOVER,
    TURN_USEDBATON
} gameTurn_t;

/**
 * @enum shipType_t
 * @brief Status enum to be used as ship type names.
 */
typedef enum {
    SHIP_TYPE_CARRIER,       // 4 HP - 1pcs
    SHIP_TYPE_BATTLESHIP,    // 3 HP - 2pcs
    SHIP_TYPE_SUBMARINE,     // 2 HP - 3pcs
    SHIP_TYPE_PATROL,        // 1 HP - 4pcs
} shipType_t;

/**
 * @enum gridState_t
 * @brief Status enum to be used as grid area status.
 */
typedef enum {
    GRID_EMPTY,
    GRID_OCCUPIED,
    GRID_SHOT,
    GRID_MISS,
    GRID_FRAME,
} gridState_t;

/**
 * @enum gridRotateState_t
 * @brief Status enum to be used as grid area status.
 */
typedef enum {
    ROTATE_HORIZONTAL,
    ROTATE_VERTICAL,
} shipRotateState_t;

typedef enum {
    STATE_SHIP_INTACT,
    STATE_SHIP_DAMAGED,
    STATE_SHIP_DEAD,
} shipState_t;

/**
 * @struct Ship
 * @brief Structure that contains ship characteristics and position values.
 * @param shipClass : Type of the ship
 * @param shipPiece : Ship lenght
 * @param shipHealth : Ship health
 * @param posRotate : Ship's facing direction
 * @param posStartX : Ship's starting point on X-Axis
 * @param posStartY : Ship's starting point on Y-Axis
 * @param posEndX : Ship's ending point on X-Axis
 * @param posEndY : Ship's ending point on Y-Axis
 * @param shipNum : Number of the ship
 * @param shipState : State of the ship (Dead or Alive)
 */
struct Ship {
    shipType_t        shipClass;
    uint8_t           shipPiece;
    uint8_t           shipHealth;
    shipRotateState_t posRotate;
    uint8_t           posStartX;
    uint8_t           posStartY;
    uint8_t           posEndX;
    uint8_t           posEndY;
    uint8_t           shipNum;
    shipState_t       shipState;
};

/**
 * @brief This function configures ship characteristics by the type of the ship.
 * @param shipPointer : Pointer to ship structure.
 * @param param : Ship type.
 **/
gameStatus_t shipConstructor(struct Ship *shipPointer, const shipType_t param);

/**
 * @brief This function automatically sets characteristics for the ships
 * @param shipPointer : Pointer to ship structure.
 **/
void shipAssembly(struct Ship *shipPointer);

/**
 * @brief This function changes ship's cordinates and writes them into ship structure object using function arguments.
 * @param shipPointer : Pointer to fleet structure.
 * @param posX : X position of the ship.
 * @param posY : Y position of the ship.
 * @param posRotate : Facing of the ship. Can be HORIZONTAL or VERTICAL.
 **/
gameStatus_t shipCommandArgs(struct Ship *shipPointer, uint8_t posX, uint8_t posY, shipRotateState_t posRotate);

/**
 * @brief This function automatically configures all ships position and places them on the map
 * @param shipPointer : Pointer to ship structure.
 * @param var : Select preset
 **/
void shipPositionDefault(struct Ship *shipPointer, uint8_t var);

/**
 * @brief This function lets you choose the position of every ship individially by manual input.
 * @param shipPointer : Pointer to fleet structure.
 * @param fleetTable : Playing grid
 **/
void shipPositionManual(struct Ship *shipPointer, gridState_t fleetTable[][TABLE_SIZE]);

/**
 * @brief This function places individual ship on the map. It takes all parameters from Ship structure object
 * @param shipPointer : Pointer to fleet structure.
 * @param fleetTable : Playing grid
 **/
gameStatus_t shipPlace(struct Ship *shipPointer, gridState_t fleetTable[][TABLE_SIZE]);

/**
 * @brief This function places all of the ships on the map. It takes all parameters from Ship structure object
 * @param shipPointer : Pointer to fleet structure.
 * @param fleetTable : Playing grid
 **/
gameStatus_t shipPlaceAll(struct Ship *shipPointer, gridState_t fleetTable[][TABLE_SIZE]);

/**
 * @brief Used to draw a full grid with all status areas.
 * @param fleetTable : Playing grid
 **/
void showMap(const gridState_t fleetTable[][TABLE_SIZE]);

/**
 * @brief Used to draw a full grid with all status areas, with visual cursor position indication.
 * @param x : Coordinate X
 * @param y : Coordinate Y
 * @param fleetTable : Playing grid
 **/
void showSmartMap(const uint8_t x, const uint8_t y, const gridState_t fleetTable[][TABLE_SIZE]);

/**
 * @brief Used to draw a *Radar* - an alternative version of the map.
 * @param fleetTable : Playing grid
 **/
void showRadar(const gridState_t fleetTable[][TABLE_SIZE]);

/**
 * @brief Used to draw a *Radar* - an alternative version of the map, with visual cursor position indication.
 * @param x : Coordinate X
 * @param y : Coordinate Y
 * @param fleetTable : Playing grid
 **/
void showSmartRadar(const uint8_t x, const uint8_t y, const gridState_t fleetTable[][TABLE_SIZE]);

#endif    // CREATOR_H
