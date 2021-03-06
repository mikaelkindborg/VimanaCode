/*
File: primfuns.h
Author: Mikael Kindborg (mikael@kindborg.com)

This file is generated from primfuns.php. The idea is to generate 
primfun definitions using different implementation styles.

A dictionary of primfun strings is generated in file primfun_names_gen.h.
A table of primfun functions is generated in file primfun_table_gen.h.

Usage: php genprimfuns.php > primfuns_gen.h 

How to install PHP on Raspberry Pi:
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

// sub2
void PrimFun_8(VInterp* interp, VItem* primFunItem)
{
  VItem* item = InterpPop(interp);
  ItemSetNumber(item,  ItemNumber(item) - 2);
  ++ ListLength(InterpStack(interp));
}

// eq
void PrimFun_9(VInterp* interp, VItem* primFunItem)
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VBool trueOrFalse = ItemEquals(item1, item2);
  ItemSetBool(item1, trueOrFalse);
  ++ ListLength(InterpStack(interp));
}

// <
void PrimFun_10(VInterp* interp, VItem* primFunItem)
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VBool trueOrFalse = ItemNumber(item1) < ItemNumber(item2);
  ItemSetBool(item1, trueOrFalse);
  ++ ListLength(InterpStack(interp));
}

// >
void PrimFun_11(VInterp* interp, VItem* primFunItem)
{
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VBool trueOrFalse = ItemNumber(item1) > ItemNumber(item2);
  ItemSetBool(item1, trueOrFalse);
  ++ ListLength(InterpStack(interp));
}

// iszero
void PrimFun_12(VInterp* interp, VItem* primFunItem)
{
  VItem* item = InterpPop(interp);
  VBool trueOrFalse = 0 == ItemNumber(item);
  ItemSetBool(item, trueOrFalse);
  ++ ListLength(InterpStack(interp));
}

// eval
void PrimFun_13(VInterp* interp, VItem* primFunItem)
{
  VItem* item = InterpPop(interp);
  InterpPushContext(interp, ItemList(item));
}

// iftrue
void PrimFun_14(VInterp* interp, VItem* primFunItem)
{
  VItem* trueBlock = InterpPop(interp);
  VItem* trueOrFalse = InterpPop(interp);
  if (ItemBool(trueOrFalse))
    InterpPushContext(interp, ItemList(trueBlock));
}

// iffalse
void PrimFun_15(VInterp* interp, VItem* primFunItem)
{
  VItem* falseBlock = InterpPop(interp);
  VItem* trueOrFalse = InterpPop(interp);
  if (!ItemBool(trueOrFalse))
    InterpPushContext(interp, ItemList(falseBlock));
}

// ifelse
void PrimFun_16(VInterp* interp, VItem* primFunItem)
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
void PrimFun_17(VInterp* interp, VItem* primFunItem)
{
  VItem* item = InterpPop(interp);
  ItemSetBool(item, !ItemBool(item));
  ++ ListLength(InterpStack(interp));
}

// drop
void PrimFun_18(VInterp* interp, VItem* primFunItem)
{
  // ITEM DROP ->
  VList* list = InterpStack(interp);
  if (ListLength(list) < 1)
    GuruMeditation(PRIMFUN_DROP_CANNOT_DROP_FROM_EMPTY_LIST);
  -- ListLength(list);
}

// dup
void PrimFun_19(VInterp* interp, VItem* primFunItem)
{
  // ITEM DUP -> ITEM ITEM
  VList* list = InterpStack(interp);
  VItem* item = ItemList_Get(list, ListLength(list) - 1);
  VIndex index = ListLength(list);
  ListCheckCapacity(list, index);
  ++ ListLength(list);
  ItemList_SetRaw(list, index, item);
}

// 2dup
void PrimFun_20(VInterp* interp, VItem* primFunItem)
{
  // ITEM1 ITEM2 2DUP -> ITEM1 ITEM2 ITEM1 ITEM2
  VList* list = InterpStack(interp);
  VItem* item;
  item = ItemList_Get(list, ListLength(list) - 2);
  ItemList_Push(list, item);
  item = ItemList_Get(list, ListLength(list) - 2);
  ItemList_Push(list, item);
}

// over
void PrimFun_21(VInterp* interp, VItem* primFunItem)
{
  // ITEM1 ITEM2 OVER -> ITEM1 ITEM2 ITEM1
  VList* list = InterpStack(interp);
  ItemList_Push(list, ItemList_Get(list, ListLength(list) - 2));
}

// swap
void PrimFun_22(VInterp* interp, VItem* primFunItem)
{
  // ITEM1 ITEM2 SWAP -> ITEM2 ITEM1
  VList* list = InterpStack(interp);
  VItem* item1 = ItemList_GetRaw(list, ListLength(list) - 1);
  VItem* item2 = ItemList_GetRaw(list, ListLength(list) - 2);
  VItem temp = *item1;
  *item1 = *item2;
  *item2 = temp;
}

// printstack
void PrimFun_23(VInterp* interp, VItem* primFunItem)
{
  Print("STACK:");
  PrintList(InterpStack(interp));
  PrintNewLine();
}

// PRIMFUNS END ------------------------------------------------

