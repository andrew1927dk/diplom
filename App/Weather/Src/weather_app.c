/**
 * @file weather_app.c
 * @brief User story: Weather
 * @date 2022-11-23
 *
 *   (c) 2022
 */
#include "weather_app.h"
// #include "cmsis_os.h"
#include "loglib.h"
#include "parser.h"
#include "modem.h"
#include "modem_command_gen.h"
#include "modem_urc_parser.h"
#include "modem_adapter.h"
#include <string.h>
#include "flash.h"

#define MAX_ARRAY_CITY_SIZE  (3)
#define MAX_CITY_NAME_SIZE   (20)
#define USER_ADD_CITY_FLAG   (0x0001)
#define ADDITIONAL_CITY_FLAG (0x0010)
#define THREAD_EXIT_FLAG     (0x0100)

typedef enum {
    ADD_CITY_OK,
    ADD_CITY_ERROR
} weather_addCity_t;

// Day weather parameters
struct weather_day_t {
    char date[11];
    char tempMax[6];
    char tempMin[6];
    char status[20];
};

// Current weather parameters
struct weather_current_t {
    char temp[6];
    char status[20];
};

// Hour weather parameters
struct weather_hour_t {
    char date[9];
    char temp[6];
    char status[20];
};

// Whole city structure
struct weather_city_t {
    char name[MAX_CITY_NAME_SIZE];

    struct weather_current_t curr;
    struct weather_hour_t    hrs[6];
    struct weather_day_t     days[3];
};
struct weather_city_t WEATHER_CityArray[MAX_ARRAY_CITY_SIZE];
uint8_t               currCityArrayLen = 0;
char                  cityName[MAX_CITY_NAME_SIZE] = { 0 };

uint8_t isTurnedOn = 0;

/**
 * @brief Make request to server
 * @param link Https link
 * @param destination Array where result will be putted
 */
static void makeRequest(char *link, char *destination);

/**
 * @brief View all available weather data
 * @param city City array
 * @param currLen Number of cities to be viewed
 */
static void viewCity(struct weather_city_t *city, uint8_t currLen);

/**
 * @brief View days weather data
 * @param days Days array
 * @param dayNum Number of days to be viewed
 */
static void viewDays(struct weather_day_t days[], uint8_t dayNum);

/**
 * @brief View current weather data
 * @param curr Current weather structure
 */
static void viewCurrent(struct weather_current_t *curr);

/**
 * @brief View hours weather data
 * @param hours Hours array
 * @param hrsNum Number of hours to be viewed
 */
static void viewHours(struct weather_hour_t hours[], uint8_t hrsNum);

/**
 * @brief Get data for chosen city
 * @param city Element of city array
 * @return ADD_CITY_OK if everything is good and ADD_CITY_ERROR if we cannot retrieve info from server
 */
static weather_addCity_t getCity(uint32_t flag);

// Fill data for different time ranges
static void getDays(struct weather_day_t days[], char *info);
static void getCurrent(struct weather_current_t *curr, char *info);
static void getHours(struct weather_hour_t hours[], char *info);

/**
 * @brief Write data to flash memory
 */
static void flashCity();

/**
 * @brief Erase all data
 */
static void eraseWeather();

osThreadId_t         weatherTaskHandle;
const osThreadAttr_t weatherTask_attr = {
    .name = "weatherTask",
    .stack_size = 1024 * 4,
    .priority = (osPriority_t) osPriorityNormal,
};

osEventFlagsId_t         userActionEventHandle;
const osEventFlagsAttr_t userActionEvent_attr = {
    .name = "userActionEvent",
};

void WEATHER_Start() {
    osDelay(100);
    PARSER_AddCommand(WEATHER_Init, "weather start");
}

