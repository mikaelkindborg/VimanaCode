/*
File: printlist.h
Author: Mikael Kindborg (mikael@kindborg.com)

Functions for printing lists and items.
*/

// -------------------------------------------------------------
// Print lists and items
// -------------------------------------------------------------

void InterpPrintList(VInterp* interp, VItem* first);

void InterpPrintItem(VInterp* interp, VItem* item)
{
  //printf("[T%i]", ItemType(item));
  if (IsTypeNone(item))
    Print("[NONE]");
  else if (IsTypeList(item))
    InterpPrintList(interp, item);
  else if (IsTypeIntNum(item))
    printf("%li", item->intNum);
  else if (IsTypeDecNum(item))
    printf("%g", item->decNum);
  else if (IsTypePrimFun(item))
    printf("P%li", item->first); // TODO: Lookup name of primfun
  else if (IsTypeSymbol(item))
    printf("%s", SymbolTableGet(ItemGetSymbol(item)));
  else if (IsTypeString(item))
    printf("{%s}", (char*)InterpGetBufferPtr(interp, item));
  else if (IsTypeBuffer(item))
    printf("[BUFFER] %lu", (long unsigned)InterpGetBufferPtr(interp, item));
  else if (IsTypeFun(item))
  {
    printf("[FUN] ");
    InterpPrintList(interp, item);
  }  
  else if (IsTypeFunX(item))
  {
    printf("[FUNX] ");
    InterpPrintList(interp, item);
  }
}

void InterpPrintList(VInterp* interp, VItem* list)
{
  PrintChar('(');

  int printSpace = FALSE;
  VItem* item = InterpGetFirst(interp, list);

  while (item)
  {
    if (printSpace) PrintChar(' ');
    InterpPrintItem(interp, item);
    item = InterpGetNext(interp, item);
    printSpace = TRUE;
  }

  PrintChar(')');
}

void InterpPrintItemArray(VInterp* interp, VItem* array, int numItems)
{
  for (int i = 0; i < numItems; ++ i)
  {
    InterpPrintItem(interp, &(array[i]));
    PrintChar(' ');
  }
}
