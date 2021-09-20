#include "vimana.h"

// MAIN ------------------------------------------------

// Run as REPL or eval file.
int main(int numargs, char* args[])
{
  VBool runREPL = FALSE;
  char* fileName = NULL;

  if (numargs > 4)
  {
    PrintLine("Too many parameters given");
  }

  for (int i = 1; i < numargs; ++i)
  {
    if (StrEquals(args[i], "--repl"))
    {
      runREPL = TRUE;
    }
    else
    {
      fileName = args[i];
    }
  }

  VInterp* interp = InterpCreate();
  VSymbolDict* dict = SymbolDictCreate();
  SymbolDictAddPrimFuns(dict);

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

      VList* code = ParseSourceCode(buf, dict);
      InterpRun(interp, code);
    }
  }
  else
  // REPL
  if (runREPL)
  {
    char input[100];

    PrintLine("WELCOME TO THE WONDERFUL WORLD OF VIMANA");
    PrintLine("Type EXIT to quit program");
    
    while (TRUE)
    {
      Print(": ");
      if (!fgets(input, sizeof(input), stdin)) 
        break;
      if (StrEquals(input, "EXIT\n") || 
          StrEquals(input, "exit\n"))
      {
        PrintLine("GOODBYE");
        break;
      }

      VList* code = ParseSourceCode(input, dict);
      InterpRun(interp, code);
      Print("STACK: ");
      PrintList(InterpStack(interp));
    }
  }
  else
  {
    PrintLine("------------------------------------------------------");
    PrintLine("VIMANA HELP");
    PrintLine("------------------------------------------------------");
    PrintLine("You can run Vimana in interactive mode as a REPL (Read");
    PrintLine("Eval Print Loop), or you can evaluate a source file.");
    PrintLine("Run REPL:");
    PrintLine("vimana --repl");
    PrintLine("Evaluate file:");
    PrintLine("vimana filename.vimana");
    PrintLine("------------------------------------------------------");
  }

  SymbolDictFree(dict);
  InterpFree(interp);

  PrintMemStat();
}
