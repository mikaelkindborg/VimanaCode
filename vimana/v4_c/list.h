
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
  Type type;
  union
  {
    Index  symbolIndex;
    double number;
    List*  list;
    void*  obj;
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
      printf("ERROR: OUT OF MEMORY IN ListPush\n");
      exit(0);
    }
    list->items = newArray;
    printf("REALLOC SUCCESSFUL IN ListPush\n");
  }
  
  list->items[list->length] = item;
  list->length++;
  return list->length - 1; // Index of new item.
}

Item ListPop(List* list)
{
  list->length--;
  return list->items[list->length];
}

Item ListGet(List* list, int index)
{
  return list->items[index];
}

void ListPrint(List* list, Interp* interp)
{
  printf("(");
  
  for (int i = 0; i < list->length; i++)
  {
    Item item = ListGet(list, i);
    if (IsSymbol(item.type))
    {
      printf("%s", InterpGetSymbol(interp, item.data.symbolIndex));
    }
    else if (IsNumber(item.type))
    {
      printf("%f", item.data.number);
    }
    else if (IsList(item.type))
    {
      ListPrint(item.data.list, interp);
    }
    
    if (i < list->length - 1)
    {
      printf(" ");
    }
  }
  printf(")");
}

void ListPrintItems(List* list, Interp* interp)
{
  for (int i = 0; i < list->length; i++)
  {
    Item item = ListGet(list, i);
    if (IsSymbol(item.type))
    {
      printf("%s", InterpGetSymbol(interp, item.data.symbolIndex));
      printf("\n");
    }
    else if (IsNumber(item.type))
    {
      printf("%f", item.data.number);
      printf("\n");
    }
    else if (IsList(item.type))
    {
      ListPrint(item.data.list, interp);
      printf("\n");
    }
  }
}

Item ItemWithSymbol(Index symbolIndex)
{
  Item item;
  item.type = TypeSymbol;
  item.data.symbolIndex = symbolIndex;
  return item;
}

Item ItemWithNumber(double number)
{
  Item item;
  item.type = TypeNumber;
  item.data.number = number;
  return item;
}

Item ItemWithList(List* list)
{
  Item item;
  item.type = TypeList;
  item.data.list = list;
  return item;
}

Item ItemWithObj(void* obj)
{
  Item item;
  item.type = TypeObj;
  item.data.obj = obj;
  return item;
}
