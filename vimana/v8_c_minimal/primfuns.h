/*
File: primfuns.h
Author: Mikael Kindborg (mikael@kindborg.com)

The idea with the macros is to be able to generate primfun 
definitions using different styles.

Symbol tables for use with the parser can be extracted using a script.
*/

#define PRIMFUNS_HEADER() switch (primFun) {
#define PRIMFUNS_FOOTER() } // End of switch

PRIMFUNS_HEADER()

#define PRIMFUN_BEGIN(name, id) case id: { 
#define PRIMFUN_END() } break;

PRIMFUN_BEGIN("print", 1)
  VItem* item = InterpPop(interp);
  PrintItem(item);
  PrintNewLine();
PRIMFUN_END()

PRIMFUN_BEGIN("+", 2)
  VItem* item2 = InterpPop(interp);
  VItem* item1 = InterpPop(interp);
  VNumber number = ItemNumber(item1) + ItemNumber(item2);
  ItemSetNumber(item1, number);
  ++ ListLength(InterpStack(interp));
PRIMFUN_END()

PRIMFUNS_FOOTER()

/*
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
*/
