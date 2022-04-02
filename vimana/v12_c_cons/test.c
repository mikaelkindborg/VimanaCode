
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

typedef struct __VMem
{
  VAddr firstFree;
  VAddr size;
  void* end;
  void* start;
}
VMem;

VMem* MemNew()
{
  VAddr size = 1000;
  VMem* mem = malloc(size);
  mem->firstFree = 0;
  mem->size = size - sizeof(VMem);
  mem->start = mem + sizeof(VMem);
  mem->end = mem->start;
  return mem;
}

void MemFree(VMem* mem)
{
  free(mem);
}

#define MemItemAddr(mem, item)    ( ( ((void*)(item)) - (mem)->start ) + 1 ) 
#define MemItemPointer(mem, addr) ( (VItem*) ((mem)->start + addr - 1) ) 

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

void MemCons(VMem* mem, VItem* item1, VItem* item2)
{
  item1->next = MemItemAddr(mem, item2);
}

VItem* MemAllocItem(VMem* mem)
{
  VItem* item;

  if (mem->firstFree)
  {
    //printf("Free item available\n");
    item = MemItemPointer(mem, mem->firstFree);
    mem->firstFree = item->next;
  }
  else
  {
    VAddr memUsed = mem->end - mem->start;
    //printf("memUsed: %i\n", memUsed);
    //printf("mem->size: %i\n", mem->size);
    if (!(memUsed < (mem->size + sizeof(VItem))))
    {
      printf("[GURU_MEDITATION: -1] OUT OF MEMORY\n");
      //exit(0); 
      return NULL;
    }
    //printf("Free space available\n");
    item = mem->end;
    mem->end += sizeof(VItem);
  }

  item->data = 0;
  item->type = 0;
  item->next = 0;

  return item;
}

void MemDeallocItem(VMem* mem, VItem* item)
{
  item->next = mem->firstFree;

  mem->firstFree = MemItemAddr(mem, item);
}

int main()
{
  printf("Hi World\n");
  PrintBinaryUInt(0x80000000);
  PrintBinaryUInt(0x1 << 1);
  //PrintBinaryUInt(0x1 >> 1);

  VMem* mem = MemNew();
/*
  VItem* item = MemAllocItem(mem);
  ItemSetGCMark(item, 1);
  ItemSetType(item, 42);
  PrintBinaryUInt(item->type);
  printf("Type: %i\n", ItemType(item));
  printf("Mark: %i\n", ItemGCMark(item));

  VItem* item2 = MemAllocItem(mem);
  ItemSetType(item2, 44);

  MemCons(mem, item, item2);

  VItem* next = MemItemPointer(mem, item->next);
  printf("Type: %i\n", ItemType(next));

  printf("MemFirstFree: %i\n", mem->firstFree);
  MemDeallocItem(mem, item);
  printf("MemFirstFree: %i\n", mem->firstFree);

  item = MemAllocItem(mem);
  printf("Type: %i\n", ItemType(item));
*/

  // Build list
  VItem* item;
  VItem* next;
  VItem* first;
  
  first = MemAllocItem(mem);
  first->data = 1;
  item = first;
  while (1)
  {
    next = MemAllocItem(mem);
    if (NULL == next) break;
    next->data = item->data + 1;
    MemCons(mem, item, next);
    //printf("Next addr: %i\n", item->next);
    //item->next = MemItemAddr(mem, item2);
    item = next;
  }

  // Print list items
  item = first;
  while (item->next)
  {
    printf("FIRST LIST: %i\n", (int)item->data);
    item = MemItemPointer(mem, item->next);
  }

  // Dealloc list items
  item = first;
  while (item)
  {
    VAddr nextAddr = item->next;
    printf("DEALLOC: %i\n", (int)item->data);
    MemDeallocItem(mem, item);
    item = NULL;
    if (nextAddr)
    {
      VItem* next = MemItemPointer(mem, nextAddr);
      item = next;
    }
  }

  // Build list again
  first = MemAllocItem(mem);
  first->data = 1;
  item = first;
  while (1)
  {
    next = MemAllocItem(mem);
    if (NULL == next) break;
    next->data = item->data + 1;
    MemCons(mem, item, next);
    //printf("Next addr: %i\n", item->next);
    //item->next = MemItemAddr(mem, item2);
    item = next;
  }

  // Print list items
  VAddr addr = MemItemAddr(mem, first);
  while (addr)
  {
    VItem* item = MemItemPointer(mem, addr);
    printf("SECOND LIST: %i\n", (int)item->data);
    addr = item->next;
  }

  MemFree(mem);

  printf("DONE\n");
}
