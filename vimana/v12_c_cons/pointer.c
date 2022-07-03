#include <stdio.h>
#include <stdlib.h>
/*
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
*/

typedef struct MyItem
{
  long a;
  long b;
}
Item;

Item* CreateArray(int numItems)
{
  Item* items = malloc(sizeof(Item) * numItems);
  for (int i = 0; i < numItems; ++ i)
  {
    items[i].a = i * 2;
    items[i].b = i * 3;
  }
  return items;
}

long StructPtrAccess(Item* array, int index1, int index2, int numLoops)
{
  long result = 0;
  Item* item;

  for (int i = 0; i < numLoops; ++ i)
  { 
    item = & array[index1];
    result += item->b;
    item = & array[index2];
    result -= item->a;
  }

  return result;
}

long BytePtrAccess(char* array, long offset1, long offset2, int numLoops)
{
  long result = 0;
  Item* item;

  for (int i = 0; i < numLoops; ++ i)
  { 
    item = (Item*) (array + offset1);
    result += item->b;
    item = (Item*) (array + offset2);
    result -= item->a;
  }

  return result;
}

int main()
{
  Item* array = CreateArray(1000);

  long result = StructPtrAccess(array, 801, 799, 1000 * 1000000);
  //long result = BytePtrAccess((char*) array, sizeof(Item) * 801, sizeof(Item) * 799, 1000 * 1000000);

  printf("Result: %li\n", result);

  free(array);
}
