/*
File: primfuns.h
Author: Mikael Kindborg (mikael@kindborg.com)

This file is generated from primfuns.php. The idea is to generate 
primfun definitions using different implementation styles.

A dictionary of primfun strings is generated in file primfun_names_gen.h.
A table of primfun functions is generated in file primfun_table_gen.h.

Usage: php genprimfuns.php > primfuns_gen.h 

Note to myself. How to install PHP on Raspberry Pi:
https://lindevs.com/install-php-8-0-on-raspberry-pi/
*/

// PRIMFUNS BEGIN ----------------------------------------------

// print
void PrimFun_0(VInterp* interp, VItem* primFunItem)
{
  VItem* item = InterpPop(interp);
  PrintItem(item);
  PrintNewLine();
}

// setglobal
void PrimFun_1(VInterp* interp, VItem* primFunItem)
{
  VItem* quotedSymbol = InterpPop(interp);
  VItem* value = InterpPop(interp);
  VIndex index = ItemSymbol(ItemList_Get(ItemList(quotedSymbol), 0));
  InterpSetGlobalVar(interp, index, value);
}

// def
void PrimFun_2(VInterp* interp, VItem* primFunItem)
{
  VItem* funBody = InterpPop(interp);
  VItem* quotedSymbol = InterpPop(interp);
  ObjSetType(ItemList(funBody), ObjTypeFun);
  VIndex index = ItemSymbol(ItemList_Get(ItemList(quotedSymbol), 0));
  InterpSetGlobalVar(interp, index, funBody);
}

// +
void PrimFun_3(VInterp* interp, VItem* primFunItem)
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) + ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
}

// -
void PrimFun_4(VInterp* interp, VItem* primFunItem)
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) - ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
}

// *
void PrimFun_5(VInterp* interp, VItem* primFunItem)
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) * ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
}

// /
void PrimFun_6(VInterp* interp, VItem* primFunItem)
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) / ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
}

// sub1
void PrimFun_7(VInterp* interp, VItem* primFunItem)
{
  VItem* item = InterpPop(interp);
  ItemSetNumber(item,  ItemNumber(item) - 1);
  ++ ListLength(InterpStack(interp));
}

// eq
void PrimFun_8(VInterp* interp, VItem* primFunItem)
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VBool trueOrFalse = ItemEquals(item1, item2);
  ItemSetBool(item1, trueOrFalse);
  ++ ListLength(InterpStack(interp));
}

// <
void PrimFun_9(VInterp* interp, VItem* primFunItem)
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VBool trueOrFalse = ItemNumber(item1) < ItemNumber(item2);
  ItemSetBool(item1, trueOrFalse);
  ++ ListLength(InterpStack(interp));
}

// >
void PrimFun_10(VInterp* interp, VItem* primFunItem)
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VBool trueOrFalse = ItemNumber(item1) > ItemNumber(item2);
  ItemSetBool(item1, trueOrFalse);
  ++ ListLength(InterpStack(interp));
}

// iszero
void PrimFun_11(VInterp* interp, VItem* primFunItem)
{
  VItem* item = InterpPop(interp);
  VBool trueOrFalse = 0 == ItemNumber(item);
  ItemSetBool(item, trueOrFalse);
  ++ ListLength(InterpStack(interp));
}

// eval
void PrimFun_12(VInterp* interp, VItem* primFunItem)
{
  VItem* item = InterpPop(interp);
  InterpPushContext(interp, ItemList(item));
}

// iftrue
void PrimFun_13(VInterp* interp, VItem* primFunItem)
{
  VItem* trueBlock = InterpPop(interp);
  VItem* trueOrFalse = InterpPop(interp);
  if (ItemBool(trueOrFalse))
    InterpPushContext(interp, ItemList(trueBlock));
}

// iffalse
void PrimFun_14(VInterp* interp, VItem* primFunItem)
{
  VItem* falseBlock = InterpPop(interp);
  VItem* trueOrFalse = InterpPop(interp);
  if (!ItemBool(trueOrFalse))
    InterpPushContext(interp, ItemList(falseBlock));
}

// ifelse
void PrimFun_15(VInterp* interp, VItem* primFunItem)
{
  VItem* falseBlock = InterpPop(interp);
  VItem* trueBlock = InterpPop(interp);
  VItem* trueOrFalse = InterpPop(interp);
  if (ItemBool(trueOrFalse))
    InterpPushContext(interp, ItemList(trueBlock));
  else
    InterpPushContext(interp, ItemList(falseBlock));
}

// not
void PrimFun_16(VInterp* interp, VItem* primFunItem)
{
  VItem* item = InterpPop(interp);
  ItemSetBool(item, !ItemBool(item));
  ++ ListLength(InterpStack(interp));
}

// drop
void PrimFun_17(VInterp* interp, VItem* primFunItem)
{
  // ItemListDrop(InterpStack(interp));
  InterpPop(interp);
}

// dup
void PrimFun_18(VInterp* interp, VItem* primFunItem)
{
  ItemListDup(InterpStack(interp));
}

// 2dup
void PrimFun_19(VInterp* interp, VItem* primFunItem)
{
  ItemList2Dup(InterpStack(interp));
}

// over
void PrimFun_20(VInterp* interp, VItem* primFunItem)
{
  ItemListOver(InterpStack(interp));
}

// swap
void PrimFun_21(VInterp* interp, VItem* primFunItem)
{
  ItemListSwap(InterpStack(interp));
}

// printstack
void PrimFun_22(VInterp* interp, VItem* primFunItem)
{
  Print("STACK:");
  PrintList(InterpStack(interp));
  PrintNewLine();
}

// PRIMFUNS END ------------------------------------------------

