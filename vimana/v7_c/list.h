// For debugging.
//int GMallocCounter = 0;
//int GReallocCounter = 0;

// LISTS -------------------------------------------------------

// Lists can be used to hold data and/or code (as in Lisp).
//
// Lists can also be used as closures using the BIND primitive.
// When using BIND one should also use GG (garbage collection),
// since the current implementation does not count references
// to shared environments.

typedef struct MyList
{
  int   gcMarker;     // Mark flag for GC
  int   printMarker;  // Mark flag for print and other list traversal
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
  //printf("MALLOC COUNTER: %i\n", GMallocCounter);

  List* list = malloc(sizeof(List));
  list->gcMarker = 0;
  list->printMarker = 0;
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

void ListFree(List* list)
{
  PrintDebug("ListFree: %lu", (unsigned long)list);

  // Free item array.
  free(list->items);

  // Environment (list->env) is deallocated by GC.
  // Using closures without GC will leak memory.

  // Free list object.
  free(list);
}

#ifdef OPTIMIZE
#define ListEmpty(list) (list)->length = 0
#else
void ListEmpty(List* list)
{
  // Empty list.
  list->length = 0;
}
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
  //printf("REALLOC COUNTER: %i\n", GReallocCounter);
  
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
}
#endif

#define ListPopInto(list, item) \
  do { \
    if ((list)->length < 1) \
      ErrorExit("ListPopInto: Cannot pop empty list"); \
    (list)->length --; \
    (item) = (list)->items[list->length]; \
  } while(0)

/* UNUSED
Item ListPop(List* list)
{
  if (list->length < 1)
    ErrorExit("ListPop: Cannot pop empty list");
  -- list->length;
  Item item = list->items[list->length];
  return item;
}
*/

#ifdef OPTIMIZE

// ./vimana  11.27s user 0.01s system 96% cpu 11.668 total
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
#define ListGet(list, index) ((list)->items[index])
#else
Item ListGet(List* list, int index)
{
  if (index >= list->length)
    ErrorExit("ListGet: Index out of bounds: %i", index);
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

// Associative list set or get. Assumes symbol and value in pairs.
Item* ListAssocSetGet(List* list, Index symbol, Item* value)
{
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

#define ListAssocGet(list, symbolIndex) \
  ListAssocSetGet(list, symbolIndex, NULL)

#define ListAssocSet(list, symbolIndex, value) \
  ListAssocSetGet(list, symbolIndex, value)

// Lookup a string and return the index if it is found.
Index ListLookupStringIndex(List* list, char* symbolString)
{
  for (int i = 0; i < ListLength(list); ++ i)
  {
    Item item = ListGet(list, i);
    char* string = item.value.string;
    if (StringEquals(string, symbolString))
      return i; // Found it.
  }
  return -1; // Not found.
}
