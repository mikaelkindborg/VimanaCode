/*
File: print.h
Author: Mikael Kindborg (mikael@kindborg.com)

Print items.
*/

void PrintItem(VmItem* item);

void PrintList(VmList* list)
{
  Print("(");
  for (VmIndex i = 0; i < ListLength(list); ++i)
  {
    VmItem* item = ListGet(list, i);
    PrintItem(item);
    if (i < ListLength(list) - 1) Print(" ");
  }
  Print(")");
}

void PrintItem(VmItem* item)
{
  if (IsNumber(*item))
  {
    PrintNum(ItemNumber(*item));
  }
  else
  if (IsPrimFun(*item))
  {
    Print("P");
    PrintNum(ItemPrimFun(*item));
  }
  else
  if (IsSymbol(*item))
  {
    Print("S");
    PrintNum(ItemSymbol(*item));
  }
  else
  if (IsList(*item))
  {
    PrintList(ItemObj(*item));
  }
  else
  if (IsString(*item))
  {
    Print(ItemString(*item));
  }
}
