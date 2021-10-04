/*
File: itemlist.h
Author: Mikael Kindborg (mikael@kindborg.com)

Functions for lists (arrays) with elements of type VItem.
Also see generated file: itemlist_gen.h
*/

#include "itemlist_gen.h"

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
    char* str = StringGetStr(ItemString(item));
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
  ItemSetString(item, string);
  return ListLength(list) - 1;
}
