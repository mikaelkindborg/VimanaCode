/*
File: list_dealloc.h
Author: Mikael Kindborg (mikael@kindborg.com)

Deallocate lists.
*/

// Free List Deep ----------------------------------------------

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
  for (VSize i = 0; i < ListLength(list); ++i)
  {
    VItem* item = ListGet(list, i);
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
void ListDeallocArrayBufDeepSafe(VList* list, VList* safeList)
{
  for (VSize i = 0; i < ListLength(list); ++i)
  {
    VItem* item = ListGet(list, i);
    if (ListContainsItem(safeList, item))
    
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