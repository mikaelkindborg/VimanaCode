/*
File: print.h
Author: Mikael Kindborg (mikael@kindborg.com)

Print items.
*/

#ifdef INCLUDE_SOURCE_CODE_PARSER
char* SymbolDictCurrentLookupSymbolName(VIndex symbolId);
char* SymbolDictCurrentLookupPrimFunName(VIndex primFunId);
#endif

void PrintItem(VItem* item);

void PrintList(VList* list)
{
  PrintChar('(');
  for (VIndex i = 0; i < ListLength(list); ++i)
  {
    VItem* item = ItemList_Get(list, i);
    PrintItem(item);
    if (i < ListLength(list) - 1) PrintChar(' ');
  }
  PrintChar(')');
#ifdef DEBUG
  PrintChar(':');
  PrintNum(ObjGetRefCount(list));
#endif
}

void PrintItem(VItem* item)
{
  if (IsVirgin(item))
  {
#ifdef INCLUDE_SOURCE_CODE_PARSER
    Print("[VIRGIN]");
#else
    PrintChar('V');
#endif
  }
  else
  if (IsNumber(item))
  {
    PrintNum(ItemNumber(item));
  }
  else
  if (IsSymbol(item))
  {
#ifdef INCLUDE_SOURCE_CODE_PARSER
    Print(SymbolDictCurrentLookupSymbolName(ItemSymbol(item)));
#else
    PrintChar('S');
    PrintNum(ItemSymbol(item));
#endif
  }
  else
  if (IsPrimFun(item))
  {
#ifdef INCLUDE_SOURCE_CODE_PARSER
    Print(SymbolDictCurrentLookupPrimFunName(ItemPrimFun(item)));
#else
    PrintChar('P');
    PrintNum(ItemPrimFun(item));
#endif
  }
  else
  if (IsFun(item))
  {
    PrintChar('F');
    PrintList(ItemList(item));
  }
  else
  if (IsList(item))
  {
    PrintList(ItemList(item));
  }
  else
  if (IsString(item))
  {
    Print(StringGetStr(ItemString(item)));
  }
  else
  {
    PrintChar('X');
    PrintBinaryULong(item->value.bits);
    GuruMeditation(SYMBOL_UNKNOWN_TYPE);
  }
}
