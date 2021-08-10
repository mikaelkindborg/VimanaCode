
// DECLARATIONS ------------------------------------------------

#define TokenBufferSize 512

int ParserWorker(Interp* interp, char* code, int i, int length, List* list);

// PARSER ------------------------------------------------------

List* ParseCode(Interp* interp, char* code)
{
  List* list = InterpListCreate(interp);
  ParserWorker(interp, code, 0, strlen(code), list);
  return list;
}

// PARSER FUNCTIONS --------------------------------------------

int ParserIsNumber(char* token)
{
  Bool decimalSignUsed = FALSE;
  for (int i = 0; i < strlen(token); ++ i)
  {
    char c = token[i];
    if ('.' == c)
    {
      if (decimalSignUsed) return 0;
      decimalSignUsed = TRUE;
    }
    else if (!isdigit(c))
    {
      return 0;
    }
  }
  return decimalSignUsed ? TypeDecNum : TypeIntNum;
}

void ParserAddSymbolOrNumber(Interp* interp, char* token, List* list)
{
  Item item;
  int type = ParserIsNumber(token);
  if (TypeIntNum == type)
  {
    long number = strtol(token, NULL, 10);
    //PrintDebug("TOKEN INTNUM: %s %li\n", token, num);
    item.type = TypeIntNum;
    //item.opCode = OpCodePushItem;
    item.value.intNum = number;
  }
  else if (TypeDecNum == type)
  {
    double number = strtod(token, NULL);
    //PrintDebug("TOKEN DECNUM: %s %f\n", token, num);
    item.type = TypeDecNum;
    //item.opCode = OpCodePushItem;
    item.value.decNum = number;
  }
  else
  {
    item = InterpAddSymbol(interp, token);
  }
  ListPush(list, item);
}

#define ParserBeginToken() \
  if (!ptoken) \
  { \
    ptoken = token; \
    tokenLength = 0; \
  } 

#define ParserEndToken() \
  if (ptoken) \
  { \
    *ptoken = 0; \
    ptoken = NULL;  \
    /*PrintDebug("ADD TOKEN: %s", token);*/ \
    ParserAddSymbolOrNumber(interp, token, list); \
  }

int ParserWorker(Interp* interp, char* code, int i, int length, List* list)
{
  char  token[TokenBufferSize];
  char* ptoken = NULL;
  int   tokenLength; // Used to check buffer overrun

  while (i < length)
  {
    // Begin list.
    if (code[i] == '(')
    {
      ParserEndToken();
      //PrintDebug("BEGIN LIST");
#ifdef USE_GC
      List* childList = GCListCreate(interp->gc);
#else
      List* childList = ListCreate();
#endif
      Item item = ItemWithList(childList);
      ListPush(list, item);
      i = ParserWorker(interp, code, i + 1, length, childList);
      continue;
    }

    // End list.
    if (code[i] == ')')
    {
      //PrintDebug("END LIST");
      ParserEndToken();
      return i + 1;
    }

    // Copy tokens separated by whitespace.
    // Whitespace separates tokens.
    if (code[i] == ' '  || code[i] == '\t' ||  
        code[i] == '\n' || code[i] == '\r')
    {
      ParserEndToken();
      ++ i;
      continue;
    }

    // When we got here, we have a new token.
    ParserBeginToken();

    // Copy char to token and move to next character.
    *ptoken = code[i];
    ++ ptoken;
    ++ i;
    ++ tokenLength;
    if (tokenLength >= TokenBufferSize)
      ErrorExit("ParserWorker: Token length exceeded");
  }
  
  // End last token.
  ParserEndToken();

  return i;
}
