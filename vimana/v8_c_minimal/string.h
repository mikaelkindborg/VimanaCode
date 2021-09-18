/*
File: string.h
Author: Mikael Kindborg (mikael@kindborg.com)

Using Vlist to implement growable strings.
*/

typedef struct __VList VString;

VString* StringCreate()
{
  VString* string = ListCreate(sizeof(char));
  string->type = TypeString;
  return string;
}

void StringFree(VString* string)
{
  return ListFree(string);
}

void StringWriteChar(VString* string, char c)
{
  char endOfString = '\0';
  ListPush(string, &c);
  ListPush(string, &endOfString);
  -- ListLength(string);
}

void StringWriteStr(VString* string, char* str)
{
  char endOfString = '\0';
  char* p = str;
  while (*p)
  {
    ListPush(string, p);
    ++ p;
  }
  ListPush(string, &endOfString);
  -- ListLength(string);
}

char* StringGetStr(VString* string)
{
  return (char*)(string->items);
}

// You must dealloc returned buffer with MemFree().
char* StringGetStrCopy(VString* string)
{
  char* buf = MemAlloc(ListLength(string) + 1);
  strcpy(buf, StringGetStr(string));
  return buf;
}

// Lookup a string and return the index if it is found.
// Returns -1 if not found.
// Assumes use of VItem to represent strings.
// All items in the list must be strings.
VIndex ListLookupString(VList* list, char* strToFind)
{
  for (int index = 0; index < ListLength(list); ++ index)
  {
    VItem* item = ListGet(list, index);
    char* str = StringGetStr(ItemObj(item));
    if (StrEquals(strToFind, str))
      return index; // Found it.
  }
  return -1; // Not found.
}

// Push string item to list.
// Copies the string.
// Returns index to the added item.
VIndex ListAddString(VList* list, char* str)
{
  VString* string = StringCreate();
  StringWriteStr(string, str);
  VItem* item = ListPushNewItem(list);
  ItemSetObj(item, string);
  return ListLength(list) - 1;
}
