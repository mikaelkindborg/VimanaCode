
// Template Functions ------------------------------------------

// Create ------------------------------------------------------

VList* ListType_Create()
{
  return ListCreate_Internal(sizeof(ItemType));
}

void ListType_Init(VList* list)
{
  ListInit_Internal(list, sizeof(ItemType));
}

// Set and Get -------------------------------------------------

#define ListType_SetRaw(list, index, item) \
  ( ((ItemType*)((list)->items))[index] = *(item) )

#define ListType_GetRaw(list, index) \
  ( & (((ItemType*)((list)->items))[index]) )

void ListType_Set(VList* list, VIndex index, ItemType* item)
{
  ListCheckCapacity(list, index);
  // Increase list length if needed.
  if (index >= ListLength(list)) 
    ListLength(list) = index + 1;
  ListType_SetRaw(list, index, item);
}

ItemType* ListType_Get(VList* list, VIndex index)
{
  if (index >= ListLength(list) || index < 0)
    GuruMeditation(LISTGET_INDEX_OUT_OF_BOUNDS);
  return ListType_GetRaw(list, index);
}

// Push and Pop ------------------------------------------------

void ListType_Push(VList* list, ItemType* item)
{
  VIndex index = ListLength(list);
  ListCheckCapacity(list, index);
  ++ ListLength(list);
  ListType_SetRaw(list, index, item);
}

ItemType* ListType_Pop(VList* list)
{
  if (ListLength(list) < 1)
    GuruMeditation(LISTPOP_CANNOT_POP_EMPTY_LIST);
  return ListType_GetRaw(list, -- ListLength(list));
}
