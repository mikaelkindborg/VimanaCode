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

char* ParseString(char* p, char** next)
{
  int   length = 0;
  int   bufSize = 1024; // TODO: Replace hardcoded max length with counting string length
  char* buf = SysAlloc(bufSize); // TODO: Check NULL != buf
  char* pBuf = buf;
  int   level = 1;

  // Position now is at opening curly

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

  // Position is now at character after closing curly

  return buf;
}

VType ParseTokenType(char* token)
{
  char* p = token;
  int   dec = 0; // Decimal sign counter

  // Default token type
  VType type = TypeSymbol;

  // A single minus sign is not a number
  if ( ('-' == *p) && (1 == strlen(token)) ) goto Exit;

  // Check number
  while (!IsEndOfString(*p))
  {
    if (!isdigit(*p))
    {
      if ('.' == *p)
        ++ dec;
      else
        goto Exit; // Not a number
    }
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
  VType type = ParseTokenType(token);

  VItem* item = AllocItem(interp);

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
      ItemSetPrimFun(item, primFunId);
      SymbolMemResetPos(InterpSymbolMem(interp));
    }
    else
    {
      int symbolIndex = SymbolTableFind(InterpGetSymbolTable(interp), token);
      if (symbolIndex < 0)
      {
        // New symbol - add it to symbol table
        SymbolMemWriteFinish(InterpSymbolMem(interp));
        symbolIndex = SymbolTableAdd(InterpGetSymbolTable(interp), token);
      }
      else
      {
        // Existing symbol, free token string
        SymbolMemResetPos(InterpSymbolMem(interp));
      }

      // Set symbol index of item
      ItemSetSymbol(item, symbolIndex);
    }
  }
  else
  {
    GURU_MEDITATION(PARSER_TOKEN_TYPE_ERROR);
  }

  return item;
}

// Read next token into string memory
char* ReadNextToken(char* p, char** next, VInterp* interp)
{
  char* token = SymbolMemGetNextFree(InterpSymbolMem(interp));

  while (!IsWhiteSpaceOrSeparatorOrEndOfString(*p))
  {
    SymbolMemWriteChar(InterpSymbolMem(interp), *p);
    ++ p;
  }

  *next = p;

  return token;
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

  VItem* list = AllocItem(interp);
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
      // Handle takes ownership of string
      char* string = ParseString(p, &p);
      item = AllocHandle(string, TypeString, interp);
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
      char* token = ReadNextToken(p, &p, interp);
      item = ParseToken(token, interp);
    }

    // Add item to list
    if (NULL != item)
    {
      if (NULL == first)
        first = item;
      else
        SetNext(prev, item, interp);

      prev = item;
      item = NULL;
    }
  }

Exit:

  if (NULL != first)
    SetFirst(list, first, interp);

  return list;
}

VItem* Parse(char* sourceCode, VInterp* interp)
{
  char* p;
  return ParseList(sourceCode, &p, interp);
}
