/*
File: codeparser.h
Author: Mikael Kindborg (mikael@kindborg.com)

Parser for symbolic code.

TODO: Add some form of syntax error checking.
*/

VBool IsWhiteSpace(char c)
{
  return (' ' == c || '\t' == c || '\n' == c || '\r' == c);
}

VBool IsQuote(char c)
{
  return ('\'' == c);
}

VBool IsLeftParen(char c)
{
  return ('(' == c);
}

VBool IsRightParen(char c)
{
  return (')' == c);
}

VBool IsParen(char c)
{
  return (IsLeftParen(c) || IsRightParen(c));
}

VBool IsEndOfString(char c)
{
  return '\0' == c;
}

VBool IsWhiteSpaceOrSeparatorOfEndOfString(char c)
{
  return IsWhiteSpace(c) || IsParen(c) || IsQuote(c) || IsEndOfString(c);
}

char* ParseNumber(char* p, VNumber* result)
{
  char* pstart = p;
  if ('-' == *p) ++ p;
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
  int bufSize = 20;
  *pBuf = MemAlloc(bufSize);
  char* pCurrent = *pBuf;
  int size = 0;
  while (!IsQuote(*p)) 
  {
    // TODO: Check string buffer overflow and realloc buffer
    if (size > bufSize - 1) GuruMeditaton(PARSESTRING_BUFFER_OVERFLOW);
    *pCurrent = *p;
    ++ p;
    ++ pCurrent;
    ++ size;
  }
  *pCurrent = '\0';
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
    if (IsQuote(*p))
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
  char* buf = MemAlloc(strlen(p + 1));
  strcpy(buf, p);
  VList* codeList = ListCreate(sizeof(VItem));
  CodeParserWorker(buf, codeList);
  MemFree(buf);
  return codeList;
}
