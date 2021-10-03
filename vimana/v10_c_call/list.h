/*
File: list.h
Author: Mikael Kindborg (mikael@kindborg.com)

List object (array) used for code and stacks.
Size of items in the list is configurable (all items must be same size).
*/

// LIST OBJECT -------------------------------------------------

typedef struct __VList
{
  VObj         header;       // Object header
  VSize        length;       // Current number of items
  VSize        maxLength;    // Max number of items
  VSize        itemSize;     // Size of a list item
  VByte*       items;        // Array of items
}
VList;

// Initial list array size and how much to grow on each reallocation.
#define ListGrowIncrement 8

// ListLength --------------------------------------------------

#define ListLength(list) ((list)->length)

// CREATE / FREE -----------------------------------------------

void ListInit_Internal(VList* list, VSize itemSize)
{
  // Set type.
  ObjSetType(list, ObjTypeList);

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

VList* ListCreate_Internal(VSize itemSize)
{
  VList* list = MemAlloc(sizeof(VList));
  ListInit_Internal(list, itemSize);
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
#ifndef OPTIMIZE
  if (newSize > VINDEXMAX)
    GuruMeditation(LISTGROW_VINDEXMAX_EXCEEDED);
#endif

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

  //PrintDebug("REALLOC successful in ListGrow");
}

#define ListCheckCapacity(list, index) \
  if ((index) >= (list)->maxLength) { ListGrow(list, (index) + ListGrowIncrement); }

// Generic Pointer-Based Functions -----------------------------

#define ListGetRaw(list, index) \
  ( ((VByte*)((list)->items)) + ((index) * (list)->itemSize) )

#define ListSetRaw(list, index, element) \
  memcpy(ListGetRaw(list, index), element, (list)->itemSize)

// Returns pointer to new element (increses length of list by 1).
void* ListPushRaw(VList* list)
{
  ListCheckCapacity(list, ListLength(list));
  ++ ListLength(list);
  return ListGetRaw(list, ListLength(list) - 1);
}
