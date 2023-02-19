/**
 * @file parser.h
 * @brief User interaction API via terminal commands
 * @version Alpha 1.0
 * @date 2022-10-14
 *
 *  (c) 2022
 */

#ifndef PARSER_H
#define PARSER_H

#define PARSER_PROMPT_MAX_LEN 25
#define PARSER_INPUT_MAX_LEN  UINT8_MAX

extern const char PARSER_prompt[PARSER_PROMPT_MAX_LEN];
extern char       PARSER_inputStr[PARSER_PROMPT_MAX_LEN + PARSER_INPUT_MAX_LEN];

typedef enum {
    PARSER_INIT_OK,
    PARSER_INIT_MEM_ERR,
    PARSER_INIT_REINIT_ERR,
    PARSER_INIT_UNINITIALIZED
} parser_initState_t;

typedef enum {
    PARSER_OK,
    PARSER_ERR
} parser_retVal_t;

parser_initState_t PARSER_Init(void *memoryPoolPtr);

/**
 * @brief Get parser initializing state
 * @param None
 * @return 1 if parser is initialized, 0 otherwise
 * */
parser_initState_t PARSER_GetInitState();

/**- "userFunc"⠀-⠀is pointer to added user function
 *⠀⠀⠀userFunc`s signature has the following arguments: (uint8_t argc, void *argv[argc])
 *⠀⠀⠀⠀- "argc" is number of transmitted arguments
 *⠀⠀⠀⠀- "args" is vector of pointers to transmitted arguments
 *
 *⠀- "argString"⠀-⠀is a template of user console command
 *⠀⠀"argString" should look like this:⠀"<LEX_MODULE>⠀<LEX_COMMAND>⠀[-LEX_ARGUMENT⠀<LEX_VALUE>]⠀⠀.⠀.⠀.⠀"
 *
 *⠀⠀Example:
 *⠀⠀For Ethernet module template may look like this: ⠀"eth⠀set⠀-addr⠀0⠀-port⠀0"
 *⠀⠀Its schematic representation will be as follows:
 * ⠀⠀"LEX_MODULE⠀LEX_COMMAND⠀-LEX_ARGUMENT⠀LEX_VALUE⠀-LEX_ARGUMENT⠀LEX_VALUE"
 *
 *!!!⠀LEX_VALUE (dec, HEX or string) in the template string may be ANY (within acceptable range) and needed only for
 *⠀⠀parser calibrating⠀!!!
 *!!!⠀Sequence of arguments in the template string must match the sequence in the function signature⠀!!!
 * */
parser_retVal_t PARSER_AddCommand(void (*userFunc)(uint8_t, void **), char *argString);

/** The function allows you to display the available commands for the module.
 *  Use without parameters
 * */
void help(uint8_t argc, void **argv);
void PARSER_RxCpltCallback(void);

#endif /* PARSER_H */
