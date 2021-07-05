#include "vimana.h"

// MAIN ------------------------------------------------

// Run as REPL or eval file.
int main(int numargs, char* args[])
{
  Interp* interp = InterpCreate();
  DefinePrimFuns(interp);
  
  // REPL
  if (1 == numargs)
  {
    char input[100];

    PrintLine("WELCOME TO THE WONDERFUL WORLD OF VIMANA");
    PrintLine("Type EXIT to quit program");
    
    while (TRUE)
    {
      printf(": ");
      if (!fgets(input, sizeof(input), stdin)) 
        break;
      if (StringEquals(input, "EXIT\n") || 
          StringEquals(input, "exit\n"))
      {
        PrintLine("GOODBYE");
        break;
      }
      List* list = ParseCode(interp, input);
      InterpRun(interp, list);
      printf("STACK: ");
      ListPrint(interp->stack, interp);
    }
  }
  else
  // Eval file
  if (2 == numargs)
  {
    char buf[100000];
    char* p = buf;
    int c;
    FILE* file = fopen(args[1], "r");
    if (file) 
    {
      while ((c = getc(file)) != EOF)
      {
        *p++ = c;
      }
      *p = 0;
      fclose(file);
      List* list = ParseCode(interp, buf);
      InterpRun(interp, list);
    }
  }
  else
  {
    PrintLine("Too many parameters given");
  }

  InterpFree(interp);
}

  //PrintLine("PRIMFUNS:");
  //ListPrintItems(interp->globalSymbolTable, interp);
  //ListPrintItems(interp->globalValueTable, interp);
  
  //PrintDebug("SYMBOL TABLE:\n");
  //ListPrintItems(interp->globalSymbolTable, interp);

  //PrintDebug("PARSED LIST:");
  //ListPrint(list, interp);
  
  //List* list = ParseCode(interp, "(ENTER-OPTIMIZED-MODE HELLO PRINT) OPTIMIZE EVAL");
  //List* list = ParseCode(interp, "HELLO PRINT");

  /*List* list = ParseCode(interp, 
    //"HELLOWORLD PRINT 1 2 + PRINT "
    "(FACT) (N => N ISZERO (1) (N 1 - FACT N *) IFELSE) DEFINE "
    "(TIMESDO) ((L N) => N ISZERO (L EVAL L N 1 - TIMESDO) IFFALSE) DEFINE "
    "10 FACT PRINT "
     //"(20 FACT DROP) 10000000 TIMESDO "
    );
  */
