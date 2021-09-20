
// Template Functions ------------------------------------------

// Create ------------------------------------------------------

VList* String_Create()
{
  return ListCreate_Internal(sizeof(char));
}

void String_Init(VList* list)
{
  ListInit_Internal(list, sizeof(char));
}

// Set and Get -------------------------------------------------

#define String_SetRaw(list, index, item) \
  ( ((char*)((list)->items))[index] = *(item) )

#define String_GetRaw(list, index) \
  ( & (((char*)((list)->items))[index]) )

void String_Set(VList* list, VIndex index, char* item)
{
  ListCheckCapacity(list, index);
  // Increase list length if needed.
  if (index >= ListLength(list)) 
    ListLength(list) = index + 1;
  String_SetRaw(list, index, item);
}

char* String_Get(VList* list, VIndex index)
{
  if (index >= ListLength(list) || index < 0)
    GuruMeditation(LISTGET_INDEX_OUT_OF_BOUNDS);
  return String_GetRaw(list, index);
}

// Push and Pop ------------------------------------------------

void String_Push(VList* list, char* item)
{
  VIndex index = ListLength(list);
  ListCheckCapacity(list, index);
  ++ ListLength(list);
  String_SetRaw(list, index, item);
}

char* String_Pop(VList* list)
{
  if (ListLength(list) < 1)
    GuruMeditation(LISTPOP_CANNOT_POP_EMPTY_LIST);
  return String_GetRaw(list, -- ListLength(list));
}
