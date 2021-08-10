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
#define OPTIMIZE_PRIMFUNS
#define USE_GC
//#define DEBUG
#define TRACK_MEMORY_USAGE

// C TYPES -----------------------------------------------------

// TODO: Prefix types with "V" for Vimana

typedef unsigned char    Byte;
typedef int              Bool;
typedef FILE             FileStream; // TODO: Rename for Arduino
typedef unsigned int     Type;
typedef int              Index;
typedef long             IntNum;
typedef double           DecNum;
typedef struct MyItem    Item;
typedef struct MyList    List;
typedef struct MyInterp  Interp;
typedef void   (*PrimFun)(Interp*);

// BOOLEAN VALUES ----------------------------------------------

#define FALSE 0
#define TRUE  1

// VIMANA TYPES ------------------------------------------------

#define TypeSymbol        1
#define TypeIntNum        2
#define TypeDecNum        4
#define TypeBool          8
#define TypeList          16
#define TypePrimFun       32
#define TypeFun           64
#define TypeString        128
#define TypeDynAlloc      256
#define TypeSpecialFun    512
#define TypeClosure       1024
#define TypeVirgin        0 // Represents unbound symbol/uninitialized item
//#define TypePushable (TypeIntNum | TypeDecNum | TypeBool | TypeList)

// ERROR HANDLING ----------------------------------------------

// TODO: Make function that displays the interpreter state.

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
int GListCreateCounter = 0;
int GListFreeCounter = 0;
int GStringCreateCounter = 0;
int GStringFreeCounter = 0;
//int GMemReallocCounter = 0;

#define MemAlloc(size) malloc(size); ++ GMemAllocCounter
#define MemFree(obj) free(obj); ++ GMemFreeCounter

void PrintMemStat()
{
  PrintLine("MemAlloc:     %d", GMemAllocCounter);
  PrintLine("MemFree:      %d", GMemFreeCounter);
  PrintLine("ListCreate:   %d", GListCreateCounter);
  PrintLine("ListFree:     %d", GListFreeCounter);
  PrintLine("StringCreate: %d", GStringCreateCounter);
  PrintLine("StringFree:   %d", GStringFreeCounter);
  //PrintLine("Realloc:    %d", GMemReallocCounter);
}

#else

#define MemAlloc(size) malloc(size)
#define MemFree(obj) free(obj)
#define PrintMemStat()

#endif
