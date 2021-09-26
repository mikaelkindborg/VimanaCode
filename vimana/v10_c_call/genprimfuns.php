<?php /* Comment below is for the generated file primfuns.h */ ?>
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

<?php
$GPrimFunCounter = -1;
$GPrimFunTable = [];
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
  // ItemListDrop(InterpStack(interp));
  InterpPop(interp);
<?php PrimFunEnd(); ?>

<?php PrimFunDef("dup"); ?>
  ItemListDup(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("2dup"); ?>
  ItemList2Dup(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("over"); ?>
  ItemListOver(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("swap"); ?>
  ItemListSwap(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("printstack"); ?>
  Print("STACK:");
  PrintList(InterpStack(interp));
  PrintNewLine();
<?php PrimFunEnd(); ?>

<?php PrimFunsFooter(); ?>

<?php 
GenerateSymbolDict("primfun_names_gen.h"); 
GeneratePrimFunTable("primfun_table_gen.h"); 
?>

<?php
function PrimFunsHeader()
{
  echo "// PRIMFUNS BEGIN ----------------------------------------------\n";
}

function PrimFunsFooter()
{
  echo "// PRIMFUNS END ------------------------------------------------";
}

function PrimFunDef($name)
{
  global $GPrimFunCounter;
  global $GPrimFunTable;

  ++ $GPrimFunCounter;

  array_push(
    $GPrimFunTable, 
    [
      "name" => $name, 
      "id" => $GPrimFunCounter,
      "fun" => "PrimFun_$GPrimFunCounter",
    ]);

  echo "// $name\nvoid PrimFun_$GPrimFunCounter(VInterp* interp, VItem* primFunItem)\n{\n";
}

function PrimFunEnd()
{
  echo "}\n";
}

function GenerateSymbolDict($filename)
{
  global $GPrimFunTable;

  $contents = <<<FILEHEADER
/*
File: {$filename}

This is file is generate by genprimfuns.php
*/

void SymbolDictAddPrimFuns(VSymbolDict* dict)
{\n
FILEHEADER;

  foreach ($GPrimFunTable as $primfun):
    $contents .= "  SymbolDictAddPrimFunName(dict, \"".$primfun["name"]."\");\n";
  endforeach;

  $contents .= "}\n";

  file_put_contents($filename, $contents);
}

function GeneratePrimFunTable($filename)
{
  global $GPrimFunTable;

  $contents = <<<FILEHEADER
/*
File: {$filename}

This is file is generate by genprimfuns.php
*/

VFunPtr GInterpPrimFunTable[] = 
{\n
FILEHEADER;

  foreach ($GPrimFunTable as $primfun):
    $contents .= "  {$primfun["fun"]},\n";
  endforeach;

  // Drop the last ",\n"
  $contents = substr($contents, 0, -2);

  $contents .= "\n};\n";

  file_put_contents($filename, $contents);
}
