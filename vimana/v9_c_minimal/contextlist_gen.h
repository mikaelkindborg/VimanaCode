
// Template Functions ------------------------------------------

// Create ------------------------------------------------------

VList* ContextList_Create()
{
  return ListCreate(sizeof(VContext));
}

void ContextList_Init(VList* list)
{
  ListInit(list, sizeof(VContext));
}

// Set and Get -------------------------------------------------

#define ContextList_SetRaw(list, index, item) \
  ( ((VContext*)((list)->items))[index] = *(item) )

#define ContextList_GetRaw(list, index) \
  ( & (((VContext*)((list)->items))[index]) )

void ContextList_Set(VList* list, VIndex index, VContext* item)
{
  ListCheckCapacity(list, index);
  // Increase list length if needed.
  if (index >= ListLength(list)) 
    ListLength(list) = index + 1;
  ContextList_SetRaw(list, index, item);
}

VContext* ContextList_Get(VList* list, VIndex index)
{
  if (index >= ListLength(list) || index < 0)
    GuruMeditation(LISTGET_INDEX_OUT_OF_BOUNDS);
  return ContextList_GetRaw(list, index);
}

// Push and Pop ------------------------------------------------

void ContextList_Push(VList* list, VContext* item)
{
  VIndex index = ListLength(list);
  ListCheckCapacity(list, index);
  ++ ListLength(list);
  ContextList_SetRaw(list, index, item);
}

VContext* ContextList_Pop(VList* list)
{
  if (ListLength(list) < 1)
    GuruMeditation(LISTPOP_CANNOT_POP_EMPTY_LIST);
  return ContextList_GetRaw(list, -- ListLength(list));
}
