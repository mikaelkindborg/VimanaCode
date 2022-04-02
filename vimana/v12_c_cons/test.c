
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

#define PrintChar(c)    printf("%c",  (char)(c))
#define PrintNewLine()  printf("\n")

void PrintBinaryUInt(unsigned int n)
{
  int numBits = sizeof(unsigned int) * 8;
  for (long i = numBits - 1 ; i >= 0; --i) 
  {
    PrintChar(n & (1L << i) ? '1' : '0');
  }
  PrintNewLine();
}

typedef unsigned long VData;
typedef unsigned int  VFlag;
typedef unsigned int  VAddr;

typedef struct __VItem
{
  VData  data;  // value  (number or pointer)
  VFlag  type;  // type info and gc mark bit
  VAddr  next;  // "address" of next item
}
VItem;

typedef struct __VItemMem
{
  VAddr size;
  VAddr firstFree;
  void* end;
  void* start;
}
VItemMem;

VItemMem* ItemMemNew()
{
  VAddr size = 10000;
  VItemMem* itemMem = malloc(size);
  itemMem->size = size;
  itemMem->firstFree = 0;
  itemMem->start = itemMem + sizeof(VItemMem);
  itemMem->end = itemMem->start;
  return itemMem;
}

void ItemMemFree(VItemMem* itemMem)
{
  free(itemMem);
}

#define ItemMemItemAddr(itemMem, item)    ( ( ((void*)(item)) - (itemMem)->start ) + 1 ) 
#define ItemMemItemPointer(itemMem, addr) ( (VItem*) ((itemMem)->start + addr - 1) ) 

#define ItemGCMark(item) ( ((item)->type) &  1 )
#define ItemType(item)   ( ((item)->type) >> 1 )
#define ItemNext(item)   ( (item)->next )
#define ItemData(item)   ( (item)->data )

static inline void ItemSetGCMark(VItem* item, VFlag mark)
{
  item->type = (item->type & ~1) | (mark & 1);
}

static inline void ItemSetType(VItem* item, VFlag type)
{
  item->type = (type << 1) | (item->type & 1);
}

void ItemMemCons(VItemMem* itemMem, VItem* item1, VItem* item2)
{
  item1->next = ItemMemItemAddr(itemMem, item2);
}

VItem* ItemMemAllocItem(VItemMem* itemMem)
{
  VItem* item;

  if (itemMem->firstFree)
  {
    printf("Free item available\n");
    item = ItemMemItemPointer(itemMem, itemMem->firstFree);
    itemMem->firstFree = item->next;
  }
  else
  {
    item = itemMem->end;
    itemMem->end += sizeof(VItem);
  }

  item->data = 0;
  item->type = 0;
  item->next = 0;

  return item;
}

void ItemMemDeallocItem(VItemMem* itemMem, VItem* item)
{
  item->next = itemMem->firstFree;
  itemMem->firstFree = ItemMemItemAddr(itemMem, item);
}

int main()
{
  printf("Hi World\n");
  PrintBinaryUInt(0x80000000);
  PrintBinaryUInt(0x1 << 1);
  //PrintBinaryUInt(0x1 >> 1);

  VItemMem* mem = ItemMemNew();

  VItem* item = ItemMemAllocItem(mem);
  ItemSetGCMark(item, 1);
  ItemSetType(item, 42);
  PrintBinaryUInt(item->type);
  printf("Type: %i\n", ItemType(item));
  printf("Mark: %i\n", ItemGCMark(item));

  VItem* item2 = ItemMemAllocItem(mem);
  ItemSetType(item2, 44);

  ItemMemCons(mem, item, item2);

  VItem* next = ItemMemItemPointer(mem, item->next);
  printf("Type: %i\n", ItemType(next));

  printf("ItemMemFirstFree: %i\n", mem->firstFree);
  ItemMemDeallocItem(mem, item);
  printf("ItemMemFirstFree: %i\n", mem->firstFree);

  item = ItemMemAllocItem(mem);
  printf("Type: %i\n", ItemType(item));

  ItemMemFree(mem);
}
