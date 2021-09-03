
// LIST OBJECT -------------------------------------------------

typedef struct __VmList
{
  int        length;       // Current number of items
  int        maxLength;    // Max number of items
  int        itemSize;     // Size of a list item
  VmByte*    items;        // Array of items
}
VmList;

// Initial list array size and how much to grow on each reallocation.
#define ListGrowIncrement 5

// CREATE / FREE -----------------------------------------------

void ListInit(VmList* list, int itemSize)
{
  // Set inital values.
  size_t size = ListGrowIncrement;
  list->length = 0;
  list->maxLength = size;
  list->itemSize = itemSize;

  // Alloc list array.
  size_t arraySize = size * itemSize;
  Item* itemArray = MemAlloc(arraySize);
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

void ListDeallocItems(List* list)
{
  MemFree(list->items);
}

void ListFree(List* list)
{
  // Free item array.
  ListDeallocItems(list);

  // Free list object.
  MemFree(list);
}

// LIST FUNCTIONS ----------------------------------------------

// ListLength --------------------------------------------------

#define ListLength(list) ((list)->length)

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
  size_t prevArraySize = list->length * list->itemSize;
  size_t numNewBytes = newArraySize - prevArraySize;
  VmByte* p = newArray + prevArraySize;
  memset(p, 0, numNewBytes);

  PrintDebug("REALLOC successful in ListGrow");
}

// ListSetFast and ListGetFast ---------------------------------

#ifdef OPTIMIZE
#define ListSetFast(list, index, item) \
  memcpy((list)->items + (index * (list)->itemSize), item, (list)->itemSize)
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
  ((void*) (((list)->items) + (index * (list)->itemSize)))
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
  // Grow list if needed.
  if (index >= list->maxLength)
    ListGrow(list, index + ListGrowIncrement);

  // Increase length if needed.
  if (index >= list->length) 
    list->length = index + 1;
  
  // Set data.
  ListSetFast(list, index, item);
}

#ifdef OPTIMIZE
#define ListGet(list, index) \
  ListGetFast(list, index)
#else
void* ListGet(VmList* list, VmIndex index)
{
  if (index >= list->length)
    ErrorExit("ListGet: Index out of bounds: %i", index);
  return ListGetFast(list, index);
}
#endif

// ListPush and ListPop -----------------------------------------

void ListPush(List* list, void* item)
{
  ListSet(list, list->length, item);
}

void ListPop(List* list)
{
  if (list->length < 1)
    ErrorExit("Cannot pop empty list");
  return ListGetFast(list, -- list->length);
}

// -------------------------------------------------------------


#ifdef OPTIMIZE
#define ListDrop(list) \
  do { \
    if (list->length < 1) \
      ErrorExit("ListDrop: Cannot drop from list of length < 0"); \
    -- list->length; \
  } while (0)
#else
void ListDrop(List* list)
{
  if (list->length < 1)
    ErrorExit("ListDrop: Cannot drop from list of length: %i", list->length);
  -- list->length;
  Item item = list->items[list->length];
}
#endif

#ifdef OPTIMIZE
#define ListDup(list, index) \
  ListPush(list, ListGet(list, index))
#else
void ListDup(List* list, Index index)
{
  Item item = ListGet(list, index);
  ListPush(list, item);
}
#endif

#ifdef OPTIMIZE
#define ListSwap(list) \
  do { \
    Index index1 = ListLength(list) - 1; \
    Index index2 = index1 - 1; \
    Item item1 = (list)->items[index1]; \
    (list)->items[index1] = (list)->items[index2]; \
    (list)->items[index2] = item1; \
  } while (0)
#else
void ListSwap(List* list)
{
  Index index1 = ListLength(list) - 1;
  Index index2 = ListLength(list) - 2;
  Item item1 = list->items[index1];
  list->items[index1] = list->items[index2];
  list->items[index2] = item1;
}
#endif

#define ListItemPtr(list, index) ((list)->items + (index))

#define ListPushItemPtrVal(list, itemPtr) \
do { \
  int length = (list)->length; \
  if (length + 1 > (list)->maxLength) \
    ListGrow(list, length + ListGrowIncrement); \
  (list)->items[length] = (*(itemPtr)); \
  (list)->length++; \
} while (0)


/*
// Experimental. Useful for updating item values "in place" 
// without having to copy and write back the item.
#ifdef OPTIMIZE
#define ListGetItemPtr(list, index) (&((list)->items[index]))
#else
Item* ListGetItemPtr(List* list, int index)
{
  if (index >= list->length)
    ErrorExit("ListGetItemPtr: Index out of bounds: %i\n", index);
  return &(list->items[index]);
}
#endif
*/
