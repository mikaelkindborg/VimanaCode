/*
File: alloc.h
Author: Mikael Kindborg (mikael@kindborg.com)

Memory allocation tracking.
*/

#ifdef TRACK_MEMORY_USAGE

  int GSysAllocCounter = 0;

  void SysAllocCounterIncr()
  {
    ++ GSysAllocCounter;
  }

  // TODO: Zero-initialize allocated buffer?
  void* SysAlloc(int size)
  {
    SysAllocCounterIncr();
    return malloc(size);
  }

  void SysFree(void* obj)
  {
    -- GSysAllocCounter; 
    free(obj);
  }

  void PrintMemStat()
  {
    Print("SysAllocCounter: "); 
    PrintIntNum(GSysAllocCounter); 
    PrintNewLine();
  }

#else

  #define SysAllocCounterIncr()
  #define SysAlloc(size) malloc(size)
  #define SysFree(obj) free(obj)
  #define PrintMemStat()

#endif
