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

#define FALSE 0
#define TRUE  1

#define Print(str)      fputs(str, stdout)
#define PrintNum(num)   printf("%ld", (long)(num))
#define PrintChar(c)    printf("%c",  (char)(c))
#define PrintNewLine()  printf("\n")
