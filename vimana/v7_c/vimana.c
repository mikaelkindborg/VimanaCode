#include "vimana.h"

// MAIN ------------------------------------------------

// Run as REPL or eval file.
int main(int numargs, char* args[])
{
  Bool runREPL = FALSE;
  char* fileName = NULL;

  Interp* interp = InterpCreate();

  if (numargs > 4)
  {
    PrintLine("Too many parameters given");
  }

  for (int i = 1; i < numargs; ++i)
  {
    if (StrEquals(args[i], "--uppercase"))
    {
      interp->symbolCase = SymbolUpperCase;
    }
    else
    if (StrEquals(args[i], "--mixedcase"))
    {
      interp->symbolCase = SymbolMixedCase;
    }
    else
    if (StrEquals(args[i], "--lowercase"))
    {
      interp->symbolCase = SymbolLowerCase;
    }
    else
    if (StrEquals(args[i], "--repl"))
    {
      runREPL = TRUE;
    }
    else
    {
      fileName = args[i];
    }
  }

  DefinePrimFuns(interp);

  // Eval file
  if (fileName)
  {
    char buf[100000]; // Host, host (Swedish joke).
    char* p = buf;
    int c;
    FILE* file = fopen(fileName, "r");
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
      //POINTERBASED_InterpRun(interp, list);
#ifndef USE_GC // ! USE_GC
      ListFreeDeep(list);
#endif
    }
  }
  
  // REPL
  if (runREPL)
  {
    char input[100];

    PrintLine("WELCOME TO THE WONDERFUL WORLD OF VIMANA");
    PrintLine("Type EXIT to quit program");
    
    while (TRUE)
    {
      printf(": ");
      if (!fgets(input, sizeof(input), stdin)) 
        break;
      if (StrEquals(input, "EXIT\n") || 
          StrEquals(input, "exit\n"))
      {
        PrintLine("GOODBYE");
        break;
      }
      List* list = ParseCode(interp, input);
      InterpRun(interp, list);
      printf("STACK: ");
      ListPrint(interp->stack, interp);
      // We should free all parsed lists from the session,
      // but that will have to wait (a long time).
      // TODO: Alloc lists using the GC in parser.
      //#ifdef USE_GC
      // InterpGC(interp);
      //#endif
    }
  }

  InterpFree(interp);

  PrintMemStat();
}

  //PrintLine("PRIMFUNS:");
  //ListPrintItems(interp->globalSymbolTable, interp);
  //ListPrintItems(interp->globalValueTable, interp);
  
  //PrintDebug("SYMBOL TABLE:\n");
  //ListPrintItems(interp->globalSymbolTable, interp);

