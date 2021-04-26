#include "base.h"
#include "list.h"
#include "interp.h"
#include "parser.h"

// 2021-04-26  484 lines 

/****************** MAIN ******************/

int main()
{
  printf("WELCOME TO THE WONDERFUL WORLD OF VIMANA\n");
  Interp* interp = InterpCreate();
  List* list = InterpParseCode(interp, "HELLO PRINTLN (1 2 +) DO PRINTLN");
  ListPrint(list, interp);
  printf("\n");
  InterpRun(interp, list);
  printf("STACK:\n");
  ListPrintItems(interp->stack, interp);
  InterpFree(interp);
  printf("PROGRAM ENDED\n");
}
