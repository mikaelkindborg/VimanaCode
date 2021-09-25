/*
File: string.h
Author: Mikael Kindborg (mikael@kindborg.com)

Using Vlist to implement growable strings.
*/

typedef struct __VList VString;

#include "string_gen.h"

VString* StringCreate()
{
  VString* string = String_Create();
  ObjSetType(string, ObjTypeString);
  return string;
}

void StringFree(VString* string)
{
  return ListFree(string);
}

void StringWriteChar(VString* string, char c)
{
  char endOfString = '\0';
  String_Push(string, &c);
  String_Push(string, &endOfString);
  -- ListLength(string);
}

void StringWriteStr(VString* string, char* str)
{
  char endOfString = '\0';
  char* p = str;
  while (*p)
  {
    String_Push(string, p);
    ++ p;
  }
  String_Push(string, &endOfString);
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
