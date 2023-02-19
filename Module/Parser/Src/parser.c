/**
 * @file parser.c
 * @brief User interaction API via terminal commands
 * @version Alpha 1.0
 * @date 2022-10-14
 *
 *  (c) 2022
 */

#include <stdint.h>
// #include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"
#include "loglib.h"
#include "usart.h"
#include "tx_api.h"

static parser_initState_t PARSER_initState = PARSER_INIT_UNINITIALIZED;

// Parser definitions
#define MODULES_NUM 11
#define CMD_NUM     10
#define ARGS_NUM    5
#define LEX_MAX_LEN 20
#define LEX_MAX_NUM 20

/** Lexeme types
 * <LEX_MODULE> <LEX_COMMAND> [-LEX_ARGUMENT <LEX_VALUE>] ... <TERMINATOR>
 */
typedef enum {
    TERMINATOR,    // '\0'
    LEX_MODULE,
    LEX_COMMAND,
    LEX_ARGUMENT,
    LEX_VALUE
} lex_t;

static char  lexBuffer[LEX_MAX_NUM][LEX_MAX_LEN] = { TERMINATOR };
static lex_t lexCodeBuffer[LEX_MAX_NUM] = { TERMINATOR };

// Commands DB
typedef struct {
    void (*userFunc)(uint8_t, void **);     // User func. pointer
    char    cmdName[LEX_MAX_LEN];           // Command name
    uint8_t argc;                           // Arguments count
    char    argv[ARGS_NUM][LEX_MAX_LEN];    // Arguments names vector
} cmdTable_t;

typedef struct {
    const char *moduleName;    // Module name
    uint8_t     CMDS_NUM;      // Current number of existing commands for each module;
    cmdTable_t *cmdPtr[CMD_NUM];
} moduleTable_t;

static moduleTable_t modules[MODULES_NUM] = {
    {    "log", 0, NULL},
    {    "rfm", 0, NULL},
    {    "eth", 0, NULL},
    {    "ind", 0, NULL},
    {  "power", 0, NULL},
    {  "flash", 0, NULL},
    {  "modem", 0, NULL},
    {   "term", 0, NULL},
    {"weather", 0, NULL},
    {   "ship", 0, NULL},
    {  "touch", 0, NULL}
};

static uint8_t m = 0;    // Module index
static uint8_t c = 0;    // Command index

// Terminal variables
#define INPUT_BUFF_NUM 1    // Number of input buffers
const char  PARSER_prompt[PARSER_PROMPT_MAX_LEN] = "\r\x1b[35mHub2>\x1b[0m ";
char        PARSER_inputStr[PARSER_PROMPT_MAX_LEN + PARSER_INPUT_MAX_LEN] = { TERMINATOR };
static char input[1 + INPUT_BUFF_NUM][PARSER_INPUT_MAX_LEN] = { TERMINATOR };

// Definition of synchronization event flags
#define FLAG_RECEIVE 0x00000001U
#define FLAG_WAIT    0x00000002U
static ULONG                actual_flags;
static TX_EVENT_FLAGS_GROUP evfParser;

// Definition of thread
#define PARSER_THREAD_STACK_SIZE 1024 * 4
static TX_THREAD parserHandle;

void help(uint8_t argc, void **argv) {
    LOG_Printf("%s", modules[m].moduleName);
    if (modules[m].CMDS_NUM == 0) {
        LOG_Printf(": No commands\n\n");
        return;
    }
    uint16_t size = LEX_MAX_LEN + ARGS_NUM * LEX_MAX_LEN;
    char     cmdList[size];
    for (uint8_t i = 0; i < modules[m].CMDS_NUM; ++i) {
        memset(cmdList, '\0', size);
        strncat(cmdList, modules[m].cmdPtr[i]->cmdName, size - strlen(cmdList));
        for (size_t j = 0; j < modules[m].cmdPtr[i]->argc; ++j) {
            strncat(cmdList, " ", size - strlen(cmdList));
            strncat(cmdList, modules[m].cmdPtr[i]->argv[j], size - strlen(cmdList));
        }
        LOG_Printf("\t[%s]\n", cmdList);
    }
}

