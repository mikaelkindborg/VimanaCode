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
  ListPrintItems(interp->symbolTable, interp);
  
  List* list = InterpParseCode(interp, "HELLO_WORLD PRINTLN (1 2 3 + +) DO PRINTLN 1 2 +");
  printf("PARSED LIST:\n");
  ListPrint(list, interp);
  printf("\n");
  InterpRun(interp, list);
  printf("PRINTING STACK:\n");
  ListPrintItems(interp->stack, interp);
  InterpFree(interp);
  printf("PROGRAM ENDED\n");
}
