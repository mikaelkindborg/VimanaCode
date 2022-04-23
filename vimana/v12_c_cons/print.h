/*
File: print.h
Author: Mikael Kindborg (mikael@kindborg.com)

Print functions.
*/

// For portability (TODO: Add support for Arduino)
#define Print(str)        fputs(str, stdout)
#define PrintIntNum(num)  printf("%ld", (long)(num))
#define PrintDecNum(num)  printf("%g", (double)(num))
#define PrintChar(c)      printf("%c",  (char)(c))
#define PrintNewLine()    printf("\n")
#define PrintLine(str)    printf("%s\n", str)

// For debugging
void PrintBinaryUInt(unsigned int n)
{
  int numBits = sizeof(unsigned int) * 8;
  for (long i = numBits - 1 ; i >= 0; --i) 
  {
    PrintChar(n & (1L << i) ? '1' : '0');
  }
  PrintNewLine();
}