static void modList(uint8_t argc, void **argv) {
    LOG_Printf("Modules available for interaction:\n");
    for (uint8_t i = 0; i < MODULES_NUM; ++i) {
        if (modules[i].CMDS_NUM > 0) {
            LOG_Printf(" - %s\n", modules[i].moduleName);
        }
    }
}

static void errorPointer(const char *str, uint8_t errIndx) {
    if (str == NULL || errIndx > strlen(str)) {
        return;
    }
    LOG_Printf("\n\n%s\n", str);

    char *errStr = (char *) malloc(errIndx + 3);    // '\n', '\0'
    memset(errStr, ' ', errIndx + 2);
    memcpy(errStr + errIndx, "^\n\0", 3);
    LOG_Printf("%s", errStr);
    free(errStr);
}

static parser_retVal_t isCommand(const char *str, uint8_t lexNum, uint8_t *SI, uint8_t *DI) {
    lexCodeBuffer[lexNum] = (lexNum == 0) ? LEX_MODULE : LEX_COMMAND;
    for (; ((isalpha((int) str[*SI]) || isdigit((int) str[*SI])) && *DI < LEX_MAX_LEN); ++(*SI), ++(*DI)) {
        lexBuffer[lexNum][*DI] = tolower(str[*SI]);
    }

    return PARSER_OK;
}

static parser_retVal_t isArg(const char *str, uint8_t lexNum, uint8_t *SI, uint8_t *DI) {
    lexCodeBuffer[lexNum] = LEX_ARGUMENT;
    lexBuffer[lexNum][(*DI)++] = tolower(str[(*SI)++]);
    if (!isalpha((int) str[*SI])) {
        LOG_Printf("[LEXER] ERROR: Missing argument or expression\n\n");
        return PARSER_ERR;
    }
    for (; (isalnum((int) str[*SI]) && *DI < LEX_MAX_LEN); ++(*SI), ++(*DI)) {
        lexBuffer[lexNum][*DI] = tolower(str[*SI]);
    }

    return PARSER_OK;
}

static parser_retVal_t isValue(const char *str, uint8_t lexNum, uint8_t *SI, uint8_t *DI) {
    lexCodeBuffer[lexNum] = LEX_VALUE;

    if (str[*SI] == '-') {
        lexBuffer[lexNum][(*DI)++] = str[(*SI)++];
    }

    if ((str[*SI] == '0') && (tolower(str[*SI + 1]) == 'x')) {
        lexBuffer[lexNum][(*DI)++] = str[(*SI)++];
        lexBuffer[lexNum][(*DI)++] = tolower(str[(*SI)++]);
        if (str[*SI] == TERMINATOR) {
            LOG_Printf("[LEXER] ERROR: Missing HEX expression\n\n");
            return PARSER_ERR;
        }
        for (; isxdigit((int) str[*SI]) && (*DI < LEX_MAX_LEN); ++(*SI), ++(*DI)) {
            lexBuffer[lexNum][*DI] = toupper(str[*SI]);
        }
    } else {
        for (; isdigit((int) str[*SI]) && (*DI < LEX_MAX_LEN); ++(*SI), ++(*DI)) {
            lexBuffer[lexNum][*DI] = str[*SI];
        }
    }
    return PARSER_OK;
}

static parser_retVal_t isString(const char *str, uint8_t lexNum, uint8_t *SI,
                                uint8_t *DI) {    // Without escape sequences
    lexCodeBuffer[lexNum] = LEX_VALUE;

    ++(*SI);
    for (; (str[*SI] != '"') && (*DI < LEX_MAX_LEN); ++(*SI), ++(*DI)) {
        if (str[*SI] == TERMINATOR) {
            LOG_Printf("[LEXER] ERROR: Unclosed expression, expected '\"'\n\n");
            return PARSER_ERR;
        }
        lexBuffer[lexNum][*DI] = str[*SI];
    }
    if (str[*SI] == '"') {
        ++(*SI);
    }

    return PARSER_OK;
}

static parser_retVal_t moduleFinder() {
    for (m = 0;; ++m) {    // Module existance check
        if (m >= MODULES_NUM) {
            // Module was not found
            return PARSER_ERR;
        }
        if (!strcmp(modules[m].moduleName, lexBuffer[0])) {
            // Module was found
            break;
        }
    }
    return PARSER_OK;
}

