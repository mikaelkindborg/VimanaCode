#include "base.h"
#include "list.h"
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
  printf("PRIMFUNS:\n");
  ListPrintItems(interp->symbolTable, interp);
  ListPrintItems(interp->symbolValueTable, interp);
  
  List* list = InterpParseCode(interp, "HELLO_WORLD PRINTLN (1.7 2.2 3 + +) DO PRINTLN 1 2 +");
  //List* list = InterpParseCode(interp, "42 FOO SET FOO PRINTLN 888888888 FOO SET FOO PRINTLN HELLO_FOO FOO SET FOO PRINTLN");
  
  //printf("SYMBOL TABLE:\n");
  //ListPrintItems(interp->symbolTable, interp);

  printf("PARSED LIST:\n");
  ListPrint(list, interp);
  printf("\n");
  InterpRun(interp, list);
  printf("PRINTING STACK:\n");
  ListPrintItems(interp->stack, interp);
  InterpFree(interp);
  printf("PROGRAM ENDED\n");
}
