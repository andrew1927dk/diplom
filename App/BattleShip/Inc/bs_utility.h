/**
 ******************************************************************************
 * @file           : bs_utility.h
 * @date           : 2022-11-24
 * @brief          : This file contains all function prototyper and structures for
 *                   file bs_utility.c
 ******************************************************************************
 */

#ifndef BS_UTILITY_H
#define BS_UTILITY_H

#include "bs_shooting.h"

/**
 * @brief Game call function
 */
void gameBattleship(uint8_t argc, void **argv);

/**
 * @struct packagePlayers
 * @brief Structure that contains player's statistics (for file save)
 * @param turnStatus : Current turn
 * @param player1Health : Health of the player 1
 * @param player2Health : Health of the player 2
 */
struct packagePlayers {
    uint8_t turnStatus;
    uint8_t player1Health;
    uint8_t player2Health;
};

/**
 * @struct packageShip
 * @brief Structure that contains ship's data (for file save)
 * @param shipClass : Type of the ship
 * @param posStartX : Ship's starting point on X-Axis
 * @param posStartY : Ship's starting point on Y-Axis
 * @param posEndX : Ship's ending point on X-Axis
 * @param posEndY : Ship's ending point on Y-Axis
 * @param posRotate : Ship's facing direction
 */
struct packageShip {
    uint8_t shipClass;
    uint8_t posStartX;
    uint8_t posStartY;
    uint8_t posEndX;
    uint8_t posEndY;
    uint8_t posRotate;
};

/**
 * @brief This function is used to write data into shipStat.bin file
 * @param shipStatistics : Pointer to packageShip structure
 */
void memoryWriteShipStat(struct packageShip *shipStatistics, uint16_t datLen);

/**
 * @brief This function is used to write data into shotPos.bin file
 * @param shootCoord : Pointer to shootCoord structure
 * @param datLen : Lenght of data
 */
void memoryWriteShotPos(struct shootCoord *shootCoord, uint16_t datLen);

/**
 * @brief This function is used to write data into statPlayers.bin file
 * @param Statistics : Pointer to packagePlayers structure]
 * @param datLen : Lenght of data
 */
void memoryWriteStatPlayers(struct packagePlayers *Statistics, uint16_t datLen);

/**
 * @brief This function is used to read data from shipStat.bin file
 * @param shipStatistics : Pointer to packageShip structure
 */
void memoryReadShipStat(struct packageShip *shipStatistics, uint16_t datLen);

/**
 * @brief This function is used to read data from shotPos.bin file
 * @param shootCoord : Pointer to shootCoord structure
 */
void memoryReadShotPos(struct shootCoord *shootCoord, uint16_t datLen);

/**
 * @brief This function is used to read data from statPlayers.bin file
 * @param Statistics : Pointer to packagePlayers structure
 */
void memoryReadStatPlayers(struct packagePlayers *Statistics, uint16_t datLen);

/**
 * @brief This function decreases player's health and writes to statPlayers.bin file
 */
void healthDecrease();

/**
 * @brief This function checks if the last shot was fatal for the ship and writes
 * data into statPlayers.bin and shipStat.bin files
 */
void shotFatal();

/**
 * @brief This function passes the turn to the next player and writes data to statPlayers.bin file
 */
void turnChange();

/**
 * @brief This function checks if the player's health is 0 and ends the game. Also writes data to statPlayers.bin file
 */
uint8_t gameOverChecker();

/**
 * @brief This function shows a results screen
 */
void resultsScreen();

/**
 * @brief This function writes ship data into shipStat.bin file
 */
void shipData(const struct Ship *shipPointer);

/**
 * @brief This function sets default health for botch players and writes to statPlayers.bin file
 */
void statCreate();

/**
 * @brief This function sets ship stats (for file use) and writes to shipStat.bin file
 */
void statShipDefault();

/**
 * @brief This function is a frame that waits for user to press ENTER button
 */
void freezeMessage();

/**
 * @brief This function draws a main menu and functional buttons
 * @param plr1FleetPtr : pointer to a PLAYER 1 fleet
 * @param plr1GameTable : a PLAYER 1 game table
 * @param plr2FleetPtr : pointer to a PLAYER 2 fleet
 * @param plr2GameTable : a PLAYER 2 game table
 */
void menuMain(struct Ship *plr1FleetPtr, gridState_t plr1GameTable[][TABLE_SIZE], struct Ship *plr2FleetPtr,
              gridState_t plr2GameTable[][TABLE_SIZE]);

/**
 * @brief Info tab in the main menu
 */
void menuInfo();

/**
 * @brief This function dynamically changes the on-screen text to aid player during the ship placement
 */
void placingInfo(const struct Ship *shipPointer);
/**
 * @brief This function is for the controls scheme. Adapts to input to aid player during the ship placement
 */
void controlsInfoPlacing(const struct Ship *shipPointer);

/**
 * @brief This function displays information about current turn
 */
void turnInfo();

/**
 * @brief This function draws a cool ship graphic
 */
void drawShip();

/**
 * @brief This function makes a turn with parameters
 * @param plr1FleetPtr : pointer to a PLAYER 1 fleet
 * @param plr1GameTable : a PLAYER 1 game table
 * @param plr1Radar : a PLAYER 1 radar
 * @param plr2FleetPtr : pointer to a PLAYER 2 fleet
 * @param plr2GameTable : a PLAYER 2 game table
 * @param plr2Radar : a PLAYER 2 radar
 * @param x : x coordinate of the shot
 * @param y : y coordinate of the shot
 */
void turnAutoPlay(struct Ship *plr1FleetPtr, gridState_t plr1GameTable[][TABLE_SIZE],
                  gridState_t plr1Radar[][TABLE_SIZE], struct Ship *plr2FleetPtr,
                  gridState_t plr2GameTable[][TABLE_SIZE], gridState_t plr2Radar[][TABLE_SIZE], uint8_t x, uint8_t y);

#endif