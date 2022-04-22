#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int GRandNum;

typedef struct MyItem
{
  long fieldA;
  double fieldB;
}
Item;

Item addRandToItem(Item item)
{
  item.fieldA = item.fieldA + rand() % 2;
  return item;
}

long addRandToLong(long x)
{
  x = x + rand() % 2;
  return x;
}

int addRandToInt(int x)
{
  x = x + rand() % 2;
  return x;
}

void addUsingItem()
{
  Item item;
  item.fieldA = 1;
  long i = 0;
  while (i < 1000000000)
  {
    item = addRandToItem(item);
    i++;
  }
  printf("Result: %li\n", item.fieldA);
}

void addUsingLong()
{
  long x;
  x = 1;
  long i = 0;
  while (i < 1000000000)
  {
    x = addRandToLong(x);
    i++;
  }
  printf("Result: %li\n", x);
}

void addUsingInt()
{
  int x;
  x = 1;
  long i = 0;
  while (i < 1000000000)
  {
    x = addRandToInt(x);
    i++;
  }
  printf("Result: %i\n", x);
}

int main()
{
  time_t t;
  srand((unsigned) time(&t));
  
  GRandNum = rand() % 2;

  printf("Test of structs\n");
  
  int size = sizeof(Item);
  printf("Size of Item: %i\n", size);

  size = sizeof(unsigned char);
  printf("Size of uchar: %i\n", size);

  size = sizeof(int);
  printf("Size of int: %i\n", size);

  size = sizeof(short);
  printf("Size of short: %i\n", size);

  size = sizeof(long);
  printf("Size of long: %i\n", size);

  size = sizeof(long long);
  printf("Size of long long: %i\n", size);

  size = sizeof(double);
  printf("Size of double: %i\n", size);

  // ./a.out  8.03s user 0.03s system 97% cpu 8.270 total
  // ./a.out  7.74s user 0.02s system 96% cpu 8.069 total
  //addUsingItem(); 

  // ./a.out  7.05s user 0.01s system 96% cpu 7.304 total
  addUsingLong();

  //addUsingInt();
}

/*
Sketch uses 26054 bytes (80%) of program storage space. Maximum is 32256 bytes.
Global variables use 434 bytes (21%) of dynamic memory, leaving 1614 bytes for local variables. Maximum is 2048 bytes.

Sketch uses 26116 bytes (80%) of program storage space. Maximum is 32256 bytes.
Global variables use 496 bytes (24%) of dynamic memory, leaving 1552 bytes for local variables. Maximum is 2048 bytes.
*/
