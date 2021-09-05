/*
File: primfuns.h
Author: Mikael Kindborg (mikael@kindborg.com)

This file is generated from primfuns.php.

The idea is to generate primfun definitions using different styles.

Usage: php primfuns.php > primfuns.h 
*/

switch (primFun) {

case 1: {
  VItem* item = InterpPop(interp);
  PrintItem(item);
  PrintNewLine();
} break;

case 2: {
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) + ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
} break;

} // End of switch

