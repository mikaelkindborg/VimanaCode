/*
File: primfuns.h
Author: Mikael Kindborg (mikael@kindborg.com)

This file is generated from primfuns.php. The idea is to generate 
primfun definitions using different implementation styles.

A dictionary of primfun strings is generated in file primfun_names_gen.h.

Usage: php genprimfuns.php > primfuns_gen.h 

How to install PHP on Raspberry Pi:
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
  VIndex index = ItemSymbol(ItemList_Get(ItemList(quotedSymbol), 0));
  InterpSetGlobalVar(interp, index, value);
}
break;

case 2: // def
{
  VItem* funBody = InterpPop(interp);
  VItem* quotedSymbol = InterpPop(interp);
  ObjSetType(ItemList(funBody), ObjTypeFun);
  VIndex index = ItemSymbol(ItemList_Get(ItemList(quotedSymbol), 0));
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

case 7: // add1
{
  VItem* item = InterpPop(interp);
  ItemSetNumber(item,  ItemNumber(item) + 1);
  ++ ListLength(InterpStack(interp));
}
break;

case 8: // sub1
{
  VItem* item = InterpPop(interp);
  ItemSetNumber(item,  ItemNumber(item) - 1);
  ++ ListLength(InterpStack(interp));
}
break;

case 9: // eq
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VBool trueOrFalse = ItemEquals(item1, item2);
  ItemSetBool(item1, trueOrFalse);
  ++ ListLength(InterpStack(interp));
}
break;

case 10: // <
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VBool trueOrFalse = ItemNumber(item1) < ItemNumber(item2);
  ItemSetBool(item1, trueOrFalse);
  ++ ListLength(InterpStack(interp));
}
break;

case 11: // >
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VBool trueOrFalse = ItemNumber(item1) > ItemNumber(item2);
  ItemSetBool(item1, trueOrFalse);
  ++ ListLength(InterpStack(interp));
}
break;

case 12: // iszero
{
  VItem* item = InterpPop(interp);
  VBool trueOrFalse = 0 == ItemNumber(item);
  ItemSetBool(item, trueOrFalse);
  ++ ListLength(InterpStack(interp));
}
break;

case 13: // eval
{
  VItem* item = InterpPop(interp);
  InterpPushContext(interp, ItemList(item));
}
break;

case 14: // iftrue
{
  VItem* trueBlock = InterpPop(interp);
  VItem* trueOrFalse = InterpPop(interp);
  if (ItemBool(trueOrFalse))
    InterpPushContext(interp, ItemList(trueBlock));
}
break;

case 15: // iffalse
{
  VItem* falseBlock = InterpPop(interp);
  VItem* trueOrFalse = InterpPop(interp);
  if (!ItemBool(trueOrFalse))
    InterpPushContext(interp, ItemList(falseBlock));
}
break;

case 16: // ifelse
{
  VItem* falseBlock = InterpPop(interp);
  VItem* trueBlock = InterpPop(interp);
  VItem* trueOrFalse = InterpPop(interp);
  if (ItemBool(trueOrFalse))
    InterpPushContext(interp, ItemList(trueBlock));
  else
    InterpPushContext(interp, ItemList(falseBlock));
}
break;

case 17: // not
{
  VItem* item = InterpPop(interp);
  ItemSetBool(item, !ItemBool(item));
  ++ ListLength(InterpStack(interp));
}
break;

case 18: // drop
{
  // ITEM DROP ->
  VList* list = InterpStack(interp);
  if (ListLength(list) < 1)
    GuruMeditation(PRIMFUN_DROP_CANNOT_DROP_FROM_EMPTY_LIST);
  -- ListLength(list);
}
break;

case 19: // dup
{
  // ITEM DUP -> ITEM ITEM
  VList* list = InterpStack(interp);
  ItemList_Push(list, ItemList_Get(list, ListLength(list) - 1));
}
break;

case 20: // 2dup
{
  // ITEM1 ITEM2 2DUP -> ITEM1 ITEM2 ITEM1 ITEM2
  VList* list = InterpStack(interp);
  VItem* item;
  item = ItemList_Get(list, ListLength(list) - 2);
  ItemList_Push(list, item);
  item = ItemList_Get(list, ListLength(list) - 2);
  ItemList_Push(list, item);
}
break;

case 21: // over
{
  // ITEM1 ITEM2 OVER -> ITEM1 ITEM2 ITEM1
  VList* list = InterpStack(interp);
  ItemList_Push(list, ItemList_Get(list, ListLength(list) - 2));
}
break;

case 22: // swap
{
  // ITEM1 ITEM2 SWAP -> ITEM2 ITEM1
  VList* list = InterpStack(interp);
  VItem* item1 = ItemList_GetRaw(list, ListLength(list) - 1);
  VItem* item2 = ItemList_GetRaw(list, ListLength(list) - 2);
  VItem temp = *item1;
  *item1 = *item2;
  *item2 = temp;
}
break;

case 23: // printstack
{
  Print("STACK:");
  PrintList(InterpStack(interp));
  PrintNewLine();
}
break;

// Arduino primitives
#ifdef PLATFORM_ARDUINO

case 24: // LED_BUILTIN
{
  // -> LED_BUILTIN
  VItem item;
  ItemSetNumber(&item, LED_BUILTIN);
  ItemList_Push(InterpStack(interp), &item);
}
break;

case 25: // OUTPUT
{
  // -> OUTPUT
  VItem item;
  ItemSetNumber(&item, OUTPUT);
  ItemList_Push(InterpStack(interp), &item);
}
break;

case 26: // HIGH
{
  // -> HIGH
  VItem item;
  ItemSetNumber(&item, HIGH);
  ItemList_Push(InterpStack(interp), &item);
}
break;

case 27: // LOW
{
  // -> LOW
  VItem item;
  ItemSetNumber(&item, LOW);
  ItemList_Push(InterpStack(interp), &item);
}
break;

case 28: // pinMode
{
  // pin mode pinMode ->
  VItem* mode = InterpPop(interp);
  VItem* pin = InterpPop(interp);
  pinMode(ItemNumber(pin), ItemNumber(mode));
}
break;

case 29: // digitalWrite
{
  // pin value digitalWrite ->
  VItem* value = InterpPop(interp);
  VItem* pin = InterpPop(interp);
  pinMode(ItemNumber(pin), ItemNumber(value));
}
break;

case 30: // delay
{
  // ms delay ->
  VItem* millis = InterpPop(interp);
  delay(ItemNumber(pin));
}
break;

#endif // PLATFORM_ARDUINO

} // End of switch


