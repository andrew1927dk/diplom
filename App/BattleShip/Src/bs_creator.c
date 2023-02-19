/**
 ******************************************************************************
 * @file           : bs_creator.c
 * @date           : 2022-11-11
 * @brief          : Utility file for ships handling
 ******************************************************************************
 */

#include "bs_creator.h"
#include "bs_utility.h"

static void shipNumeration(struct Ship *shipPointer);

gameStatus_t shipConstructor(struct Ship *shipPointer, const shipType_t param) {
    switch (param) {
        case SHIP_TYPE_CARRIER:
            shipPointer->shipPiece = 4;
            break;

        case SHIP_TYPE_BATTLESHIP:
            shipPointer->shipPiece = 3;
            break;

        case SHIP_TYPE_SUBMARINE:
            shipPointer->shipPiece = 2;
            break;

        case SHIP_TYPE_PATROL:
            shipPointer->shipPiece = 1;
            break;

        default:
            break;
    }

    shipPointer->shipClass = param;
    shipPointer->shipHealth = shipPointer->shipPiece;
    shipPointer->posRotate = 0;
    shipPointer->posStartX = 0;
    shipPointer->posStartY = 0;
    shipPointer->posEndX = 0;
    shipPointer->posEndY = 0;

    return GAME_OK;
}

void shipAssembly(struct Ship *shipPointer) {
    uint8_t shipTypeCounter = 0;
    uint8_t edgeCounter = 0;
    for (int i = 0; i < FLEET_SIZE; i++) {
        shipConstructor(shipPointer + i, shipTypeCounter);
        edgeCounter++;
        if (edgeCounter > shipTypeCounter) {
            shipTypeCounter++;
            edgeCounter = 0;
        }
    }
    shipNumeration(shipPointer);

    LOG_INFO("PLAYER FLEET ASSEMBLED");
}

gameStatus_t shipCommandArgs(struct Ship *shipPointer, uint8_t posX, uint8_t posY, shipRotateState_t posRotate) {
    if (posX >= TABLE_SIZE || posX < 0) {
        return GAME_X_ERROR;
    }

    if (posY >= TABLE_SIZE || posY < 0) {
        return GAME_Y_ERROR;
    }

    shipPointer->posStartX = posX;
    shipPointer->posStartY = posY;
    shipPointer->posRotate = posRotate;

    return GAME_OK;
}

void shipPositionDefault(struct Ship *shipPointer, uint8_t var) {

    switch (var) {
        case 1:
            shipCommandArgs(shipPointer, 1, 1, ROTATE_HORIZONTAL);
            shipCommandArgs(shipPointer + 1, 6, 3, ROTATE_VERTICAL);
            shipCommandArgs(shipPointer + 2, 1, 4, ROTATE_HORIZONTAL);
            shipCommandArgs(shipPointer + 3, 8, 1, ROTATE_VERTICAL);
            shipCommandArgs(shipPointer + 4, 1, 7, ROTATE_HORIZONTAL);
            shipCommandArgs(shipPointer + 5, 7, 7, ROTATE_HORIZONTAL);
            shipCommandArgs(shipPointer + 6, 8, 4, ROTATE_HORIZONTAL);
            shipCommandArgs(shipPointer + 7, 0, 9, ROTATE_HORIZONTAL);
            shipCommandArgs(shipPointer + 8, 6, 9, ROTATE_HORIZONTAL);
            shipCommandArgs(shipPointer + 9, 9, 9, ROTATE_HORIZONTAL);
            break;

        case 2:
            shipCommandArgs(shipPointer, 0, 0, ROTATE_HORIZONTAL);
            shipCommandArgs(shipPointer + 1, 3, 3, ROTATE_HORIZONTAL);
            shipCommandArgs(shipPointer + 2, 6, 6, ROTATE_HORIZONTAL);
            shipCommandArgs(shipPointer + 3, 8, 1, ROTATE_VERTICAL);
            shipCommandArgs(shipPointer + 4, 1, 5, ROTATE_VERTICAL);
            shipCommandArgs(shipPointer + 5, 4, 6, ROTATE_VERTICAL);
            shipCommandArgs(shipPointer + 6, 5, 1, ROTATE_HORIZONTAL);
            shipCommandArgs(shipPointer + 7, 0, 3, ROTATE_HORIZONTAL);
            shipCommandArgs(shipPointer + 8, 0, 9, ROTATE_HORIZONTAL);
            shipCommandArgs(shipPointer + 9, 9, 9, ROTATE_HORIZONTAL);
            break;

        default:
            break;
    }
}

