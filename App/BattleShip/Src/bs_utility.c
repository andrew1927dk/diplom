/**
 ******************************************************************************
 * @file           : bs_utility.c
 * @date           : 2022-11-24
 * @brief          : Utility file for file system handling
 ******************************************************************************
 */

#include "bs_utility.h"
#include "unistd.h"
#define TERMINAL_WIDTH  70
#define TERMINAL_HEIGHT 35

#define MEM_PLAYERSTAT 0
#define MEM_SHOTPOS    25
#define MEM_SHIPSTAT   60

void memoryWriteShipStat(struct packageShip *shipStatistics, uint16_t datLen) {
    FLASH_Erase(MEM_SHIPSTAT, datLen);
    if (FLASH_Write((uint8_t *) shipStatistics, MEM_SHIPSTAT, datLen) == FLASH_OK) {
        LOG_Printf("\n  \033[0;33m>> Ship statistics write - OK!\033[0m");
    }
}

void memoryWriteShotPos(struct shootCoord *shootCoord, uint16_t datLen) {
    FLASH_Erase(MEM_SHOTPOS, datLen);
    if (FLASH_Write((uint8_t *) shootCoord, MEM_SHOTPOS, datLen) == FLASH_OK) {
        LOG_Printf("\n  \033[0;33m>> Coordinates write - OK!\033[0m\n");
    }
}

void memoryWriteStatPlayers(struct packagePlayers *Statistics, uint16_t datLen) {
    FLASH_Erase(MEM_PLAYERSTAT, datLen);
    if (FLASH_Write((uint8_t *) Statistics, MEM_PLAYERSTAT, datLen) == FLASH_OK) {
        LOG_Printf("\n  \033[0;33m>> Statistics write - OK!\033[0m");
    }
}

void memoryReadShipStat(struct packageShip *shipStatistics, uint16_t datLen) {
    FLASH_Read((uint8_t *) shipStatistics, MEM_SHIPSTAT, datLen);
}

void memoryReadShotPos(struct shootCoord *shootCoord, uint16_t datLen) {
    FLASH_Read((uint8_t *) shootCoord, MEM_SHOTPOS, datLen);
}

void memoryReadStatPlayers(struct packagePlayers *Statistics, uint16_t datLen) {
    FLASH_Read((uint8_t *) Statistics, MEM_PLAYERSTAT, datLen);
}

void statCreate() {
    struct packagePlayers  Statistics;
    struct packagePlayers *StatisticsPtr = &Statistics;
    Statistics.player1Health = 10;
    Statistics.player2Health = 10;
    Statistics.turnStatus = TURN_PLAYER1;

    LOG_Printf("\n Stats creation..");

    memoryWriteStatPlayers(StatisticsPtr, sizeof(Statistics));
}

void healthDecrease() {
    struct packagePlayers  Statistics;
    struct packagePlayers *StatisticsPtr = &Statistics;
    memoryReadStatPlayers(StatisticsPtr, sizeof(Statistics));

    LOG_INFO("PLAYER'S STATISTICS CHANGE! (Health decrease)");
    if (Statistics.turnStatus == TURN_PLAYER2) {
        Statistics.player1Health--;
        LOG_Printf(" > Player 1 health: %u", Statistics.player1Health);
    } else {
        Statistics.player2Health--;
        LOG_Printf(" > Player 2 health: %u", Statistics.player2Health);
    }

    memoryWriteStatPlayers(StatisticsPtr, sizeof(Statistics));
}

void turnChange() {
    struct packagePlayers  Statistics;
    struct packagePlayers *StatisticsPtr = &Statistics;
    memoryReadStatPlayers(StatisticsPtr, sizeof(Statistics));

    switch (Statistics.turnStatus) {
        case TURN_PLAYER1:
            Statistics.turnStatus = TURN_PLAYER2;
            break;
        case TURN_PLAYER2:
            Statistics.turnStatus = TURN_PLAYER1;
            break;
        default:
            break;
    }

    memoryWriteStatPlayers(StatisticsPtr, sizeof(Statistics));
}

