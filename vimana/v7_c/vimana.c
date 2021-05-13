#include "base.h"
#include "item.h"
#include "list.h"
#include "print.h"
#include "interp.h"
#include "parser.h"
#include "primfuns.h"

/****************** MAIN ******************/

int main()
{
  PrintLine("WELCOME TO THE WONDERFUL WORLD OF VIMANA");
  
  Interp* interp = InterpCreate();
  InterpDefinePrimFuns(interp);
  
  //PrintLine("PRIMFUNS:");
  //ListPrintItems(interp->symbolTable, interp);
  //ListPrintItems(interp->symbolValueTable, interp);
  
  //List* list = InterpParseCode(interp, "HELLO_WORLD PRINTLN (1.7 2.2 3 + +) DO PRINTLN 1 2 +");

  //List* list = InterpParseCode(interp, "((HELLO_WORLD PRINTLN) DO) DO");

  //List* list = InterpParseCode(interp, "(() () (HELLO_FUN_WORLD PRINTLN)) FUN HELLO SET HELLO");

  //List* list = InterpParseCode(interp, "((A) () (A PRINTLN)) FUN HELLO SET (WELCOME TO VIMANA) HELLO");

  //List* list = InterpParseCode(interp, "HELLO1 PRINTLN ((HELLO2 PRINTLN) DO) DO");

  //List* list = InterpParseCode(interp, "(HELLO_WORLD PRINTLN) LIST SET LIST DO");

  //List* list = InterpParseCode(interp, "42 21 2 * EQ (IT_IS_TRUE PRINT) IFTRUE");
  
  //List* list = InterpParseCode(interp, "41 21 2 * EQ (IT_IS_TRUE PRINT) (IT_IS_FALSE PRINT) IFELSE");

  //List* list = InterpParseCode(interp, "88 FOO SET TRUE (FOO) (42) IFELSE 2 * PRINT");

  //List* list = InterpParseCode(interp, "((A) (B) (A A + B SET  (B B + B SET) DO  B PRINTLN)) FUN DOUBLE-PRINT SET 42 DOUBLE-PRINT");
/*
  List* list = InterpParseCode(interp, 
    "(() (F) ("
    "  (() () ("
    "    HELLO_INNER_FUN PRINTLN"
    "  )) FUN F SET "
    "  F"
    ")) FUN MYFUN SET "
    "MYFUN");
*/
  //List* list = InterpParseCode(interp, "2 2 EQ PRINTLN");

  //List* list = InterpParseCode(interp, "42 FOO SET FOO PRINTLN 888888888 FOO SET FOO PRINTLN HELLO_FOO FOO SET FOO PRINTLN");
  
  //List* list = InterpParseCode(interp, "((A) (B C) (A B C HELLO PRINTLN)) FUN PRINTLN");
/*
  List* list = InterpParseCode(interp, 
    "((N) () ("
    "  N 0 EQ (1) (N 1 - FACT N *) IFELSE))"
    "FUN FACT SET "
    "3 FACT PRINT");
*/

/*
Benchmarks 210507 and 210508

((N) () (
  N 0 EQ (1) (N 1 - FACT N *) IFELSE))
  FUN FACT SET

((L N) () (
  N 0 EQ NOT (L DO L N 1 - TIMESDO) IFTRUE))
  FUN TIMESDO SET 

(20 FACT DOC) 100000 TIMESDO

20 FACT 100000 iterations:

Non-optimised code:
./vimana  2.54s user 0.01s system 85% cpu 2.964 total

Reuse of stackframes (minimize malloc):
./vimana  1.57s user 0.00s system 99% cpu 1.578 total

Inlining using macros:
./vimana  1.01s user 0.00s system 72% cpu 1.388 total

Some more macro inlining:
./vimana  0.87s user 0.00s system 99% cpu 0.880 total

With compiler optimization:
cc vimana.c -Ofast -o vimana
./vimana  0.41s user 0.00s system 49% cpu 0.830 total
./vimana  0.35s user 0.00s system 99% cpu 0.360 total

10000000 interations 20 FACT:
cc vimana.c -o vimana -O3
./vimana  38.98s user 0.07s system 98% cpu 39.635 total

After further inlining 10000000 interations 20 FACT:
cc vimana.c -o vimana -Ofast
./vimana  29.47s user 0.03s system 98% cpu 29.886 total

After padding struct Item 10000000 interations 20 FACT:
cc vimana.c -o vimana -Ofast
./vimana  26.61s user 0.02s system 95% cpu 27.992 total

Compiled code size is around 70280 bytes regardless of 
compiler optimization and macro inlining.

210508: Source code 1572 lines

*/
  List* list = InterpParseCode(interp, 
    "((N) () ("
    "  N 0 EQ (1) (N 1 - FACT N *) IFELSE))"
    "FUN FACT SET "
    "((L N) () ("
    " N 0 EQ NOT (L DO L N 1 - TIMESDO) IFTRUE))"
    "FUN TIMESDO SET "
    "(20 FACT DOC) 100000 TIMESDO");
                 //10000000

  //printf("SYMBOL TABLE:\n");
  //ListPrintItems(interp->symbolTable, interp);

  //PrintLine("PARSED LIST:");
  //ListPrint(list, interp);
  
  InterpRun(interp, list);
  
  //PrintLine("PRINTING STACK:");
  //ListPrintItems(interp->stack, interp);
  
  InterpFree(interp);
  PrintLine("PROGRAM ENDED");
}