void shipPositionManual(struct Ship *shipPointer, gridState_t fleetTable[][TABLE_SIZE]) {
    int key = 0;
    shipPointer->posStartX = 0;
    shipPointer->posStartY = 0;
    shipPointer->posRotate = ROTATE_HORIZONTAL;
    while (key != 10) {
        controlsInfoPlacing(shipPointer);
        showSmartMap(shipPointer->posStartX, shipPointer->posStartY, fleetTable);
        placingInfo(shipPointer);
        // INPUT HERE
        if (key == 'D' || key == 'd') {
            shipPointer->posStartX++;
            if (shipPointer->posStartX == 10) {
                shipPointer->posStartX = 0;
            }
        }
        if (key == 'A' || key == 'a') {
            if (shipPointer->posStartX == 0) {
                shipPointer->posStartX = 9;
            } else if (shipPointer->posStartX > 0) {
                shipPointer->posStartX--;
            }
        }
        if (key == 'S' || key == 's') {
            shipPointer->posStartY++;
            if (shipPointer->posStartY == 10) {
                shipPointer->posStartY = 0;
            }
        }
        if (key == 'W' || key == 'w') {
            if (shipPointer->posStartY == 0) {
                shipPointer->posStartY = 9;
            } else if (shipPointer->posStartY > 0) {
                shipPointer->posStartY--;
            }
        }
        if (key == 'H' || key == 'h') {
            shipPointer->posRotate = ROTATE_HORIZONTAL;
        }
        if ((key == 'V' || key == 'v') && shipPointer->shipClass != SHIP_TYPE_PATROL) {
            shipPointer->posRotate = ROTATE_VERTICAL;
        }
        system("clear");
    }
}

static gameStatus_t shipPlacerHorizontal(struct Ship *shipPointer, gridState_t fleetTable[][TABLE_SIZE]) {
    /* Check if ship has only 1 piece. Needed for correct behaviour */
    if (shipPointer->shipPiece != 1) {
        shipPointer->posEndX = shipPointer->posStartX + shipPointer->shipPiece - 1;
    } else {
        shipPointer->posEndX = shipPointer->posStartX;
    }

    shipPointer->posEndY = shipPointer->posStartY;
    /* Check if placed ship does not exceed game table size */
    if (shipPointer->posEndX >= TABLE_SIZE) {
        return GAME_PLACEMENT_ERROR;
    }

    /* Sequence for checking beforehand if any of the ship pieces are violating GRID_OCCUPIED or GRID_FRAME'd area */
    for (int i = shipPointer->posStartX; i <= shipPointer->posEndX; i++) {
        /* Check if current cord is GRID_OCCUPIED or GRID_FRAME'd */
        if (fleetTable[shipPointer->posStartY][i] == GRID_OCCUPIED ||
            fleetTable[shipPointer->posStartY][i] == GRID_FRAME) {
            return GAME_PLACEMENT_ERROR;
        }
    }

    /* This sequence is needed to place a column of GRID_FRAME behind the ROTATE_HORIZONTAL shipPointer */
    /* If counter == starting shipPointer position AND if estimated area is not out of bounds on X axis*/
    if ((shipPointer->posStartX) > 0) {
        if ((shipPointer->posStartY - 1) >= 0) {
            /* Place GRID_FRAME UPPER LEFT from the shipPointer end position */
            fleetTable[shipPointer->posStartY - 1][shipPointer->posStartX - 1] = GRID_FRAME;
        }

        if ((shipPointer->posStartY + 1) < TABLE_SIZE) {
            /* Place GRID_FRAME DOWN LEFT from the shipPointer end position */
            fleetTable[shipPointer->posStartY + 1][shipPointer->posStartX - 1] = GRID_FRAME;
        }

        /* Place GRID_FRAME BEFORE the shipPointer end position */
        fleetTable[shipPointer->posStartY][shipPointer->posStartX - 1] = GRID_FRAME;
    }

    /* Sequence for shipPointer placement */

    uint8_t loopCounter = shipPointer->posStartX;
    do {
        /* Place shipPointer piece */
        fleetTable[shipPointer->posStartY][loopCounter] = GRID_OCCUPIED;

        /* Check in case if area above the shipPointer piece is out of bounds */
        if ((shipPointer->posStartY - 1) >= 0) {
            /* Place GRID_FRAME above the shipPointer piece */
            fleetTable[shipPointer->posStartY - 1][loopCounter] = GRID_FRAME;
        }

        /* Check in case if area below the shipPointer piece is out of bounds */
        if ((shipPointer->posStartY + 1) < TABLE_SIZE) {
            /* Place GRID_FRAME below the shipPointer piece */
            fleetTable[shipPointer->posStartY + 1][loopCounter] = GRID_FRAME;
        }
        loopCounter++;

    } while (loopCounter <= shipPointer->posEndX);

    /* This sequence is needed to place a column of GRID_FRAME after the ROTATE_HORIZONTAL shipPointer */
    /* Check bounds first */
    if ((shipPointer->posEndX + 1) < TABLE_SIZE) {

        if ((shipPointer->posEndY - 1) >= 0) {
            /* Place GRID_FRAME UPPER RIGHT from the shipPointer end position */
            fleetTable[shipPointer->posEndY - 1][shipPointer->posEndX + 1] = GRID_FRAME;
        }

        if ((shipPointer->posEndY + 1) < TABLE_SIZE) {
            /* Place GRID_FRAME DOWN RIGHT from the shipPointer end position */
            fleetTable[shipPointer->posEndY + 1][shipPointer->posEndX + 1] = GRID_FRAME;
        }

        /* Place GRID_FRAME after the shipPointer end position */
        fleetTable[shipPointer->posEndY][shipPointer->posEndX + 1] = GRID_FRAME;
    }

    return GAME_OK;
}

