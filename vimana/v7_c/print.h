
// DECLARTIONS -------------------------------------------------

char* InterpGetSymbolString(Interp* interp, Index index);
void  ListPrintToStream(FileStream* stream, List* list, Bool useNewLine, Interp* interp);

// PRINT ITEMS -------------------------------------------------

void ItemPrintToStream(FileStream* stream, Item item, Interp* interp)
{
  if (IsIntNum(item))
  {
    PrintToStream(stream, "%li", item.value.intNum);
  }
  else if (IsDecNum(item))
  {
    PrintToStream(stream, "%f", item.value.decNum);
  }
  else if (IsPrimFun(item))
  {
    PrintToStream(stream, "[PRIMFUN]");
  }
  //else if (IsOptimizedList(item))
  //{
  //  PrintToStream(stream, "[OPTIMIZEDLIST]");
  //}
  else if (IsList(item))
  {
    //ListPrint(item.value.list, interp);
    //PrintToStream(stream, "[LIST]");
    ListPrintToStream(stream, item.value.list, FALSE, interp);
  }
  else if (IsFun(item))
  {
    // TODO: Print original source list.
    //PrintToStream(stream, item.value.list, interp);
    PrintToStream(stream, "[FUN]");
    ListPrintToStream(stream, item.value.list, FALSE, interp);
  }
  else if (IsString(item))
  {
    PrintToStream(stream, "%s", item.value.string);
  }
  else if (IsSymbol(item))
  {
    char* str = InterpGetSymbolString(interp, item.value.symbol);
    if (NULL == str)
    {
      ErrorExit("ItemToString: symbol has no string\n");
    }
    PrintToStream(stream, "%s", str);
  }
  //else if (IsLocalVar(item))
  //{
  //  PrintToStream(stream, "[VAR %li]", (long)item.value.symbol);
  //}
  else if (IsVirgin(item))
  {
    PrintToStream(stream, "[VIRGIN]");
  }
  else if (IsBool(item))
  {
    if (item.value.truth)
      PrintToStream(stream, "TRUE");
    else
      PrintToStream(stream, "FALSE");
  }
  else
  {
    PrintToStream(stream, "[UNKNOWN]");
  }
}

// Use free() to deallocate the returned buffer.
char* ItemToString(Item item, Interp* interp)
{
  char* buffer;
  size_t size;
  FileStream* stream = open_memstream(&buffer, &size);
  ItemPrintToStream(stream, item, interp);
  fclose(stream);
  return buffer;
}

void ItemPrint(Item item, Interp* interp)
{
  char* string = ItemToString(item, interp);
  puts(string);
  free(string);
}

// PRINT LISTS -------------------------------------------------

void ListPrintWorker(FileStream* stream, List* list, Bool useNewLine, Interp* interp)
{
  for (int i = 0; i < ListLength(list); i++)
  {
    if (i > 0)
    {
      PrintToStream(stream, " ");
    }
    
    Item item = ListGet(list, i);
    //if (IsList(item) && !IsOptimizedList(item))
    if (IsList(item))
    {
      //ListPrintToStream(stream, item.value.list, useNewLine, interp);
      ListPrintToStream(stream, item.value.list, FALSE, interp);
    }
    else
    {
      ItemPrintToStream(stream, item, interp);
    }
    
    if (useNewLine)
    {
      PrintToStream(stream, "\n");
    }
  }
}

void ListPrintToStream(FileStream* stream, List* list, Bool useNewLine, Interp* interp)
{
  PrintToStream(stream, "(");
  ListPrintWorker(stream, list, useNewLine, interp);
  PrintToStream(stream, ")");
}

void ListPrintHelper(List* list, Bool useNewLine, Interp* interp)
{
  char* buffer;
  size_t size;
  FileStream* stream = open_memstream(&buffer, &size);
  ListPrintToStream(stream, list, useNewLine, interp);
  fclose(stream);
  puts(buffer);
  free(buffer);
}

// Print list using parens with no line breaks.
void ListPrint(List* list, Interp* interp)
{
  ListPrintHelper(list, FALSE, interp);
}

// Print list with each top-level element on a new line.
void ListPrintItems(List* list, Interp* interp)
{
  ListPrintHelper(list, TRUE, interp);
}
