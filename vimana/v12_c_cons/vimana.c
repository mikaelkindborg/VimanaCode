
//#define TRACK_MEMORY_USAGE
#define OPTIMIZE 

#include "vimana.h"

// TODO: Provide hook for exiting interactive mode from Vimana
int GInteractiveMode = FALSE;

int main(int numargs, char* args[])
{
  int interativeMode = FALSE;
  char* fileName = NULL;

  if (numargs < 2)
  {
    PrintLine("-----------------------------------------------------------");
    PrintLine("Welcome to the wonderful world of Vimana");
    PrintNewLine();
    PrintLine("You can run Vimana in interactive mode with live reload of");
    PrintLine("the source file, or you can evaluate a source file once.");
    PrintNewLine();
    PrintLine("Evaluate source file once and exit (default mode):");
    PrintLine("./vimana filename");
    PrintNewLine();
    PrintLine("Start interactive mode (quit with CTRL-C):");
    PrintLine("./vimana --interactive filename");
    PrintNewLine();
    PrintLine("In interative mode the source file will be reloaded when");
    PrintLine("you save you save the file in the code editor.");
    PrintLine("Quit interative mode with CTRL-C.");
    PrintLine("-----------------------------------------------------------");
    return 0;
  }
  else
  if (numargs > 3)
  {
    PrintLine("Too many parameters given");
    return 0;
  }

  for (int i = 1; i < numargs; ++i)
  {
    if (StrEquals(args[i], "--interactive"))
    {
      GInteractiveMode = TRUE;
    }
    else
    {
      fileName = args[i];
    }
  }

  VInterp* interp = InterpNew();

  time_t lastUpdate = 0;
/*
  int sourceSize = 0;
  int prevSourceSize = 0;
*/
  do
  {
    // Read source file
    char* source = FileRead(fileName);
    if (NULL == source) 
    {
      PrintLine("Cannot read source code file");
      break;
    }

    // Parse code
    VItem* list = ParseSourceCode(source, interp->mem);
    SysFree(source);

    // Evaluate code
    InterpEval(interp, list);

    // Handle interactive mode
    if (GInteractiveMode)
    {
      printf("Interactive mode (quit with CTRL-C)\n");
      while (TRUE)
      {
        int result = FileIsModified(fileName, &lastUpdate);

        if (FILE_STAT_ERROR == result)
        {
          GInteractiveMode = FALSE;
          break;
        }

        if (FILE_STAT_MODIFIED == result)
        {
          break;
        }

        usleep(100000);
      }
    }
  }
  while (GInteractiveMode);

  InterpFree(interp);

  PrintMemStat();
}
