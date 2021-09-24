#include "vimana.h"

// MAIN ------------------------------------------------

// Run as REPL or eval file.
int main(int numargs, char* args[])
{
  VBool runREPL = FALSE;
  VBool genSym = FALSE;
  VBool evalFile = FALSE;
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
      break;
    }
    else
    if (StrEquals(args[i], "--gensym"))
    {
      if (numargs < i + 2)
      {
        PrintLine("--gensym needs a filename");
        return 0;
      } 
      genSym = TRUE;
      fileName = args[i + 1];
      break;
    }
    else
    {
      evalFile = TRUE;
      fileName = args[i];
      break;
    }
  }

  VInterp* interp = InterpCreate();
  VSymbolDict* dict = SymbolDictCreate();
  SymbolDictAddPrimFuns(dict);

  // Eval file
  if (fileName)
  {
    char buf[100000]; // Hepp.
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

      if (evalFile)
      {
        VList* code = ParseSourceCode(buf, dict);
        InterpEval(interp, code);
      }
      else
      if (genSym)
      {
        char* symbolicCode = GenerateSymbolicCode(buf, dict);
        PrintLine(symbolicCode);
        free(symbolicCode);
      }
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
      InterpEval(interp, code);
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
