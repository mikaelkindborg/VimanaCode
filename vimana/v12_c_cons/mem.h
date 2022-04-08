/*
File: mem.h
Author: Mikael Kindborg (mikael@kindborg.com)

Memory manager for linked items.
*/

typedef struct __VMem
{
  VAddr firstFree;
  VAddr size;
  void* end;
  void* start;
}
VMem;

VMem* MemNew(int memSize)
{
  int memByteSize = memSize * sizeof(VItem);
  VAddr size = memByteSize;
  VMem* mem = SysAlloc(size);
  mem->firstFree = 0;
  mem->size = size - sizeof(VMem);
  mem->start = mem + sizeof(VMem);
  mem->end = mem->start;
  return mem;
}

void MemFree(VMem* mem)
{
  SysFree(mem);
}

#define MemItemAddr(mem, item)    ( ( ((void*)(item)) - (mem)->start ) + 1 ) 
#define MemItemPointer(mem, addr) ( (VItem*) ((mem)->start + addr - 1) ) 

void MemCons(VMem* mem, VItem* item1, VItem* item2)
{
  item1->next = MemItemAddr(mem, item2);
}

VItem* MemItemNext(VMem* mem, VItem* item)
{
  if (ItemNext(item))
  {
    return MemItemPointer(mem, ItemNext(item));
  }
  else
  {
    return NULL;
  }
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

  ItemInit(item);

  return item;
}

void MemDeallocItem(VMem* mem, VItem* item)
{
  item->next = mem->firstFree;

  mem->firstFree = MemItemAddr(mem, item);
}

void MemPrintList(VMem* mem, VItem* first);

void MemPrintItem(VMem* mem, VItem* item)
{
  //printf("type: %i ", ItemType(item));
  if (TypeList == ItemType(item))
    MemPrintList(mem, MemItemPointer(mem, ItemData(item)));
  else if (TypeIntNum == ItemType(item))
    printf("N%i", (int)ItemData(item));
  else if (TypePrimFun == ItemType(item))
    printf("P%i", (int)ItemData(item));
}

void MemPrintList(VMem* mem, VItem* first)
{
  printf("(");

  VItem* item = first;
  //VAddr addr = MemItemAddr(mem, first);
  int printSpace = FALSE;
  //while (addr)
  while (item)
  {
    if (printSpace) printf(" ");
    //VItem* item = MemItemPointer(mem, addr);
    MemPrintItem(mem, item);
    //addr = ItemNext(item);
    item = MemItemNext(mem, item);
    printSpace = TRUE;
  }
  printf(")");
}
