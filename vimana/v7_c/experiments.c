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
  
  //List* list = ParseCode(interp, "(ENTER-OPTIMIZED-MODE HELLO PRINT) OPTIMIZE EVAL");
  //List* list = ParseCode(interp, "HELLO PRINT");

  List* list = ParseCode(interp, 
    //"HELLOWORLD PRINT 1 2 + PRINT "
    "(FACT) (N => N ISZERO (1) (N 1 - FACT N *) IFELSE) DEFINE "
    "(TIMESDO) ((L N) => N ISZERO (L EVAL L N 1 - TIMESDO) IFFALSE) DEFINE "
    //"10 FACT PRINT "
    "(20 FACT DROP) 10000000 TIMESDO "
    );

  // Recursve FACT and TIMESDO with variables
  List* list0 = ParseCode(interp, 
    //"HELLOWORLD PRINT "
    //"(FACT) ((N) => N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEFINE "
    //"(TIMESDO) ((L N) => N 0 EQ NOT (L EVAL L N 1 - TIMESDO) IFTRUE) DEFINE "
    //"(FACT) ((N) => N ISZERO (1) (N 1 - FACT N *) IFELSE) DEFINE "
    "(FACT) (N => N ISZERO (1) (N 1 - FACT N *) IFELSE) DEFINE "
    "(TIMESDO) ((L N) => N 0 EQ (L EVAL L N 1 - TIMESDO) IFFALSE) DEFINE "
    //"(TIMESDO) LISTFIRST VALUE OPTIMIZE "
    //"(FACT) LISTFIRST VALUE OPTIMIZE "
    //"(TIMESDO) LISTFIRST VALUE PRINT "
    //"(20 FACT DROP) OPTIMIZE 10000000 TIMESDO "
    //"(20 FACT DROP) 10000000 TIMESDO "
    //"(20 FACT DROP) 10000000 TIMESDO "
    //"(TIMESDO) LISTFIRST VALUE OPTIMIZE (TIMESDO) LISTFIRST SET "
    //"(FACT) LISTFIRST VALUE OPTIMIZE (FACT) LISTFIRST SET "
    //"(TIMESDO) LISTFIRST VALUE PRINT "
    //"(ENTER-OPTIMIZED-MODE (20 FACT DROP) 10000000 TIMESDO) OPTIMIZE EVAL"  
    "((20 FACT DROP) 10000000 TIMESDO) EVAL"  
    );
  // Test 210526
  // ./vimana  17.05s user 0.01s system 98% cpu 17.296 total
  // With linked contexts:
  // ./vimana  16.63s user 0.02s system 99% cpu 16.664 total
  // Tests 210527
  // With linked contexts:
  // ./vimana  17.78s user 0.01s system 99% cpu 17.803 total - why so slow?
  // With NO OPTIMIZE for interpreter loop tailcalls:
  // ./vimana  16.50s user 0.01s system 97% cpu 16.892 total - faster
  // ./vimana  15.96s user 0.01s system 92% cpu 17.189 total - faster
  // With OPTIMIZE for interpreter loop tailcalls:
  // ./vimana  16.98s user 0.01s system 97% cpu 17.373 total - slower
  // ./vimana  16.55s user 0.01s system 99% cpu 16.585 total - slower
  // Conclusion: The inner loop tailcall check costs more than it gains
  // Using ISZERO:
  // ./vimana  16.06s user 0.01s system 97% cpu 16.499 total
  // With N => instead of (N) =>
  // ./vimana  15.91s user 0.01s system 96% cpu 16.467 total
  // Direct function call optimization:
  // ./vimana  14.88s user 0.01s system 97% cpu 15.243 total
  // ./vimana  14.51s user 0.01s system 96% cpu 15.045 total
  // Cleanup of InterpRun:
  // ./vimana  14.35s user 0.01s system 94% cpu 15.152 total
  // ./vimana  14.14s user 0.02s system 99% cpu 14.176 total
  // WITHOUT optimised funs:
  // ./vimana  14.48s user 0.03s system 89% cpu 16.276 total
  // ./vimana  14.12s user 0.01s system 99% cpu 14.233 total
  // ./vimana  14.17s user 0.01s system 99% cpu 14.265 total
  // WITH optimised funs:
  // ./vimana  13.98s user 0.01s system 96% cpu 14.469 total
  // ./vimana  13.69s user 0.02s system 99% cpu 13.726 total
  // Removed primfun optimization:
  // ./vimana  14.62s user 0.01s system 99% cpu 14.648 total
  // Tests 210529
  // WITH rewrite of optimized funs:
  // ./vimana  13.82s user 0.03s system 99% cpu 13.898 total
  // ./vimana  13.58s user 0.01s system 94% cpu 14.370 total
  // WITHOUT OPTIMIZED MODE:
  // ./vimana  15.63s user 0.01s system 95% cpu 16.405 total

  // TIMESDO RECURSIVE, VARS
  List* list1a = ParseCode(interp,
    "((L N) => N 0 EQ NOT (L EVAL L N 1 - TIMESDO) IFTRUE) (TIMESDO) DEF "
    "(HELLO DROP) 100000000 TIMESDO");
  // Test 210525
  //./vimana  10.72s user 0.01s system 97% cpu 10.957 total
  // Test 210526
  // ./vimana  9.82s user 0.01s system 96% cpu 10.216 total
  // Test 210527
  // ./vimana  9.17s user 0.01s system 90% cpu 10.110 total
  // ./vimana  9.06s user 0.01s system 99% cpu 9.083 total

  // TIMESDO RECURSIVE, STACKOPS
  List* list1b = ParseCode(interp,
    "(SWAP DUP EVAL SWAP 1 - DUP 0 EQ (DROP DROP) (TIMESDO) IFELSE) (TIMESDO) DEF "
    "(HELLO DROP) 100000000 TIMESDO");
  // Test 210525
  //./vimana  7.83s user 0.01s system 96% cpu 8.098 total

  // TIMESDO ITERATIVE, VARS
  List* list2a = ParseCode(interp,
    "((L N) => L EVAL N 1 - N => "
      "N 0 EQ 2 GOTOIFFALSE) (TIMESDO) DEF "
    "(HELLO DROP) 100000000 TIMESDO");
  // Test 210525
  //./vimana  7.93s user 0.01s system 95% cpu 8.294 total

  // TIMESDO ITERATIVE, STACKOPS
  List* list2b = ParseCode(interp,
    "(SWAP DUP EVAL SWAP 1 - DUP 0 EQ 0 GOTOIFFALSE) (TIMESDO) DEF "
    "(HELLO DROP) 100000000 TIMESDO");
  // Test 210525
  //./vimana  6.45s user 0.01s system 94% cpu 6.816 total

  // EXPERIMEMNT WITH GOTO
  // 0 FACT NOT HANDLED!
  List* list1 = ParseCode(interp,
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
  // Test 210525
  // ./vimana  19.41s user 0.02s system 98% cpu 19.677 total  !!!!!!!!
  // Local set and lookup is slow.

  List* list2 = ParseCode(interp,
    "((L N) => "
      //"LOOP LABEL "
        "L EVAL "
        "N 1 -  N => "
        "N 0 EQ "
      "2 GOTOIFFALSE) "
    "(TIMESDO) DEF "
    "(DUP 1 EQ (DROP 1) (DUP 1 - FACT *) IFELSE) (FACT) DEF "
    "(20 FACT) 10000000 TIMESDO");
  // Test 210525
  // ./vimana  13.22s user 0.07s system 97% cpu 13.565 total
  // ./vimana  14.25s user 0.01s system 96% cpu 14.733 total (with DROP)
  // ./vimana  14.67s user 0.07s system 96% cpu 15.255 total (without DROP)

  // Iterative TIMESDO Recursive FACT
  List* list3 = ParseCode(interp,
    "(SWAP DUP EVAL SWAP 1 - DUP 0 EQ 0 GOTOIFFALSE DROP DROP) (TIMESDO) DEF "
    "(DUP 0 EQ (DROP 1) (DUP 1 - FACT *) IFELSE) (FACT) DEF "
    "(20 FACT DROP) 10000000 TIMESDO "
    //"(6 FACT PRINT) 2 TIMESDO "
    );
  // Test 210525
  // ./vimana  12.95s user 0.01s system 94% cpu 13.769 total
  // ./vimana  13.07s user 0.01s system 90% cpu 14.422 total
  // Test 210527
  // ./vimana  10.87s user 0.01s system 95% cpu 11.412 total
  // After correcting FACT for 0 FACT:
  // ./vimana  11.65s user 0.01s system 97% cpu 11.899 total

  // Recursive
  List* list4 = ParseCode(interp,
    //"(HELLO WORLD) PRINT " 
    "(SWAP DUP EVAL SWAP 1 - DUP 0 EQ "
      "(DROP DROP) (TIMESDO) IFELSE) (TIMESDO) DEF "
    "(DUP 1 EQ (DROP 1) (DUP 1 - FACT *) IFELSE) (FACT) DEF "
    "(20 FACT DROP) 10000000 TIMESDO");
  // ./vimana  14.21s user 0.01s system 98% cpu 14.482 total
  // Test 210527
  // ./vimana  10.98s user 0.01s system 92% cpu 11.932 total
  // ./vimana  10.92s user 0.01s system 99% cpu 10.950 total

  // Iterative
  // Problem here is that 0 FACT is not handled.
  List* list5 = ParseCode(interp,
    "(SWAP DUP EVAL SWAP 1 - DUP 0 EQ 0 GOTOIFFALSE DROP DROP) (TIMESDO) DEF "
    "(DUP 1 * SWAP 1 - SWAP OVER DUP 0 EQ 2 GOTOIFFALSE DROP SWAP DROP) "
    "(FACT) DEF "
    "(20 FACT DROP) 10000000 TIMESDO");
  // ./vimana  12.06s user 0.01s system 97% cpu 12.345 total
  // ./vimana  11.99s user 0.01s system 99% cpu 12.011 total
  // After optimization of currentContext:
  // ./vimana  10.44s user 0.01s system 96% cpu 10.856 total
  // With new macros for DUP/SWAP it got slower (this was 
  // probably because the machine was hot!!!):
  // ./vimana  13.94s user 0.03s system 95% cpu 14.680 total
  // Running again with presumably colder machine:
  // ./vimana  11.55s user 0.01s system 96% cpu 12.024 total
  // ./vimana  11.38s user 0.01s system 96% cpu 11.742 total
  // ./vimana  11.22s user 0.01s system 96% cpu 11.618 total
  // Test 210526 (optimizing EnterContext)
  // ./vimana  10.98s user 0.01s system 96% cpu 11.375 total
  // ./vimana  10.89s user 0.01s system 96% cpu 11.305 total
  // Test 210527 (cleanup of EnterContext) 
  // ./vimana  11.70s user 0.01s system 96% cpu 12.109 total - why slower?
  // ./vimana  10.62s user 0.01s system 99% cpu 10.638 total - faster on second run
  // Conclusion: Results not consistently reliable on macOS

  // Iterative FACT and TIMESDO with GOTOIFNOTZERO (does not handle 0 FACT)
  List* list6 = ParseCode(interp,
    "(SWAP DUP EVAL SWAP 1 - DUP 0 GOTOIFNOTZERO DROP DROP) (TIMESDO) DEF "
    "(DUP 1 * SWAP 1 - SWAP OVER DUP 2 GOTOIFNOTZERO DROP SWAP DROP) (FACT) DEF "
    //"(20 FACT DROP) 10000000 TIMESDO"
    "(TIMESDO) LISTFIRST VALUE OPTIMIZE (TIMESDO) LISTFIRST SET "
    "(FACT) LISTFIRST VALUE OPTIMIZE (FACT) LISTFIRST SET "
    "(20 FACT DROP) OPTIMIZE 10000000 TIMESDO "
    );
  // Test 210527
  // ./vimana  9.25s user 0.01s system 93% cpu 9.853 total
  // ./vimana  9.09s user 0.01s system 95% cpu 9.472 total
  // ./vimana  9.13s user 0.01s system 96% cpu 9.512 total
  // After cleanup of InterpRun: 
  // ./vimana  8.93s user 0.02s system 90% cpu 9.946 total
  // Optimized function calls:
  // ./vimana  8.74s user 0.01s system 94% cpu 9.275 total
  // ./vimana  8.23s user 0.01s system 99% cpu 8.246 total

  // Recursive FACT using EQ Iterative TIMESDO
  List* list7 = ParseCode(interp,
    "(SWAP DUP EVAL SWAP 1 - DUP 0 GOTOIFNOTZERO DROP DROP) (TIMESDO) DEF "
    "(DUP 0 EQ (DROP 1) (DUP 1 - FACT *) IFELSE) (FACT) DEF "
    "(20 FACT DROP) 10000000 TIMESDO");
  // Test 210527
  // ./vimana  11.60s user 0.01s system 96% cpu 12.030 total
  // ./vimana  11.22s user 0.01s system 99% cpu 11.240 total

  // Recursive FACT using ISZERO Iterative TIMESDO
  List* list8 = ParseCode(interp,
    "(SWAP DUP EVAL SWAP 1 - DUP 0 GOTOIFNOTZERO DROP DROP) (TIMESDO) DEF "
    "(DUP ISZERO (DROP 1) (DUP 1 - FACT *) IFELSE) (FACT) DEF "
    "(20 FACT DROP) 10000000 TIMESDO "
    //"(TIMESDO) LISTFIRST VALUE OPTIMIZE (TIMESDO) LISTFIRST SET "
    //"(FACT) LISTFIRST VALUE OPTIMIZE (FACT) LISTFIRST SET "
    //"(20 FACT DROP) OPTIMIZE 10000000 TIMESDO "
    //"(ENTER-OPTIMIZED-MODE (20 FACT DROP) 10000000 TIMESDO) OPTIMIZE EVAL" 
    );
  // Test 210527
  // ./vimana  10.88s user 0.01s system 89% cpu 12.155 total
  // After direct call optimization:
  // ./vimana  10.34s user 0.01s system 87% cpu 11.759 total
  // ./vimana  10.28s user 0.01s system 96% cpu 10.695 total
  // ./vimana  10.17s user 0.01s system 99% cpu 10.188 total
  // After cleanup of InterpRun:
  // ./vimana  10.10s user 0.01s system 95% cpu 10.534 total
  // ./vimana  9.97s user 0.01s system 99% cpu 9.996 total
  // Test 210529
  // Rewrite of optimized mode:
  // ./vimana  9.94s user 0.01s system 99% cpu 9.969 total

// https://www.forth.com/starting-forth/2-stack-manipulation-operators-arithmetic/

/*
  N
DUP
  N N
1
  N N RES (1)
LOOP: *
  N RES
SWAP
  RES N
1 -
  RES N 
SWAP OVER
  N RES N
DUP
  N RES N N
0 EQ
  N RES N TRUE/FALSE
1 GOTOIFFALSE
DROP
SWAP
DROP

DUP 1 * SWAP 1 - SWAP OVER DUP 0 EQ 2 GOTOIFFALSE DROP SWAP DROP
*/

  //PrintDebug("PARSED LIST:");
  //ListPrint(list, interp);
  
  InterpRun(interp, list);
  
  //PrintDebug("SYMBOL TABLE:\n");
  //ListPrintItems(interp->globalSymbolTable, interp);

  //PrintDebug("PRINTING STACK:");
  //PrintLine("Stack size: %i", ListLength(interp->stack));
  //ListPrintItems(interp->stack, interp);
  
  InterpFree(interp);
  PrintDebug("PROGRAM ENDED");
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


// UNUSED CODE FROM primfuns.h
// -------------------------------------------------------------

/*
  //InterpAddPrimFun("Optimize", Prim_OPTIMIZE, interp);
  //InterpAddPrimFun("Define", Prim_DEFINE, interp);
  //InterpAddPrimFun("GotoIfTrue", Prim_GOTOIFTRUE, interp);
  //InterpAddPrimFun("GotoIfFalse", Prim_GOTOIFFALSE, interp);
  //InterpAddPrimFun("GotoIfNotZero", Prim_GOTOIFNOTZERO, interp);
*/

/*
void PrimEval_EvalList(Interp* interp, List* list)
{
  // Enter new context with current env.
  InterpEnterContext(interp, list);
}
*/

/*
void PrimEval_EvalFun(Interp* interp, List* fun)
{
  // Just push it on the callstack, binding is done by primitives
  TODO: InterpEnterContext(interp, fun, ContextNewEnv);
}
*/

/*
// NUM NUM MINUS -> NUM
void Prim_MINUS(Interp* interp)
{
  Item a, b;

  InterpPopInto(interp, b);
  InterpPopInto(interp, a);

  if (IsIntNum(a) && IsIntNum(b))
  {
    a.type = TypeIntNum;
    a.value.intNum = a.value.intNum - b.value.intNum;
  }
  InterpPush(interp, a);
}

// NUM NUM TIMES -> NUM
void Prim_TIMES(Interp* interp)
{
  Item a, b, res;

  InterpPopInto(interp, b);
  InterpPopInto(interp, a);

  if (IsIntNum(a) && IsIntNum(b))
  {
    a.type = TypeIntNum;
    a.value.intNum = a.value.intNum * b.value.intNum;
  }
  InterpPush(interp, a);
}
*/

/*
// This creates a circular list structure for recursive functions,
// and therefore protection against this is added to ListPrint()
// in the form of a new type flag: TypeOptimizedFun
void Prim_Optimize_Worker(Interp* interp, List* list, List* optimizedFuns)
{
  for (int i = 0; i < ListLength(list); ++i)
  {
    Item element = ListGet(list, i);
    if (IsSymbol(element))
    {
      // Lookup symbol value
      Item item = ListGet(interp->globalValueTable, element.value.symbol);
      if (IsFun(item))
      {
        // Optimize function
        if ( ! ListContainsSymbol(optimizedFuns, element) )
        {
          // Mark function as optimized
          ListPush(optimizedFuns, element);
          Prim_Optimize_Worker(interp, ItemList(item), optimizedFuns);
        }

        // Replace symbol with function
        item.type = item.type | TypeOptimizedList;
        ListSet(list, i, item);
      }
      else
      if (IsPrimFun(item))
      {
        // Replace symbol with primfun
        ListSet(list, i, item);
      }
    }
    else
    if (IsList(element))
    {
      Prim_Optimize_Worker(interp, ItemList(element), optimizedFuns);
    }
  }
}

// Optimize simplu replaces function svymbols with actual
// function objects, to eliminate symbol lookup.
// Note that this creates a circular list structure for 
// recursive functions.
// LIST OPTIMIZE -> LIST
void Prim_OPTIMIZE(Interp* interp)
{
  // Replace function symbols with list items.
  Item item;
  InterpPopInto(interp, item);
  if (!IsList(item))
    ErrorExit("Prim_OPTIMIZE: Got a non-list!");

  // Update the list in place.
  List* optimizedFuns = ListCreate();
  Prim_Optimize_Worker(interp, ItemList(item), optimizedFuns);
  ListFree(optimizedFuns, ListFreeShallow);

  InterpPush(interp, item);

  // Direct call branch added in InterpRunOptimized()

  // TODO
  // Do non-destructive version (do this in the target language):
  // Reintroduce VALUE
  // (FOO) (...) DEFINE
  // (FOO) LISTFIRST VALUE OPTIMIZE (FOO) LISTFIRST SET
}
*/

/*
// Order is BODY NAME
// (FUNBODY) (FUNNAME) DEF ->
void Prim_DEF(Interp* interp)
{
  // (SWAP FUN SWAP FIRST SET) FUN (DEF) LISTFIRST SET
  Prim_SWAP(interp);
  Prim_FUN(interp);
  Prim_SWAP(interp);
  Prim_LISTFIRST(interp);
  Prim_SET(interp);
}

// Note that order is NAME BODY
// (FUNNAME) (FUNBODY) DEFINE ->
void Prim_DEFINE(Interp* interp)
{
  // (FUN SWAP FIRST SET) FUN (DEFINE) LISTFIRST SET
  Prim_FUN(interp);
  Prim_SWAP(interp);
  Prim_LISTFIRST(interp);
  Prim_SET(interp);
}
*/

/*

void Prim_GOTOIFTRUE(Interp* interp)
{
  Item codePointer, boolVal;

  InterpPopInto(interp, codePointer);
  InterpPopInto(interp, boolVal);

  if (!IsIntNum(codePointer))
    ErrorExit("Prim_GOTOIFTRUE: Expected TypeIntNum");
  if (!IsBool(boolVal))
    ErrorExit("Prim_GOTOIFTRUE: Expected TypeBool");

  if (boolVal.value.truth)
  {
    PrintDebug("GOTO codepointer: %li", codePointer.value.intNum);
    interp->currentContext->codePointer = codePointer.value.intNum - 1;
  }
}

void Prim_GOTOIFFALSE(Interp* interp)
{
  Item codePointer, boolVal;

  InterpPopInto(interp, codePointer);
  InterpPopInto(interp, boolVal);

  if (!IsIntNum(codePointer))
    ErrorExit("Prim_GOTOIFFALSE: Expected TypeIntNum");
  if (!IsBool(boolVal))
    ErrorExit("Prim_GOTOIFFALSE: Expected TypeBool");

  if (!boolVal.value.truth)
  {
    PrintDebug("GOTO codepointer: %li", codePointer.value.intNum);
    interp->currentContext->codePointer = codePointer.value.intNum - 1;
  }
}

void Prim_GOTOIFNOTZERO(Interp* interp)
{
  Item codePointer, intVal;

  InterpPopInto(interp, codePointer);
  InterpPopInto(interp, intVal);

  if (!IsIntNum(codePointer))
    ErrorExit("Prim_GOTOIFNOTZERO: Expected TypeIntNum (1)");
  if (!IsIntNum(intVal))
    ErrorExit("Prim_GOTOIFNOTZERO: Expected TypeIntNum (2)");

  if (0 != intVal.value.intNum)
  {
    PrintDebug("GOTO codepointer: %li", codePointer.value.intNum);
    interp->currentContext->codePointer = codePointer.value.intNum - 1;
  }
}
*/


/* TODO: Remove.
#ifndef OPTIMIZE_PRIMFUNS
      // Non-optimized primfun calls.
      // Lookup global value of symbol and check if it is a primfun.
      item = ListGet(interp->globalValueTable, element.value.symbol);
      if (IsPrimFun(item))
      {
        item.value.primFun(interp);
        goto exit;
      }
#endif
*/