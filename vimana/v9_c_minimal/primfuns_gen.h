/*
File: primfuns.h
Author: Mikael Kindborg (mikael@kindborg.com)

This file is generated from primfuns.php. The idea is to generate 
primfun definitions using different implementation styles.

Usage: php genprimfuns.php > primfuns_gen.h 

Install PHP on Raspberry Pi:
https://lindevs.com/install-php-8-0-on-raspberry-pi/
*/

switch (primFun) {

case 0: // print
{
  VItem* item = InterpPop(interp);
  PrintItem(item);
  PrintNewLine();
  // InterpGC(interp, item);
}
break;

case 1: // setglobal
{
  VItem* quotedSymbol = InterpPop(interp);
  VItem* value = InterpPop(interp);
  VIndex index = ItemSymbol(ItemList_Get(ItemObj(quotedSymbol), 0));
  InterpSetGlobalVar(interp, index, value);
}
break;

case 2: // def
{
  VItem* funBody = InterpPop(interp);
  VItem* quotedSymbol = InterpPop(interp);
  ItemList(funBody)->type = TypeFun;
  VIndex index = ItemSymbol(ItemList_Get(ItemObj(quotedSymbol), 0));
  InterpSetGlobalVar(interp, index, funBody);
}
break;

case 3: // +
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) + ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
}
break;

case 4: // -
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) - ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
}
break;

case 5: // *
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) * ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
}
break;

case 6: // /
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) / ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
}
break;

case 7: // eq
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VBool booleanValue = ItemEquals(item1, item2);
  ItemSetBool(item1, booleanValue);
  ++ ListLength(InterpStack(interp));
}
break;

case 8: // <
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VBool booleanValue = ItemNumber(item1) < ItemNumber(item2);
  ItemSetBool(item1, booleanValue);
  ++ ListLength(InterpStack(interp));
}
break;

case 9: // >
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VBool booleanValue = ItemNumber(item1) > ItemNumber(item2);
  ItemSetBool(item1, booleanValue);
  ++ ListLength(InterpStack(interp));
}
break;

case 10: // iszero
{
  VItem* item = InterpPop(interp);
  VBool booleanValue = 0 == ItemNumber(item);
  ItemSetBool(item, booleanValue);
  ++ ListLength(InterpStack(interp));
}
break;

case 11: // eval
{
  VItem* item = InterpPop(interp);
  InterpPushContext(interp, ItemList(item));
}
break;

case 12: // iftrue
{
  VItem* trueBlock = InterpPop(interp);
  VItem* booleanValue = InterpPop(interp);
  if (ItemBool(booleanValue))
    InterpPushContext(interp, ItemList(trueBlock));
}
break;

case 13: // iffalse
{
  VItem* falseBlock = InterpPop(interp);
  VItem* booleanValue = InterpPop(interp);
  if (!ItemBool(booleanValue))
    InterpPushContext(interp, ItemList(falseBlock));
}
break;

case 14: // ifelse
{
  VItem* falseBlock = InterpPop(interp);
  VItem* trueBlock = InterpPop(interp);
  VItem* booleanValue = InterpPop(interp);
  if (ItemBool(booleanValue))
    InterpPushContext(interp, ItemList(trueBlock));
  else
    InterpPushContext(interp, ItemList(falseBlock));
}
break;

case 15: // not
{
  VItem* item = InterpPop(interp);
  ItemSetBool(item, ! ItemBool(item));
  ++ ListLength(InterpStack(interp));
}
break;

case 16: // drop
{
  ItemListDrop(InterpStack(interp));
}
break;

case 17: // dup
{
  ItemListDup(InterpStack(interp));
}
break;

case 18: // 2dup
{
  ItemList2Dup(InterpStack(interp));
}
break;

case 19: // over
{
  ItemListOver(InterpStack(interp));
}
break;

case 20: // swap
{
  ItemListSwap(InterpStack(interp));
}
break;

case 21: // printstack
{
  Print("STACK:");
  PrintList(InterpStack(interp));
  PrintNewLine();
}
break;

} // End of switch


