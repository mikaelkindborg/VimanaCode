
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef int Bool;

#define FALSE 0
#define TRUE  1

#define ListFreeShallow 1
#define ListFreeDeep    2
#define ListFreeDeeper  3

#define StringEquals(s1, s2) (0 == strcmp((s1), (s2)))

