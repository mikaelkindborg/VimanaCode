/*
File: gurumeditation.h
Author: Mikael Kindborg (mikael@kindborg.com)

This file contains error messages. Error handling in Vimana is simple -
if an error occurs the progam exits.
*/

/*
Cool macros :)

#define STRINGIFY(x) #x
#define STRVALUE(x) STRINGIFY(x)

#define CODE_PARSER_UNDEFINED_TYPE 1

#define GURU(name) GuruMeditation(name, #name)

GURU(CODE_PARSER_UNDEFINED_TYPE);

STRINGIFY(CODE_PARSER_UNDEFINED_TYPE)
STRVALUE(CODE_PARSER_UNDEFINED_TYPE)
*/

#define GURU(name) GuruMeditation(name, #name)

enum GURU_TABLE
{
  GURU_MEDITATION = 0,
  CODE_PARSER_UNDEFINED_TYPE,
  DATA_STACK_OVERFLOW,
  DATA_STACK_IS_EMPTY,
  CONTEXT_STACK_OVERFLOW,
  CONTEXT_STACK_IS_EMPTY,
  CALL_STACK_OVERFLOW,
  CALL_STACK_IS_EMPTY,
  GLOBALVARS_OVERFLOW,
  MEM_OUT_OF_MEMORY,
  PARSER_TOKEN_TYPE_ERROR,
  INTERP_UNEXPECTED_TYPE,
  FIRST_OBJECT_IS_NOT_A_LIST,
  REST_OBJECT_IS_NOT_A_LIST,
  CONS_OBJECT_IS_NOT_A_LIST,
  CONS_OUT_OF_MEMORY,
  __SENTINEL__
};

void GuruMeditation(int errorId, char* errorString)
{
  printf("[GURU_MEDITATION] %i %s\n", errorId, errorString);
  exit(1); 
}

// For unit testing
void ShouldHold(char* description, int condition)
{
  if (!condition) 
  {
    printf("[SHOULD_HOLD_FAILED] %s\n", description);
  }
}
