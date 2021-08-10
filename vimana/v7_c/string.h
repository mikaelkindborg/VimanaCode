/*
File: string.h
Author: Mikael Kindborg (mikael@kindborg.com)

String object with GC support.
*/

typedef struct _VString
{
  VObjHeader header;
  char       str[1];
}
VString;

VString* StringCreate(char* str)
{
#ifdef TRACK_MEMORY_USAGE
  ++ GStringCreateCounter;
#endif
  VString* string = MemAlloc(sizeof(VString) + strlen(str));
  string->header.type = TypeString;
  string->header.gcMarker = 0;
  strcpy(string->str, str);
  return string;
}

void StringFree(VString* string)
{
#ifdef TRACK_MEMORY_USAGE
  ++ GStringFreeCounter;
#endif
  MemFree(string);
}

#define StringStr(string) ((string)->str)
