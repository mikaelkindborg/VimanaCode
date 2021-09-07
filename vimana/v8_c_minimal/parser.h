/*
File: parser.h
Author: Mikael Kindborg (mikael@kindborg.com)

Parser for symbolic code.

TODO: Add some form of syntax error checking.
*/

char* ParseNumber(char* p, VNumber* result)
{
  char* pstart = p;
  while (isdigit(*p)) ++ p;
  char restoreMe = *p;
  *p = 0;
  *result = strtol(pstart, NULL, 10);
  *p = restoreMe;
  return p;
}

// Returned string in pBuf must be deallocated.
char* ParseString(char* p, char** pBuf)
{
  *pBuf = MemAlloc(20);
  char* pCurrent = *pBuf;
  int size = 0;
  while ('\'' != *p) 
  {
    // TODO: Check string buffer overflow and realloc buffer
    if (size > 19) GuruMeditaton(PARSESTRING_BUFFER_OVERFLOW);
    *pCurrent = *p;
    ++ p;
    ++ pCurrent;
    ++ size;
  }
  *pCurrent = '\0';
  printf("%s\n", *pBuf);
  *pBuf = realloc(*pBuf, strlen(*pBuf) + 1);
  printf("%s\n", *pBuf);
  return p + 1;
}

VBool IsWhiteSpace(char c)
{
  return (' ' == c || '\t' == c || '\n' == c || '\r' == c);
}

// Parse a zero-terminated string with symbolic codes.
// p is the current position in the string.
// Returns the new current position. 
// Parse result is in codeList.
char* ParserWorker(char* p, VList* codeList)
{
  VItem*   item;
  VList*   childList;
  VNumber  number;
  char     c;
  char*    pBuf;
  
  while ('\0' != *p)
  {
    if (IsWhiteSpace(*p))
    {
      ++ p;
    }
    else
    if ('(' == *p)
    {
      childList = ListCreate(sizeof(VItem));
      p = ParserWorker(p + 1, childList);
      item = ListPushNewItem(codeList);
      ItemSetObj(item, childList);
    }
    else
    if (')' == *p)
    {
      return p + 1;
    }
    else
    if ('\'' == *p)
    {
      p = ParseString(p + 1, &pBuf);
      item = ListPushNewItem(codeList);
      ItemSetString(item, pBuf);
    }
    else
    {
      c = *p;
      p = ParseNumber(p + 1, &number);
      item = ListPushNewItem(codeList);
      if ('N' == c)
        ItemSetNumber(item, number);
      else
      if ('P' == c)
        ItemSetPrimFun(item, number);
      else
      if ('S' == c)
        ItemSetSymbol(item, number);
      else
        ItemSetVirgin(item);
    }
  }

  return p;
}

// Returned list contains the parsed code. 
// This list must be deallocated along with its children.
VList* ParseCode(char* p)
{
  char* buf = MemAlloc(strlen(p + 1));
  strcpy(buf, p);
  VList* codeList = ListCreate(sizeof(VItem));
  ParserWorker(buf, codeList);
  MemFree(buf);
  return codeList;
}
