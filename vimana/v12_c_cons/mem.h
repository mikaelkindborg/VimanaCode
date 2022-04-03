/*
File: mem.h
Author: Mikael Kindborg (mikael@kindborg.com)

Memory manager for linked items.
*/

#ifdef TRACK_MEMORY_USAGE

int GMemAllocCounter = 0;
int GMemFreeCounter = 0;

#define SysAlloc(size) malloc(size); ++ GMemAllocCounter
#define SysFree(obj) free(obj); ++ GMemFreeCounter

void PrintMemStat()
{
  Print("SysAlloc: "); PrintNum(GMemAllocCounter); PrintNewLine();
  Print("SysFree:  "); PrintNum(GMemFreeCounter);  PrintNewLine();
}

#else

#define SysAlloc(size) malloc(size)
#define SysFree(obj) free(obj)
#define PrintMemStat()

#endif

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
  VAddr size = memSize;
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
