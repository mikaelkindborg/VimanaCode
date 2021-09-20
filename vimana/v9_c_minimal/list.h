/*
File: list.h
Author: Mikael Kindborg (mikael@kindborg.com)

List object (array) used for code and stacks.
Size of items in the list is configurable (all items must be same size).
*/

#define VItemType VItem

// LIST OBJECT -------------------------------------------------

typedef struct __VList
{
  VType        type;         // List type
  VSize        length;       // Current number of items
  VSize        maxLength;    // Max number of items
  VSize        itemSize;     // Size of a list item
  VByte*       items;        // Array of items
}
VList;

// Initial list array size and how much to grow on each reallocation.
#define ListGrowIncrement 5

// ListLength --------------------------------------------------

#define ListLength(list) ((list)->length)

// CREATE / FREE -----------------------------------------------

void ListInit(VList* list, VSize itemSize)
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

VList* ListCreate(VSize itemSize)
{
  VList* list = MemAlloc(sizeof(VList));
  ListInit(list, itemSize);
  return list;
}

void ListDeallocArrayBuf(VList* list)
{
  MemFree(list->items);
}

void ListFree(VList* list)
{
  // Free item array.
  ListDeallocArrayBuf(list);

  // Free list object.
  MemFree(list);
}

// LIST FUNCTIONS ----------------------------------------------

// ListGrow ----------------------------------------------------

void ListGrow(VList* list, VSize newSize)
{
  if (newSize > VINDEXMAX)
    GuruMeditation(LISTGROW_VINDEXMAX_EXCEEDED);

  // Make space for more items.
  size_t newArraySize = newSize * list->itemSize;
  VByte* newArray = realloc(list->items, newArraySize);
  if (NULL == newArray)
    GuruMeditation(LISTGROW_OUT_OF_MEMORY);
  list->items = newArray;
  list->maxLength = newSize;

  // Set new entries in the array to zero.
  size_t prevArraySize = ListLength(list) * list->itemSize;
  size_t numNewBytes = newArraySize - prevArraySize;
  VByte* p = newArray + prevArraySize;
  memset(p, 0, numNewBytes);

  PrintDebug("REALLOC successful in ListGrow");
}

#define ListCheckCapacity(list, index) \
do { \
  if ((index) < 0) GuruMeditation(LISTCHECKCAPACITY_LESS_THAN_ZERO); \
  if ((index) > VINDEXMAX) GuruMeditation(LISTCHECKCAPACITY_VINDEXMAX_EXCEEDED); \
  if ((index) >= (list)->maxLength) ListGrow(list, (index) + ListGrowIncrement); \
} while (0)

// Generic Pointer-Based Functions -----------------------------

#define ListGetItemPtr(list, index) \
  ( ((VByte*)((list)->items)) + ((index) * (list)->itemSize) )

#define ListSetItemPtr(list, index, item) \
  memcpy(ListGetItemPtr(list, index), item, (list)->itemSize)

// Returns pointer to new item (increses length of list by 1).
void* ListPushNewItemPtr(VList* list)
{
  ListCheckCapacity(list, ListLength(list));
  return ListGetItemPtr(list, ListLength(list) - 1);
}

/*
// Forth Stack Operations --------------------------------------

// ITEM DROP ->
#ifdef OPTIMIZE

  #define ListDrop(list) \
    do { \
      if (ListLength(list) < 1) \
        GuruMeditation(LISTDROP_CANNOT_DROP_FROM_EMPTY_LIST); \
      -- ListLength(list); \
    } while (0)
  
#else

  void ListDrop(VList* list)
  {
    if (ListLength(list) < 1)
      GuruMeditation(LISTDROP_CANNOT_DROP_FROM_EMPTY_LIST);
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
  void* item1 = ListItemPtr(list, ListLength(list) - 1);
  void* item2 = ListItemPtr(list, ListLength(list) - 2);
  memcpy(temp, item1, size);
  memcpy(item1, item2, size);
  memcpy(item2, temp, size);
}

// Object Access -----------------------------------------------

VList* ItemObjAsList(VItem item)
{
  if (!IsObj(item)) 
    GuruMeditation(ITEMOBJASLIST_NOT_POINTER);
  return item.value.obj;
}

// Check if an item is in the list.
VBool ListContainsItem(VList* list, VItem item)
{
  for (int i = 0; i < ListLength(list); ++i)
  {
    if (ItemEquals(item, ListItemPtr(list, i)))
      return TRUE;
  }

  return FALSE;
}
*/

