#define TRACK_MEMORY_USAGE
//#define DEBUG
#define OPTIMIZE

#include "vimana.h"
//#include "socket.h"

#define MACHINE_BYTE_SIZE 30728  // Size in bytes
#define SYMBOL_TABLE_SIZE 100    // Number of symbols in symbol table
#define DATA_STACK_SIZE   100    // Number of items on the data stack
#define CALL_STACK_SIZE   100    // Number of stack frames
#define LIST_MEMORY_SIZE  1000   // Number of items in garbage collected memory

// TODO: Provide hook for exiting interactive mode from Vimana
static int GlobalInteractiveMode = FALSE;

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

  for (int i = 1; i < numargs; ++ i)
  {
    if (StrEquals(args[i], "--interactive"))
    {
      GlobalInteractiveMode = TRUE;
    }
    else
    {
      fileName = args[i];
    }
  }

  // Create the Vimana machine

  MachineAllocate(MACHINE_BYTE_SIZE);
  MachineAddCorePrimFuns();
  //SocketPrimFunsAdd();
  MachineCreate(
    SYMBOL_TABLE_SIZE, 
    DATA_STACK_SIZE, 
    CALL_STACK_SIZE, 
    LIST_MEMORY_SIZE);

#ifdef DEBUG
  MachinePrintMemoryUse();
#endif

  time_t lastUpdate = 0;
/*
  int sourceSize = 0;
  int prevSourceSize = 0;
*/
  do
  {
    // Read source file
    char* code = FileRead(fileName);
    if (NULL == code) 
    {
      PrintLine("Cannot read source code file");
      break;
    }

    // Parse source code
    VItem* list = InterpParse(MachineInterp(), code);
    SysFree(code);

    // Evaluate code
    InterpEval(MachineInterp(), list);

    // Handle interactive mode
    if (GlobalInteractiveMode)
    {
      printf("Interactive mode (quit with CTRL-C)\n");
      while (TRUE)
      {
        int result = FileIsModified(fileName, &lastUpdate);

        if (FILE_STAT_ERROR == result)
        {
          GlobalInteractiveMode = FALSE;
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
  while (GlobalInteractiveMode);

  MachineFree();

  SysPrintMemStat();

  return 0;
}
