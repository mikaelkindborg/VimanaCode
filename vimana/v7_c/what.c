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

  size = sizeof(long);
  printf("Size of long: %i\n", size);

  size = sizeof(long long);
  printf("Size of long long: %i\n", size);

  size = sizeof(double);
  printf("Size of double: %i\n", size);

  //addUsingItem(); // ./a.out  8.03s user 0.03s system 97% cpu 8.270 total

  addUsingLong(); // ./a.out  7.05s user 0.01s system 96% cpu 7.304 total

  //addUsingInt();
}
