/*
File: parser.h
Author: Mikael Kindborg (mikael@kindborg.com)
*/

#define IsWhiteSpace(c) \
  (' ' == (c) || '\t' == (c) || '\n' == (c) || '\r' == (c))
#define IsLeftParen(c) ('(' == (c))
#define IsRightParen(c) (')' == (c))
#define IsParen(c) (IsLeftParen(c) || IsRightParen(c))
#define IsStringSeparator(c) ('\'' == (c))
#define IsEndOfString(c) ('\0' == (c))
#define IsWhiteSpaceOrSeparatorOrEndOfString(c) \
  (IsWhiteSpace(c) || IsParen(c) || IsStringSeparator(c) || IsEndOfString(c))

char GTokenBuffer[512];

char* ParseString(char* p, char** next)
{
  int   length = 0;
  int   bufSize = 1024;
  char* buf = SysAlloc(bufSize);
  char* pBuf = buf;

  while (! (IsStringSeparator(*p) || IsEndOfString(*p)) )
  {
    *pBuf = *p;
    ++ pBuf;
    ++ p;
    ++ length;
  }

  *next = p + 1;

  *pBuf = 0;
  buf = realloc(buf, length + 1);

  return buf;
}

char* GetNextToken(char* p, char** next)
{
  char* pbuf = GTokenBuffer;

  while (!IsWhiteSpaceOrSeparatorOrEndOfString(*p))
  {
    *pbuf = *p;
    ++ pbuf;
    ++ p;
  }

  *pbuf = 0;
  *next = p;

  return GTokenBuffer;
}

VType TokenType(char* token)
{
  char* p = token;
  int   dec = 0;

  VType type = TypeSymbol;

  // Single minus sign is not a number
  if ( ('-' == *p) && (1 == strlen(token)) ) goto Exit;

  while (!IsEndOfString(*p))
  {
    if (!isdigit(*p))
    {
      if ('.' == *p)
        ++ dec;
      else
        goto Exit;
    }
    ++ p;
  }

  if (0 == dec)
    type = TypeIntNum;
  else
  if (1 == dec)
    type = TypeDecNum;

Exit:
  return type;
}

VItem* ParseToken(char* token, VMem* mem)
{
  VItem* item = MemAllocItem(mem);

  VType type = TokenType(token);

  if (TypeIntNum == type)
  {
    int intNum = strtol(token, NULL, 10);
    ItemSetIntNum(item, intNum);
  }
  else
  if (TypeDecNum == type)
  {
    double decNum = strtod(token, NULL);
    ItemSetDecNum(item, decNum);
  }
  else
  if (TypeSymbol == type)
  {
    int primFunId = LookupPrimFun(token);
    if (primFunId > -1)
    {
      #ifdef OPTIMIZE
        VPrimFunPtr fun = LookupPrimFunPtr(primFunId);
        ItemSetPrimFun(item, fun);
      #else
        ItemSetPrimFun(item, primFunId);
      #endif
    }
    else
    {
      int symbol = GSymbolTableFindAdd(token);
      ItemSetSymbol(item, symbol);
    }
  }
  else
  {
    GURU(PARSER_TOKEN_TYPE_ERROR);
  }

  return item;
}

// A comment begins and ends with three hyphens: /-- comment --/

#define IsComment(p)    (('/' == *(p)) && ('-' == *((p)+1)) && ('-' == *((p)+2)))
#define IsCommentEnd(p) (('-' == *(p)) && ('-' == *((p)+1)) && ('/' == *((p)+2)))

char* SkipComment(char* p)
{
  if (IsComment(p))
  {
    p = p + 3;

    while (0 != *p)
    {
      if (IsCommentEnd(p)) 
        return p + 3; // End of comment
      
      ++p;
    }

    return NULL; // End of string
  }
  else
    return p; // Not a comment
}

VItem* ParseCode(char* code, char** next, VMem* mem)
{
  VItem* first = NULL;
  VItem* item  = NULL;
  VItem* prev;

  VItem* list = MemAllocItem(mem);
  ItemSetType(list, TypeList);
  
  char* p = code;

  while (!IsEndOfString(*p))
  {
    if (IsWhiteSpace(*p))
    {
      // Skip whitespace
      ++ p;
    }
    else
    if (IsLeftParen(*p))
    {
      // Parse child list
      item = ParseCode(p + 1, &p, mem);
    }
    else
    if (IsRightParen(*p))
    {
      // Done parsing list
      ++ p;
      *next = p;
      goto Exit;
    }
    else
    if (IsStringSeparator(*p))
    {
      char* string = ParseString(p + 1, &p);
      item = MemAllocItem(mem);
      MemItemSetString(mem, item, string);
      SysFree(string);
    }
    else
    {
      p = SkipComment(p);
      if (NULL == p) goto Exit;

      char* token = GetNextToken(p, &p);
      item = ParseToken(token, mem);
    }

    // Add item to list
    if (NULL != item)
    {
      if (NULL == first)
        first = item;
      else
        MemItemSetNext(mem, prev, item);

      prev = item;
      item = NULL;
    }
  }

Exit:
  MemItemSetFirst(mem, list, first);
  return list;
}

VItem* ParseSourceCode(char* sourceCode, VMem* mem)
{
  char* p;
  return ParseCode(sourceCode, &p, mem);
}
