/*
File: parser2.h
Author: Mikael Kindborg (mikael@kindborg.com)

Parser for source code.
*/

VBool IsWhiteSpace(char c)
{
  return (' ' == c || '\t' == c || '\n' == c || '\r' == c);
}

VBool IsSeparator(char c)
{
  return ('(' == c || '\'' == c);
}

VBool IsWhiteSpaceOrSeparator(char c)
{
  return IsWhiteSpace(c) || IsSeparator(c);
}

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
  while (!IsSeparator(*p)) 
  {
    // TODO: Check string buffer overflow and realloc buffer
    if (size > 19) ErrorExit("ParseString: Buffer overflow");
    *pCurrent = *p;
    ++ p;
    ++ pCurrent;
    ++ size;
  }
  *pCurrent = '\0';
  *pBuf = realloc(*pBuf, strlen(*pBuf) + 1);
  return p + 1;
}

// Returned token in pBuf must be deallocated.
char* ParseToken(char* p, char** pBuf)
{
  *pBuf = MemAlloc(20);
  char* pCurrent = *pBuf;
  int size = 0;
  while (!IsWhiteSpaceOrSeparator(*p)) 
  {
    // TODO: Check buffer overflow and realloc buffer
    if (size > 19) ErrorExit("ParseToken: Buffer overflow");
    *pCurrent = *p;
    ++ p;
    ++ pCurrent;
    ++ size;
  }
  *pCurrent = '\0';
  *pBuf = realloc(*pBuf, strlen(*pBuf) + 1);
  return p + 1;
}

// Parse a zero-terminated string with symbolic codes.
// p is the current position in the string.
// Returns the new current position. 
// Parse result is in codeList.
char* ParserWorker(char* p, VList* codeList)
{
  VItem    item;
  VNumber  number;
  VList*   childList;
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
      item = ItemWithObj(childList);
      ListPush(codeList, &item);
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
      item = ItemWithString(pBuf);
      ListPush(codeList, &item);
    }
    else
    {
      p = ParseToken(p + 1, &pBuf);
      if (TokenIsNumber(pBuf))
      {
        number = GetNumber(pBuf);
        item = ItemWithNumber(number);
      }
      else
      if (TokenIsPrimFun(pBuf))
      {
        number = GetPrimFunNumber(pBuf);
        item = ItemWithPrimFun(number);
      }
      else
      {
        number = GetSymbolNumber(pBuf);
        item = ItemWithSymbol(number);
      }
      MemFree(pBuf);
      ListPush(codeList, &item);
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
