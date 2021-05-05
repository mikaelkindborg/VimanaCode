
/****************** PRINT LISTS ******************/

void ListPrintWorker(List* list, Bool useNewLine, Interp* interp);

void ListPrint(List* list, Interp* interp)
{
  Print("(");
  ListPrintWorker(list, FALSE, interp);
  Print(")");
}

void ListPrintItems(List* list, Interp* interp)
{
  ListPrintWorker(list, TRUE, interp);
}

void ListPrintWorker(List* list, Bool useNewLine, Interp* interp)
{
  // TODO: Make string type that can grow.
  char buf[128];
  
  for (int i = 0; i < ListLength(list); i++)
  {
    if (i > 0)
    {
      Print(" ");
    }
    
    Item item = ListGet(list, i);
    if (IsList(item))
    {
      ListPrint(item.value.list, interp);
    }
    else
    {
      ItemToString(item, buf, interp);
      Print("%s", buf);
    }
    
    if (useNewLine)
    {
      PrintLine("");
    }
  }
}

/****************** PRINT ITEMS ******************/

void ItemToString(Item item, char* stringbuf, Interp* interp)
{
  if (IsIntNum(item))
  {
    sprintf(stringbuf, "%li", item.value.intNum);
  }
  else if (IsDecNum(item))
  {
    sprintf(stringbuf, "%f", item.value.decNum);
  }
  else if (IsList(item))
  {
    //ListPrint(item.value.list, interp);
    sprintf(stringbuf, "[LIST]");
  }
  else if (IsPrimFun(item))
  {
    sprintf(stringbuf, "[PRIMFUN]");
  }
  else if (IsFun(item))
  {
    ListPrint(item.value.list, interp);
  }
  else if (IsString(item))
  {
    sprintf(stringbuf, "%s", item.value.string);
  }
  else if (IsSymbol(item))
  {
    char* str = InterpGetSymbolString(interp, item.value.symbol);
    if (NULL == str)
    {
      ErrorExit("ItemToString: symbol has no string\n");
    }
    sprintf(stringbuf, "%s", str);
  }
  else
  {
    sprintf(stringbuf, "[UNKNOWN]");
  }
}
