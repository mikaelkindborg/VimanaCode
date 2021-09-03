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

#define DEBUG
#define TRACK_MEMORY_USAGE

// TYPES -----------------------------------------------------

typedef unsigned char    VmByte;
typedef void*            VmPointer;
typedef long             VmNumber;
typedef unsigned long    VmUNumber;
typedef int              VmBool;
typedef int              VmIndex;

// BOOLEAN VALUES ----------------------------------------------

#define FALSE 0
#define TRUE  1

// ERROR HANDLING ----------------------------------------------

// TODO: Make function that displays the interpreter state.
// TODO: Arduino serial print.

#define ErrorExit(str, args...)  do { printf("[ERROR] " str "\n", ## args); exit(0); } while (0)

// PRINTING ----------------------------------------------------

#define Print(str, args...)      printf(str, ## args)
#define PrintLine(str, args...)  printf(str "\n", ## args)
#define PrintToStream(stream, str, args...)  fprintf(stream, str, ## args)

#ifdef DEBUG
#define PrintDebug(str, args...) printf("[DEBUG] " str "\n", ## args)
#else
#define PrintDebug(str, args...)
#endif

void PrintBinaryULong(unsigned long n)
{
  int numBits = sizeof(unsigned long) * 8;
  for (long i = numBits - 1 ; i >= 0; --i) 
  {
      //printf("%lu", (n & (1L <<< i)) >> i);
      printf("%c", n & (1L << i) ? '1' : '0');
      //printf("%c", n & (((unsigned long)i) << 1) ? '1' : '0');
  }
  printf("\n");
}

// C STRING FUNCTIONS ------------------------------------------

#define StrEquals(s1, s2) (0 == strcmp(s1, s2))

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

// MEMORY TRACKING ---------------------------------------------

#ifdef TRACK_MEMORY_USAGE

int GMemAllocCounter = 0;
int GMemFreeCounter = 0;

#define MemAlloc(size) malloc(size); ++ GMemAllocCounter
#define MemFree(obj) free(obj); ++ GMemFreeCounter

void PrintMemStat()
{
  PrintLine("MemAlloc:     %d", GMemAllocCounter);
  PrintLine("MemFree:      %d", GMemFreeCounter);
}

#else

#define MemAlloc(size) malloc(size)
#define MemFree(obj) free(obj)
#define PrintMemStat()

#endif
