/*
File: mem.h
Author: Mikael Kindborg (mikael@kindborg.com)

Memory manager for linked items.
*/

// -------------------------------------------------------------
// VMem struct
// -------------------------------------------------------------

typedef struct __VMem
{
  VAddr firstFree;
  VAddr size;
  void* end;
  void* start;
}
VMem;

static int GAllocCounter = 0;

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

  printf("GAllocCounter: %i\n", GAllocCounter);
}

VAddr MemItemAddr(VMem* mem, VItem* item)
{
  if (NULL != item)
    return ( ((void*)(item)) - (mem)->start ) + 1;
  else
    return 0;
}     

// -------------------------------------------------------------
// Alloc/dealloc
// -------------------------------------------------------------

VItem* MemAllocItem(VMem* mem)
{
  VItem* item;

  ++ GAllocCounter;

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
  if (IsTypeNone(item))
  {
    return;
  }

  -- GAllocCounter;

  if (IsTypeStringHolder(item))
  {
    //printf("free string: %s\n", (char*) item->data);
    SysFree((void*)ItemData(item));
  }

  ItemSetType(item, TypeNone);

  item->next = mem->firstFree;

  mem->firstFree = MemItemAddr(mem, item);
}

// -------------------------------------------------------------
// Item access
// -------------------------------------------------------------

void MemItemSetFirst(VMem* mem, VItem* item, VItem* first)
{
  ItemSetData(item, MemItemAddr(mem, first));
}

VItem* MemItemFirst(VMem* mem, VItem* item)
{
  if (ItemData(item))
    return MemItemPointer(mem, ItemData(item));
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
    return MemItemPointer(mem, ItemNext(item));
  else
    return NULL;
}

// Allocates a new item to hold the string
void MemItemSetString(VMem* mem, VItem* item, char* string)
{
  char* s = SysAlloc(strlen(string) + 1);
  strcpy(s, string);
  VItem* holder = MemAllocItem(mem);
  ItemSetData(holder, (VData)s);
  ItemSetType(holder, TypeStringHolder);

  ItemSetType(item, TypeString);
  MemItemSetFirst(mem, item, holder);
}

char* MemItemString(VMem* mem, VItem* item)
{
  VItem* holder = MemItemFirst(mem, item);

  if (NULL == holder) return NULL;
  if (!IsTypeStringHolder(holder)) return NULL;

  return (char*)holder->data;
}

// -------------------------------------------------------------
// GC
// -------------------------------------------------------------

void MemMark(VMem* mem, VItem* item)
{
  while (item)
  {
    if (ItemGCMark(item))
    {
      return;
    }

    //printf("mark item\n");
    ItemSetGCMark(item, 1);

    if (IsTypeList(item) || IsTypeString(item))
    {
      VItem* child = MemItemFirst(mem, item);
      MemMark(mem, child);
    }

    item = MemItemNext(mem, item);
  }
}

void MemSweep(VMem* mem)
{
  VItem* item = mem->start;

  while ((void*)item < mem->end)
  {
    if (ItemGCMark(item))
    {
      //printf("unmark\n");
      ItemSetGCMark(item, 0);
    }
    else
    {
      //printf("dealloc\n");
      MemDeallocItem(mem, item);
    }

    ++ item;
  }
}

// -------------------------------------------------------------
// Print items
// -------------------------------------------------------------

void MemPrintList(VMem* mem, VItem* first);

void MemPrintItem(VMem* mem, VItem* item)
{
  //printf("[T%i]", ItemType(item));
  if (IsTypeList(item))
    MemPrintList(mem, item);
  else if (IsTypeIntNum(item))
    printf("N%li", item->intNum);
  else if (IsTypeDecNum(item))
    printf("D%g", item->decNum);
  else if (IsTypePrimFun(item))
    printf("P%li", ItemData(item));
  else if (IsTypeSymbol(item))
    printf("S%li", ItemData(item));
  else if (IsTypeString(item))
    printf("'%s'", (char*)MemItemString(mem, item));
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
