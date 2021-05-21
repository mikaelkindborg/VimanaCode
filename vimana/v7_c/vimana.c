#include "vimana.h"

// MAIN ------------------------------------------------

int main()
{
  //PrintLine("WELCOME TO THE WONDERFUL WORLD OF VIMANA");

  Interp* interp = InterpCreate();
  DefinePrimFuns(interp);
  
  //PrintLine("PRIMFUNS:");
  //ListPrintItems(interp->globalSymbolTable, interp);
  //ListPrintItems(interp->globalValueTable, interp);
  
  List* list1 = ParseCode(interp, "HELLO_WORLD PRINT 1 2 + PRINT");
  List* list2 = ParseCode(interp, "(HELLO_WORLD PRINT) DO");
  List* list3 = ParseCode(interp, "(HELLO_WORLD PRINT) SAYHELLO DEF SAYHELLO");
  List* list4 = ParseCode(interp, "(X PRINT) SAYHELLO DEF HELLO_WORLD X SET SAYHELLO HELLO_AGAIN PRINT");
  List* list5 = ParseCode(interp, "HELLO_SETLOCAL_TEST X SETLOCAL X PRINT FOOBAR X SETLOCAL X PRINT");
  List* list6 = ParseCode(interp, "IT-WORKS! X SETLOCAL (X PRINT) TESTLOCAL DEF TESTLOCAL");
  List* list7 = ParseCode(interp, 
    "(N : N 0 EQ (1) (N 1 - FACT N *) IFELSE) FACT DEF 6 FACT PRINT");
  List* list8 = ParseCode(interp, 
    "((N) => N 0 EQ (1) (N 1 - FACT N *) IFELSE) FACT DEF 6 FACT PRINT");

  List* list = ParseCode(interp, 
    "(N : N 0 EQ (1) (N 1 - FACT N *) IFELSE) FACT DEF "
    "(N : L : N 0 EQ NOT (L DO L N 1 - TIMESDO) IFTRUE) TIMESDO DEF "
    "(20 FACT DROP) 10000000 TIMESDO"
    );
  // ./vimana  18.85s user 0.01s system 97% cpu 19.266 total (OPTIMIZE + -Ofast)
  // WITH GC (cold machine):
  // ./vimana  19.02s user 0.01s system 97% cpu 19.585 total
  // WITHOUT GC (cold machine):
  // ./vimana  18.89s user 0.01s system 96% cpu 19.532 total

  List* list11 = ParseCode(interp, 
    "((N) => N 0 EQ (1) (N 1 - FACT N *) IFELSE) FACT DEF "
    "((L N) => N 0 EQ NOT (L DO L N 1 - TIMESDO) IFTRUE) TIMESDO DEF "
    "(20 FACT DROP) 10000000 TIMESDO"
    );
  // ./vimana  18.99s user 0.01s system 95% cpu 19.939 total (OPTIMIZE + -Ofast)

  List* list12 = ParseCode(interp, 
    "(DUP 1 EQ (DROP 1) (DUP 1 - FACT *) IFELSE) FACT DEF "
    "(N : L : N 0 EQ NOT (L DO L N 1 - TIMESDO) IFTRUE) TIMESDO DEF "
    "(20 FACT DROP) 10000000 TIMESDO"
    );
  // ./vimana  13.80s user 0.01s system 96% cpu 14.357 total (OPTIMIZE + -Ofast)
  // ./vimana  14.53s user 0.01s system 97% cpu 14.881 total
  // WITH GC (cold machine):
  // ./vimana  14.43s user 0.01s system 93% cpu 15.462 total
  // WITHOUT GC (cold machine):
  // ./vimana  13.64s user 0.01s system 96% cpu 14.141 total

  List* list13 = ParseCode(interp, 
    "HELLO-WORLD MESSAGE : "
    "(DUP 0 EQ (DROP DROP) (SWAP DUP DO SWAP 1 - TIMESDO) IFELSE) TIMESDO DEF "
    "(MESSAGE PRINT) BLOCK : "
    "BLOCK 10 TIMESDO FOO"
    );

  List* list14 = ParseCode(interp, 
    "(DUP 1 EQ (DROP 1) (DUP 1 - FACT *) IFELSE) FACT DEF "
    "(DUP 0 EQ (DROP DROP) (SWAP DUP DO SWAP 1 - TIMESDO) IFELSE) TIMESDO DEF "
    "(20 FACT DROP) 10000000 TIMESDO"
    );
  // ./vimana  18.51s user 0.17s system 96% cpu 19.265 total
  // ./vimana  15.86s user 0.04s system 97% cpu 16.253 total

  // Experimenting with GC.
  List* list15 = ParseCode(interp, 
    "LISTNEW L SET L PRINT 42 L SET L PRINT "
    "LISTNEW L : L PRINT 43 L : L PRINT"
    );
  List* list16 = ParseCode(interp, 
    "(A B C) L SET L PRINT  42 L SET L PRINT "
    "(D E F) L : L PRINT 43 L : L PRINT"
    );
  List* list17 = ParseCode(interp, 
    "LISTNEW DROP LISTNEW PRINT "
    );



/*
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
*/

/*
Tests 210514:
./vimana  23.05s user 0.03s system 98% cpu 23.320 total (-O3 + OPTIMIZE)
After refactoring with Context struct (branch performance):
./vimana  17.79s user 0.02s system 99% cpu 17.925 total(-O3 + OPTIMIZE)
Rewrite of FACT with DUP etc:
./vimana  14.54s user 0.01s system 96% cpu 15.024 total
Rewrite also of TIMESDO with DUP and SWAP:
./vimana  15.00s user 0.02s system 99% cpu 15.061 total
./vimana  14.90s user 0.02s system 98% cpu 15.155 total
./vimana  14.56s user 0.03s system 97% cpu 14.919 total
*/

  //PrintDebug("SYMBOL TABLE:\n");
  //ListPrintItems(interp->globalSymbolTable, interp);

  //PrintDebug("PARSED LIST:");
  //ListPrint(list, interp);
  
  InterpRun(interp, list);
  
  //PrintDebug("PRINTING STACK:");
  //ListPrintItems(interp->stack, interp);
  
  InterpFree(interp);
  //PrintLine("PROGRAM ENDED");
}
