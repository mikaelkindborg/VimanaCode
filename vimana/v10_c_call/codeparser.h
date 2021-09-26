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
  //printf("Number in ParseNumber: %ld\n", *result);
  return pNext;
}

// Returned string in pBuf must be deallocated.
char* ParseString(char* p, VString** pString)
{
  VString* string = StringCreate();
  while (!IsStringSeparator(*p)) 
  {
    StringWriteChar(string, *p);
    ++ p;
  }
  *pString = string;
  // OLD CODE
  //*pBuf = StringGetStrCopy(string);
  //StringFree(string);
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
  VString* string;
  VNumber  number;
  char     c;
  
  while (!IsEndOfString(*p))
  {
    //PrintChar(*p);

    if (IsWhiteSpace(*p))
    {
      // Skip whitespace
      ++ p;
    }
    else
    if (IsLeftParen(*p))
    {
      // Create child list
      childList = ItemList_Create();
      p = CodeParserWorker(p + 1, childList);
      item = ListPushRaw(codeList);
      ItemSetList(item, childList);
    }
    else
    if (IsRightParen(*p))
    {
      // Close list
      return p + 1;
    }
    else
    if (IsStringSeparator(*p))
    {
      // Create string
      p = ParseString(p + 1, &string);
      item = ListPushRaw(codeList);
      ItemSetString(item, string);
    }
    else
    {
      // Create number/primfun/symbol
      c = *p;
      p = ParseNumber(p + 1, &number);
      item = ListPushRaw(codeList);
      if ('N' == c)
        ItemSetNumber(item, number);
      else
      if ('P' == c)
      {
        VFunPtr fun = GInterpPrimFunTable[number];
        ItemSetPrimFun(item, fun, number);
      }
      else
      if ('S' == c)
        ItemSetSymbol(item, number);
      else
        ItemSetVirgin(item);
    }
  }

  // Reached end of string, 
  // return pointer to string end.
  return p;
}

// Returned list contains the parsed code. 
// This list must be deallocated along with its children.
VList* ParseSymbolicCode(char* p)
{
  VList* codeList = ItemList_Create();
  CodeParserWorker(p, codeList);
  return codeList;
}
