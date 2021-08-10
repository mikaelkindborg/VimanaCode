// GARBAGE COLLECTOR -------------------------------------------

// Code below based on this article by Bob Nystrom:
// https://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/

// DECLARATIONS ------------------------------------------------

void GCMarkChildren(List* list, Index startIndex);

typedef struct _GCEntry 
{
  VObj* object;
  struct _GCEntry* next;
}
GCEntry;

typedef struct MyGarbageCollector
{
  GCEntry* firstEntry;
}
GarbageCollector;

// FUNCTIONS ---------------------------------------------------

GarbageCollector* GCCreate() 
{
  GarbageCollector* gc = MemAlloc(sizeof(GarbageCollector));
  gc->firstEntry = NULL;
  return gc;
}

void GCPushObject(GarbageCollector* gc, VObj* object)
{
  PrintDebug("GCPushObject: %lu", (unsigned long)object);
  GCEntry* entry = MemAlloc(sizeof(GCEntry));
  entry->object = object;
  entry->next = gc->firstEntry;
  gc->firstEntry = entry;
}

List* GCListCreate(GarbageCollector* gc)
{
  List* list = ListCreate();
  GCPushObject(gc, (VObj*)list);
  return list;
}

/* TODO
String* GCStringCreate(GarbageCollector* gc)
{
  String* string = StringCreate();
  GCPushObject(gc, (VObj*)string);
  return string;
}
*/

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