// [LEXER]
static parser_retVal_t commandLexer(const char *str) {
    // Erasing information tables
    for (uint8_t i = 0; i < LEX_MAX_NUM; ++i) {
        memset(lexBuffer[i], TERMINATOR, LEX_MAX_LEN);
    }
    memset(lexCodeBuffer, TERMINATOR, LEX_MAX_NUM);

    for (uint8_t SI = 0, DI = 0, lexNum = 0; str[SI] != TERMINATOR;
         ++SI) {    // SI - Source index(str); DI - Destination index (lexBuffer)

        if (isspace((int) str[SI])) {    // Space check
            continue;
        }

        if (lexNum >= LEX_MAX_NUM) {    // Lexeme num overflow check
            errorPointer(str, SI);
            LOG_Printf("[LEXER] ERROR: Too many tokens, max amount is %u\n\n", LEX_MAX_NUM);
            return PARSER_ERR;
        }

        DI = 0;                          // lexBuffer[lexNum] CR
        if (isalpha((int) str[SI])) {    // LEX_MODULE or LEX_COMMAND check
            if (isCommand(str, lexNum, &SI, &DI) != PARSER_OK) {
                return PARSER_ERR;
            }
        } else if (str[SI] == '-') {             // LEX_ARGUMENT or negative LEX_VALUE check
            if (isdigit((int) str[SI + 1])) {    //      negative LEX_VALUE check
                if (isValue(str, lexNum, &SI, &DI) != PARSER_OK) {
                    return PARSER_ERR;
                }
            } else if (isArg(str, lexNum, &SI, &DI) != PARSER_OK) {    //      LEX_ARGUMENT
                return PARSER_ERR;
            }
        } else if (isdigit((int) str[SI])) {                      // LEX_VALUE check
            if (isValue(str, lexNum, &SI, &DI) != PARSER_OK) {    // Decimal & HEX
                return PARSER_ERR;
            }
        } else if (str[SI] == '"') {
            if (isString(str, lexNum, &SI, &DI) != PARSER_OK) {
                return PARSER_ERR;
            }
        }

        ++lexNum;    // Lexeme num increment
        if (str[SI] == TERMINATOR) {
            break;
        }
        if (!isspace((int) str[SI])) {    // Lexeme len overflow and invalid character check
            if (DI >= LEX_MAX_LEN) {
                errorPointer(str, SI);
                LOG_Printf("[LEXER] ERROR: Too long token, max length is %u\n\n", LEX_MAX_LEN);
            } else {
                errorPointer(str, SI);
                LOG_Printf("[LEXER] ERROR: Invalid char in token\n\n");
            }
            return PARSER_ERR;
        }
    }

    return PARSER_OK;
}

// [REGEX]
static parser_retVal_t commandRegex(const char *str) {
    if (lexCodeBuffer[0] == TERMINATOR) {    // Is lexCodeBuffer empty?
        return PARSER_ERR;
    }

    if (lexCodeBuffer[0] != LEX_MODULE) {    // Is first lexeme - module name?
        LOG_Printf("[REGEX] ERROR: missing a module and a command name\n\n");
        return PARSER_ERR;
    }

    if (lexCodeBuffer[1] != LEX_COMMAND) {    // Is second lexeme - command name?
        LOG_Printf("[REGEX] ERROR: missing a command name\n");
        LOG_Printf("Try \"%s help\" to see available commands\n\n", lexBuffer[0]);
        return PARSER_ERR;
    }

    for (uint8_t i = 2; (i < LEX_MAX_NUM) && (lexCodeBuffer[i] != TERMINATOR);) {
        if (lexCodeBuffer[i] == LEX_COMMAND) {
            ++i;
        } else if (lexCodeBuffer[i] == LEX_ARGUMENT) {
            if (lexCodeBuffer[++i] == LEX_VALUE) {
                ++i;
            } else {
                LOG_Printf("[REGEX] ERROR: Missing value of argument \"%s\"\n\n", lexBuffer[i - 1]);
                return PARSER_ERR;
            }
        } else if (lexCodeBuffer[i] == LEX_VALUE) {
            LOG_Printf("[REGEX] ERROR: Missing argument of value \"%s\"\n\n", lexBuffer[i]);
            return PARSER_ERR;
        }
    }

    return PARSER_OK;
}

