/*
DO NOT USE, WORKING ON ANOTHER SOLUTION.

File: primfuns.h
Author: Mikael Kindborg (mikael@kindborg.com)

This file is generated from primfuns.php.

The idea is to generate primfun definitions using different styles.

DO NOT USE! Usage: php primfuns.php > primfuns.h 
*/

<?php
/*
function PrimFunsHeader()
{
  echo "switch (primFun) {\n";
}

function PrimFunsFooter()
{
  echo "} // End of switch\n";
}

function PrimFunBegin($name, $id)
{
  echo "case $id: {\n";
}

function PrimFunEnd()
{
  echo "} break;\n";
}
*/
?>
<?php PrimFunsHeader(); ?>

<?php PrimFunBegin("print", 1); ?>
  VItem* item = InterpPop(interp);
  PrintItem(item);
  PrintNewLine();
<?php PrimFunEnd(); ?>

<?php PrimFunBegin("+", 2); ?>
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) + ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
<?php PrimFunEnd(); ?>

<?php PrimFunsFooter(); ?>

<?php /*
  // funify
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
