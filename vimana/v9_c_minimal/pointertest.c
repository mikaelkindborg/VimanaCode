#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/*
void print_binary(int number, int num_digits) 
{
    int digit;
    for(digit = num_digits - 1; digit >= 0; digit--) {
        printf("%c", number & (1 << digit) ? '1' : '0');
    }
}


void PrintAsBinary64bit(unsigned long n )

char buffer [65];
  itoa (i,buffer,2);
  printf ("binary: %s\n",buffer);
*/

void PrintBinary(unsigned long n)
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

void PrintBinarySeq()
{
  int numBits = sizeof(unsigned long) * 8;
  unsigned long num;
   long one = 1;
  for (int i = numBits - 1 ; i >= 0; --i) 
  {
      num = one << i;
      printf("%lu\n", num);
      //printf("%c", n & (1 << i) ? '1' : '0');
      //printf("%c", n & (((unsigned long)i) << 1) ? '1' : '0');
  }
  printf("\n");
}

void PrintBinary2(unsigned long n)
{
  int numBits = sizeof (unsigned long) * 8;
  for (int i = 0; i < numBits; ++i) 
  {
    if (n & 1)
        printf("1");
    else
        printf("0");
    n >>= 1;
  }
  printf("\n");
}

int main()
{
  long num;
  unsigned long one = 1;

  //PrintBinarySeq();

  num = 255L;

  printf("%ld\n", num);
  PrintBinary(num);
  //PrintBinary2(num);

  printf("\n");

  printf("%ld\n", num << 1);
  PrintBinary(num << 1);

  printf("%ld\n", num >> 1);
  PrintBinary(num >> 1);

  //printf("%lu\n", one << num);
  //printf("%lu\n", one >> num);

  printf("\n");

  char* ptr;
  ptr = malloc(1000);
  PrintBinary((unsigned long) ptr);
  ptr = malloc(1000);
  PrintBinary((unsigned long) ptr);
  ptr = malloc(1000);
  PrintBinary((unsigned long) ptr);
  ptr = malloc(1000);
  PrintBinary((unsigned long) ptr);

  *ptr = 'X';
  printf("%c\n", *ptr);
  void* vptr = ptr;
  ptr = vptr;
  *ptr = 'Z';
  printf("%c\n", *ptr);
}
