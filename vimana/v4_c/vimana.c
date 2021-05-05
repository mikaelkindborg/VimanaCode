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
  printf("WELCOME TO THE WONDERFUL WORLD OF VIMANA\n");
  
  Interp* interp = InterpCreate();
  InterpDefinePrimFuns(interp);
  
  //PrintLine("PRIMFUNS:");
  //ListPrintItems(interp->symbolTable, interp);
  //ListPrintItems(interp->symbolValueTable, interp);
  
  //List* list = InterpParseCode(interp, "HELLO_WORLD PRINTLN (1.7 2.2 3 + +) DO PRINTLN 1 2 +");
  
  //List* list = InterpParseCode(interp, "42 FOO SET FOO PRINTLN 888888888 FOO SET FOO PRINTLN HELLO_FOO FOO SET FOO PRINTLN");
  
  List* list = InterpParseCode(interp, "(FUN (HELLO_WORLD PRINTLN))");
  
  //printf("SYMBOL TABLE:\n");
  //ListPrintItems(interp->symbolTable, interp);

  PrintLine("PARSED LIST:");
  ListPrint(list, interp);
  PrintLine("");
  
  InterpRun(interp, list);
  
  PrintLine("PRINTING STACK:");
  ListPrintItems(interp->stack, interp);
  
  InterpFree(interp);
  PrintLine("PROGRAM ENDED\n");
}
