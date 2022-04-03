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

char* ParseIntNum(char* p, VIntNum* result)
{
  char* pNext;
  *result = strtol(p, &pNext, 10);
  printf("Number in ParseNumber: %ld\n", *result);
  return pNext;
}

char* ParseDecNum(char* p, VIntNum* result)
{
  char* pNext;
  *result = strtol(p, &pNext, 10);
  printf("Number in ParseNumber: %ld\n", *result);
  return pNext;
}

/*
// Returned string in pBuf must be deallocated.
char* ParseString(char* p, VString** pString)
{
  // TODO: Count strlen then alloc buf and copy
  VString* string = StringCreate();
  while (!IsStringSeparator(*p)) 
  {
    StringWriteChar(string, *p);
    ++ p;
  }
  *pString = string;
  // OLD CODE
  // *pBuf = StringGetStrCopy(string);
  // StringFree(string);
  return p + 1;
}
*/

// Parse a zero-terminated string with symbolic codes.
// p is the current position in the string.
// Returns the new current position. 
// Parsed list is returned in first.
char* __ParseSymbolicCode(char* p, VItem** first, VMem* mem)
{
  VItem*   item;
  VItem*   child;
  VItem*   prev;
  //VString* string;
  VIntNum  number;
  char     c;

  *first = NULL;

  while (!IsEndOfString(*p))
  {
    //PrintChar(*p);

    if (IsRightParen(*p))
    {
      // Done parsing list
      return p + 1;
    }
    else
    if (IsWhiteSpace(*p))
    {
      // Skip whitespace
      ++ p;
    }
    else
    {
      item = MemAllocItem(mem);

      if (NULL == *first) 
        *first = item;
      else
        MemCons(mem, prev, item);

      if (IsLeftParen(*p))
      {
        // Parse child list
        p = __ParseSymbolicCode(p + 1, &child, mem);
        ItemSetList(item, MemItemAddr(mem, child));
      }
      else
      /*if (IsStringSeparator(*p))
      {
        p = ParseString(p + 1, &string);
        ItemSetString(item, string);
      }
      else*/
      {
        // Create number/primfun/symbol
        c = *p;
        if ('N' == c)
        {
          p = ParseIntNum(p + 1, &number);
          ItemSetIntNum(item, number);
        }
        //TODO: Implement decimal numbers
        //else
        //if ('D' == c)
        //{
        //  p = ParseDecNum(p + 1, &number);
        //  ItemSetDecNum(item, number);
        //}
        else
        if ('P' == c)
        {
          p = ParseIntNum(p + 1, &number);
          ItemSetPrimFun(item, number);
        }
        else
        if ('S' == c)
        {
          p = ParseIntNum(p + 1, &number);
          ItemSetSymbol(item, number);
        }
        else
          GuruMeditation(CODE_PARSER_UNDEFINED_TYPE);
      }

      prev = item;
    }
  }

  // Reached end of string
  return p;
}

// Returns list. 
VItem* ParseSymbolicCode(char* p, VMem* mem)
{
  VItem* list;
  __ParseSymbolicCode(p, &list, mem);
  return list;
}
