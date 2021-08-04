// GARBAGE COLLECTOR -------------------------------------------

// Code below based on this article by Bob Nystrom:
// https://journal.stuffwithstuff.com/2013/12/08/babys-first-garbage-collector/

// DECLARATIONS ------------------------------------------------

void GCMarkChildren(List* list, Index startIndex);

// Incremented marker used to track traversed lists during GC.
// For each GC this global marker is incremented, which 
// allows us to not have to reset the marker list field.
unsigned long GCMarker = 0;

// Call before mark.
#define GCPrepare() (++ GCMarker)

typedef struct MyGCEntry 
{
  struct MyGCEntry* next;
  List* object;
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

void GCPushObject(GarbageCollector* gc, List* object)
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
  GCPushObject(gc, list);
  return list;
}

void GCMarkList(List* list)
{ 
  if (list->gcMarker == GCMarker) return; // Already traversed.

  // Mark list as traversed.
  list->gcMarker = GCMarker;
  
  // Traverse list.
  GCMarkChildren(list, 0);

  // Traverse environment of closure list.
  if (NULL != list->env)
  {
    GCMarkChildren(list->env, 0);
  }
}

// GC children by traversing the list tree.
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
  }
}

void GCSweep(GarbageCollector* gc)
{ 
  PrintLine("GCSweep BEGIN");
  int i = 1;
  GCEntry** entry = &(gc->firstEntry);
  while (*entry)
  {
    PrintLine("GCSweep ENTRY %i", i++);
    if ((*entry)->object->gcMarker != GCMarker)
    {
      PrintLine("GCSweep DEALLOC: %lu",(unsigned long)((*entry)->object));
      GCEntry* unreachableEntry = *entry;
      *entry = (*entry)->next;
      ListFree(unreachableEntry->object);
      MemFree(unreachableEntry);
    }
    else
    {
      PrintLine("GCSweep NEXT");
      entry = &((*entry)->next);
    }
  }
  PrintLine("GCSweep END");
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
  GCPrepare();
  GCSweep(gc);
  MemFree(gc);
}

/*

UNUSED!

// DYNAMIC ARRAY -----------------------------------------------

#define DynArraySize 10
#define DynArrayType int

typedef struct MyDynArray
{
  int size;
  int maxSize;
  DynArrayType* values; // Array of values
}
DynArray;

DynArray* DynArrayCreate()
{
  // Allocate array object.
  DynArray* array = malloc(sizeof(DynArray));

  // Allocate array entries.
  size_t arraySize = DynArraySize * sizeof(DynArrayType);
  array->values = = malloc(arraySize);

  // Init entries.
  memset(array->values, 0, arraySize);

  // Return array object.
  return array;
}

DynArray* DynArrayFree(DynArray* array)
{
  free(array->values);
  free(array);
}

*/
