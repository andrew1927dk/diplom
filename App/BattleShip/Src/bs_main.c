/**
 ******************************************************************************
 * @file           : bs_main.c
 * @date           : 2022-11-11
 * @brief          : Main game file
 ******************************************************************************
 */

#include "bs_utility.h"

void gameBattleship(uint8_t argc, void **argv) {
    drawShip();
    osDelay(1000);
    struct Ship  plr1Fleet[FLEET_SIZE] = { 0 };
    struct Ship *plr1FleetPtr = &plr1Fleet[0];

    gridState_t plr1GameTable[TABLE_SIZE][TABLE_SIZE] = { { 0 }, { 0 } };
    gridState_t plr1Radar[TABLE_SIZE][TABLE_SIZE] = { { 0 }, { 0 } };

    struct Ship  plr2Fleet[FLEET_SIZE] = { 0 };
    struct Ship *plr2FleetPtr = &plr2Fleet[0];

    gridState_t plr2GameTable[TABLE_SIZE][TABLE_SIZE] = { { 0 }, { 0 } };
    gridState_t plr2Radar[TABLE_SIZE][TABLE_SIZE] = { { 0 }, { 0 } };

    statCreate();
    statShipDefault();

    memset(plr1GameTable, 0, sizeof(plr1GameTable));
    memset(plr1Radar, 0, sizeof(plr1Radar));
    shipAssembly(plr1Fleet);

    memset(plr2GameTable, 0, sizeof(plr2GameTable));
    shipAssembly(plr2Fleet);

    shipPositionDefault(plr1FleetPtr, 1);
    shipPositionDefault(plr2FleetPtr, 2);

    shipPlaceAll(plr1FleetPtr, plr1GameTable);
    shipPlaceAll(plr2FleetPtr, plr2GameTable);

    LOG_Printf("\n Player 1 Grid:");
    showMap(plr1GameTable);
    LOG_Printf("\n");
    LOG_Printf("\n Player 2 Grid:");
    showMap(plr2GameTable);
    LOG_Printf("\n\n");

    LOG_INFO("GAME BEGIN");
    LOG_Printf("\n\n");

    memset(plr1Radar, 0, sizeof(plr1Radar));
    memset(plr2Radar, 0, sizeof(plr2Radar));

    /* PLAYER 1 turn sequence */
    for (uint8_t i = 0; i < 4; i++) {
        turnAutoPlay(plr1FleetPtr, plr1GameTable, plr1Radar, plr2FleetPtr, plr2GameTable, plr2Radar, i, 0);
    }

    for (uint8_t i = 0; i < 2; i++) {
        turnAutoPlay(plr1FleetPtr, plr1GameTable, plr1Radar, plr2FleetPtr, plr2GameTable, plr2Radar, 8, i + 1);
        turnAutoPlay(plr1FleetPtr, plr1GameTable, plr1Radar, plr2FleetPtr, plr2GameTable, plr2Radar, 1, i + 5);
        turnAutoPlay(plr1FleetPtr, plr1GameTable, plr1Radar, plr2FleetPtr, plr2GameTable, plr2Radar, 4, i + 6);
    }

    for (uint8_t i = 0; i < 3; i++) {
        turnAutoPlay(plr1FleetPtr, plr1GameTable, plr1Radar, plr2FleetPtr, plr2GameTable, plr2Radar, i + 3, 3);
        turnAutoPlay(plr1FleetPtr, plr1GameTable, plr1Radar, plr2FleetPtr, plr2GameTable, plr2Radar, i + 6, 6);
    }

    turnAutoPlay(plr1FleetPtr, plr1GameTable, plr1Radar, plr2FleetPtr, plr2GameTable, plr2Radar, 5, 1);
    turnAutoPlay(plr1FleetPtr, plr1GameTable, plr1Radar, plr2FleetPtr, plr2GameTable, plr2Radar, 0, 3);
    turnAutoPlay(plr1FleetPtr, plr1GameTable, plr1Radar, plr2FleetPtr, plr2GameTable, plr2Radar, 0, 8);

    /* PLAYER 2 turn sequence */
    for (uint8_t i = 0; i < 4; i++) {
        turnAutoPlay(plr1FleetPtr, plr1GameTable, plr1Radar, plr2FleetPtr, plr2GameTable, plr2Radar, i + 1, 1);
    }
    for (uint8_t i = 0; i < 3; i++) {
        turnAutoPlay(plr1FleetPtr, plr1GameTable, plr1Radar, plr2FleetPtr, plr2GameTable, plr2Radar, i + 2, 4);
    }

    /* PLAYER 1 turn sequence */
    turnAutoPlay(plr1FleetPtr, plr1GameTable, plr1Radar, plr2FleetPtr, plr2GameTable, plr2Radar, 0, 9);
    turnAutoPlay(plr1FleetPtr, plr1GameTable, plr1Radar, plr2FleetPtr, plr2GameTable, plr2Radar, 9, 9);

    resultsScreen();
}
