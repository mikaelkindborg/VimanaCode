/*
File: gc_refcount.h
Author: Mikael Kindborg (mikael@kindborg.com)

Reference counting GC.
*/

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

//void PrintList(VList* list);

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
