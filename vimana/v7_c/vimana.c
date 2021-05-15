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
  

  List* list1 = ParseCode(interp, 
    "(()() (DUP 1 EQ (DROP 1) (DUP 1 - FACT *) IFELSE)) FUN FACT SET 6 FACT PRINT");

  List* list2 = ParseCode(interp, 
    "((X)()(X PRINT)) FUN HELLO SET ***WORLD*** HELLO");

  List* list3 = ParseCode(interp, 
    "((N) () ("
    "  N 0 EQ (1) (N 1 - FACT N *) IFELSE))"
    "FUN FACT SET "
    "((L N) () ("
    " N 0 EQ NOT (L DO L N 1 - TIMESDO) IFTRUE))"
    "FUN TIMESDO SET "
    "(3 FACT PRINT) 3 TIMESDO");
    //"3 FACT PRINT");
    //"(HELLO_WORLD PRINT) 3 TIMESDO");

  List* list4 = ParseCode(interp, 
    "((N) () ("
    "  N 0 EQ (1) (N 1 - FACT N *) IFELSE))"
    "FUN FACT SET "
    "((L N) () ("
    " N 0 EQ NOT (L DO L N 1 - TIMESDO) IFTRUE))"
    "FUN TIMESDO SET "
    "(20 FACT DROP) 10000000 TIMESDO");
                  //10000000

  // ./vimana  14.49s user 0.01s system 99% cpu 14.530 total (-Ofast)
  List* list5 = ParseCode(interp, 
    "(()() (DUP 1 EQ (DROP 1) (DUP 1 - FACT *) IFELSE)) FUN FACT SET "
    "((L N) () ("
    " N 0 EQ NOT (L DO L N 1 - TIMESDO) IFTRUE))"
    "FUN TIMESDO SET "
    "(20 FACT DROP) 10000000 TIMESDO");

  // ./vimana  15.09s user 0.03s system 96% cpu 15.688 total (-Ofast)
  List* list = ParseCode(interp, 
    "(()() (DUP 1 EQ (DROP 1) (DUP 1 - FACT *) IFELSE)) FUN FACT SET "
    "(() () ("
    " DUP 0 EQ (DROP DROP) (SWAP DUP DO SWAP 1 - TIMESDO) IFELSE))"
    "FUN TIMESDO SET "
    "(20 FACT DROP) 10000000 TIMESDO");
/*
Tests 210514:
./vimana  23.05s user 0.03s system 98% cpu 23.320 total (-O3 + OPTIMIZE)
After refactoring with Context struct:
./vimana  17.79s user 0.02s system 99% cpu 17.925 total(-O3 + OPTIMIZE)
Rewrite of FACT with DUP etc:
./vimana  14.54s user 0.01s system 96% cpu 15.024 total
Reprite also of TIMESDO with DUP and SWAP:
./vimana  15.00s user 0.02s system 99% cpu 15.061 total
./vimana  14.90s user 0.02s system 98% cpu 15.155 total
./vimana  14.56s user 0.03s system 97% cpu 14.919 total
*/


  //PrintDebug("SYMBOL TABLE:\n");
  //ListPrintItems(interp->symbolTable, interp);

  //PrintDebug("PARSED LIST:");
  //ListPrint(list, interp);
  
  InterpRun(interp, list);
  
  PrintDebug("PRINTING STACK:");
  ListPrintItems(interp->stack, interp);
  
  InterpFree(interp);
  PrintLine("PROGRAM ENDED");
}
