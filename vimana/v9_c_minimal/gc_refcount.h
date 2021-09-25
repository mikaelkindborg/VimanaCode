/*
File: gc_refcount.h
Author: Mikael Kindborg (mikael@kindborg.com)

Reference counting GC.
*/

void ItemGC(VItem* item);

void ItemIncrRefCount(VItem* item)
{
  if (IsPtr(item))
  {
    ObjIncrRefCount(ItemObj(item));
    //PrintStrNum("INCR_REF_COUNT: ", ItemObj(item)->refCount);
  }
}

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
  ObjDecrRefCount(obj);

  //PrintStrNum("DECR_REF_COUNT: ", ObjGetRefCount(obj));

  if (ObjGetRefCount(obj) <= 0)
  {
    if (ObjIsString(obj))
    {
      StringFree((VString*) obj);
    }
    else
    if (ObjIsList(obj))
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
  if (IsPtr(item)) ObjGC(ItemObj(item));
}
