/*
File: gc_refcount.h
Author: Mikael Kindborg (mikael@kindborg.com)

Minimalistic reference counting garbage collection.
*/

void ItemGC(VItem* item);

void ItemIncrRefCount(VItem* item)
{
  if (IsObj(item))
  {
    ObjIncrRefCount(ItemObj(item));
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

void ObjGC(VObj* obj)
{
  ObjDecrRefCount(obj);

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
  ObjGC(AsObj(list));
}

void ItemGC(VItem* item)
{
  if (IsObj(item)) ObjGC(ItemObj(item));
}
