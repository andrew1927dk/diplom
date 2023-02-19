/**
 * @file weather_app.h
 * @brief User story: Weather
 * @date 2022-11-23
 *
 *   (c) 2022
 */

#ifndef WEATHER_APP_H_
#define WEATHER_APP_H_

#include <stdio.h>

/**
 * @brief Start weather app with console command "weather start"
 */
void WEATHER_Init(uint8_t argc, void **argv);

/**
 * @brief Exit weather app with console command "weather exit"
 */
void WEATHER_Deinit(uint8_t argc, void **argv);

/**
 * @brief Add parser command and initialize modem
 */
void WEATHER_Start();

/**
 * @brief Add new city with console command "weather add -city "your_city"". Max city name len is 20
 */
void WEATHER_AddCity(uint8_t argc, void **argv);

/**
 * @brief Delete last added city with console command "weather delete"
 */
void WEATHER_DelCity(uint8_t argc, void **argv);

/**
 * @brief View list of used city names
 */
void WEATHER_ViewCityList(uint8_t argc, void **argv);

/**
 * @brief Add weather additional city but not remember data. Console command "weather additional -city "your_city""
 */
void WEATHER_AdditionalCity(uint8_t argc, void **argv);

/**
 * @brief View weather data for all added cities with console command "weather view"
 */
void WEATHER_View(uint8_t argc, void **argv);

/**
 * @brief Erase all data with console command "weather erase"
 */
void WEATHER_Erase(uint8_t argc, void **argv);

#endif    // WEATHER_APP_H_
