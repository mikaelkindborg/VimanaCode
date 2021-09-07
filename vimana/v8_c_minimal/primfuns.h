/*
GENERATED FILE.

File: primfuns.h
Author: Mikael Kindborg (mikael@kindborg.com)

This file is generated from primfuns.php. The idea is to generate 
primfun definitions using different implementation styles.

Usage: php genprimfuns.php > primfuns.h 

Install PHP on Raspberry Pi:
https://lindevs.com/install-php-8-0-on-raspberry-pi/
*/

switch (primFun) {

case 1: // print
{
  VItem* item = InterpPop(interp);
  PrintItem(item);
  PrintNewLine();
}
break;

case 2: // setglobal
{
  VItem* quotedSymbol = InterpPop(interp);
  VItem* value = InterpPop(interp);
  VIndex index = ItemSymbol(ListGet(ItemObj(quotedSymbol), 0));
  InterpSetGlobal(interp, index, value);
}
break;

case 3: // def
{
  VItem* funBody = InterpPop(interp);
  VItem* quotedSymbol = InterpPop(interp);
  ItemObjAsList(funBody)->type = TypeFun;
  VIndex index = ItemSymbol(ListGet(ItemObj(quotedSymbol), 0));
  InterpSetGlobal(interp, index, funBody);
}
break;

case 4: // add
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) + ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
}
break;

case 5: // sub
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) - ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
}
break;

case 6: // mult
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) * ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
}
break;

case 7: // div
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) / ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
}
break;

} // End of switch

#define PRIMFUN_print 1
#define PRIMFUN_setglobal 2
#define PRIMFUN_def 3
#define PRIMFUN_add 4
#define PRIMFUN_sub 5
#define PRIMFUN_mult 6
#define PRIMFUN_div 7


