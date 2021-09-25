/*
File: gc_marksweep.h
Author: Mikael Kindborg (mikael@kindborg.com)

Mark and sweep memory allocator and garbage collector.
Based on this article by Bob Nystrom:
https://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/
*/

typedef struct __GCEntry 
{
  VObj* obj;
  struct __GCEntry* next;
}
GCEntry;

typedef struct __VGarbageCollector
{
  GCEntry* firstEntry;
}
VGarbageCollector;

VGarbageCollector* GCCreate() 
{
  VGarbageCollector* gc = MemAlloc(sizeof(VGarbageCollector));
  gc->firstEntry = NULL;
  return gc;
}

// Add an object to the garbage collector. 
// The collector will now track and free this object.
void GCAddObj(VGarbageCollector* gc, VObj* obj)
{
  GCEntry* entry = MemAlloc(sizeof(GCEntry));
  entry->obj = obj;
  entry->next = gc->firstEntry;
  gc->firstEntry = entry;
}

// Add a list and its children to the garbage collector.
void GCAddObjDeep(VGarbageCollector* gc, VObj* obj)
{
  GCAddObj(gc, obj);

  if (!ObjIsList(obj)) return;

  for (int i = 0; i < ListLength((VList*)obj); ++i)
  {
    VItem* item = ItemList_GetRaw((VList*)obj, i);
    if (IsObj(item))
    {
      GCAddObjDeep(gc, ItemObj(item));
    }
  }
}

void GCMarkChildren(VList* list);

void GCMarkObj(VObj* obj)
{ 
  if (ObjIsMarked(obj)) return; // Already traversed.

  // Mark object as traversed.
  ObjSetMark(obj);
  
  // Traverse children.
  if (ObjIsList(obj))
  {
    GCMarkChildren((VList*) obj);
  }
}

// Mark children by traversing the list tree.
void GCMarkChildren(VList* list)
{ 
  for (int i = 0; i < ListLength(list); ++i)
  {
    VItem* item = ItemList_Get(list, i);
    if (IsObj(item))
    {
      GCMarkObj(ItemObj(item));
    }
  }
}

void GCSweep(VGarbageCollector* gc)
{ 
  PrintDebug("GCSweep BEGIN");
  GCEntry** entry = &(gc->firstEntry);
  while (*entry)
  {
    if (! ObjIsMarked((*entry)->obj))
    {
      PrintDebugStrNum("GCSweep DEALLOC: ", (unsigned long)((*entry)->obj));
      GCEntry* deallocated = *entry;
      *entry = (*entry)->next;
      ListFree((VList*)deallocated->obj);
      MemFree(deallocated);
    }
    else
    {
      PrintDebug("GCSweep NEXT");
      ObjClearMark((*entry)->obj);
      entry = &((*entry)->next);
    }
  }
  PrintDebug("GCSweep END");
}

void GCSweepUnmarkAll(VGarbageCollector* gc)
{
  GCEntry** entry = &(gc->firstEntry);
  while (*entry)
  {
    ObjClearMark((*entry)->obj);
    entry = &((*entry)->next);
  }
}

void GCPrintEntries(VGarbageCollector* gc)
{ 
  PrintDebug("GC Entries:");
  GCEntry** entry = &(gc->firstEntry);
  while (*entry)
  {
    PrintDebugStrNum("  GCEntry: ",(unsigned long)((*entry)->obj));
    entry = &((*entry)->next);
  }
}

// Free allocated objects.
void GCFree(VGarbageCollector* gc) 
{
  //GCSweepUnmarkAll(gc);
  GCSweep(gc);
  MemFree(gc);
}
