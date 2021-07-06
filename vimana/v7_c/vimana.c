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

