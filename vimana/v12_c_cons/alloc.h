/*
File: alloc.h
Author: Mikael Kindborg (mikael@kindborg.com)

Memory allocation tracking.
*/

#ifdef TRACK_MEMORY_USAGE

  static int GlobalSysAllocCounter = 0;

  void SysAllocCounterIncr()
  {
    ++ GlobalSysAllocCounter;
  }

  void* SysAlloc(int size)
  {
    SysAllocCounterIncr();
    return malloc(size);
  }

  void SysFree(void* obj)
  {
    -- GlobalSysAllocCounter; 
    free(obj);
  }

  void SysPrintMemStat()
  {
    Print("SysAllocCounter: "); 
    PrintIntNum(GlobalSysAllocCounter); 
    PrintNewLine();
  }

#else

  #define SysAllocCounterIncr()
  #define SysAlloc(size) malloc(size)
  #define SysFree(obj) free(obj)
  #define SysPrintMemStat()

#endif
