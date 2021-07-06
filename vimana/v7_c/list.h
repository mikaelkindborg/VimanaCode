int GMallocCounter = 0;
int GReallocCounter = 0;

// LISTS -------------------------------------------------------

#define ListFreeShallow 1
#define ListFreeDeep    2
#define ListFreeDeeper  3

typedef struct MyList
{
  //int   listType;
  int   refCount;     // Reference counter for GC
  int   length;       // Current number of items; TODO: last ???
  int   maxLength;    // Max number of items
  Item* items;        // Array of items
  struct MyList* env; // Local environment for closures
}
List;

// Initial list array size and how much to grow on each reallocation.
#define ListGrowIncrement 10

// CREATE / FREE -----------------------------------------------

List* ListCreate()
{
  // Alloc list object.
  size_t size = ListGrowIncrement;

  //GMallocCounter ++;
  //printf("MALLOC CONUTER: %i\n", GMallocCounter);

  List* list = malloc(sizeof(List));
  list->refCount = 0;
  list->length = 0;
  list->maxLength = size;
  list->env = NULL;

  // Alloc list array.
  size_t arraySize = size * sizeof(Item);
  Item* itemArray = malloc(arraySize);
  list->items = itemArray;

  // Init list array.
  memset(itemArray, 0, arraySize);

  // Return list object.
  return list;
}

// TODO: whatToFree is how deep to free.
// But proper GC should be used instead.
void ListFree(List* list, int whatToFree)
{
  free(list->items);
  if (list->env) free(list->env);
  free(list);
}

// GARBAGE COLLECTION ------------------------------------------

#ifdef USE_GC

#ifdef OPTIMIZE

#define ItemRefCountIncr(item) \
do { \
  if (IsDynAlloc(item)) \
    ++ ((item).value.list->refCount); \
} while(0)

#define ItemRefCountDecr(item) \
do { \
  if (IsDynAlloc(item)) \
    -- ((item).value.list->refCount); \
} while(0)

#define ItemGC(item) \
do { \
  if (IsDynAlloc(item)) \
    if (0 >= (item).value.list->refCount) \
      ListFree((item).value.list, ListFreeShallow); \
} while(0)

#else

void ItemRefCountIncr(Item item)
{
  if (IsDynAlloc(item))
  {
    PrintDebug("ItemRefCountIncr");
    ++ (item.value.list->refCount);
    PrintDebug("  refCount: %i", item.value.list->refCount);
  }
}

void ItemRefCountDecr(Item item)
{
  if (IsDynAlloc(item))
  {
    PrintDebug("ItemRefCountDecr");
    -- (item.value.list->refCount);
    PrintDebug("  refCount: %i", item.value.list->refCount);
  }
}

void ItemGC(Item item)
{
  if (IsDynAlloc(item))
  {
    if (0 >= item.value.list->refCount)
    {
      PrintDebug("ItemGC: ListFree");
      PrintDebug("  refCount: %i", item.value.list->refCount);
      ListFree(item.value.list, ListFreeShallow);
    }
  }
}

#endif

#else

#define ItemRefCountIncr(item)
#define ItemRefCountDecr(item)
#define ItemGC(item)

#endif

// LIST FUNCTIONS ----------------------------------------------

#ifdef OPTIMIZE
#define ListLength(list) ((list)->length)
#else
int ListLength(List* list)
{
  return list->length;
}
#endif

void ListGrow(List* list, size_t newSize)
{
  // TODO: Does not compile, reallocarray not found.
  //Item* newArray = reallocarray(list->items, sizeof(Item), newSize);

  //GReallocCounter ++;
  //printf("REALLOC CONUTER: %i\n", GReallocCounter);
  
  // Make space for more items.
  size_t newArraySize = newSize * sizeof(Item);
  Item* newArray = realloc(list->items, newArraySize);
  if (NULL == newArray)
    ErrorExit("ListGrow: Out of memory");
  list->items = newArray;
  list->maxLength = newSize;

  // Set new entries in the array to zero.
  size_t prevArraySize = list->length * sizeof(Item);
  size_t numNewBytes = newArraySize - prevArraySize;
  Byte* p = (Byte*) newArray;
  p = p + prevArraySize;
  memset(p, 0, numNewBytes);

  //PrintDebug("REALLOC successful in ListGrow");
}

#ifdef OPTIMIZE
#define ListPush(list, item) \
do { \
  if ((list)->length + 1 > (list)->maxLength) \
    ListGrow(list, (list)->length + ListGrowIncrement); \
  (list)->items[(list)->length] = (item); \
  (list)->length++; \
  ItemRefCountIncr(item); \
} while (0)
#else
void ListPush(List* list, Item item)
{
  // Grow list array if needed.
  if (list->length + 1 > list->maxLength)
    ListGrow(list, list->length + ListGrowIncrement);
  list->items[list->length] = item;
  list->length++;
  ItemRefCountIncr(item);
}
#endif

