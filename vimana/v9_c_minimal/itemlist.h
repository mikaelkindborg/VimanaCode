
// Template Functions ------------------------------------------

// Create ------------------------------------------------------

VList* ItemList_Create()
{
  return ListCreate(sizeof(VItem));
}

void ItemList_Init(VList* list)
{
  ListInit(list, sizeof(VItem));
}

// Set and Get -------------------------------------------------

#define ItemList_SetRaw(list, index, item) \
  ( ((VItem*)((list)->items))[index] = *(item) )

#define ItemList_GetRaw(list, index) \
  ( & (((VItem*)((list)->items))[index]) )

void ItemList_Set(VList* list, VIndex index, VItem* item)
{
  ListCheckCapacity(list, index);
  // Increase list length if needed.
  if (index >= ListLength(list)) 
    ListLength(list) = index + 1;
  ItemList_SetRaw(list, index, item);
}

VItem* ItemList_Get(VList* list, VIndex index)
{
  if (index >= ListLength(list) || index < 0)
    GuruMeditation(LISTGET_INDEX_OUT_OF_BOUNDS);
  return ItemList_GetRaw(list, index);
}

// Push and Pop ------------------------------------------------

void ItemList_Push(VList* list, VItem* item)
{
  VIndex index = ListLength(list);
  ListCheckCapacity(list, index);
  ++ ListLength(list);
  ItemList_SetRaw(list, index, item);
}

VItem* ItemList_Pop(VList* list)
{
  if (ListLength(list) < 1)
    GuruMeditation(LISTPOP_CANNOT_POP_EMPTY_LIST);
  return ItemList_GetRaw(list, -- ListLength(list));
}
