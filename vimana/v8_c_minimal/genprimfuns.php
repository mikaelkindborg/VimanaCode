<?php /* Comment below is for the generated file primfuns.h */ ?>
/*
File: primfuns.h
Author: Mikael Kindborg (mikael@kindborg.com)

This file is generated from primfuns.php. The idea is to generate 
primfun definitions using different implementation styles.

Usage: php genprimfuns.php > primfuns.h 

Install PHP on Raspberry Pi:
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
  // InterpGC(interp, item);
<?php PrimFunEnd(); ?>

<?php PrimFunDef("setglobal"); ?>
  VItem* quotedSymbol = InterpPop(interp);
  VItem* value = InterpPop(interp);
  VIndex index = ItemSymbol(ListGet(ItemObj(quotedSymbol), 0));
  InterpSetGlobalVar(interp, index, value);
<?php PrimFunEnd(); ?>

<?php PrimFunDef("def"); ?>
  VItem* funBody = InterpPop(interp);
  VItem* quotedSymbol = InterpPop(interp);
  ItemObjAsList(funBody)->type = TypeFun;
  VIndex index = ItemSymbol(ListGet(ItemObj(quotedSymbol), 0));
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
  VBool booleanValue = ItemEquals(item1, item2);
  ItemSetBool(item1, booleanValue);
  ++ ListLength(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("<"); ?>
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VBool booleanValue = ItemNumber(item1) < ItemNumber(item2);
  ItemSetBool(item1, booleanValue);
  ++ ListLength(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef(">"); ?>
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VBool booleanValue = ItemNumber(item1) > ItemNumber(item2);
  ItemSetBool(item1, booleanValue);
  ++ ListLength(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("iszero"); ?>
  VItem* item = InterpPop(interp);
  VBool booleanValue = 0 == ItemNumber(item);
  ItemSetBool(item, booleanValue);
  ++ ListLength(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("eval"); ?>
  VItem* item = InterpPop(interp);
  InterpPushContext(interp, ItemList(item));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("iftrue"); ?>
  VItem* trueBlock = InterpPop(interp);
  VItem* booleanValue = InterpPop(interp);
  if (ItemBool(booleanValue))
    InterpPushContext(interp, ItemList(trueBlock));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("iffalse"); ?>
  VItem* falseBlock = InterpPop(interp);
  VItem* booleanValue = InterpPop(interp);
  if (!ItemBool(booleanValue))
    InterpPushContext(interp, ItemList(falseBlock));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("ifelse"); ?>
  VItem* falseBlock = InterpPop(interp);
  VItem* trueBlock = InterpPop(interp);
  VItem* booleanValue = InterpPop(interp);
  if (ItemBool(booleanValue))
    InterpPushContext(interp, ItemList(trueBlock));
  else
    InterpPushContext(interp, ItemList(falseBlock));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("drop"); ?>
  ListDrop(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("dup"); ?>
  ListDup(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("2dup"); ?>
  List2Dup(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("over"); ?>
  ListOver(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("swap"); ?>
  ListSwap(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDef("printstack"); ?>
  Print("STACK:");
  PrintList(InterpStack(interp));
  PrintNewLine();
<?php PrimFunEnd(); ?>

<?php PrimFunsFooter(); ?>

<?php GenerateSymbolDict("symboldict_primfuns.h"); ?>

<?php
function PrimFunsHeader()
{
  echo "switch (primFun) {\n";
}

function PrimFunsFooter()
{
  echo "} // End of switch\n";
}

function GenerateSymbolDict($filename)
{
  global $PrimFunTable;

  $contents = <<<FILEHEADER
/*
File: {$filename}

This is file is generate by genprimfuns.php
*/
\n
FILEHEADER;
/*
  foreach ($PrimFunTable as $primfun):
    $contents .= "#define PRIMFUN_".$primfun["name"]." ".$primfun["id"]."\n";
  endforeach;
*/
  $contents .= <<<FUNHEADER
void SymbolDictAddPrimFuns(VSymbolDict* dict)
{\n
FUNHEADER;

  foreach ($PrimFunTable as $primfun):
    $contents .= "  SymbolDictAddPrimFunName(dict, \"".$primfun["name"]."\");\n";
  endforeach;

  $contents .= <<<FUNFOOTER
}\n
FUNFOOTER;

  file_put_contents($filename, $contents);
}

function PrimFunDef($name)
{
  global $PrimFunCounter;
  global $PrimFunTable;

  ++ $PrimFunCounter;

  array_push($PrimFunTable, ["name" => $name, "id" => $PrimFunCounter]);

  echo "case $PrimFunCounter: // $name\n{\n";
}

function PrimFunDefWithAlias($name, $alias)
{
  PrimFunDef($name);
}

function PrimFunEnd()
{
  echo "}\nbreak;\n";
}

/*
Funify:
  VItem* item = InterpPop(interp);
  VList* list = ItemObj(item);
  list->type = TypeFun;
  ++ ListLength(InterpStack(interp));
*/

/*
  // setglobal
  // getat
  // setat
  // length
  // def
  // eval
  // sub
  // sub1
  // ifelse
  // iftrue
  // iffalse
  // iszero
  // >, <, eq
  // drop, swap, dup, over, 2dup

(FIB)
  (DUP 2 < (DUP SUB1 FIB SWAP 2 - FIB +) IFFALSE) DEF

(TIMESDO) 
  (DUP ISZERO 
    (DROP DROP) 
    (SWAP DUP EVAL SWAP SUB1 TIMESDO) 
  IFELSE) DEFSPECIAL
*/