static gameStatus_t shipPlacerVertical(struct Ship *shipPointer, gridState_t fleetTable[][TABLE_SIZE]) {
    if (shipPointer->shipPiece == 1) {
        return GAME_PLACEMENT_ERROR;
    }
    shipPointer->posEndX = shipPointer->posStartX;
    shipPointer->posEndY = shipPointer->posStartY + shipPointer->shipPiece - 1;
    /* Check if placed shipPointer does not exceed game table size */
    if (shipPointer->posEndY >= TABLE_SIZE) {
        return GAME_PLACEMENT_ERROR;
    }

    /* Sequence for checking beforehand if any of the ship pieces are violating GRID_OCCUPIED or GRID_FRAME'd area */
    for (int i = shipPointer->posStartY; i <= shipPointer->posEndY; i++) {
        /* Check if current cord is GRID_OCCUPIED or GRID_FRAME'd */
        if (fleetTable[i][shipPointer->posStartX] == GRID_OCCUPIED ||
            fleetTable[i][shipPointer->posStartX] == GRID_FRAME) {
            return GAME_PLACEMENT_ERROR;
        }
    }

    /* This sequence is needed to place a row of GRID_FRAME at the top of the Vertical ship */
    /* Check bounds first */
    if ((shipPointer->posStartY) > 0) {
        if ((shipPointer->posStartX - 1) >= 0) {
            /* Place GRID_FRAME UPPER LEFT from the ship end position */
            fleetTable[shipPointer->posStartY - 1][shipPointer->posStartX - 1] = GRID_FRAME;
        }

        if ((shipPointer->posStartX + 1) <= TABLE_SIZE) {
            /* Place GRID_FRAME UPPER RIGHT from the ship end position */
            fleetTable[shipPointer->posStartY - 1][shipPointer->posStartX + 1] = GRID_FRAME;
        }

        /* Place GRID_FRAME UPPER from the ship end position */
        fleetTable[shipPointer->posStartY - 1][shipPointer->posStartX] = GRID_FRAME;
    }

    /* Sequence for ship placement */
    uint8_t loopCounter = shipPointer->posStartY;
    do {
        /* Place ship piece */
        fleetTable[loopCounter][shipPointer->posStartX] = GRID_OCCUPIED;

        /* Check in case if area BEHIND the ship piece is out of bounds */
        if ((shipPointer->posStartX - 1) >= 0) {
            /* Place GRID_FRAME BEHING from the ship piece */
            fleetTable[loopCounter][shipPointer->posStartX - 1] = GRID_FRAME;
        }

        /* Check in case if area AFTER the ship piece is out of bounds */
        if ((shipPointer->posStartX + 1) < TABLE_SIZE) {
            /* Place GRID_FRAME AFTER the ship piece */
            fleetTable[loopCounter][shipPointer->posStartX + 1] = GRID_FRAME;
        }
        loopCounter++;

    } while (loopCounter <= shipPointer->posEndY);

    /* This sequence is needed to place a row of GRID_FRAME below the Vertical ship */
    /* Check bounds first */
    if ((shipPointer->posEndY + 1) < TABLE_SIZE) {
        if ((shipPointer->posEndX - 1) >= 0) {
            /* Place GRID_FRAME DOWN LEFT from the ship end position */
            fleetTable[shipPointer->posEndY + 1][shipPointer->posEndX - 1] = GRID_FRAME;
        }

        if ((shipPointer->posEndX + 1) < TABLE_SIZE) {
            /* Place GRID_FRAME DOWN RIGHT from the ship end position */
            fleetTable[shipPointer->posEndY + 1][shipPointer->posEndX + 1] = GRID_FRAME;
        }

        /* Place GRID_FRAME LOWER from the ship end position */
        fleetTable[shipPointer->posEndY + 1][shipPointer->posEndX] = GRID_FRAME;
    }

    return GAME_OK;
}