// [SEMANTIC]
static parser_retVal_t commandSemantic(const char *str) {
    if (lexCodeBuffer[0] != LEX_MODULE || lexCodeBuffer[1] != LEX_COMMAND) {
        return PARSER_ERR;
    }

    if (moduleFinder() != PARSER_OK) {
        LOG_Printf("[SEMANTIC] ERROR: Module \"%s\" unknown\n", lexBuffer[0]);
        LOG_Printf("Print \"term list\" to see available modules\n\n");
        return PARSER_ERR;
    }

    for (uint8_t cmdIndx = 1; (cmdIndx < LEX_MAX_NUM) && (lexCodeBuffer[cmdIndx] != TERMINATOR);) {
        for (c = 0;; ++c) {    // Command existance check
            if ((c >= modules[m].CMDS_NUM) || (modules[m].cmdPtr[c] == NULL)) {
                // Command was not found
                LOG_Printf("[SEMANTIC] ERROR: Command \"%s\" unknown\n", lexBuffer[cmdIndx]);
                LOG_Printf("Try \"%s help\" to see available commands\n\n", modules[m].moduleName);
                return PARSER_ERR;
            }
            if (!strcmp(modules[m].cmdPtr[c]->cmdName, lexBuffer[cmdIndx])) {
                // Command was found
                break;
            }
        }

        void **args =
            (modules[m].cmdPtr[c]->argc == 0) ?
                NULL :
                (void **) malloc((modules[m].cmdPtr[c]->argc) * sizeof(void *));    // User function arguments buffer
        uint8_t j = cmdIndx + 1;
        for (uint8_t i = 0; i < modules[m].cmdPtr[c]->argc; ++i) {
            uint8_t repetition_flag = 0;
            for (j = cmdIndx + 1;
                 (j < LEX_MAX_NUM) && (lexCodeBuffer[j] != TERMINATOR) && (lexCodeBuffer[j] != LEX_COMMAND); ++j) {

                if ((lexCodeBuffer[j] == LEX_ARGUMENT) && !strcmp(modules[m].cmdPtr[c]->argv[i], lexBuffer[j])) {
                    if (repetition_flag) {    // Argument repetition check
                        LOG_Printf("[SEMANTIC] ERROR: Repetition of arguments \"%s\"\n\n", lexBuffer[j]);
                        free(args);
                        return PARSER_ERR;
                    }
                    repetition_flag = 1;
                    args[i] = (void *) lexBuffer[j + 1];
                    ++j;
                }
            }
            if (!repetition_flag) {    // Argument existance check
                LOG_Printf("[SEMANTIC] ERROR: Missing argument \"%s\"\n\n", modules[m].cmdPtr[c]->argv[i]);
                free(args);
                return PARSER_ERR;
            }
        }

        if ((j - cmdIndx) / 2 > modules[m].cmdPtr[c]->argc) {
            LOG_Printf("[SEMANTIC] WARNING: Excess argument(s)\n\n");
        }

        modules[m].cmdPtr[c]->userFunc(modules[m].cmdPtr[c]->argc, args);
        if (args != NULL) {
            free(args);
        }
        cmdIndx = j;
    }

    return PARSER_OK;
}

