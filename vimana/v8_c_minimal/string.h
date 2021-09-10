/*
File: string.h
Author: Mikael Kindborg (mikael@kindborg.com)

Using Vlist for growable strings.
*/

typedef struct __VList VString;

VString* StringCreate()
{
  return ListCreate(sizeof(char));
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

// You must dealloc returned buffer.
char* StringGetStrCopy(VString* string)
{
  char* buf = MemAlloc(ListLength(string) + 1);
  strcpy(buf, StringGetStr(string));
  return buf;
}
