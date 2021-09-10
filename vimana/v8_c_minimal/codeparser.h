/*
File: codeparser.h
Author: Mikael Kindborg (mikael@kindborg.com)

Parser for symbolic code.

TODO: Add some form of syntax error checking.
*/

#define EndOfString ('\0')
#define StringSeparator ('\'')
#define IsWhiteSpace(c) \
  (' ' == (c) || '\t' == (c) || '\n' == (c) || '\r' == (c))
#define IsStringSeparator(c) (StringSeparator == (c))
#define IsLeftParen(c) ('(' == (c))
#define IsRightParen(c) (')' == (c))
#define IsEndOfString(c) (EndOfString == (c))

char* ParseNumber(char* p, VNumber* result)
{
  // First char may be a minus sign.
  if ('-' == *p) ++ p;
  char* pNext;
  *result = strtol(p, &pNext, 10);
  return pNext;
}

// Returned string in pBuf must be deallocated.
char* ParseString(char* p, char** pBuf)
{
  int bufSize = 20;
  *pBuf = MemAlloc(bufSize);
  char* pCurrent = *pBuf;
  int size = 0;
  while (!IsStringSeparator(*p)) 
  {
    // TODO: Check string buffer overflow and realloc buffer
    if (size > bufSize - 1) GuruMeditaton(PARSESTRING_BUFFER_OVERFLOW);
    *pCurrent = *p;
    ++ p;
    ++ pCurrent;
    ++ size;
  }
  *pCurrent = EndOfString;
  //printf("%s\n", *pBuf);
  *pBuf = realloc(*pBuf, strlen(*pBuf) + 1);
  //printf("%s\n", *pBuf);
  return p + 1;
}

// Parse a zero-terminated string with symbolic codes.
// p is the current position in the string.
// Returns the new current position. 
// Parse result is in codeList.
char* CodeParserWorker(char* p, VList* codeList)
{
  VItem*   item;
  VList*   childList;
  VNumber  number;
  char     c;
  char*    pBuf;
  
  while (!IsEndOfString(*p))
  {
    if (IsWhiteSpace(*p))
    {
      ++ p;
    }
    else
    if (IsLeftParen(*p))
    {
      childList = ListCreate(sizeof(VItem));
      p = CodeParserWorker(p + 1, childList);
      item = ListPushNewItem(codeList);
      ItemSetObj(item, childList);
    }
    else
    if (IsRightParen(*p))
    {
      return p + 1;
    }
    else
    if (IsStringSeparator(*p))
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
VList* ParseSymbolicCode(char* p)
{
  VList* codeList = ListCreate(sizeof(VItem));
  CodeParserWorker(p, codeList);
  return codeList;
}