gameStatus_t shipPlace(struct Ship *shipPointer, gridState_t fleetTable[][TABLE_SIZE]) {
    gameStatus_t STATE = GAME_OK;
    switch (shipPointer->posRotate) {
        case ROTATE_HORIZONTAL:
            STATE = shipPlacerHorizontal(shipPointer, fleetTable);
            break;
        case ROTATE_VERTICAL:
            STATE = shipPlacerVertical(shipPointer, fleetTable);
            break;
        default:
            break;
    }

    return STATE;
}

gameStatus_t shipPlaceAll(struct Ship *shipPointer, gridState_t fleetTable[][TABLE_SIZE]) {
    for (int i = 0; i < FLEET_SIZE; i++) {
        if (shipPlace(shipPointer + i, fleetTable) != GAME_OK) {
            return GAME_PLACEMENT_ERROR;
        }
    }
    return GAME_OK;
}

void showMap(const gridState_t fleetTable[][TABLE_SIZE]) {
    LOG_Printf("\n\n");
    LOG_Printf("\t\t\t Fleet\n");
    LOG_Printf("\t\t\t   ");

    for (int i = 0; i < TABLE_SIZE; i++) {
        LOG_Printf("\033[0;35m");
        LOG_Printf("%d ", i);
        LOG_Printf("\033[0m");
    }
    LOG_Printf("\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        LOG_Printf("\t\t\t");
        LOG_Printf("\033[0;35m");
        LOG_Printf("%d  ", i);
        LOG_Printf("\033[0m");
        for (int j = 0; j < TABLE_SIZE; j++) {
            switch (fleetTable[i][j]) {
                case GRID_EMPTY:
                    LOG_Printf("\033[0;34m");
                    LOG_Printf("~ ");
                    LOG_Printf("\033[0m");
                    break;
                case GRID_OCCUPIED:
                    LOG_Printf("\033[0;32m");
                    LOG_Printf("# ");
                    LOG_Printf("\033[0m");
                    break;
                case GRID_SHOT:
                    LOG_Printf("\033[0;31m");
                    LOG_Printf("X ");
                    LOG_Printf("\033[0m");
                    break;
                case GRID_MISS:
                    LOG_Printf("\033[0;36m");
                    LOG_Printf("O ");
                    LOG_Printf("\033[0m");
                    break;
                case GRID_FRAME:
                    LOG_Printf("\033[0;34m");
                    LOG_Printf("~ ");
                    LOG_Printf("\033[0m");
                    break;

                default:
                    break;
            }
            osDelay(1);
        }
        LOG_Printf("\n");
    }
}

