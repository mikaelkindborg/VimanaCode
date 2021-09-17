/*
File: primfuns.h
Author: Mikael Kindborg (mikael@kindborg.com)

This file is generated from primfuns.php. The idea is to generate 
primfun definitions using different implementation styles.

Usage: php genprimfuns.php > primfuns.h 

Install PHP on Raspberry Pi:
https://lindevs.com/install-php-8-0-on-raspberry-pi/
*/

switch (primFun) {

case 0: // print
{
  VItem* item = InterpPop(interp);
  PrintItem(item);
  PrintNewLine();
}
break;

case 1: // setglobal
{
  VItem* quotedSymbol = InterpPop(interp);
  VItem* value = InterpPop(interp);
  VIndex index = ItemSymbol(ListGet(ItemObj(quotedSymbol), 0));
  InterpSetGlobal(interp, index, value);
}
break;

case 2: // def
{
  VItem* funBody = InterpPop(interp);
  VItem* quotedSymbol = InterpPop(interp);
  ItemObjAsList(funBody)->type = TypeFun;
  VIndex index = ItemSymbol(ListGet(ItemObj(quotedSymbol), 0));
  InterpSetGlobal(interp, index, funBody);
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

} // End of switch