void startWeatherTask(void *argument) {
    osDelay(100);

    LOG_INFO("WEATHER APP IS READY");

    FLASH_Read(&currCityArrayLen, 0x3E0000, sizeof(uint8_t));
    // Check if data is corrupted
    if (currCityArrayLen > 3) {
        eraseWeather();
    }

    if (currCityArrayLen != 0) {
        FLASH_Read((uint8_t *) WEATHER_CityArray, 0x3E0000 + sizeof(uint8_t),
                   sizeof(struct weather_city_t) * MAX_ARRAY_CITY_SIZE);
        osDelay(100);

        viewCity(WEATHER_CityArray, currCityArrayLen);
    }

    for (;;) {
        osDelay(100);
        uint32_t flag =
            osEventFlagsWait(userActionEventHandle, USER_ADD_CITY_FLAG | ADDITIONAL_CITY_FLAG | THREAD_EXIT_FLAG,
                             osFlagsWaitAny, osWaitForever);

        if (flag == THREAD_EXIT_FLAG) {
            osThreadExit();
        }

        if (getCity(flag) == ADD_CITY_ERROR) {
            LOG_ERROR("Unable receive data from server");
            continue;
        }
    }
}

void WEATHER_Init(uint8_t argc, void **argv) {
    static uint8_t firstInit = 1;

    // We don`t need to add parser commands again
    if (firstInit) {
        PARSER_AddCommand(WEATHER_AddCity, "weather add -city 0");
        PARSER_AddCommand(WEATHER_DelCity, "weather delete -city 0");
        PARSER_AddCommand(WEATHER_Deinit, "weather exit");
        PARSER_AddCommand(WEATHER_ViewCityList, "weather list");
        PARSER_AddCommand(WEATHER_AdditionalCity, "weather additional -city 0");
        PARSER_AddCommand(WEATHER_View, "weather view");
        PARSER_AddCommand(WEATHER_Erase, "weather erase");

        firstInit = 0;
    }

    // check if app is turned on/off
    if (!isTurnedOn) {
        userActionEventHandle = osEventFlagsNew(&userActionEvent_attr);
        weatherTaskHandle = osThreadNew(startWeatherTask, NULL, &weatherTask_attr);
    }
    isTurnedOn = 1;
}

void WEATHER_Deinit(uint8_t argc, void **argv) {
    osEventFlagsSet(userActionEventHandle, THREAD_EXIT_FLAG);
    osDelay(50);
    osEventFlagsDelete(userActionEventHandle);

    isTurnedOn = 0;
}

void makeRequest(char *link, char *destination) {
    char req[23] = { 0 };
    sprintf(req, "AT+QHTTPURL=%d", strlen(link));
    MOD_SendATCommand(req);
    MOD_SendATCommand("\r");
    osDelay(100);

    MOD_SendATCommand(link);
    MOD_SendATCommand("\032");    // modem is waiting for combination ctrl+z to end input
    osDelay(100);

    MOD_SendATCommand("AT+QHTTPGET\r");
    osDelay(4000);
    MOD_SendATCommand("AT+QHTTPREAD\r");
    osDelay(2000);

    MOD_GetResult(destination);
}

static void viewDays(struct weather_day_t days[], uint8_t dayNum) {
    LOG_Printf("\n");

    for (uint8_t i = 0; i < dayNum; ++i) {
        LOG_Printf("Date - %s\n\tTemp max - %s\n\tTemp min - %s\n\tStatus - %s\n", days[i].date, days[i].tempMax,
                   days[i].tempMin, days[i].status);
        osDelay(10);
    }
}

static void getDays(struct weather_day_t days[], char *info) {
    char *token = strtok(info, "\n");

    uint8_t i = 0;
    while (token != NULL) {

        token = strtok(NULL, ",\n");
        strcpy(days[i].date, token);

        token = strtok(NULL, ",\n");
        strcpy(days[i].tempMax, token);

        token = strtok(NULL, ",\n");
        strcpy(days[i].tempMin, token);

        token = strtok(NULL, ",\n");
        strcpy(days[i].status, token);

        ++i;
    }
}

