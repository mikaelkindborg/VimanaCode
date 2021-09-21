/*
File: itemlist.h
Author: Mikael Kindborg (mikael@kindborg.com)

Functions for lists (arrays) with elements of type VItem.
Also see generated file: itemlist_gen.h
*/

#include "itemlist_gen.h"

// ItemList Access ---------------------------------------------

#define ItemList(item) ((VList*)ItemObj(item))

#define ItemObjHeader(item) ((VObj*)ItemObj(item))

// Forth Stack Operations --------------------------------------

// ITEM DROP ->
#ifdef OPTIMIZE

  #define ItemListDrop(list) \
    do { \
      if (ListLength(list) < 1) \
        GuruMeditation(LISTDROP_CANNOT_DROP_FROM_EMPTY_LIST); \
      -- ListLength(list); \
    } while (0)

#else

  void ItemListDrop(VList* list)
  {
    if (ListLength(list) < 1)
      GuruMeditation(LISTDROP_CANNOT_DROP_FROM_EMPTY_LIST);
    -- ListLength(list);
  }

#endif

// ITEM DUP -> ITEM ITEM
#ifdef OPTIMIZE

  #define ItemListDup(list) \
    do { \
      void* item = ItemList_Get(list, ListLength(list) - 1); \
      ItemList_Push(list, item); \
    } while (0)

#else

  void ItemListDup(VList* list)
  {
    void* item = ItemList_Get(list, ListLength(list) - 1);
    ItemList_Push(list, item);
  }

#endif

// ITEM1 ITEM2 2DUP -> ITEM1 ITEM2 ITEM1 ITEM2
#ifdef OPTIMIZE

  #define ItemList2Dup(list) \
    do { \
      void* item; \
      item = ItemList_Get(list, ListLength(list) - 2); \
      ItemList_Push(list, item); \
      item = ItemList_Get(list, ListLength(list) - 2); \
      ItemList_Push(list, item); \
    } while (0)

#else

  void ItemList2Dup(VList* list)
  {
    void* item;
    item = ItemList_Get(list, ListLength(list) - 2);
    ItemList_Push(list, item);
    item = ItemList_Get(list, ListLength(list) - 2);
    ItemList_Push(list, item);
  }

#endif

// ITEM1 ITEM2 OVER -> ITEM1 ITEM2 ITEM1
#ifdef OPTIMIZE

  #define ItemListOver(list) \
    do { \
      void* item = ItemList_Get(list, ListLength(list) - 2); \
      ItemList_Push(list, item); \
    } while (0)

#else

  void ItemListOver(VList* list)
  {
    void* item = ItemList_Get(list, ListLength(list) - 2);
    ItemList_Push(list, item);
  }

#endif

// ITEM1 ITEM2 SWAP -> ITEM2 ITEM1
#ifdef OPTIMIZE

  #define ItemListSwap(list) \
    do { \
      VItem temp; \
      VItem* item1 = ItemList_GetRaw(list, ListLength(list) - 1); \
      VItem* item2 = ItemList_GetRaw(list, ListLength(list) - 2); \
      temp = *item1; \
      *item1 = *item2; \
      *item2 = temp; \
    } while (0)

#else

  void ItemListSwap(VList* list)
  {
    VItem temp;
    VItem* item1 = ItemList_GetRaw(list, ListLength(list) - 1);
    VItem* item2 = ItemList_GetRaw(list, ListLength(list) - 2);
    temp = *item1;
    *item1 = *item2;
    *item2 = temp;
  }

#endif

// List Search -------------------------------------------------

// Check if an item is in the list.
VBool ItemListContains(VList* list, VItem* item)
{
  for (int i = 0; i < ListLength(list); ++i)
  {
    if (ItemEquals(item, ItemList_GetRaw(list, i)))
      return TRUE;
  }

  return FALSE;
}

// Lookup a string and return the index if it is found.
// Returns -1 if not found.
// Assumes use of VItem to represent strings.
// All items in the list must be strings.
VIndex ListLookupString(VList* list, char* strToFind)
{
  for (int index = 0; index < ListLength(list); ++ index)
  {
    VItem* item = ItemList_Get(list, index);
    char* str = StringGetStr(ItemObj(item));
    if (StrEquals(strToFind, str))
      return index; // Found it.
  }
  return -1; // Not found.
}

// Push string item to list.
// Copies the string.
// Returns index to the added item.
VIndex ListAddString(VList* list, char* str)
{
  VString* string = StringCreate();
  StringWriteStr(string, str);
  VItem* item = ListPushRaw(list);
  ItemSetObj(item, string);
  return ListLength(list) - 1;
}

// Tiny GC -----------------------------------------------------

void ItemGC(VItem* item);

void ItemIncrRefCount(VItem* item)
{
  if (IsObj(item)) 
  {
    ++ ItemObjHeader(item)->refCount;
    //PrintStrNum("INCR_REF_COUNT: ", ItemObjHeader(item)->refCount);
  }
}

/*
void ItemDecrRefCount(VItem* item)
{
  if (IsObj(item)) -- ItemObjHeader(item)->refCount;
}
*/

void ListGCChildren(VList* list)
{
  for (int i = 0; i < ListLength(list); ++i)
  {
    VItem* item = ItemList_GetRaw(list, i);
    ItemGC(item);
  }
}

void PrintList(VList* list);

void ObjGC(VObj* obj)
{
  -- obj->refCount;

  //PrintStrNum("DECR_REF_COUNT: ", obj->refCount);

  if (obj->refCount <= 0)
  {
    if (TypeString == obj->type)
    {
      StringFree((VString*) obj);
    }
    else
    if (TypeList == obj->type || TypeFun == obj->type)
    {
      ListGCChildren((VList*) obj);
      ListFree((VList*) obj);
    }
    else
    {
      GuruMeditation(OBJGC_UNKNOWN_TYPE);
    }
  }
}

void ListGC(VList* list)
{
  ObjGC((VObj*) list);
}

void ItemGC(VItem* item)
{
  if (IsObj(item)) ObjGC(ItemObj(item));
}

// Free List Deep ----------------------------------------------

/*
void ListDeallocArrayBufDeep(VList* list);

// Assumes that list contains VItem:s.
// Does not work for circular lists!
void ListFreeDeep(VList* list)
{
  // Free items.
  ListDeallocArrayBufDeep(list);

  // Free list object.
  MemFree(list);
}

// Assumes that list contains VItem:s.
// Does not deallocate the top-level list object.
void ListDeallocArrayBufDeep(VList* list)
{
  for (VIndex i = 0; i < ListLength(list); ++i)
  {
    VItem* item = ItemList_Get(list, i);
    if (IsString(item))
    {
      StringFree(ItemObj(item));
    }
    else
    if (IsList(item))
    {
      ListFreeDeep(ItemObj(item));
    }
    else
    if (IsFun(item))
    {
      ListFreeDeep(ItemObj(item));
    }
  }

  // Free list array.
  ListDeallocArrayBuf(list);
}
*/
