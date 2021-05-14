
/****************** PARSER ******************/

int ParserWorker(Interp* interp, char* code, int i, int length, List* list);

List* ParseCode(Interp* interp, char* code)
{
  List* list = ListCreate();
  ParserWorker(interp, code, 0, strlen(code), list);
  return list;
}

int ParserIsNumber(char* token)
{
  Bool decimalSignUsed = FALSE;
  for (int i = 0; i < strlen(token); i++)
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
    long num = strtol(token, NULL, 10);
    //PrintDebug("TOKEN INTNUM: %s %li\n", token, num);
    item = ItemWithIntNum(num);
  }
  else if (TypeDecNum == type)
  {
    double num = strtod(token, NULL);
    //PrintDebug("TOKEN DECNUM: %s %f\n", token, num);
    item = ItemWithDecNum(num);
  }
  else
  {
    item = InterpAddSymbol(interp, token);
  }
  ListPush(list, item);
}

int ParserWorker(Interp* interp, char* code, int i, int length, List* list)
{
  int foo = 0;
  char token[512]; // TODO: Check buffer overrun
  char* ptoken = NULL;

  while (i < length)
  {
    // Begin list.
    if (code[i] == '(')
    {
      //PrintDebug("BEGIN LIST");
      List* childList = ListCreate();
      Item item = ItemWithList(childList);
      ListPush(list, item);
      // ParserWorker will add iten to the child list.
      i = ParserWorker(interp, code, i + 1, length, childList);
      continue;
    }

    // End list.
    if (code[i] == ')')
    {
      //PrintDebug("END LIST");  
      // End token
      if (ptoken)
      {
        *ptoken = 0; // Zero terminate token
        //PrintDebug("ADD TOKEN: %s", token);
        ParserAddSymbolOrNumber(interp, token, list);
      }
      // Returning ends the list
      return i + 1;
    }
    
    // Copy tokens separated by whitespace.
    // Whitespace separates tokens.
    if (code[i] == ' '  || code[i] == '\t' ||  
        code[i] == '\n' || code[i] == '\r')
    {
      // End token
      if (ptoken)
      {
        
        *ptoken = 0; // Zero terminate token
        ptoken = NULL; // Indicates no token 
        //PrintDebug("ADD TOKEN: %s", token);
        ParserAddSymbolOrNumber(interp, token, list);
      }
      i++;
      continue;
    }

    // When we are here, we have a token character.

    // Begin token.
    if (!ptoken)
    {
      ptoken = token;
    }
    
    // Copy char to token
    *ptoken = code[i];
    ptoken++;
    i++;
  }
  
  // End last token
  if (ptoken)
  {
    *ptoken = 0; // Zero terminate token
    //PrintDebug("ADD TOKEN: %s", token);
    ParserAddSymbolOrNumber(interp, token, list);
  }

  return i;
}
