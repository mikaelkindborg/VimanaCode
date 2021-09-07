<?php /* Comment below is for the generated file primfuns.h */ ?>
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

<?php
$PrimFunCounter = 0;
$PrimFunTable = [];
?>
<?php PrimFunsHeader(); ?>

<?php PrimFunDefine("print"); ?>
  VItem* item = InterpPop(interp);
  PrintItem(item);
  PrintNewLine();
<?php PrimFunEnd(); ?>

<?php PrimFunDefine("setglobal"); ?>
  VItem* quotedSymbol = InterpPop(interp);
  VItem* value = InterpPop(interp);
  VIndex index = ItemSymbol(ListGet(ItemObj(quotedSymbol), 0));
  InterpSetGlobal(interp, index, value);
<?php PrimFunEnd(); ?>

<?php PrimFunDefine("def"); ?>
  VItem* funBody = InterpPop(interp);
  VItem* quotedSymbol = InterpPop(interp);
  ItemObjAsList(funBody)->type = TypeFun;
  VIndex index = ItemSymbol(ListGet(ItemObj(quotedSymbol), 0));
  InterpSetGlobal(interp, index, funBody);
<?php PrimFunEnd(); ?>

<?php PrimFunDefineWithAlias("add", "+"); ?>
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) + ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDefineWithAlias("sub", "-"); ?>
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) - ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDefineWithAlias("mult", "*"); ?>
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) * ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunDefineWithAlias("div", "/"); ?>
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) / ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunsFooter(); ?>

<?php
function PrimFunsHeader()
{
  echo "switch (primFun) {\n";
}

function PrimFunsFooter()
{
  echo "} // End of switch\n\n";

  global $PrimFunTable;

  foreach ($PrimFunTable as $primfun):
    echo "#define PRIMFUN_".$primfun["name"]." ".$primfun["id"]."\n";
  endforeach;
}

function PrimFunDefine($name)
{
  global $PrimFunCounter;
  global $PrimFunTable;

  ++ $PrimFunCounter;

  array_push($PrimFunTable, ["name" => $name, "id" => $PrimFunCounter]);

  echo "case $PrimFunCounter: // $name\n{\n";
}

function PrimFunDefineWithAlias($name, $alias)
{
  PrimFunDefine($name);
}

function PrimFunEnd()
{
  echo "}\nbreak;\n";
}
?>

<?php 
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
*/ ?>
