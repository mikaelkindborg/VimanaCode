#include "base.h"
#include "list.h"
#include "print.h"
#include "interp.h"
#include "parser.h"
#include "primfuns.h"

// 2021-04-26  484 lines 

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

  List* list = InterpParseCode(interp, "((A) () (A PRINTLN)) FUN HELLO SET (WELCOME TO VIMANA) HELLO");

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
  
  //printf("SYMBOL TABLE:\n");
  //ListPrintItems(interp->symbolTable, interp);

  PrintLine("PARSED LIST:");
  ListPrint(list, interp);
  
  InterpRun(interp, list);
  
  PrintLine("PRINTING STACK:");
  ListPrintItems(interp->stack, interp);
  
  InterpFree(interp);
  PrintLine("PROGRAM ENDED");
}
