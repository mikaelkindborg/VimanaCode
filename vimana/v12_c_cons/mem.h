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

#define MemItemPointer(mem, addr) ( (VItem*) ((mem)->start + addr - 1) ) 

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

VAddr MemItemAddr(VMem* mem, VItem* item)
{
  if (NULL != item)
    return ( ((void*)(item)) - (mem)->start ) + 1;
  else
    return 0;
}     

void MemItemSetFirst(VMem* mem, VItem* list, VItem* item)
{
  ItemSetList(list, MemItemAddr(mem, item));
}

VItem* MemItemFirst(VMem* mem, VItem* list)
{
  if (ItemData(list))
    return MemItemPointer(mem, ItemData(list));
  else
    return NULL;
}

void MemItemSetNext(VMem* mem, VItem* item1, VItem* item2)
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

// Conses value into list (another item)
// Allocs and returns a new item
VItem* MemCons(VMem* mem, VItem* value, VItem* list)
{
  VItem* item = MemAllocItem(mem);
  if (!item) return NULL;
  *item = *value;
  if (NULL == list)
    item->next = 0;
  else
    item->next = MemItemAddr(mem, list);
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
  //printf("[T%i]", ItemType(item));
  if (TypeList == ItemType(item))
    MemPrintList(mem, item);
  else if (TypeIntNum == ItemType(item))
    printf("N%i", (int)ItemData(item));
  else if (TypePrimFun == ItemType(item))
    printf("P%i", (int)ItemData(item));
  else if (TypeSymbol == ItemType(item))
    printf("S%i", (int)ItemData(item));
  else if (TypeString == ItemType(item))
    printf("'%s'", (char*)ItemData(item));
}

void MemPrintList(VMem* mem, VItem* list)
{
  printf("(");

  int printSpace = FALSE;
  VItem* item = MemItemFirst(mem, list);

  while (item)
  {
    if (printSpace) printf(" ");
    MemPrintItem(mem, item);
    item = MemItemNext(mem, item);
    printSpace = TRUE;
  }

  printf(")");
}