static void getCurrent(struct weather_current_t *curr, char *info) {
    char *token = strtok(info, "\n");
    token = strtok(NULL, ",");

    token = strtok(NULL, ",");
    strcpy(curr->temp, token);

    token = strtok(NULL, ",");
    strcpy(curr->status, token);
}

static void viewCurrent(struct weather_current_t *curr) {
    LOG_Printf("\nCurrent\n\tTemp - %s\n\tStatus - %s\n", curr->temp, curr->status);
    osDelay(10);
}

static void getHours(struct weather_hour_t hours[], char *info) {
    char *token = strtok(info, "\n");

    uint8_t i = 0;
    uint8_t j = 3;
    while (token != NULL) {
        // Add only every 4th hour including first
        if (j != 3) {
            token = strtok(NULL, "\n");
            ++j;

            continue;
        }

        token = strtok(NULL, ",\n");
        if (token == NULL) {
            continue;
        }
        strcpy(hours[i].date, token + 11);

        token = strtok(NULL, ",\n");
        strcpy(hours[i].temp, token);

        token = strtok(NULL, ",\n");
        strcpy(hours[i].status, token);

        j = 0;
        ++i;
    }
}

static void viewHours(struct weather_hour_t hours[], uint8_t hrsNum) {
    LOG_Printf("\nToday\n");

    for (uint8_t i = 0; i < hrsNum; ++i) {
        LOG_Printf("Time - %.5s\n\tTemp - %s\n\tStatus - %s\n", hours[i].date, hours[i].temp, hours[i].status);
        osDelay(10);
    }
}

static weather_addCity_t getCity(uint32_t flag) {
    char fullLink[255] = { 0 };
    char baseLink[] = "https://weather.visualcrossing.com/VisualCrossingWebServices/rest/services/timeline/";
    char param1Link[] = "?unitGroup=metric&elements=datetime";
    char param2Link[] = "%2Cicon&key=5SHEEEJK5EXUJYNH9E22TQ6P9&contentType=csv&include=";

    char  result[1200] = { 0 };
    char *info = NULL;

    struct weather_city_t  additionalCity;
    struct weather_city_t *city = NULL;

    if (flag == USER_ADD_CITY_FLAG) {
        city = &WEATHER_CityArray[currCityArrayLen];
    } else {
        city = &additionalCity;
    }

    strcpy(city->name, strupr(cityName));

    // Form link
    sprintf(fullLink, "%s%s,ua/today%s%%2Ctemp%scurrent", baseLink, cityName, param1Link, param2Link);
    makeRequest(fullLink, result);
    info = strstr(result, "datetime");    // Check if we get correct data
    if (info == NULL) {
        return ADD_CITY_ERROR;
    }
    getCurrent(&city->curr, info);

    sprintf(fullLink, "%s%s,ua/next2days%s%%2Ctempmax%%2Ctempmin%sdays", baseLink, cityName, param1Link, param2Link);
    makeRequest(fullLink, result);
    info = strstr(result, "datetime");
    if (info == NULL) {
        return ADD_CITY_ERROR;
    }
    getDays(city->days, info);

    sprintf(fullLink, "%s%s,ua/today%s%%2Ctemp%shours", baseLink, cityName, param1Link, param2Link);
    makeRequest(fullLink, result);
    info = strstr(result, "datetime");
    if (info == NULL) {
        return ADD_CITY_ERROR;
    }
    getHours(city->hrs, info);

    if (flag == USER_ADD_CITY_FLAG) {
        ++currCityArrayLen;

        // Write new data to flash memory
        flashCity();
        viewCity(WEATHER_CityArray, currCityArrayLen);
    } else {
        viewCity(&additionalCity, 1);
    }
    return ADD_CITY_OK;
}

