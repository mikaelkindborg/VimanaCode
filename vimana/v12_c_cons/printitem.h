/*
File: printlist.h
Author: Mikael Kindborg (mikael@kindborg.com)

Functions for printing lists and items.
*/

// -------------------------------------------------------------
// Print lists and items
// -------------------------------------------------------------

void PrintList(VItem* list, VInterp* interp);

void PrintItem(VItem* item, VInterp* interp)
{
  //printf("[T%i]", ItemType(item));
  if (IsTypeNone(item))
  {
    Print("[NONE]");
  }
  else if (IsTypeList(item))
  {
    PrintList(item, interp);
  }
  else if (IsTypeIntNum(item))
  {
    printf("%li", item->intNum);
  }
  else if (IsTypeDecNum(item))
  {
    printf("%g", item->decNum);
  }
  else if (IsTypePrimFun(item))
  {
    //printf("P%lu", (unsigned long) ItemGetPtr(item)); 
    printf("%s", LookupPrimFunName(ItemGetPtr(item)));
  }
  else if (IsTypeSymbol(item))
  {
    printf("%s", SymbolTableGet(ItemGetSymbol(item)));
    //printf("S%li", ItemGetSymbol(item));
  }
  else if (IsTypeString(item))
  {
    printf("{%s}", (char*)GetHandlePtr(item, interp));
  }
  else if (IsTypeHandle(item))
  {
    printf("[HANDLE] %lu", (unsigned long)GetHandlePtr(item, interp));
  }
  else if (IsTypeFun(item))
  {
    printf("[FUN] ");
    PrintList(item, interp);
  }  
  else if (IsTypeFunX(item))
  {
    printf("[FUNX] ");
    PrintList(item, interp);
  }
}

void PrintList(VItem* list, VInterp* interp)
{
  PrintChar('(');

  int printSpace = FALSE;
  VItem* item = GetFirst(list, interp);

  while (item)
  {
    if (printSpace) PrintChar(' ');
    PrintItem(item, interp);
    item = GetNext(item, interp);
    printSpace = TRUE;
  }

  PrintChar(')');
}

void PrintItemArray(VItem* array, int numItems, VInterp* interp)
{
  for (int i = 0; i < numItems; ++ i)
  {
    PrintItem(&(array[i]), interp);
    PrintChar(' ');
  }
}
