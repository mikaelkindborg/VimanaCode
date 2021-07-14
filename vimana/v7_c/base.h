
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define OPTIMIZE
#define OPTIMIZE_PRIMFUNS
//#define USE_GC
//#define DEBUG
//#define TRACK_MEMORY_USAGE

typedef unsigned char Byte;
typedef int           Bool;
typedef FILE          FileStream; // TODO: Rename for Arduino

#define FALSE 0
#define TRUE  1

#define Print(str, args...)      printf(str, ## args)
#define PrintLine(str, args...)  printf(str "\n", ## args)
#define PrintToStream(stream, str, args...)  fprintf(stream, str, ## args)

#define ErrorExit(str, args...)  do { printf("[ERROR] " str "\n", ## args); exit(0); } while (0)

#ifdef DEBUG
#define PrintDebug(str, args...) printf("[DEBUG] " str "\n", ## args)
#else
#define PrintDebug(str, args...)
#endif

#define StringEquals(s1, s2) (0 == strcmp(s1, s2))

void StringToUpper(char* s)
{
  char* p = s;
  while (*p)
  {
    *p = toupper((unsigned char) *p);
    ++ p;
  }
}

void StringToLower(char* s)
{
  char* p = s;
  while (*p)
  {
    *p = tolower((unsigned char) *p);
    ++ p;
  }
}

#ifdef TRACK_MEMORY_USAGE

int GMemAllocCounter = 0;
int GMemFreeCounter = 0;
//int GMemReallocCounter = 0;
int GListCreateCounter = 0;
int GListFreeCounter = 0;

#define MemAlloc(size) malloc(size); ++ GMemAllocCounter
#define MemFree(obj) free(obj); ++ GMemFreeCounter

void PrintMemStat()
{
  PrintLine("MemAlloc:   %d", GMemAllocCounter);
  PrintLine("MemFree:    %d", GMemFreeCounter);
  PrintLine("ListCreate: %d", GListCreateCounter);
  PrintLine("ListFree:   %d", GListFreeCounter);
}

#else

#define MemAlloc(size) malloc(size)
#define MemFree(obj) free(obj)
#define PrintMemStat()

#endif
