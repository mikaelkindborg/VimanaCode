
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define OPTIMIZE

typedef unsigned char Byte;
typedef int           Bool;
typedef FILE          Stream;

#define FALSE 0
#define TRUE  1

#define ListFreeShallow 1
#define ListFreeDeep    2
#define ListFreeDeeper  3

#define StringEquals(s1, s2) (0 == strcmp(s1, s2))

#define Print(str, args...)      printf(str, ## args)
#define PrintLine(str, args...)  printf(str "\n", ## args)
#define PrintToStream(stream, str, args...)  fprintf(stream, str, ## args)
#define PrintDebug(str, args...) printf("[DEBUG] " str "\n", ## args)
#define ErrorExit(str, args...)  do { printf("[ERROR] " str "\n", ## args); exit(0); } while (0)
