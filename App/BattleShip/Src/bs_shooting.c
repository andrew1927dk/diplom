/**
 ******************************************************************************
 * @file           : bs_shooting.c
 * @date           : 2022-11-14
 * @brief          : Utility file for shooting into ships
 ******************************************************************************
 */

/* Includes */
#include "bs_shooting.h"
#include "bs_utility.h"

/**
 * @brief This function detecting position for shoot. In this function player point
 * @param shot : Pointer to shootCoord structure.
 * @param fleetTable : Fleet grid. Used to draw the fleet grid of the SENDING PLAYER
 * @param fleetRadar : Radar grid of the SENDING PLAYER
 **/
// static void shipPoint(struct shootCoord *shot, gridState_t fleetTable[][TABLE_SIZE],
//                       gridState_t fleetRadar[][TABLE_SIZE]);

/**
 * @brief This function detecting position for shoot. In this function player point
 * @param shot : Pointer to shootCoord structure.
 * @param shipPointer : Pointer to ship.
 **/
static uint8_t detectShipHor(struct shootCoord *shot, struct Ship *shipPointer);

/**
 * @brief This function detecting position for shoot. In this function player point
 * @param shot : Pointer to shootCoord structure.
 * @param shipPointer : Pointer to a ship structure.
 **/
static uint8_t detectShipVer(struct shootCoord *shot, struct Ship *shipPointer);

/**
 * @brief This function is used to show horisontal frame after ship's death
 * @param shipPointer : Pointer to ship structure.
 * @param fleetRadar : Radar grid
 */
static void showframeHor(struct packageShip *shipPointer, gridState_t fleetRadar[][TABLE_SIZE]);

/**
 * @brief This function is used to show vertical frame after ship's death
 * @param shipPointer : Pointer to ship structure.
 * @param fleetRadar : Radar grid
 */
static void showframeVer(struct packageShip *shipPointer, gridState_t fleetRadar[][TABLE_SIZE]);

gameStatus_t shootSend(gridState_t fleetTable[][TABLE_SIZE], gridState_t fleetRadar[][TABLE_SIZE], uint8_t x,
                       uint8_t y) {

    struct shootCoord  shot;
    struct shootCoord *shotPtr = &shot;
    // shipPoint(shotPtr, fleetTable, fleetRadar);
    shot.shotRes_x = x;
    shot.shotRes_y = y;
    shot.fatal = 0;
    shot.shotResRadar = GRID_EMPTY;
    if (fleetRadar[shot.shotRes_y][shot.shotRes_x] == GRID_SHOT) {
        return GAME_SHOT_DENIED;
    }
    if (fleetRadar[shot.shotRes_y][shot.shotRes_x] == GRID_MISS) {
        return GAME_SHOT_DENIED;
    }

    memoryWriteShotPos(shotPtr, sizeof(shot));

    return GAME_SHOT_REGISTERED;
}

gameStatus_t shootRecieve(gridState_t fleetTable[][TABLE_SIZE]) {

    struct shootCoord  shot = { 0 };
    struct shootCoord *shotPtr = &shot;

    memoryReadShotPos(shotPtr, sizeof(shot));
    LOG_Printf(" > Shot coordinates: (%u,%u)\n", shot.shotRes_x, shot.shotRes_y);

    if (fleetTable[shot.shotRes_y][shot.shotRes_x] == GRID_OCCUPIED) {
        return GAME_SHOT_SHIP;
    }

    return GAME_SHOT_MISS;
}

/* Function for detecting horisontal ship */
static uint8_t detectShipHor(struct shootCoord *shot, struct Ship *shipPointer) {

    uint8_t buff = shipPointer->posStartX;
    while (shipPointer->posStartX <= shipPointer->posEndX) {
        if (shot->shotRes_x == shipPointer->posStartX && shot->shotRes_y == shipPointer->posStartY) {
            shipPointer->posStartX = buff;
            if (shipPointer->posRotate != ROTATE_VERTICAL) {
                return shipPointer->shipNum;
            }
        }
        shipPointer->posStartX++;
    }
    shipPointer->posStartX = buff;

    return -1;
}

/* Function for detecting vertical ship */
static uint8_t detectShipVer(struct shootCoord *shot, struct Ship *shipPointer) {
    uint8_t buff = shipPointer->posStartY;
    while (shipPointer->posStartY <= shipPointer->posEndY) {
        if (shot->shotRes_x == shipPointer->posStartX && shot->shotRes_y == shipPointer->posStartY) {
            shipPointer->posStartY = buff;
            if (shipPointer->posRotate != ROTATE_HORIZONTAL) {
                return shipPointer->shipNum;
            }
        }
        shipPointer->posStartY++;
    }
    shipPointer->posStartY = buff;

    return -1;
}

