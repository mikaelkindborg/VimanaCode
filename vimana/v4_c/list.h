
/****************** LISTS ******************/

typedef struct MyList
{
  int   length;     // Current number of items
  int   maxLength;  // Max number of items
  Item* items;      // Array of items
}
List;

typedef struct MyItem
{
  Type  type;
  union
  {
    Index index;  // Index in the symbol table or the local environment
    char* string; // String in symbol table entries
  }
  symbol;
  union
  {
    double  decNum;
    long    intNum;
    List*   list;
    void*   obj;
    PrimFun primFun;
  }
  data;
}
Item;

List* ListCreate()
{
  int size = 10;
  List* list = malloc(sizeof(List));
  list->length = 0;
  list->maxLength = size;
  Item* itemArray = malloc(size * sizeof(Item));
  list->items = itemArray;
  return list;
}

void ListFree(List* list, int whatToFree)
{
  free(list->items);
  free(list);
}

// Returns the index of the new item.
Index ListPush(List* list, Item item)
{
  // Grow list array if needed.
  if (list->length + 1 > list->maxLength)
  {
    size_t newSize = list->length + 10;
    Item* newArray = realloc(list->items, newSize * sizeof(Item));
    if (NULL == newArray)
    {
      printf("ERROR: Out of memory in ListPush\n");
      exit(0);
    }
    list->items = newArray;
    printf("REALLOC successful in ListPush\n");
  }
  
  list->items[list->length] = item;
  list->length++;
  return list->length - 1; // Index of new item.
}

Item ListPop(List* list)
{
  if (list->length < 1)
  {
    printf("ERROR: ListPop cannot pop list of length: %i\n", list->length);
    exit(0);
  }
  list->length--;
  return list->items[list->length];
}

Item ListGet(List* list, int index)
{
  if (index >= list->length)
  {
    printf("ERROR: ListGet out of bounds at index: %i\n", list->length);
    exit(0);
  }
  return list->items[index];
}

void ListPrintWorker(List* list, Bool useNewLine, Interp* interp);

void ListPrint(List* list, Interp* interp)
{
  printf("(");
  ListPrintWorker(list, FALSE, interp);
  printf(")");
}

void ListPrintItems(List* list, Interp* interp)
{
  ListPrintWorker(list, TRUE, interp);
}

// TODO: Make function to get Item as string
// Possibly custom string type that can grow.
  
void ListPrintWorker(List* list, Bool useNewLine, Interp* interp)
{
  for (int i = 0; i < list->length; i++)
  {
    Item item = ListGet(list, i);
    if (IsIntNum(item.type))
    {
      printf("%li", item.data.intNum);
    }
    else if (IsDecNum(item.type))
    {
      printf("%f", item.data.decNum);
    }
    else if (IsList(item.type))
    {
      ListPrint(item.data.list, interp);
    }
    else if (IsPrimFun(item.type))
    {
      //printf("[PRIMFUN: %s TYPE: %u]", item.symbol.string, item.type);
      printf("[PRIMFUN: %s]", item.symbol.string);
    }
    else if (IsFun(item.type))
    {
      ListPrint(item.data.list, interp);
    }
    else if (IsString(item.type))
    {
      printf("%s", item.symbol.string);
    }
    else if (IsSymbol(item.type))
    {
      printf("%s", InterpGetSymbolString(interp, item.symbol.index));
    }
    
    if (i < list->length - 1)
    {
      printf(" ");
    }
    
    if (useNewLine)
    {
      printf("\n");
    }
  }
}

Item ItemWithSymbol(Index symbolIndex)
{
  Item item;
  item.type = TypeSymbol;
  item.symbol.index = symbolIndex;
  return item;
}

Item ItemWithString(char* string)
{
  Item item;
  item.type = TypeString;
  char* stringbuf = malloc(strlen(string) + 1);
  strcpy(stringbuf, string);
  item.symbol.string = stringbuf;
  printf("[ItemWithString: %s]\n", item.symbol.string);
  return item;
}

Item ItemWithIntNum(long number)
{
  Item item;
  item.type = TypeIntNum;
  item.data.intNum = number;
  return item;
}

Item ItemWithDecNum(long number)
{
  Item item;
  item.type = TypeDecNum;
  item.data.decNum = number;
  return item;
}

Item ItemWithList(List* list)
{
  Item item;
  item.type = TypeList;
  item.data.list = list;
  return item;
}

Item ItemWithFun(List* fun)
{
  Item item;
  item.type = TypeFun;
  item.data.list = fun;
  return item;
}

Item ItemWithPrimFun(PrimFun fun)
{
  Item item;
  item.type = TypePrimFun;
  item.data.primFun = fun;
  return item;
}

Item ItemWithObj(void* obj)
{
  Item item;
  item.type = TypeObj;
  item.data.obj = obj;
  return item;
}

Item ItemWithVirgin()
{
  Item item;
  item.type = TypeVirgin;
  return item;
}

Item ItemWithStackFrame(void* obj)
{
  Item item;
  item.type = TypeStackFrame;
  item.data.obj = obj;
  return item;
}
