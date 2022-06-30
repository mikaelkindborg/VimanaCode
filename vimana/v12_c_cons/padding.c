#include <stdio.h>
/*
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
*/

typedef struct MyItem
{
  //void* ptr;
  //unsigned long addr;
  int a;
  unsigned int b;
}
Item;

int main()
{
  int size;

  size = sizeof(Item);
  printf("Size of Item: %i\n", size);

  size = sizeof(long);
  printf("Size of long: %i\n", size);

  size = sizeof(int);
  printf("Size of int: %i\n", size);

  size = sizeof(short);
  printf("Size of short: %i\n", size);

}
