
/****************** PARSER ******************/

int InterpParserWorker(Interp* interp, char* code, int i, int length, List* list);

List* InterpParseCode(Interp* interp, char* code)
{
  List* list = ListCreate();
  InterpParserWorker(interp, code, 0, strlen(code), list);
  return list;
}

int InterpParserIsNumber(char* token)
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

void InterpParserAddSymbolOrNumber(Interp* interp, char* token, List* list)
{
  Item item;
  int type = InterpParserIsNumber(token);
  if (IsIntNum(type))
  {
    long num = strtol(token, NULL, 10);
    item = ItemWithIntNum(num);
  }
  else if (IsDecNum(type))
  {
    double num = strtod(token, NULL);
    item = ItemWithDecNum(num);
  }
  else
  {
    item = InterpAddSymbol(interp, token);
  }
  ListPush(list, item);
}

int InterpParserWorker(Interp* interp, char* code, int i, int length, List* list)
{
  int copying = 0;
  char token[32];
  char* ptoken;

  while (i < length)
  {
    // Begin list.
    if (code[i] == '(')
    {
      //printf("BEGIN LIST\n");
      List* childList = ListCreate();
      Item item = ItemWithList(childList);
      ListPush(list, item);
      i = InterpParserWorker(interp, code, i + 1, length, childList);
      continue;
    }
    
    // Skip whitespace or end list.
    if (code[i] == ' '  || code[i] == '\t' ||  code[i] == '\n' || 
        code[i] == '\r' || code[i] == ')')
    {
      // Copy end.
      if (copying)
      {
        copying = 0;
        *ptoken = 0;
        //printf("%s\n", token);
        InterpParserAddSymbolOrNumber(interp, token, list);
        
        // End list.
        if (code[i] == ')')
        {
          //printf("END LIST\n");  
          return i;
        }
      }
    }
    else
    {
      // If we are not copying begin.
      if (!copying)
      {
        copying = 1;
        ptoken = token;
      }
      
      // copy to token
      *ptoken = code[i];
      ptoken++;
    }
    
    i++;
  }
  
  if (copying)
  {
    // end token
    *ptoken = 0;
    //printf("%s\n", token);
    InterpParserAddSymbolOrNumber(interp, token, list);
  }

  return i;
}