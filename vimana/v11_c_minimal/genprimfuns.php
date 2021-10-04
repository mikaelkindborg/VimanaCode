<?php /* Comment below is for the generated file primfuns.h */ ?>
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

<?php
$PrimFunCounter = -1;
$PrimFunTable = [];
?>
<?php PrimFunsHeader(); ?>

<?php PrimFunDef("print"); ?>
  VItem* item = InterpPop(interp);
  PrintItem(item);
  PrintNewLine();
<?php PrimFunEnd(); ?>

<?php PrimFunDef("setglobal"); ?>
  VItem* quotedSymbol = InterpPop(interp);
  VItem* value = InterpPop(interp);
  VIndex index = ItemSymbol(ItemList_Get(ItemList(quotedSymbol), 0));
  InterpSetGlobalVar(interp, index, value);
<?php PrimFunEnd(); ?>

<?php PrimFunDef("def"); ?>
  VItem* funBody = InterpPop(interp);
  VItem* quotedSymbol = InterpPop(interp);
  ObjSetType(ItemList(funBody), ObjTypeFun);
  VIndex index = ItemSymbol(ItemList_Get(ItemList(quotedSymbol), 0));
  InterpSetGlobalVar(interp, index, funBody);
<?php PrimFunEnd(); ?>

<?php PrimFunDef("+"); ?>
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) + ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("-"); ?>
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) - ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("*"); ?>
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) * ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("/"); ?>
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) / ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("sub1"); ?>
  VItem* item = InterpPop(interp);
  ItemSetNumber(item,  ItemNumber(item) - 1);
  ++ ListLength(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("eq"); ?>
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VBool trueOrFalse = ItemEquals(item1, item2);
  ItemSetBool(item1, trueOrFalse);
  ++ ListLength(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("<"); ?>
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VBool trueOrFalse = ItemNumber(item1) < ItemNumber(item2);
  ItemSetBool(item1, trueOrFalse);
  ++ ListLength(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef(">"); ?>
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VBool trueOrFalse = ItemNumber(item1) > ItemNumber(item2);
  ItemSetBool(item1, trueOrFalse);
  ++ ListLength(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("iszero"); ?>
  VItem* item = InterpPop(interp);
  VBool trueOrFalse = 0 == ItemNumber(item);
  ItemSetBool(item, trueOrFalse);
  ++ ListLength(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("eval"); ?>
  VItem* item = InterpPop(interp);
  InterpPushContext(interp, ItemList(item));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("iftrue"); ?>
  VItem* trueBlock = InterpPop(interp);
  VItem* trueOrFalse = InterpPop(interp);
  if (ItemBool(trueOrFalse))
    InterpPushContext(interp, ItemList(trueBlock));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("iffalse"); ?>
  VItem* falseBlock = InterpPop(interp);
  VItem* trueOrFalse = InterpPop(interp);
  if (!ItemBool(trueOrFalse))
    InterpPushContext(interp, ItemList(falseBlock));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("ifelse"); ?>
  VItem* falseBlock = InterpPop(interp);
  VItem* trueBlock = InterpPop(interp);
  VItem* trueOrFalse = InterpPop(interp);
  if (ItemBool(trueOrFalse))
    InterpPushContext(interp, ItemList(trueBlock));
  else
    InterpPushContext(interp, ItemList(falseBlock));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("not"); ?>
  VItem* item = InterpPop(interp);
  ItemSetBool(item, !ItemBool(item));
  ++ ListLength(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("drop"); ?>
  // ITEM DROP ->
  VList* list = InterpStack(interp);
  if (ListLength(list) < 1)
    GuruMeditation(PRIMFUN_DROP_CANNOT_DROP_FROM_EMPTY_LIST);
  -- ListLength(list);
<?php PrimFunEnd(); ?>

<?php PrimFunDef("dup"); ?>
  // ITEM DUP -> ITEM ITEM
  VList* list = InterpStack(interp);
  ItemList_Push(list, ItemList_Get(list, ListLength(list) - 1));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("2dup"); ?>
  // ITEM1 ITEM2 2DUP -> ITEM1 ITEM2 ITEM1 ITEM2
  VList* list = InterpStack(interp);
  VItem* item;
  item = ItemList_Get(list, ListLength(list) - 2);
  ItemList_Push(list, item);
  item = ItemList_Get(list, ListLength(list) - 2);
  ItemList_Push(list, item);
<?php PrimFunEnd(); ?>

<?php PrimFunDef("over"); ?>
  // ITEM1 ITEM2 OVER -> ITEM1 ITEM2 ITEM1
  VList* list = InterpStack(interp);
  ItemList_Push(list, ItemList_Get(list, ListLength(list) - 2));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("swap"); ?>
  // ITEM1 ITEM2 SWAP -> ITEM2 ITEM1
  VList* list = InterpStack(interp);
  VItem* item1 = ItemList_GetRaw(list, ListLength(list) - 1);
  VItem* item2 = ItemList_GetRaw(list, ListLength(list) - 2);
  VItem temp = *item1;
  *item1 = *item2;
  *item2 = temp;
<?php PrimFunEnd(); ?>

<?php PrimFunDef("printstack"); ?>
  Print("STACK:");
  PrintList(InterpStack(interp));
  PrintNewLine();
<?php PrimFunEnd(); ?>

<?php PrimFunsFooter(); ?>

<?php
GenerateSymbolDict("primfun_names_gen.h");
?>

<?php
function PrimFunsHeader()
{
  echo "switch (primFun) {\n";
}

function PrimFunsFooter()
{
  echo "} // End of switch\n";
}

function PrimFunDef($name)
{
  global $PrimFunCounter;
  global $PrimFunTable;

  ++ $PrimFunCounter;

  array_push(
    $PrimFunTable, 
    [
      "name" => $name,
      "id" => $PrimFunCounter,
    ]);

  echo "case $PrimFunCounter: // $name\n{\n";
}

function PrimFunEnd()
{
  echo "}\nbreak;\n";
}

function GenerateSymbolDict($filename)
{
  global $PrimFunTable;

  $contents = <<<FILEHEADER
/*
File: {$filename}

This is file is generate by genprimfuns.php
*/

void SymbolDictAddPrimFuns(VSymbolDict* dict)
{\n
FILEHEADER;

  foreach ($PrimFunTable as $primfun):
    $contents .= "  SymbolDictAddPrimFunName(dict, \"".$primfun["name"]."\");\n";
  endforeach;

  $contents .= "}\n";

  file_put_contents($filename, $contents);
}