parser_retVal_t PARSER_AddCommand(void (*userFunc)(uint8_t, void **), char *argString) {
    if (PARSER_initState != PARSER_INIT_OK) {
        return PARSER_ERR;
    }
    if (commandLexer(argString) != PARSER_OK) {    // Lexer result check
        LOG_Printf("PARSER_AddCommand [ERROR]: Lexer return state: error\n\n");
        return PARSER_ERR;
    }
    if (commandRegex(argString) != PARSER_OK) {    // Regex result check
        LOG_Printf("PARSER_AddCommand [ERROR]: Regex return state: error\n\n");
        return PARSER_ERR;
    }
    if (moduleFinder() != PARSER_OK) {
        LOG_Printf("\nPARSER_AddCommand [ERROR]: Module \"%s\" was not found\n\n", lexBuffer[0]);
        return PARSER_ERR;
    }

    if (modules[m].CMDS_NUM >= LEX_MAX_NUM) {    // Command num overflow check
        return PARSER_ERR;
    }

    for (uint8_t j = 0; j < modules[m].CMDS_NUM; ++j) {    // Command existance check
        if (!strcmp(modules[m].cmdPtr[j]->cmdName, lexBuffer[1])) {
            LOG_Printf("PARSER_AddCommand [ERROR]: Command \"%s\" for \"%s\" module is exist already\n\n", lexBuffer[1],
                       modules[m].moduleName);
            return PARSER_ERR;
        }
    }

    // Adding new command
    c = modules[m].CMDS_NUM;
    modules[m].cmdPtr[c] = (cmdTable_t *) malloc(sizeof(cmdTable_t));    // New command memory allocate
    modules[m].cmdPtr[c]->userFunc = userFunc;                           // Save userFunc pointer
    strcpy(modules[m].cmdPtr[c]->cmdName, lexBuffer[1]);                 // Save command name

    uint8_t argc = 0;                                                                   // Arguments counter
    for (uint8_t j = 2; (j < LEX_MAX_NUM) && (lexBuffer[j][0] != TERMINATOR); ++j) {    // Save arguments list
        if (lexCodeBuffer[j] == LEX_ARGUMENT) {
            for (uint8_t i = 0; i < argc; ++i) {    // Argument existance check
                if (!strcmp(modules[m].cmdPtr[c]->argv[i], lexBuffer[j])) {
                    LOG_Printf("PARSER_AddCommand [ERROR]: Argument \"%s\" already specified\n\n", lexBuffer[j]);
                    free(modules[m].cmdPtr[c]);
                    return PARSER_ERR;
                }
            }
            if (argc < ARGS_NUM) {
                strcpy(modules[m].cmdPtr[c]->argv[argc++], lexBuffer[j]);
            } else {
                LOG_Printf("PARSER_AddCommand [ERROR]: Too many args, max num is %u\n\n", ARGS_NUM);
                free(modules[m].cmdPtr[c]);
                return PARSER_ERR;
            }
        } else if (lexCodeBuffer[j] != LEX_VALUE) {
            LOG_Printf("PARSER_AddCommand [ERROR]: You can add only one command per call\n\n");
            free(modules[m].cmdPtr[c]);
            return PARSER_ERR;
        }
    }
    modules[m].cmdPtr[c]->argc = argc;
    modules[m].CMDS_NUM++;    // Commands num increment

    return PARSER_OK;
}

// Parser entry point
static void PARSER_Term(const char *str) {
    if (str[0] == TERMINATOR) {
        return;
    }

    // [LEXER] start
    if (commandLexer(str) != PARSER_OK) {
        for (uint8_t i = 0; i < LEX_MAX_NUM; ++i) {
            memset(lexBuffer[i], TERMINATOR, LEX_MAX_LEN);
        }
        memset(lexCodeBuffer, TERMINATOR, LEX_MAX_NUM);
        return;
    }

    // [REGEX] start
    if (commandRegex(str) != PARSER_OK) {
        for (uint8_t i = 0; i < LEX_MAX_NUM; ++i) {
            memset(lexBuffer[i], TERMINATOR, LEX_MAX_LEN);
        }
        memset(lexCodeBuffer, TERMINATOR, LEX_MAX_NUM);
        return;
    }

    // [SEMANTIC] start
    if (commandSemantic(str) != PARSER_OK) {
        for (uint8_t i = 0; i < LEX_MAX_NUM; ++i) {
            memset(lexBuffer[i], TERMINATOR, LEX_MAX_LEN);
        }
        memset(lexCodeBuffer, TERMINATOR, LEX_MAX_NUM);
        return;
    }

    // Testing indicator
    LOG_Printf("\ncommand OK\n\n");
}