void showSmartMap(const uint8_t x, const uint8_t y, const gridState_t fleetTable[][TABLE_SIZE]) {
    LOG_Printf("\n\n\n\n\n");
    LOG_Printf("\t\t\t   ");
    for (int i = 0; i < TABLE_SIZE; i++) {
        LOG_Printf("\033[0;35m");
        LOG_Printf("%d ", i);
        LOG_Printf("\033[0m");
    }
    LOG_Printf("\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        LOG_Printf("\t\t\t");
        LOG_Printf("\033[0;35m");
        LOG_Printf("%d  ", i);
        LOG_Printf("\033[0m");
        for (int j = 0; j < TABLE_SIZE; j++) {

            if (i == y && j == x) {
                LOG_Printf("\033[0;31m");
                LOG_Printf("# ");
                LOG_Printf("\033[0m");
            } else {
                switch (fleetTable[i][j]) {
                    case GRID_EMPTY:
                        LOG_Printf("\033[0;34m");
                        LOG_Printf("~ ");
                        LOG_Printf("\033[0m");
                        break;
                    case GRID_OCCUPIED:
                        LOG_Printf("\033[0;32m");
                        LOG_Printf("# ");
                        LOG_Printf("\033[0m");
                        break;
                    case GRID_SHOT:
                        LOG_Printf("\033[0;31m");
                        LOG_Printf("# ");
                        LOG_Printf("\033[0m");
                        break;
                    case GRID_MISS:
                        LOG_Printf("\033[0;36m");
                        LOG_Printf("O ");
                        LOG_Printf("\033[0m");
                        break;
                    case GRID_FRAME:
                        LOG_Printf("\033[0;33m");
                        LOG_Printf("~ ");
                        LOG_Printf("\033[0m");
                        break;

                    default:
                        break;
                }
            }
        }
        LOG_Printf("\n");
    }
}

void showRadar(const gridState_t fleetTable[][TABLE_SIZE]) {
    LOG_Printf("\n\n");
    LOG_Printf("\t\t\t   ");
    for (int i = 0; i < TABLE_SIZE; i++) {
        LOG_Printf("\033[0;35m");
        LOG_Printf("%d ", i);
        LOG_Printf("\033[0m");
    }
    LOG_Printf("\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        LOG_Printf("\t\t\t");
        LOG_Printf("\033[0;35m");
        LOG_Printf("%d  ", i);
        LOG_Printf("\033[0m");
        for (int j = 0; j < TABLE_SIZE; j++) {
            switch (fleetTable[i][j]) {
                case GRID_SHOT:
                    LOG_Printf("\033[0;31m");
                    LOG_Printf("# ");
                    LOG_Printf("\033[0m");
                    break;
                case GRID_MISS:
                    LOG_Printf("\033[0;36m");
                    LOG_Printf("O ");
                    LOG_Printf("\033[0m");
                    break;
                default:
                    LOG_Printf("\033[0;34m");
                    LOG_Printf("~ ");
                    LOG_Printf("\033[0m");
                    break;
            }
        }
        LOG_Printf("\n");
    }
}

void showSmartRadar(const uint8_t x, const uint8_t y, const gridState_t fleetTable[][TABLE_SIZE]) {

    for (int i = 0; i < TABLE_SIZE; i++) {
        LOG_Printf("\033[0;35m");
        LOG_Printf("%d ", i);
        LOG_Printf("\033[0m");
    }
    LOG_Printf("\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        LOG_Printf("\t\t\t");
        LOG_Printf("\033[0;35m");
        LOG_Printf("%d  ", i);
        LOG_Printf("\033[0m");
        for (int j = 0; j < TABLE_SIZE; j++) {

            if (i == y && j == x) {
                LOG_Printf("\033[0;32m");
                LOG_Printf("X ");
                LOG_Printf("\033[0m");
            } else {
                switch (fleetTable[i][j]) {
                    case GRID_SHOT:
                        LOG_Printf("\033[0;31m");
                        LOG_Printf("# ");
                        LOG_Printf("\033[0m");
                        break;
                    case GRID_MISS:
                        LOG_Printf("\033[0;36m");
                        LOG_Printf("O ");
                        LOG_Printf("\033[0m");
                        break;
                    default:
                        LOG_Printf("\033[0;34m");
                        LOG_Printf("~ ");
                        LOG_Printf("\033[0m");
                        break;
                }
            }
        }
        LOG_Printf("\n");
    }
}

static void shipNumeration(struct Ship *shipPointer) {
    for (int i = 0; i < FLEET_SIZE; i++) {
        shipPointer->shipNum = i;
        shipPointer->shipState = STATE_SHIP_INTACT;
        shipPointer++;
    }
}