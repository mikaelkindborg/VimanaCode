/*
File: list.h
Author: Mikael Kindborg (mikael@kindborg.com)

List object (array) used for code and stacks.
Size of items in the list is configurable (all items must be same size).
*/

// LIST OBJECT -------------------------------------------------

typedef struct __VmList
{
  int        type;         // List type
  int        length;       // Current number of items
  int        maxLength;    // Max number of items
  int        itemSize;     // Size of a list item
  VmByte*    items;        // Array of items
}
VmList;

// Initial list array size and how much to grow on each reallocation.
#define ListGrowIncrement 5

// ListLength --------------------------------------------------

#define ListLength(list) ((list)->length)

// CREATE / FREE -----------------------------------------------

void ListInit(VmList* list, int itemSize)
{
  list->type = TypeList;
  
  // Set inital values.
  size_t size = ListGrowIncrement;
  ListLength(list) = 0;
  list->maxLength = size;
  list->itemSize = itemSize;

  // Alloc list array.
  size_t arraySize = size * itemSize;
  void* itemArray = MemAlloc(arraySize);
  list->items = itemArray;

  // Init list array.
  memset(itemArray, 0, arraySize);
}

VmList* ListCreate(int itemSize)
{
  VmList* list = MemAlloc(sizeof(VmList));
  ListInit(list, itemSize);
  return list;
}

void ListDeallocItems(VmList* list)
{
  MemFree(list->items);
}

void ListFree(VmList* list)
{
  // Free item array.
  ListDeallocItems(list);

  // Free list object.
  MemFree(list);
}

// LIST FUNCTIONS ----------------------------------------------

// ListGrow ----------------------------------------------------

void ListGrow(VmList* list, size_t newSize)
{
  // Make space for more items.
  size_t newArraySize = newSize * list->itemSize;
  VmByte* newArray = realloc(list->items, newArraySize);
  if (NULL == newArray)
    ErrorExit("ListGrow: Out of memory");
  list->items = newArray;
  list->maxLength = newSize;

  // Set new entries in the array to zero.
  size_t prevArraySize = ListLength(list) * list->itemSize;
  size_t numNewBytes = newArraySize - prevArraySize;
  VmByte* p = newArray + prevArraySize;
  memset(p, 0, numNewBytes);

  PrintDebug("REALLOC successful in ListGrow");
}

// ListSetFast and ListGetFast ---------------------------------

#ifdef OPTIMIZE

  #define ListSetFast(list, index, item) \
    memcpy((list)->items + ((index) * (list)->itemSize), (item), (list)->itemSize)

#else

  void ListSetFast(VmList* list, VmIndex index, void* item)
  {
    size_t offset = index * list->itemSize;
    VmByte* p = list->items + offset;
    memcpy(p, item, list->itemSize); // dest, src, nbytes
  }
  
#endif

#ifdef OPTIMIZE

  #define ListGetFast(list, index) \
    ((void*) (((list)->items) + ((index) * (list)->itemSize)))

#else

  void* ListGetFast(VmList* list, VmIndex index)
  {
    size_t offset = index * list->itemSize;
    return list->items + offset;
  }
  
#endif

// ListSet and ListGet -----------------------------------------

void ListSet(VmList* list, VmIndex index, void* item)
{
  if (index < 0)
    ErrorExit("ListSet: Index < 0");
    
  // Grow list if needed.
  if (index >= list->maxLength)
    ListGrow(list, index + ListGrowIncrement);

  // Increase length if needed.
  if (index >= ListLength(list)) 
    ListLength(list) = index + 1;
  
  // Set data.
  ListSetFast(list, index, item);
}

void* ListGet(VmList* list, VmIndex index)
{
  if (index >= ListLength(list) || index < 0)
    ErrorExitNum("ListGet: Index out of bounds: ", index);
  return ListGetFast(list, index);
}

// ListPush and ListPop ----------------------------------------

void ListPush(VmList* list, void* item)
{
  ListSet(list, ListLength(list), item);
}

void* ListPop(VmList* list)
{
  if (ListLength(list) < 1)
    ErrorExit("ListPop: Cannot pop empty list");
  return ListGetFast(list, -- ListLength(list));
}

// Forth Stack Operations --------------------------------------

// ITEM DROP ->
#ifdef OPTIMIZE

  #define ListDrop(list) \
    do { \
      if (ListLength(list) < 1) \
        ErrorExit("ListDrop: Cannot drop from list of length < 0"); \
      -- ListLength(list); \
    } while (0)
  
#else

  void ListDrop(VmList* list)
  {
    if (ListLength(list) < 1)
      ErrorExit("ListDrop: Cannot drop from list of length < 0");
    -- ListLength(list);
  }

#endif

// ITEM DUP -> ITEM ITEM
void ListDup(VmList* list)
{
  void* item = ListGet(list, ListLength(list) - 1);
  ListPush(list, item);
}

// ITEM1 ITEM2 2DUP -> ITEM1 ITEM2 ITEM1 ITEM2
void List2Dup(VmList* list)
{
  void* item;
  item = ListGet(list, ListLength(list) - 2);
  ListPush(list, item);
  item = ListGet(list, ListLength(list) - 2);
  ListPush(list, item);
}

// ITEM1 ITEM2 OVER -> ITEM1 ITEM2 ITEM1
void ListOver(VmList* list)
{
  void* item = ListGet(list, ListLength(list) - 2);
  ListPush(list, item);
}

// ITEM1 ITEM2 SWAP -> ITEM2 ITEM1
void ListSwap(VmList* list)
{
  size_t size = list->itemSize;
  char temp[size];
  void* item1 = ListGet(list, ListLength(list) - 1);
  void* item2 = ListGet(list, ListLength(list) - 2);
  memcpy(temp, item1, size);
  memcpy(item1, item2, size);
  memcpy(item2, temp, size);
}