void shotFatal() {
    struct shootCoord  shot;
    struct shootCoord *shotPtr = &shot;
    memoryReadShotPos(shotPtr, sizeof(shot));

    if (shot.fatal != 1) {
        shot.fatal = 1;
        LOG_Printf("\n \033[0;31m> Coordinates of fatal shot: (%u,%u)\033[0m\n", shot.shotRes_x, shot.shotRes_y);
    } else {
        shot.fatal = 0;
    }

    memoryWriteShotPos(shotPtr, sizeof(shot));
}

void shipData(const struct Ship *shipPointer) {
    struct packageShip  shipBuf;
    struct packageShip *shipBufPtr = &shipBuf;
    shipBuf.posStartX = shipPointer->posStartX;
    shipBuf.posEndX = shipPointer->posEndX;

    shipBuf.posStartY = shipPointer->posStartY;
    shipBuf.posEndY = shipPointer->posEndY;

    shipBuf.shipClass = shipPointer->shipClass;
    shipBuf.posRotate = shipPointer->posRotate;

    memoryWriteShipStat(shipBufPtr, sizeof(shipBuf));
}

void statShipDefault() {
    struct shootCoord  shot;
    struct shootCoord *shotPtr = &shot;
    shot.fatal = 0;
    shot.shotRes_x = 0;
    shot.shotRes_y = 0;

    // fileWriteShotPos(shotPtr);
    memoryWriteShotPos(shotPtr, sizeof(shot));
}

uint8_t gameOverChecker() {
    struct packagePlayers  Statistics;
    struct packagePlayers *StatisticsPtr = &Statistics;
    memoryReadStatPlayers(StatisticsPtr, sizeof(Statistics));

    if (Statistics.player1Health == 0) {
        Statistics.turnStatus = TURN_PLAYER1_GAMEOVER;

        memoryWriteStatPlayers(StatisticsPtr, sizeof(Statistics));

        return 1;
    } else if (Statistics.player2Health == 0) {
        Statistics.turnStatus = TURN_PLAYER2_GAMEOVER;

        memoryWriteStatPlayers(StatisticsPtr, sizeof(Statistics));

        return 1;
    }

    return 0;
}

void resultsScreen() {
    struct packagePlayers  Statistics;
    struct packagePlayers *StatisticsPtr = &Statistics;
    memoryReadStatPlayers(StatisticsPtr, sizeof(Statistics));

    if (Statistics.turnStatus == TURN_PLAYER1_GAMEOVER) {
        LOG_Printf("\n\t\t\t   ----- RESULTS ----- \t\t\t\t");
        LOG_Printf("\n\t\t\t      Player 2 Won! \t\t\t\t");
        LOG_Printf("\n\t        Thanks for playing! See you on the screen :)\n\n\n\n");
    }

    if (Statistics.turnStatus == TURN_PLAYER2_GAMEOVER) {
        LOG_Printf("\n\t\t\t   ----- RESULTS ----- \t\t\t\t");
        LOG_Printf("\n\t\t\t      Player 1 Won! \t\t\t\t");
        LOG_Printf("\n\t        Thanks for playing! See you on the screen :)\n\n\n\n");
    }
}

void freezeMessage() {
    int key = 0;
    while (key != 10) {
        // INPUT HERE
    }
    system("clear");
}

void menuInfo() {
    printf("\n\t This is the console version of the Battleship app");
    printf("\n\t Play fair and have fun!\n\n\n\n\n");
    drawShip();
    freezeMessage();
}

void turnInfo() {
    struct packagePlayers  Statistics;
    struct packagePlayers *StatisticsPtr = &Statistics;
    memoryReadStatPlayers(StatisticsPtr, sizeof(Statistics));

    switch (Statistics.turnStatus) {
        case TURN_PLAYER1:
            printf("\n\t\t\t --- PLAYER 1 TURN ---");
            printf("\n Ships intact: %d ", Statistics.player1Health);
            printf("\n Enemy ships destroyed: %d", 10 - Statistics.player2Health);
            break;
        case TURN_PLAYER2:
            printf("\n\t\t\t --- PLAYER 2 TURN ---");
            printf("\n Ships intact: %d ", Statistics.player2Health);
            printf("\n Enemy ships destroyed: %d", 10 - Statistics.player1Health);
            break;
        default:
            break;
    }
}

