/*
File: base.h
Author: Mikael Kindborg (mikael@kindborg.com)

Basic data types and functions.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

// BUILD FLAGS -------------------------------------------------

#define PLATFORM_LINUX
#define OPTIMIZE
#define DEBUG
#define GURUMEDITATION_STRINGS
#define TRACK_MEMORY_USAGE
#define INCLUDE_SOURCE_CODE_PARSER

#ifdef PLATFORM_ARDUINO
  #undef PLATFORM_LINUX
  #define PLATFORM_MINIMAL
#endif

#ifdef PLATFORM_MINIMAL
  #undef TRACK_MEMORY_USAGE
  #undef GURUMEDITATION_STRINGS
  #undef INCLUDE_SOURCE_CODE_PARSER
#endif

// BASIC TYPES -------------------------------------------------

#ifdef PLATFORM_LINUX
  typedef unsigned char     VByte;
  typedef long              VNumber;
  typedef unsigned long     VBits;
  typedef VNumber           VBool;
  typedef unsigned int      VType;
  typedef int               VSize;
  typedef int               VIndex;
  #define VSIZEMAX          INT_MAX
  #define VINDEXMAX         INT_MAX
#endif

#ifdef PLATFORM_MINIMAL
  typedef unsigned char     VByte;
  typedef int               VNumber;
  typedef unsigned int      VBits;
  typedef char              VBool;
  typedef unsigned char     VType;
  typedef char              VSize;
  typedef char              VIndex;
  #define VSIZEMAX          CHAR_MAX
  #define VINDEXMAX         CHAR_MAX
#endif

// INTERPRETER TYPES -------------------------------------------

typedef struct __VInterp    VInterp;
typedef struct __VItem      VItem;
typedef struct __VList      VList;
typedef struct __VList      VString;

// BOOLEAN VALUES ----------------------------------------------

#define FALSE 0
#define TRUE  1

// PRINTING ----------------------------------------------------

#ifdef PLATFORM_LINUX

  #define Print(str)      fputs(str, stdout)
  #define PrintNum(num)   printf("%ld", (long)(num))
  #define PrintChar(c)    printf("%c",  (char)(c))
  #define PrintNewLine()  printf("\n")

  void PrintLine(char* str)
  {
    printf("%s\n", str);
  }

  void PrintStrNum(char* str, long num)
  {
    Print(str); 
    PrintNum(num); 
    PrintNewLine(); 
  }

  void PrintBinaryULong(unsigned long n)
  {
    int numBits = sizeof(unsigned long) * 8;
    for (long i = numBits - 1 ; i >= 0; --i) 
    {
      PrintChar(n & (1L << i) ? '1' : '0');
    }
    PrintNewLine();
  }

  #ifdef DEBUG

    void PrintDebug(char* str)
    {
      printf("[DEBUG] %s\n", str);
    }

    void PrintDebugStrNum(char* str, long num)
    {
      Print("[DEBUG] ");
      Print(str); 
      PrintNum(num); 
      PrintNewLine(); 
    }

  #else

    #define PrintDebug(str)
    #define PrintDebugStrNum(str, num)
    
  #endif

#endif

#ifdef PLATFORM_ARDUINO

  #define serial Serial

  #define Print(str)      serial.print(str)
  #define PrintNum(num)   serial.print(num)
  #define PrintChar(c)    serial.print(c)
  #define PrintNewLine()  serial.print('\n')
  #define PrintLine(str) \
    Print(str); PrintNewLine();
  #define PrintStrNum(str, num) \
    Print(str); PrintNum(num); PrintNewLine();

  #ifdef DEBUG
    #define PrintDebug(str)             PrintLine(str)
    #define PrintDebugStrNum(str, num)  PrintStrNum(str, num)
  #else
    #define PrintDebug(str)
    #define PrintDebugStrNum(str, num)
  #endif

#endif

// MEMORY TRACKING ---------------------------------------------

#ifdef TRACK_MEMORY_USAGE

  int GMemAllocCounter = 0;
  int GMemFreeCounter = 0;

  #define MemAlloc(size) malloc(size); ++ GMemAllocCounter
  #define MemFree(obj) free(obj); ++ GMemFreeCounter

  void PrintMemStat()
  {
    Print("MemAlloc: "); PrintNum(GMemAllocCounter); PrintNewLine();
    Print("MemFree:  "); PrintNum(GMemFreeCounter);  PrintNewLine();
  }

#else

  #define MemAlloc(size) malloc(size)
  #define MemFree(obj) free(obj)
  #define PrintMemStat()

#endif

#ifdef PLATFORM_ARDUINO
  int GetFreeMem() 
  {
    extern int  __heap_start;
    extern int* __brkval;
    int v;
    return ((int) &v) - ( (__brkval == 0) ? ((int) &__heap_start) : ((int) __brkval) );
  }
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

// ERROR HANDLING ----------------------------------------------

// TODO: Make function that displays interpreter state.

#include "gurumeditation_gen.h"

#ifdef GURUMEDITATION_STRINGS
  void GuruMeditation(int num)
  {
    printf("[GURU_MEDITATION: %d] %s", num, GuruMeditationTable[num]);
    exit(0); 
  }
#else
  void GuruMeditation(int num)
  {
    PrintChar('G'); 
    PrintChar('U'); 
    PrintChar('R');
    PrintChar('U'); 
    PrintChar(':'); 
    PrintNum(num); 
    PrintNewLine();
    exit(0); 
  }
#endif

// UNIT TEST HELPER -------------------------------------------

void ShouldHold(char* description, VBool condition)
{
  if (!condition) 
  {
    Print("[SHOULD_HOLD] ");
    PrintLine(description);
  }
}