static void StartParser(void *argument) {
    uint8_t PROMPT_LEN = strlen(PARSER_prompt);
    uint8_t DI = 0, buffIndx = 0;

    char *inputStr = input[0];
    char  inputCh;

    strncat(PARSER_inputStr, PARSER_prompt, PARSER_PROMPT_MAX_LEN);
    while (1) {
        HAL_UART_Receive_IT(&LOG_UART, (uint8_t *) &inputCh, 1);
        tx_event_flags_get(&evfParser, FLAG_RECEIVE, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);
        if (inputCh == '\n' || inputCh == '\r' ||
            DI == PARSER_INPUT_MAX_LEN - 1) {                         // LF, CR handler (Linux and Windows only)
            inputStr[DI] = '\0';                                      // Terminate input string
            memset(PARSER_inputStr, '\0', sizeof PARSER_inputStr);    // Clear echo input string

            LOG_Printf("\n");
            if (inputStr[0] != '\0') {
                PARSER_Term(inputStr);    // Parse
                DI = 0;
                buffIndx = (buffIndx == INPUT_BUFF_NUM) ? 0 : (buffIndx + 1);
                inputStr = memset(input[buffIndx], '\0', PARSER_INPUT_MAX_LEN);
            }
            strncat(PARSER_inputStr, PARSER_prompt, PARSER_PROMPT_MAX_LEN);

        } else if (inputCh == '\b') {    // backspace handler
            if (DI > 0) {                // backspace blocker
                LOG_Printf("\b \b");
                --DI;
                inputStr[DI] = PARSER_inputStr[DI + PROMPT_LEN] = '\0';
            }
        } else if (isprint(inputCh)) {
            if (inputCh == '[') {    // Come back to the previous command
                tx_event_flags_set(&evfParser, FLAG_WAIT, TX_OR);
                HAL_UART_Receive_IT(&LOG_UART, (uint8_t *) &inputCh, 1);
                tx_event_flags_get(&evfParser, FLAG_RECEIVE, TX_OR_CLEAR, &actual_flags, TX_WAIT_FOREVER);
                if (inputCh == 'A' || inputCh == 'B') {
                    if (inputCh == 'A') {
                        buffIndx = (buffIndx == 0) ? (INPUT_BUFF_NUM) : (buffIndx - 1);
                    } else {
                        buffIndx = (buffIndx == INPUT_BUFF_NUM) ? 0 : (buffIndx + 1);
                    }
                    inputStr = input[buffIndx];
                    char *clear = (char *) calloc(1, PROMPT_LEN + DI + 1);    // Create a terminal input clearing array
                    memset(clear, ' ', PROMPT_LEN + DI);
                    DI = strlen(inputStr);    // Set actual destination index
                    memset(PARSER_inputStr, '\0',
                           PARSER_PROMPT_MAX_LEN + PARSER_INPUT_MAX_LEN);        // Erase input buffer array
                    strcat(strcat(PARSER_inputStr, PARSER_prompt), inputStr);    // Fill input buffer array
                    LOG_Printf("\r%s%s", clear, PARSER_inputStr);
                    free(clear);
                    tx_event_flags_set(&evfParser, FLAG_WAIT, TX_OR);
                    continue;
                }
            }
            LOG_Printf("%c", inputCh);
            inputStr[DI] = PARSER_inputStr[DI + PROMPT_LEN] = inputCh;
            ++DI;
        }
        tx_event_flags_set(&evfParser, FLAG_WAIT, TX_OR);
    }
}

void PARSER_RxCpltCallback(void) {
    tx_event_flags_set(&evfParser, FLAG_RECEIVE, TX_OR);
}

parser_initState_t PARSER_Init(void *memoryPoolPtr) {
    if (PARSER_initState == PARSER_INIT_OK) {
        return PARSER_INIT_REINIT_ERR;
    }
    if (tx_event_flags_create(&evfParser, "PARSER event flags") != TX_SUCCESS) {
        PARSER_initState = PARSER_INIT_MEM_ERR;
        return PARSER_INIT_MEM_ERR;
    }
    TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL *) memoryPoolPtr;
    CHAR         *pointer = NULL;
    if (tx_byte_allocate(byte_pool, (void **) &pointer, PARSER_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS) {
        PARSER_initState = PARSER_INIT_MEM_ERR;
        return PARSER_INIT_MEM_ERR;
    }
    if (tx_thread_create(&parserHandle, "PARSER Thread", (void *) StartParser, 1, pointer, PARSER_THREAD_STACK_SIZE, 0,
                         0, TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS) {
        PARSER_initState = PARSER_INIT_MEM_ERR;
        return PARSER_INIT_MEM_ERR;
    }

    PARSER_initState = PARSER_INIT_OK;
    PARSER_AddCommand(modList, "term list");
    PARSER_AddCommand(help, "term help");

    return PARSER_INIT_OK;
}

parser_initState_t PARSER_GetInitState() {
    return PARSER_initState;
}
