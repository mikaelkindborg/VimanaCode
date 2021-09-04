
switch (primFun)
{
  VItem* item1;
  VItem* item2;
  
  case 1: // print
    item1 = InterpPop(interp);
    PrintItem(item1);
    //PrintNum(ItemNumber(*item1)); 
    PrintNewLine();
    break;
  case 2: // add
    item2 = InterpPop(interp);
    item1 = InterpPop(interp);
    ItemSetNumber(item1, ItemNumber(*item1) + ItemNumber(*item2));
    InterpPush(interp, item1);
    break;
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

/*
(FIB)
  (DUP 2 < (DUP SUB1 FIB SWAP 2 - FIB +) IFFALSE) DEF

(TIMESDO) 
  (DUP ISZERO 
    (DROP DROP) 
    (SWAP DUP EVAL SWAP SUB1 TIMESDO) 
  IFELSE) DEFSPECIAL
*/

}