/* Shooting function */
gridState_t shipShoot(struct Ship *shipPointer, gridState_t fleetTable[][TABLE_SIZE]) {

    gameStatus_t gridStatus = shootRecieve(fleetTable);

    if (gridStatus == GAME_SHOT_SHIP) {
        gridState_t shotBuf = GRID_SHOT;

        uint8_t damagedShip = 0;
        for (int i = 0; i < FLEET_SIZE; i++) {
            struct shootCoord  shot = { 0 };
            struct shootCoord *shotPtr = &shot;

            memoryReadShotPos(shotPtr, sizeof(shot));

            switch (shipPointer->posRotate) {
                case ROTATE_HORIZONTAL:
                    damagedShip = detectShipHor(&shot, shipPointer);
                    break;
                case ROTATE_VERTICAL:
                    damagedShip = detectShipVer(&shot, shipPointer);
                    break;
            }
            if (shipPointer->shipNum == damagedShip) {
                shipPointer->shipHealth--;

                shipPointer->shipState = STATE_SHIP_DAMAGED;
                if (shipPointer->shipHealth == 0) {
                    shipPointer->shipState = STATE_SHIP_DEAD;
                    LOG_Printf("\n \033[0;31m> Shot was fatal for the ship number %u!\033[0m\n", shipPointer->shipNum);
                    healthDecrease();
                    shotFatal();
                    LOG_Printf(" \033[0;31m> Flag for fatal shot was set to 1!\033[0m\n");
                    shipData(shipPointer);
                    LOG_Printf("\n \033[0;31m> Statistics of the damaged ship were written!\033[0m\n");
                    deathFrame(fleetTable);
                }
                return shotBuf;
            }
            shipPointer++;
        }
    }

    if (gridStatus == GAME_SHOT_MISS) {
        gridState_t shotBuf = GRID_MISS;
        return shotBuf;
    }
    return GRID_EMPTY;
}
/*Functoin for showing horisontal frame after ship's death*/
static void showframeHor(struct packageShip *shipPointer, gridState_t fleetRadar[][TABLE_SIZE]) {
    if ((shipPointer->posStartX) > 0) {
        if ((shipPointer->posStartY - 1) >= 0) {
            fleetRadar[shipPointer->posStartY - 1][shipPointer->posStartX - 1] = GRID_MISS;
        }

        if ((shipPointer->posStartY + 1) < TABLE_SIZE) {
            fleetRadar[shipPointer->posStartY + 1][shipPointer->posStartX - 1] = GRID_MISS;
        }

        fleetRadar[shipPointer->posStartY][shipPointer->posStartX - 1] = GRID_MISS;
    }
    uint8_t loopCounter = shipPointer->posStartX;
    do {
        if ((shipPointer->posStartY - 1) >= 0) {
            fleetRadar[shipPointer->posStartY - 1][loopCounter] = GRID_MISS;
        }
        if ((shipPointer->posStartY + 1) < TABLE_SIZE) {
            fleetRadar[shipPointer->posStartY + 1][loopCounter] = GRID_MISS;
        }
        loopCounter++;

    } while (loopCounter <= shipPointer->posEndX);
    if ((shipPointer->posEndX + 1) < TABLE_SIZE) {

        if ((shipPointer->posEndY - 1) >= 0) {
            fleetRadar[shipPointer->posEndY - 1][shipPointer->posEndX + 1] = GRID_MISS;
        }

        if ((shipPointer->posEndY + 1) < TABLE_SIZE) {
            fleetRadar[shipPointer->posEndY + 1][shipPointer->posEndX + 1] = GRID_MISS;
        }
        fleetRadar[shipPointer->posEndY][shipPointer->posEndX + 1] = GRID_MISS;
    }
}

/*Functoin for showing vertical frame after ship's death*/
static void showframeVer(struct packageShip *shipPointer, gridState_t fleetRadar[][TABLE_SIZE]) {

    if ((shipPointer->posStartY) > 0) {
        if ((shipPointer->posStartX - 1) >= 0) {
            fleetRadar[shipPointer->posStartY - 1][shipPointer->posStartX - 1] = GRID_MISS;
        }

        if ((shipPointer->posStartX + 1) <= TABLE_SIZE) {
            fleetRadar[shipPointer->posStartY - 1][shipPointer->posStartX + 1] = GRID_MISS;
        }

        fleetRadar[shipPointer->posStartY - 1][shipPointer->posStartX] = GRID_MISS;
    }

    uint8_t loopCounter = shipPointer->posStartY;
    do {
        if ((shipPointer->posStartX - 1) >= 0) {
            fleetRadar[loopCounter][shipPointer->posStartX - 1] = GRID_MISS;
        }

        if ((shipPointer->posStartX + 1) < TABLE_SIZE) {
            fleetRadar[loopCounter][shipPointer->posStartX + 1] = GRID_MISS;
        }
        loopCounter++;

    } while (loopCounter <= shipPointer->posEndY);

    if ((shipPointer->posEndY + 1) < TABLE_SIZE) {
        if ((shipPointer->posEndX - 1) >= 0) {
            fleetRadar[shipPointer->posEndY + 1][shipPointer->posEndX - 1] = GRID_MISS;
        }

        if ((shipPointer->posEndX + 1) < TABLE_SIZE) {
            fleetRadar[shipPointer->posEndY + 1][shipPointer->posEndX + 1] = GRID_MISS;
        }
        fleetRadar[shipPointer->posEndY + 1][shipPointer->posEndX] = GRID_MISS;
    }
}

/*Functoin for choosing frame using ship's rotation parametr*/
void deathFrame(gridState_t fleetRadar[][TABLE_SIZE]) {
    struct packageShip  ship;
    struct packageShip *shipPointer = &ship;

    memoryReadShipStat(shipPointer, sizeof(ship));

    switch (shipPointer->posRotate) {
        case ROTATE_HORIZONTAL:
            showframeHor(shipPointer, fleetRadar);
            break;
        case ROTATE_VERTICAL:
            showframeVer(shipPointer, fleetRadar);
            break;
    }
}
