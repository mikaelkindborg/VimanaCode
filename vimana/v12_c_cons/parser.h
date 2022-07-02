/*
File: parser.h
Author: Mikael Kindborg (mikael@kindborg.com)
*/

#define IsWhiteSpace(c) \
  (' ' == (c) || '\t' == (c) || '\n' == (c) || '\r' == (c))
#define IsLeftParen(c) ('(' == (c))
#define IsRightParen(c) (')' == (c))
#define IsStringBegin(c) ('{' == (c))
#define IsStringEnd(c) ('}' == (c))
#define IsEndOfString(c) ('\0' == (c))
#define IsWhiteSpaceOrSeparatorOrEndOfString(c) \
  (IsEndOfString(c) || IsLeftParen(c)  || IsStringBegin(c) || \
   IsWhiteSpace(c)  || IsRightParen(c) || IsStringEnd(c))

char GTokenBuffer[512]; // TODO: Hardcoded max length

char* ParseString(char* p, char** next)
{
  int   length = 0;
  int   bufSize = 1024; // TODO: Hardcoded max length
  char* buf = SysAlloc(bufSize);
  char* pBuf = buf;
  int   level = 0;

  // Position is at opening curly

  // Move past opening curly
  ++ p;

  while (!IsEndOfString(*p))
  {
    // Handle nested strings
    if (IsStringBegin(*p)) ++ level;
    if (IsStringEnd(*p))   -- level;

    // Check if this was the ending closing curly
    if (level <= 0) break;

    // TODO: Quoted curlies

    // Copy character to string
    *pBuf = *p;

    ++ pBuf;
    ++ p;
    ++ length;
  }
  
  // Terminate string and realloc to fit
  *pBuf = 0;
  buf = realloc(buf, length + 1);

  // Move past closing curly
  *next = p + 1;

  // Position is at character after closing curly

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

VUInt TokenType(char* token)
{
  char* p = token;
  int   dec = 0; // Decimal sign counter

  // Default token type
  VUInt type = TypeSymbol;

  // Single minus sign is not a number
  if ( ('-' == *p) && (1 == strlen(token)) ) goto Exit;

  // Check number
  while (!IsEndOfString(*p))
  {
    if ('.' == *p) ++ dec;
    else goto Exit; // Not a number

    ++ p;
  }

  // Check decimal sign (more than 1 is not a number)
  if (0 == dec)
    type = TypeIntNum;
  else
  if (1 == dec)
    type = TypeDecNum;

Exit:
  return type;
}

VItem* ParseToken(char* token, VInterp* interp)
{
  VItem* item = InterpAllocItem(interp);
  VUInt type = TokenType(token);

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
      int symbol = SymbolTableFindAdd(token);
      ItemSetSymbol(item, symbol);
    }
  }
  else
  {
    GURU_MEDITATION(PARSER_TOKEN_TYPE_ERROR);
  }

  return item;
}

// A comment looks like this: /-- comment --/

// TODO: Handle nested comments

#define IsComment(p)    (StrStartsWith(p, "/--"))
#define IsCommentEnd(p) (StrStartsWith(p, "--/"))

char* SkipComment(char* p)
{
  if (!IsComment(p)) return p; // Not a comment

  // Move past opening comment
  p = p + 3;

  // Scan for end comment
  while (0 != *p)
  {
    if (IsCommentEnd(p)) return p + 3; // End of comment
    
    ++p;
  }

  return NULL; // End of string reached
}

VItem* ParseList(char* code, char** next, VInterp* interp)
{
  VItem* first = NULL;
  VItem* item  = NULL;
  VItem* prev;

  VItem* list = InterpAllocItem(interp);
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
      item = ParseList(p + 1, &p, interp);
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
    if (IsStringBegin(*p))
    {
      char* string = ParseString(p, &p);
      item = InterpAllocBuffer(interp, StrCopy(string));
      ItemSetType(item, TypeString);
      SysFree(string);
    }
    else
    if (IsStringEnd(*p))
    {
      GURU_MEDITATION(PARSER_UNBALANCED_STRING_END);
    }
    else
    if (IsComment(p))
    {
      p = SkipComment(p);
      if (NULL == p) goto Exit;
    }
    else
    {
      char* token = GetNextToken(p, &p);
      item = ParseToken(token, interp);
    }

    // Add item to list
    if (NULL != item)
    {
      if (NULL == first)
      {
        first = item;
      }
      else
      {
        InterpSetNext(interp, prev, item);
      }

      prev = item;
      item = NULL;
    }
  }

Exit:
  InterpSetFirst(interp, list, first);
  return list;
}

VItem* Parse(char* sourceCode, VInterp* interp)
{
  char* p;
  return ParseList(sourceCode, &p, interp);
}