void placingInfo(const struct Ship *shipPointer) {
    printf("\n Rotation: ");
    switch (shipPointer->posRotate) {
        case 0:
            printf("Horizontal");
            break;
        case 1:
            printf("Vertical");
            break;
        default:
            break;
    }
    printf("\n Now placing: ");
    switch (shipPointer->shipClass) {
        case 0:
            printf("Carrier");
            break;
        case 1:
            printf("Battleship");
            break;
        case 2:
            printf("Submarine");
            break;
        case 3:
            printf("Patrol");
            break;
        default:
            break;
    }
    printf("\n\n    ");
    for (int i = 0; i < shipPointer->shipPiece; i++) {

        if (shipPointer->posRotate == 0) {
            printf("\033[0;32m");
            printf("# ");
            printf("\033[0m");
        } else {
            printf("\033[0;32m");
            printf("#\n");
            printf("\033[0m");
            printf("    ");
        }
    }
}

void controlsInfoPlacing(const struct Ship *shipPointer) {
    printf("\n\t\t\t--- SHIPS PLACEMENT --- \n");
    printf(" \n Move cursor with [W][A][S][D] buttons.");
    switch (shipPointer->posRotate) {
        case 0:
            if (shipPointer->shipClass != SHIP_TYPE_PATROL) {
                printf(" \n Press [V] button to make ship vertical.");
            } else {
                printf("\n");
            }
            break;
        case 1:
            printf(" \n Press [H] button to make ship horizontal.");
            break;
        default:
            break;
    }
}

void drawShip() {
    LOG_Printf("\n\n\n\n");
    LOG_Printf("\t\t\t\t            +\n");
    LOG_Printf("\t\t\t\t          ++\n");
    LOG_Printf("\t\t\t\t         +\n");
    LOG_Printf("\t\t\t\t        ||\n");
    LOG_Printf("\t\t\t\t        ||\n");
    LOG_Printf("\t\t\t\t----------------\n");
    LOG_Printf("\t\t\t\t\\    @         |\n");
    for (int i = 0; i < 35; i++) {
        LOG_Printf("\033[0;34m");
        LOG_Printf("~-");
        LOG_Printf("\033[0m");
    }
}

