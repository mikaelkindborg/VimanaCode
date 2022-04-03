/*
File: gurumeditation.h
Author: Mikael Kindborg (mikael@kindborg.com)

This file contains error messages. Error handling in Vimana is simple -
if an error occurs the progam exits.
*/

#define GURU_MEDITATION 0
#define CODE_PARSER_UNDEFINED_TYPE 1

char* GuruMeditationTable[] = 
{ 
  "Guru Meditation"
  ,"Code parser undefined type"
}; 

void GuruMeditation(int errorId)
{
  printf("[GURU_MEDITATION: %i] %s", errorId, GuruMeditationTable[errorId]);
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
