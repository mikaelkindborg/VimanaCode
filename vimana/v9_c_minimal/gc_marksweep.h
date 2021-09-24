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

VObj* GCAllocObj(VGarbageCollector* gc, size_t size)
{
  GCEntry* entry = MemAlloc(sizeof(GCEntry));
  VObj* obj = MemAlloc(size);
  obj->type = 0;
  entry->obj = obj;
  entry->next = gc->firstEntry;
  gc->firstEntry = entry;
  return obj;
}

void GCMarkChildren(VList* list);

void GCMarkObj(VObj* obj)
{ 
  if (ObjIsMarked(obj)) return; // Already traversed.

  // Mark object as traversed.
  VObjSetMark(obj);
  
  // Traverse children.
  if (ObjHasChildren(obj))
  {
    GCMarkChildren((VList*) obj);
  }
}

// Mark children by traversing the list tree.
void GCMarkChildren(VList* list)
{ 
  for (int i = 0; i < ListLength(list); ++i)
  {
    VItem* item = ListGet(list, i);
    if (IsObj(item))
    {
      GCMarkObj(ItemObj(item));
    }
  }
}

void GCSweep(GarbageCollector* gc)
{ 
  PrintDebug("GCSweep BEGIN");
  GCEntry** entry = &(gc->firstEntry);
  while (*entry)
  {
    PrintDebug("GCSweep ENTRY %i", i++);
    if (! ObjIsMarked((*entry)->obj))
    {
      PrintDebug("GCSweep DEALLOC: %lu",(unsigned long)((*entry)->obj));
      GCEntry* deallocated = *entry;
      *entry = (*entry)->next;
      ListFree((List*)deallocated->obj);
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

void GCSweepUnmarkAll(GarbageCollector* gc)
{
  GCEntry** entry = &(gc->firstEntry);
  while (*entry)
  {
    ObjClearMark((*entry)->obj);
    entry = &((*entry)->next);
  }
}

void GCPrintEntries(GarbageCollector* gc)
{ 
  PrintLine("GC Entries:");
  GCEntry** entry = &(gc->firstEntry);
  while (*entry)
  {
    PrintLine("  GCEntry: %lu",(unsigned long)((*entry)->obj));
    entry = &((*entry)->next);
  }
}

// Free allocated objects.
void GCFree(GarbageCollector* gc) 
{
  //GCSweepUnmarkAll(gc);
  GCSweep(gc);
  MemFree(gc);
}



void GCMarkList(List* list)
{ 
  if (1 == list->header.gcMarker) return; // Already traversed.

  // Mark list as traversed.
  list->header.gcMarker = 1;
  
  // Traverse list.
  GCMarkChildren(list, 0);
}

// Mark children by traversing the list tree.
void GCMarkChildren(List* list, Index startIndex)
{ 
  PrintDebug("GCMarkChildren: %d %lu", ListLength(list), (unsigned long)list);

  for (int i = startIndex; i < list->length; ++i)
  {
    Item item = ListGet(list, i);
    if (IsDynAlloc(item))
    {
      GCMarkList(ItemList(item));
    }
    else
    if (IsString(item))
    {
      item.value.string->header.gcMarker = 1;
    }
  }
}

void GCSweep(GarbageCollector* gc)
{ 
  PrintDebug("GCSweep BEGIN");
  int i = 1;
  GCEntry** entry = &(gc->firstEntry);
  while (*entry)
  {
    PrintDebug("GCSweep ENTRY %i", i++);
    if (0 == (*entry)->object->header.gcMarker)
    {
      PrintDebug("GCSweep DEALLOC: %lu",(unsigned long)((*entry)->object));
      GCEntry* unreachableEntry = *entry;
      *entry = (*entry)->next;
      if (TypeList & unreachableEntry->object->header.type)
        ListFree((List*)unreachableEntry->object);
      else
      if (TypeString & unreachableEntry->object->header.type)
        StringFree((VString*)unreachableEntry->object);
      MemFree(unreachableEntry);
    }
    else
    {
      PrintDebug("GCSweep NEXT");
      (*entry)->object->header.gcMarker = 0;
      entry = &((*entry)->next);
    }
  }
  PrintDebug("GCSweep END");
}

void GCSweepUnmarkAll(GarbageCollector* gc)
{ 
  int i = 1;
  GCEntry** entry = &(gc->firstEntry);
  while (*entry)
  {
    (*entry)->object->header.gcMarker = 0;
    entry = &((*entry)->next);
  }
}

void GCPrintEntries(GarbageCollector* gc)
{ 
  PrintLine("GC Entries:");
  GCEntry** entry = &(gc->firstEntry);
  while (*entry)
  {
    PrintLine("  GCEntry: %lu",(unsigned long)((*entry)->object));
    entry = &((*entry)->next);
  }
}

// Free allocated objects.
void GCFree(GarbageCollector* gc) 
{
  //GCSweepUnmarkAll(gc);
  GCSweep(gc);
  MemFree(gc);
}
