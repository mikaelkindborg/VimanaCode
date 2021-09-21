/*
File: print.h
Author: Mikael Kindborg (mikael@kindborg.com)

Print items.
*/

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
}

void PrintItem(VItem* item)
{
  if (IsVirgin(item))
  {
    PrintChar('V');
  }
  else
  if (IsNumber(item))
  {
    PrintNum(ItemNumber(item));
  }
  else
  if (IsSymbol(item))
  {
    PrintChar('S');
    PrintNum(ItemSymbol(item));
  }
  else
  if (IsPrimFun(item))
  {
    PrintChar('P');
    PrintNum(ItemPrimFun(item));
  }
  else
  if (IsFun(item))
  {
    PrintChar('F');
    PrintList(ItemObj(item));
  }
  else
  if (IsList(item))
  {
    PrintChar('L');
    PrintList(ItemObj(item));
  }
  else
  if (IsString(item))
  {
    Print(StringGetStr(ItemObj(item)));
  }
  else
  {
    PrintChar('X');
    PrintBinaryULong(item->value.bits);
    GuruMeditation(SYMBOL_UNKNOWN_TYPE);
  }
}