#define ListPopInto(list, item) \
  do { \
    if ((list)->length < 1) \
      ErrorExit("ListPopInto: Cannot pop list of length < 0"); \
    (list)->length --; \
    (item) = (list)->items[list->length]; \
    ItemRefCountDecr(item); \
  } while(0)

/* UNUSED
Item ListPop(List* list)
{
  if (list->length < 1)
    ErrorExit("ListPop: Cannot pop list of length: %i", list->length);
  -- list->length;
  Item item = list->items[list->length];
  ItemRefCountDecr(item);
  return item;
}
*/

#ifdef OPTIMIZE
// TODO: Test which style is faster.
/*
// ./vimana  11.53s user 0.01s system 96% cpu 11.898 total
#define ListDrop(list) \
  do { \
    Index length = (list)->length; \
    -- length; \
    if (length < 0)  \
      ErrorExit("ListDrop: Cannot drop from list length < 0"); \
    ItemRefCountDecr((list)->items[length]); \
    ItemGC((list)->items[length]); \
    (list)->length = length; \
  } while (0)
*/
/*
// ./vimana  11.32s user 0.01s system 96% cpu 11.722 total
#define ListDrop(list) \
  do { \
    if (list->length < 1) \
      ErrorExit("ListDrop: Cannot drop from list of length: %i", list->length); \
    -- list->length; \
    Item item = list->items[list->length]; \
    ItemRefCountDecr(item); \
    ItemGC(item); \
  } while (0)
*/

// ./vimana  11.27s user 0.01s system 96% cpu 11.668 total
#define ListDrop(list) \
  do { \
    if (list->length < 1) \
      ErrorExit("ListDrop: Cannot drop from list of length < 0"); \
    -- list->length; \
    ItemRefCountDecr(list->items[list->length]); \
    ItemGC(list->items[list->length]); \
  } while (0)
#else
void ListDrop(List* list)
{
  if (list->length < 1)
    ErrorExit("ListDrop: Cannot drop from list of length: %i", list->length);
  -- list->length;
  Item item = list->items[list->length];
  ItemRefCountDecr(item);
  ItemGC(item);
}
#endif

#ifdef OPTIMIZE
#define ListGet(list, index) ((list)->items[index])
#else
Item ListGet(List* list, int index)
{
  if (index >= list->length)
    ErrorExit("ListGet: Index out of bounds: %i\n", index);
  return list->items[index];
}
#endif

#ifdef OPTIMIZE
#define ListSet(list, index, item) \
  do { \
    if ((index) >= (list)->maxLength) \
      ListGrow((list), (index) + ListGrowIncrement); \
    if ((index) >= (list)->length) \
      (list)->length = (index) + 1; \
    ItemRefCountDecr((list)->items[index]); \
    ItemGC((list)->items[index]); \
    ItemRefCountIncr(item); \
    (list)->items[index] = (item); \
  } while (0)
#else
void ListSet(List* list, int index, Item item)
{
  PrintDebug("ListSet");
  // Grow list if needed.
  if (index >= list->maxLength)
    ListGrow(list, index + ListGrowIncrement);
  if (index >= list->length)
    list->length = index + 1;
  // GC
  ItemRefCountDecr(list->items[index]);
  ItemGC(list->items[index]);
  ItemRefCountIncr(item);
  // Set new item.
  list->items[index] = item;
}
#endif

#ifdef OPTIMIZE
#define ListDup(list, index) \
  do { \
    Item item = ListGet(list, index); \
    ListPush(list, item); \
  } while (0)
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
    Index index2 = ListLength(list) - 2; \
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

// Associative list set or get. Assumes symbol and value in pairs.
Item* ListAssocSetGet(List* list, Index symbol, Item* value)
{
  PrintDebug("ListAssocSetGet");

  int    length = list->length;
  size_t itemSize = sizeof(Item);
  Item*  item = list->items;
  int    i = 0;

  while (i < length)
  {
    if (IsSymbol(*item) && (item->value.symbol == symbol))
    {
      // Found symbol entry.
      Item* p = item + 1;
      if (value)
      {
        ItemRefCountDecr(*p);
        ItemGC(*p);
        ItemRefCountIncr(*value);
        *p = *value;
      }
      return p;
    }
    i = i + 2;
    item = item + 2;
  }

  // Reached end of list, add symbol/value if set.
  if (value)
  {
    ListPush(list, ItemWithSymbol(symbol));
    ListPush(list, *value);
    return value;
  }

  // Otherwise return NULL.
  return NULL;
}

Bool ListContainsSymbol(List* list, Item item)
{
  int length = list->length;

  for (int i = 0; i < length; ++i)
  {
    Item current = ListGet(list, i);
    if (current.type == item.type && current.value.symbol == item.value.symbol)
    {
      return TRUE;
    }
  }

  return FALSE;
}

/*
// UNUSED 
// TODO: Make macros for these?
// Associative list lookup.
Item* ListAssocGet(List* list, Index symbolIndex)
{
  return ListAssocSetGet(list, symbolIndex, NULL);
}

// Associative list set.
void ListAssocSet(List* list, Index symbolIndex, Item* value)
{
  ListAssocSetGet(list, symbolIndex, value);
}
*/
