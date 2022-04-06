/*
File: gurumeditation.h
Author: Mikael Kindborg (mikael@kindborg.com)

This file contains error messages. Error handling in Vimana is simple -
if an error occurs the progam exits.
*/

#define GURU_MEDITATION 0
#define CODE_PARSER_UNDEFINED_TYPE 1
#define DATA_STACK_OVERFLOW 2
#define DATA_STACK_IS_EMPTY 3
#define CALL_STACK_OVERFLOW 4
#define CALL_STACK_IS_EMPTY 5

char* GuruMeditationTable[] = 
{ 
  "GURU MEDITATION",
  "CODE PARSER UNDEFINED TYPE",
  "DATA STACK OVERFLOW",
  "DATA STACK IS EMPTY",
  "CALL STACK OVERFLOW",
  "CALL STACK IS EMPTY"
}; 

void GuruMeditation(int errorId)
{
  printf("[GURU_MEDITATION: %i] %s\n", errorId, GuruMeditationTable[errorId]);
  exit(0); 
}

// For unit testing
void ShouldHold(char* description, int condition)
{
  if (!condition) 
  {
    printf("[SHOULD_HOLD_FAILED] %s\n", description);
  }
}
