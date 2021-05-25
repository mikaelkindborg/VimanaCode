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
  
  //PrintDebug("SYMBOL TABLE:\n");
  //ListPrintItems(interp->globalSymbolTable, interp);

  //PrintDebug("PARSED LIST:");
  //ListPrint(list, interp);
  

  // TIMESDO RECURSIVE VARS
  List* list1a = ParseCode(interp,
    "((L N) => N 0 EQ NOT (L EVAL L N 1 - TIMESDO) IFTRUE) (TIMESDO) DEF "
    "(HELLO DROP) 100000000 TIMESDO");
  //./vimana  10.72s user 0.01s system 97% cpu 10.957 total

  // TIMESDO RECURSIVE STACK
  List* list1b = ParseCode(interp,
    "(SWAP DUP EVAL SWAP 1 - DUP 0 EQ (DROP DROP) (TIMESDO) IFELSE) (TIMESDO) DEF "
    "(HELLO DROP) 100000000 TIMESDO");
  //./vimana  7.83s user 0.01s system 96% cpu 8.098 total

  // TIMESDO ITERATIVE VARS
  List* list2a = ParseCode(interp,
    "((L N) => L EVAL N 1 - N => "
      "N 0 EQ 2 GOTOIFFALSE) (TIMESDO) DEF "
    "(HELLO DROP) 100000000 TIMESDO");
  //./vimana  7.93s user 0.01s system 95% cpu 8.294 total

  // TIMESDO ITERATIVE STACK
  List* list2b = ParseCode(interp,
    "(SWAP DUP EVAL SWAP 1 - DUP 0 EQ 0 GOTOIFFALSE) (TIMESDO) DEF "
    "(HELLO DROP) 100000000 TIMESDO");
  //./vimana  6.45s user 0.01s system 94% cpu 6.816 total



  List* list1 = ParseCode(interp,
    //"(HELLO WORLD) PRINT " 
    "((L N) => "
      //"LOOP LABEL "
        "L EVAL "
        "N 1 -  N => "
        "N 0 EQ "
      "2 GOTOIFFALSE) "
    "(TIMESDO) DEF "
    "(DUP 1 EQ (DROP 1) (DUP 1 - FACT *) IFELSE) (FACT) DEF "
    "(20 FACT) 10000000 TIMESDO");
  // ./vimana  13.22s user 0.07s system 97% cpu 13.565 total
  // ./vimana  14.25s user 0.01s system 96% cpu 14.733 total (with DROP)
  // ./vimana  14.67s user 0.07s system 96% cpu 15.255 total (without DROP)

  List* list2 = ParseCode(interp,
    //"(HELLO WORLD) PRINT " 
    "(SWAP DUP EVAL SWAP 1 - DUP 0 EQ 0 GOTOIFFALSE) (TIMESDO) DEF "
    "(DUP 1 EQ (DROP 1) (DUP 1 - FACT *) IFELSE) (FACT) DEF "
    "(20 FACT DROP) 10000000 TIMESDO");
  // ./vimana  12.95s user 0.01s system 94% cpu 13.769 total
  // ./vimana  13.07s user 0.01s system 90% cpu 14.422 total

  List* list3 = ParseCode(interp,
    //"(HELLO WORLD) PRINT " 
    "((L N) => "
      //"LOOP LABEL "
        "L EVAL "
        "N 1 -  N => "
        "N 0 EQ "
      "2 GOTOIFFALSE) "
    "(TIMESDO) DEF "
    "((N) => "
      "1 RES => "
      "N RES * RES => "
      "N 1 - N => "
      "N 0 EQ "
      "5 GOTOIFFALSE "
      "RES 0 +) (FACT) DEF "
    "(20 FACT DROP) 10000000 TIMESDO");
  // ./vimana  19.39s user 0.01s system 97% cpu 19.805 total  !!!!!!!!

  List* list4 = ParseCode(interp,
    //"(HELLO WORLD) PRINT " 
    "(SWAP DUP EVAL SWAP 1 - DUP 0 EQ (DROP DROP) (TIMESDO) IFELSE) (TIMESDO) DEF "
    "(DUP 1 EQ (DROP 1) (DUP 1 - FACT *) IFELSE) (FACT) DEF "
    "(20 FACT DROP) 10000000 TIMESDO");
  // ./vimana  14.21s user 0.01s system 98% cpu 14.482 total


  List* list5 = ParseCode(interp,
    "(1 SWAP DUP * SWAP 1 - SWAP DUP 0 EQ 1 GOTOIFFALSE DROP) "
    "(FACT) DEF "
    "6 FACT PRINT");

/*
  N
1
  N 1
SWAP
  1 N
DUP
  1 N N

  N N RES
*
  N RES
SWAP
  RES N
1 -
  RES N
DUP
  RES N N
0 EQ
  RES N TRUE/FALSE
1 GOTOIFFALSE
DROP

1 SWAP DUP * SWAP 1 - SWAP DUP 0 EQ 1 GOTOIFFALSE DROP
*/

  InterpRun(interp, list);
  
  //PrintDebug("PRINTING STACK:");
  //ListPrintItems(interp->stack, interp);
  
  InterpFree(interp);
  //PrintLine("PROGRAM ENDED");
}



