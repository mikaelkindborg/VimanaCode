int GMallocCounter = 0;
int GReallocCounter = 0;

// LISTS -------------------------------------------------------

typedef struct MyList
{
  // TODO: int last;
  int   length;     // Current number of items
  int   maxLength;  // Max number of items
  Item* items;      // Array of items
  int   refCount;
  int   listType;
}
List;

// Initial list array size and how much to grow on each reallocation.
#define ListGrowIncrement 10

List* ListCreate()
{
  // Alloc list object.
  size_t size = ListGrowIncrement;

  //GMallocCounter ++;
  //printf("MALLOC CONUTER: %i\n", GMallocCounter);

  List* list = malloc(sizeof(List));
  list->length = 0;
  list->maxLength = size;

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
  free(list);
}

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
} while (0)
#else
void ListPush(List* list, Item item)
{
  // Grow list array if needed.
  if (list->length + 1 > list->maxLength)
    ListGrow(list, list->length + ListGrowIncrement);
  list->items[list->length] = item;
  list->length++;
  //return list->length - 1; // Index of new item.
}
#endif

#define ListPopSet(list, item) \
  do { \
    if ((list)->length < 1) \
      ErrorExit("ListPopSet: Cannot pop list of length: %i", (list)->length); \
    (list)->length--; \
    (item) = (list)->items[list->length]; \
  } while(0)

Item ListPop(List* list)
{
  if (list->length < 1)
    ErrorExit("ListPop: Cannot pop list of length: %i", list->length);
  list->length--;
  return list->items[list->length];
}

#ifdef OPTIMIZE
#define ListArray(list) ((list)->items)
#else
Item* ListArray(List* list)
{
  return list->items;
}
#endif

/*
#ifdef OPTIMIZE
#define ListTop(list) ((list)->items[(list)->length - 1])
#else
Item ListTop(List* list)
{
  return list->items[list->length - 1];
}
#endif
*/

#ifdef OPTIMIZE
#define ListDrop(list) \
do { \
  if ((list)->length < 1) \
    ErrorExit("ListDrop: Cannot drop element from list of length: %i", (list)->length); \
  (list)->length--; \
} while(0)
#else
void ListDrop(List* list)
{
  if (list->length < 1)
    ErrorExit("ListDrop: Cannot drop element from list of length: %i", list->length);
  list->length--;
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
  (list)->items[index] = (item); \
} while (0)
#else
void ListSet(List* list, int index, Item item)
{
  // Grow list if needed.
  if (index >= list->maxLength)
    ListGrow(list, index + ListGrowIncrement);
  if (index >= list->length)
    list->length = index + 1;
  list->items[index] = item;
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
  int    length = list->length;
  size_t itemSize = sizeof(Item);
  Item*  item = list->items;
  int    i = 0;

  PrintDebug("ListAssocSetGet:");

  while (i < length)
  {
    PrintDebug("  i = %i", i);
    if (IsSymbol(*item) && (item->value.symbol == symbol))
    {
      PrintDebug("  Found symbol: %i", item->value.symbol);
      // Found symbol entry.
      Item* p = item + 1;
      if (value)
      {
        PrintDebug("  Set existing symbol: %i to valueType: %lu", symbol, value->type);
        *p = *value;
      }
      PrintDebug("  return symbol: %i valueType: %lu", p->value.symbol, p->type);
      return p;
    }
    i = i + 2;
    item = item + 2;
  }

  // Reached end of list, add symbol/value if set.
  if (value)
  {
    PrintDebug("  Set symbol: %i to valueType: %lu", symbol, (*value).type);
    ListPush(list, ItemWithSymbol(symbol));
    ListPush(list, *value);
    return value;
  }

  // Otherwise return NULL.
  return NULL;
}

// Associative list lookup.
Item* ListAssocGet(List* list, Index symbolIndex)
{
  PrintDebug("ListAssocGet:");
  return ListAssocSetGet(list, symbolIndex, NULL);
}

// Associative list set.
void ListAssocSet(List* list, Index symbolIndex, Item* value)
{
  PrintDebug("ListAssocSet:");
  ListAssocSetGet(list, symbolIndex, value);
}
