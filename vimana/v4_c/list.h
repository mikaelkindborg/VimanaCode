/***************************************************************

Massa svammel här:

The global symbol table is a list with string items 
of TypeString. This table is found in interp.h.

The global symbol table works in tandem with the global
value table. Entries that are bound appear in this list
as value items.

Items on the stack are like items in the program code.
Items in the local environment table are the same.

Some type values for lists are:

TypeSymbol - value.symbol refers to index in the symbol table
TypeIntNum - value.intNum holds the integer value
TypeDecNum - value.decNum holds the decimal number value
TypeList   - value.list points to the list

In a function body, local vars refer to entries in the 
local environment table of the stack frame. This type of
item uses the following field:

TypeLocalSymbol - value.symbol is an index to the stackframe 
environment table. The actual item that holds the type and value 
is in the environment table.

Note that array indexes are used in place of string symbols 
(hash maps) to speed up execution. Symbol lookups (in the
local environment and in the global symbol table) always 
use indexed array access.

***************************************************************/

int GMallocCounter = 0;
int GReallocCounter = 0;

/****************** LISTS ******************/

// List (growable array) that holds items.
// Lists are used frequently throughout the implementation,
// for the code and the data stack, but also in place of
// C records for stackframes and function objects. This is
// a bit experimental, I want the C-implementation to share
// as much as possible with the high-level language. These
// structures could then be accessed from the high-level 
// language. The interpreter object could also be a list.
// TODO: Flag to say that list has been freed? GC flag?

typedef struct MyList
{
  int   length;     // Current number of items
  int   maxLength;  // Max number of items
  Item* items;      // Array of items
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

int ListLength(List* list)
{
  return list->length;
}

void ListGrow(List* list, size_t newSize)
{
  // TODO: Does not compile, reallocarray not found.
  //Item* newArray = reallocarray(list->items, sizeof(Item), newSize);

  GReallocCounter ++;
  printf("REALLOC CONUTER: %i\n", GReallocCounter);
  
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

// Returns the index of the new item.
Index ListPush(List* list, Item item)
{
  // Grow list array if needed.
  if (list->length + 1 > list->maxLength)
    ListGrow(list, list->length + ListGrowIncrement);
  list->items[list->length] = item;
  list->length++;
  return list->length - 1; // Index of new item.
}

Item ListPop(List* list)
{
  if (list->length < 1)
    ErrorExit("ListPop: Cannot pop list of length: %i", list->length);
  list->length--;
  return list->items[list->length];
}

Item ListGet(List* list, int index)
{
  if (index >= list->length)
    ErrorExit("ListGet: Index out of bounds: %i\n", index);
  return list->items[index];
}

void ListSet(List* list, int index, Item item)
{
  // Grow list if needed.
  if (index >= list->maxLength)
    ListGrow(list, index + ListGrowIncrement);
  if (index >= list->length)
    list->length = index + 1;
  list->items[index] = item;
}

// Experimental. Useful for updating item values "in place" 
// without having to copy and write back the item.
Item* ListGetItemPtr(List* list, int index)
{
  if (index >= list->length)
    ErrorExit("ListGetItemPtr: Index out of bounds: %i\n", index);
  return &(list->items[index]);
}

// TODO: Delete?
// Associative list
/*
Item ListLookup(List* list, Index symbolIndex)
{
  // TODO
  return ItemWithVirgin();
}
*/