/*
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

  // Testing variable scope.
  List* list13 = ParseCode(interp, 
    "HELLO-WORLD MESSAGE : "
    "(DUP 0 EQ (DROP DROP) (SWAP DUP DO SWAP 1 - TIMESDO) IFELSE) TIMESDO DEF "
    "(MESSAGE PRINT) BLOCK : "
    "BLOCK 10 TIMESDO FOO"
    );

  // BENCHMARKS

  List* list10 = ParseCode(interp, 
    "(N : N 0 EQ (1) (N 1 - FACT N *) IFELSE) FACT DEF "
    "(N : L : N 0 EQ NOT (L DO L N 1 - TIMESDO) IFTRUE) TIMESDO DEF "
    "(20 FACT DROP) 10000000 TIMESDO"
    );
  // Pre-GC version:
  // ./vimana  18.85s user 0.01s system 97% cpu 19.266 total (OPTIMIZE + -Ofast)
  // WITH GC (cold machine):
  // ./vimana  19.02s user 0.01s system 97% cpu 19.585 total
  // WITHOUT GC (cold machine):
  // ./vimana  18.86s user 0.01s system 97% cpu 19.294 total
  // ./vimana  18.79s user 0.01s system 97% cpu 19.259 total

  List* list11 = ParseCode(interp, 
    "((N) => N 0 EQ (1) (N 1 - FACT N *) IFELSE) FACT DEF "
    "((L N) => N 0 EQ NOT (L DO L N 1 - TIMESDO) IFTRUE) TIMESDO DEF "
    "(20 FACT DROP) 10000000 TIMESDO"
    );
  // Pre-GC version:
  // ./vimana  18.99s user 0.01s system 95% cpu 19.939 total (OPTIMIZE + -Ofast)
  // WITH GC:
  // ./vimana  19.60s user 0.01s system 98% cpu 19.994 total
  // WITHOUT GC:
  // ./vimana  19.85s user 0.01s system 99% cpu 19.879 total (warm machine possibly)
  // ./vimana  19.30s user 0.01s system 98% cpu 19.644 total (setting full fan)
  // ./vimana  19.26s user 0.01s system 97% cpu 19.784 total

  List* list12 = ParseCode(interp, 
    "(DUP 1 EQ (DROP 1) (DUP 1 - FACT *) IFELSE) FACT DEF "
    "(N : L : N 0 EQ NOT (L DO L N 1 - TIMESDO) IFTRUE) TIMESDO DEF "
    "(20 FACT DROP) 10000000 TIMESDO"
    );
  // Pre-GC version:
  // ./vimana  13.80s user 0.01s system 96% cpu 14.357 total (OPTIMIZE + -Ofast)
  // ./vimana  14.53s user 0.01s system 97% cpu 14.881 total
  // WITH GC (cold machine):
  // ./vimana  14.43s user 0.01s system 93% cpu 15.462 total
  // WITHOUT GC (cold machine):
  // ./vimana  13.64s user 0.01s system 96% cpu 14.141 total

  List* list14 = ParseCode(interp, 
    "(DUP 1 EQ (DROP 1) (DUP 1 - FACT *) IFELSE) FACT DEF "
    "(DUP 0 EQ (DROP DROP) (SWAP DUP DO SWAP 1 - TIMESDO) IFELSE) TIMESDO DEF "
    "(20 FACT DROP) 10000000 TIMESDO"
    );
  // Pre-GC version (warm machine):
  // ./vimana  18.51s user 0.17s system 96% cpu 19.265 total
  // ./vimana  15.86s user 0.04s system 97% cpu 16.253 total
  // WITH GC (cold machine):
  // ./vimana  13.72s user 0.01s system 97% cpu 14.123 total
  // WITHOUT GC (cold machine):
  // ./vimana  13.17s user 0.01s system 96% cpu 13.615 total

  // Experimenting with GC.
  List* list15 = ParseCode(interp, 
    "LISTNEW L SET L PRINT 42 L SET L PRINT "
    "LISTNEW L : L PRINT 43 L : L PRINT"
    );
  List* list16 = ParseCode(interp, 
    "(A B C) L SET L PRINT 42 L SET L PRINT "
    "(D E F) L : L PRINT 43 L : L PRINT"
    );
  List* list17 = ParseCode(interp, 
    "LISTNEW DROP LISTNEW PRINT "
    );

  List* list18 = ParseCode(interp, 
    "FOO BAR SET "
    "(HELLO PRINT) (FOO) SYMBOL DEF FOO "
    "(HELLO2222 PRINT) (BAR) VALUE DEF FOO "
    );
  List* list19 = ParseCode(interp, 
    "(HELLO PRINT) : FOO DEF FOO "
    "(HELLO2222 PRINT) : FOO DEF FOO "
    );
  List* list = ParseCode(interp, 
    "(X POP  X PRINT) (FOO) DEF  HELLO2222 FOO "
    "((X Y) =>  X PRINT Y PRINT) (FOO) DEF  HELLO WORLD FOO "
    );
*/

/*
  List* list = ParseCode(interp, 
    "(Hello-I-am-the-Vimana-interpreter. PRINT "
    "(your-name) => I-can-see-that-your-name-is: PRINT "
    "your-name PRINT) say-hello DEF "
    "Mikael say-hello");

  TESTS FOR OLDER VERSION:

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
