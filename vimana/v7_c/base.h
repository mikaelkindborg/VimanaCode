
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//#define OPTIMIZE
//#define USE_GC
#define DEBUG

typedef unsigned char Byte;
typedef int           Bool;
typedef FILE          Stream;

#define FALSE 0
#define TRUE  1

#define Print(str, args...)      printf(str, ## args)
#define PrintLine(str, args...)  printf(str "\n", ## args)
#define PrintToStream(stream, str, args...)  fprintf(stream, str, ## args)
#define ErrorExit(str, args...)  do { printf("[ERROR] " str "\n", ## args); exit(0); } while (0)
//#define ErrorExit(str, args...)  do { } while (0)
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