static void viewCity(struct weather_city_t *city, uint8_t currLen) {

    for (uint8_t i = 0; i < currLen; ++i) {
        LOG_Printf("\n%s:\n", city[i].name);

        viewCurrent(&city[i].curr);
        viewDays(city[i].days, 3);
        viewHours(city[i].hrs, 6);
    }
}

void WEATHER_AddCity(uint8_t argc, void **argv) {
    // check if app is turned on/off
    if (!isTurnedOn) {
        return;
    }

    if (currCityArrayLen == MAX_ARRAY_CITY_SIZE) {
        LOG_WARN("Cannot add more cities");
        return;
    }

    strcpy(cityName, (char *) argv[0]);

    osEventFlagsSet(userActionEventHandle, USER_ADD_CITY_FLAG);
}

void WEATHER_DelCity(uint8_t argc, void **argv) {
    // check if app is turned on/off
    if (!isTurnedOn) {
        return;
    }

    if (currCityArrayLen == 0) {
        LOG_WARN("There is nothing to delete");
        return;
    }

    uint8_t foundFlag = 0;
    for (uint8_t i = 0; i < currCityArrayLen; ++i) {

        if (strcmp(WEATHER_CityArray[i].name, strupr((char *) argv[0])) != 0) {
            continue;
        }
        ++foundFlag;

        for (uint8_t j = i + 1; j < currCityArrayLen; ++j) {
            memcpy(&WEATHER_CityArray[j - 1], &WEATHER_CityArray[j], sizeof(struct weather_city_t));
        }
        break;
    }

    if (!foundFlag) {
        LOG_WARN("There is no city with such name");
        return;
    }

    --currCityArrayLen;
    flashCity();

    viewCity(WEATHER_CityArray, currCityArrayLen);
}

void WEATHER_ViewCityList(uint8_t argc, void **argv) {
    // check if app is turned on/off
    if (!isTurnedOn) {
        return;
    }

    LOG_Printf("\n%d/%d cities added:\n", currCityArrayLen, MAX_ARRAY_CITY_SIZE);
    for (uint8_t i = 0; i < currCityArrayLen; ++i) {
        LOG_Printf("\t%d. %s\n", i + 1, WEATHER_CityArray[i].name);
    }
}

void WEATHER_AdditionalCity(uint8_t argc, void **argv) {
    // check if app is turned on/off
    if (!isTurnedOn) {
        return;
    }

    strcpy(cityName, (char *) argv[0]);
    osEventFlagsSet(userActionEventHandle, ADDITIONAL_CITY_FLAG);
}

void WEATHER_View(uint8_t argc, void **argv) {
    // check if app is turned on/off
    if (!isTurnedOn) {
        return;
    }
    viewCity(WEATHER_CityArray, currCityArrayLen);
}

static void flashCity() {
    FLASH_ChangeProtectionStatus(FLASH_BLNONE);

    FLASH_Erase(0x3E0000, sizeof(struct weather_city_t) * MAX_ARRAY_CITY_SIZE + sizeof(uint8_t));
    FLASH_Write(&currCityArrayLen, 0x3E0000, sizeof(uint8_t));
    FLASH_Write((uint8_t *) WEATHER_CityArray, 0x3E0000 + sizeof(uint8_t),
                sizeof(struct weather_city_t) * MAX_ARRAY_CITY_SIZE);

    FLASH_ChangeProtectionStatus(FLASH_BL62to63);
}

void WEATHER_Erase(uint8_t argc, void **argv) {
    eraseWeather();
}

static void eraseWeather() {
    if (!currCityArrayLen) {
        LOG_WARN("There is nothing to erase");
        return;
    }
    currCityArrayLen = 0;

    FLASH_ChangeProtectionStatus(FLASH_BLNONE);

    FLASH_Erase(0x3E0000, sizeof(uint8_t));
    FLASH_Write(&currCityArrayLen, 0x3E0000, sizeof(uint8_t));

    FLASH_ChangeProtectionStatus(FLASH_BL62to63);
}