void turnAutoPlay(struct Ship *plr1FleetPtr, gridState_t plr1GameTable[][TABLE_SIZE],
                  gridState_t plr1Radar[][TABLE_SIZE], struct Ship *plr2FleetPtr,
                  gridState_t plr2GameTable[][TABLE_SIZE], gridState_t plr2Radar[][TABLE_SIZE], uint8_t x, uint8_t y) {
    struct packagePlayers  Statistics;
    struct packagePlayers *pStatistics = &Statistics;
    memoryReadStatPlayers(pStatistics, sizeof(Statistics));

    LOG_Printf("\n\n\t\t\t\033[0;35m ---- PLAYER %u TURN! ---- \033[0m\t\t\n\n", Statistics.turnStatus + 1);

    struct shootCoord  shot = { 0 };
    struct shootCoord *shotPtr = &shot;
    gridState_t        radarBuf = GRID_EMPTY;

    switch (pStatistics->turnStatus) {
        case TURN_PLAYER1:

            LOG_INFO("PLAYER 1 IS SENDING SHOT");
            while (shootSend(plr1GameTable, plr1Radar, x, y) != GAME_SHOT_REGISTERED) {
            }

            LOG_INFO("PLAYER 2 IS RECIEVING SHOT");
            if (shootRecieve(plr2GameTable) == GAME_SHOT_SHIP) {
                LOG_INFO("PLAYER 2 HAS RECIEVED SHOT. PLAYER 1 BOMBED SHIP.");
                radarBuf = shipShoot(plr2FleetPtr, plr2GameTable);

                memoryReadShotPos(shotPtr, sizeof(shot));
                plr2GameTable[shot.shotRes_y][shot.shotRes_x] = GRID_SHOT;
                shot.shotResRadar = radarBuf;
                memoryWriteShotPos(shotPtr, sizeof(shot));
                LOG_Printf(" \033[0;31m> PLAYER 2 wrote %u into a GRID_STATUS!\033[0m\n", GRID_SHOT);
                LOG_INFO("PLAYER 2 HAS SENT A RESPONSE.");

                if (shot.fatal == 1) {
                    deathFrame(plr1Radar);
                    shotFatal();
                    LOG_Printf(" \033[0;31m> Flag for fatal shot was set to 0 by PLAYER 1!\033[0m\n");
                }
            } else {
                LOG_INFO("PLAYER 2 HAS RECIEVED SHOT. PLAYER 1 MISSED");
                memoryReadShotPos(shotPtr, sizeof(shot));

                plr2GameTable[shot.shotRes_y][shot.shotRes_x] = GRID_MISS;
                shot.shotResRadar = GRID_MISS;
                memoryWriteShotPos(shotPtr, sizeof(shot));
                LOG_INFO("PLAYER 2 HAS SENT A RESPONSE.");
                turnChange();
            }

            memoryReadShotPos(shotPtr, sizeof(shot));
            plr1Radar[shot.shotRes_y][shot.shotRes_x] = shot.shotResRadar;
            break;
        case TURN_PLAYER2:

            LOG_INFO("PLAYER 2 IS SENDING SHOT");
            while (shootSend(plr2GameTable, plr2Radar, x, y) != GAME_SHOT_REGISTERED) {
            }

            LOG_INFO("PLAYER 1 IS RECIEVING SHOT");
            if (shootRecieve(plr1GameTable) == GAME_SHOT_SHIP) {
                LOG_INFO("PLAYER 1 HAS RECIEVED SHOT. PLAYER 2 BOMBED SHIP.");
                radarBuf = shipShoot(plr1FleetPtr, plr1GameTable);
                memoryReadShotPos(shotPtr, sizeof(shot));

                plr1GameTable[shot.shotRes_y][shot.shotRes_x] = GRID_SHOT;
                shot.shotResRadar = radarBuf;
                LOG_Printf("\033[0;31m> PLAYER 1 wrote %u into a GRID_STATUS!\033[0m\n", GRID_SHOT);
                memoryWriteShotPos(shotPtr, sizeof(shot));
                LOG_INFO("PLAYER 1 HAS SENT A RESPONSE.");

                if (shot.fatal == 1) {
                    deathFrame(plr2Radar);
                    shotFatal();
                    LOG_Printf("\033[0;31m> Flag for fatal shot was set to 0 by PLAYER 1!\033[0m\n");
                }
            } else {
                LOG_INFO("PLAYER 1 HAS RECIEVED SHOT. PLAYER 2 MISSED");
                memoryReadShotPos(shotPtr, sizeof(shot));

                plr1GameTable[shot.shotRes_y][shot.shotRes_x] = GRID_MISS;
                shot.shotResRadar = GRID_MISS;
                memoryWriteShotPos(shotPtr, sizeof(shot));
                turnChange();
                LOG_INFO("PLAYER 1 HAS SENT A RESPONSE.");
            }

            memoryReadShotPos(shotPtr, sizeof(shot));
            plr2Radar[shot.shotRes_y][shot.shotRes_x] = shot.shotResRadar;
        default:
            break;
    }

    LOG_Printf("\n Player 1 Grid:");
    showMap(plr1GameTable);
    LOG_Printf("\n");
    LOG_Printf("\n Player 1 Radar:");
    showMap(plr1Radar);
    LOG_Printf("\n");
    LOG_Printf("\n Player 2 Grid:");
    showMap(plr2GameTable);
    LOG_Printf("\n Player 2 Radar:");
    showMap(plr2Radar);
    LOG_Printf("\n");

    LOG_Printf("\n\n\t\t\t\033[0;35m ---- PLAYER %u TURN END! ---- \033[0m\t\t\n\n", Statistics.turnStatus + 1);

    LOG_Printf("\n\n\t\t\t\033[0;36m  ---- TURN RESULTS ---- \033[0m\t\t\n\n");
    memoryReadStatPlayers(pStatistics, sizeof(Statistics));
    LOG_INFO("READ PLAYERS STATISTICS");
    LOG_Printf(" > Player 1 HP: %u", Statistics.player1Health);
    LOG_Printf("\n > Player 2 HP: %u", Statistics.player2Health);
    LOG_Printf("\n > Current turn: %u\n", Statistics.turnStatus);
    LOG_INFO("END PLAYERS STATISTICS");
    LOG_Printf("\n\n\t\t\t\033[0;36m  ---- TURN RESULTS ---- \033[0m\t\t\n\n");
    gameOverChecker();
}