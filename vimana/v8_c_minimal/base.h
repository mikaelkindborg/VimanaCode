/*
File: base.h
Author: Mikael Kindborg (mikael@kindborg.com)

Basic data types and functions.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// FLAGS -------------------------------------------------------

#define OPTIMIZE
#define DEBUG
#define TRACK_MEMORY_USAGE
#define INCLUDE_SOURCE_CODE_PARSER

// BASIC TYPES -------------------------------------------------

typedef unsigned char    VByte;
typedef long             VNumber;
typedef unsigned long    VUNumber;
typedef int              VBool;
typedef int              VSize;

// BOOLEAN VALUES ----------------------------------------------

#define FALSE 0
#define TRUE  1

// PRINTING ----------------------------------------------------

// TODO: Make function that displays the interpreter state.
// TODO: Arduino serial print.

#define Print(str)      fputs(str, stdout)
#define PrintNum(num)   printf("%ld", (long)(num))
#define PrintChar(c)    printf("%c",  (char)(c))
#define PrintNewLine()  printf("\n")
#define PrintLine(str)  printf(str "\n")
#define PrintStrNumLine(str, num)  printf(str "%ld" "\n", (long)num)

#ifdef DEBUG
  #define PrintDebug(str)             PrintLine("[DEBUG] " str)
  #define PrintDebugStrNum(str, num)  PrintStrNumLine("[DEBUG] " str, num)
#else
  #define PrintDebug(str)
  #define PrintDebugStrNum(str, num)
#endif

// MEMORY TRACKING ---------------------------------------------

#ifdef TRACK_MEMORY_USAGE

  int GMemAllocCounter = 0;
  int GMemFreeCounter = 0;

  #define MemAlloc(size) malloc(size); ++ GMemAllocCounter
  #define MemFree(obj) free(obj); ++ GMemFreeCounter

  void PrintMemStat()
  {
    Print("MemAlloc: "); PrintNum(GMemAllocCounter); PrintLine("");
    Print("MemFree:  "); PrintNum(GMemFreeCounter);  PrintLine("");
  }

#else

  #define MemAlloc(size) malloc(size)
  #define MemFree(obj) free(obj)
  #define PrintMemStat()

#endif

// C STRING FUNCTIONS ------------------------------------------

#define StrEquals(s1, s2) (0 == strcmp(s1, s2))

char* StrCopy(char* str)
{
  char* newStr = MemAlloc(strlen(str + 1));
  strcpy(newStr, str);
  return newStr;
}

void StrToUpper(char* s)
{
  char* p = s;
  while (*p)
  {
    *p = toupper((unsigned char) *p);
    ++ p;
  }
}

void StrToLower(char* s)
{
  char* p = s;
  while (*p)
  {
    *p = tolower((unsigned char) *p);
    ++ p;
  }
}

// TODO: Move to new file debug.h
void PrintBinaryULong(unsigned long n)
{
  int numBits = sizeof(unsigned long) * 8;
  for (long i = numBits - 1 ; i >= 0; --i) 
  {
    //printf("%lu", (n & (1L <<< i)) >> i);
    PrintChar(n & (1L << i) ? '1' : '0');
    //printf("%c", n & (((unsigned long)i) << 1) ? '1' : '0');
  }
  PrintLine("");
}

// ERROR HANDLING ----------------------------------------------

#define GuruMeditation(num) \
  do { printf("[GURU_MEDITATION] %d\n", num); exit(0); } while (0)

// UNIT TEST HELPER -------------------------------------------

#define ShouldHold(description, condition) \
  do { if (!condition) PrintLine("[SHOULD_HOLD] " description); } while(0)
