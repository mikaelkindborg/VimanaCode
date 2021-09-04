/*
File: list.h
Author: Mikael Kindborg (mikael@kindborg.com)

List object (array) used for code and stacks.
Size of items in the list is configurable (all items must be same size).
*/

// LIST OBJECT -------------------------------------------------

typedef struct __VList
{
  int        type;         // List type
  int        length;       // Current number of items
  int        maxLength;    // Max number of items
  int        itemSize;     // Size of a list item
  VByte*    items;        // Array of items
}
VList;

// Initial list array size and how much to grow on each reallocation.
#define ListGrowIncrement 5

// ListLength --------------------------------------------------

#define ListLength(list) ((list)->length)

// CREATE / FREE -----------------------------------------------

void ListInit(VList* list, int itemSize)
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

VList* ListCreate(int itemSize)
{
  VList* list = MemAlloc(sizeof(VList));
  ListInit(list, itemSize);
  return list;
}

void ListDeallocItems(VList* list)
{
  MemFree(list->items);
}

void ListFree(VList* list)
{
  // Free item array.
  ListDeallocItems(list);

  // Free list object.
  MemFree(list);
}

// LIST FUNCTIONS ----------------------------------------------

// ListGrow ----------------------------------------------------

void ListGrow(VList* list, size_t newSize)
{
  // Make space for more items.
  size_t newArraySize = newSize * list->itemSize;
  VByte* newArray = realloc(list->items, newArraySize);
  if (NULL == newArray)
    ErrorExit("ListGrow: Out of memory");
  list->items = newArray;
  list->maxLength = newSize;

  // Set new entries in the array to zero.
  size_t prevArraySize = ListLength(list) * list->itemSize;
  size_t numNewBytes = newArraySize - prevArraySize;
  VByte* p = newArray + prevArraySize;
  memset(p, 0, numNewBytes);

  PrintDebug("REALLOC successful in ListGrow");
}

// ListSetFast and ListGetFast ---------------------------------

#ifdef OPTIMIZE

  #define ListSetFast(list, index, item) \
    memcpy((list)->items + ((index) * (list)->itemSize), (item), (list)->itemSize)

#else

  void ListSetFast(VList* list, VIndex index, void* item)
  {
    size_t offset = index * list->itemSize;
    VByte* p = list->items + offset;
    memcpy(p, item, list->itemSize); // dest, src, nbytes
  }
  
#endif

#ifdef OPTIMIZE

  #define ListGetFast(list, index) \
    ((void*) (((list)->items) + ((index) * (list)->itemSize)))

#else

  void* ListGetFast(VList* list, VIndex index)
  {
    size_t offset = index * list->itemSize;
    return list->items + offset;
  }
  
#endif

// ListSet and ListGet -----------------------------------------

void ListSet(VList* list, VIndex index, void* item)
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

void* ListGet(VList* list, VIndex index)
{
  if (index >= ListLength(list) || index < 0)
    ErrorExitNum("ListGet: Index out of bounds: ", index);
  return ListGetFast(list, index);
}

// ListPush and ListPop ----------------------------------------

void ListPush(VList* list, void* item)
{
  ListSet(list, ListLength(list), item);
}

void* ListPop(VList* list)
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

  void ListDrop(VList* list)
  {
    if (ListLength(list) < 1)
      ErrorExit("ListDrop: Cannot drop from list of length < 0");
    -- ListLength(list);
  }

#endif

// ITEM DUP -> ITEM ITEM
void ListDup(VList* list)
{
  void* item = ListGet(list, ListLength(list) - 1);
  ListPush(list, item);
}

// ITEM1 ITEM2 2DUP -> ITEM1 ITEM2 ITEM1 ITEM2
void List2Dup(VList* list)
{
  void* item;
  item = ListGet(list, ListLength(list) - 2);
  ListPush(list, item);
  item = ListGet(list, ListLength(list) - 2);
  ListPush(list, item);
}

// ITEM1 ITEM2 OVER -> ITEM1 ITEM2 ITEM1
void ListOver(VList* list)
{
  void* item = ListGet(list, ListLength(list) - 2);
  ListPush(list, item);
}

// ITEM1 ITEM2 SWAP -> ITEM2 ITEM1
void ListSwap(VList* list)
{
  size_t size = list->itemSize;
  char temp[size];
  void* item1 = ListGet(list, ListLength(list) - 1);
  void* item2 = ListGet(list, ListLength(list) - 2);
  memcpy(temp, item1, size);
  memcpy(item1, item2, size);
  memcpy(item2, temp, size);
}


// Free List Deep ----------------------------------------------

// Note: For lists that contain VItem:s.
// Does not work for circular lists!
void ListFreeDeep(VList* list)
{
  for (VIndex i = 0; i < ListLength(list); ++i)
  {
    VItem* item = ListGet(list, i);
    if (IsList(*item))
    {
      ListFreeDeep(ItemObj(*item));
    }
    else
    if (IsString(*item))
    {
      MemFree(ItemString(*item));
    }
  }

  // Free item array.
  ListDeallocItems(list);

  // Free list object.
  MemFree(list);
}
