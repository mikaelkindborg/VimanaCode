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

#define GURU_MEDITATION(name) GuruMeditation(name, #name)

GURU_MEDITATION(CODE_PARSER_UNDEFINED_TYPE);

STRINGIFY(CODE_PARSER_UNDEFINED_TYPE)
STRVALUE(CODE_PARSER_UNDEFINED_TYPE)
*/

#define GURU_MEDITATION(name) GuruMeditation(name, #name)

enum GURU_TABLE
{
  GURU_MEDITATION = 0,
  CODE_PARSER_UNDEFINED_TYPE,
  PARSER_UNBALANCED_STRING_END,
  DATA_STACK_OVERFLOW,
  DATA_STACK_IS_EMPTY,
  CONTEXT_STACK_OVERFLOW,
  CONTEXT_STACK_IS_EMPTY,
  CALL_STACK_OVERFLOW,
  CALL_STACK_IS_EMPTY,
  GLOBALVARS_OVERFLOW,
  ALLOC_OUT_OF_ITEM_MEMORY,
  PARSER_TOKEN_TYPE_ERROR,
  INTERP_UNEXPECTED_TYPE,
  FIRST_OBJECT_IS_NOT_A_LIST,
  REST_OBJECT_IS_NOT_A_LIST,
  CONS_OBJECT_IS_NOT_A_LIST,
  CONS_OUT_OF_MEMORY,
  SETFIRST_OBJECT_IS_NOT_A_LIST,
  SETFIRST_OUT_OF_MEMORY,
  SLEEP_NOT_INTNUM,
  SOCKET_CREATE_ERROR,
  SOCKET_CREATE_FCNTL_ERROR,
  SOCKET_BIND_ERROR,
  SOCKET_LISTEN_ERROR,
  SOCKET_ACCEPT_ERROR,
  ARRAY_OUT_OF_BOUNDS,
  __GURU_SENTINEL__
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
    printf("[ShouldHold] %s\n", description);
  }
}
