/*
File: alloc.h
Author: Mikael Kindborg (mikael@kindborg.com)

Memory allocation tracking.
*/

#ifdef TRACK_MEMORY_USAGE

int GMemAllocCounter = 0;
int GMemFreeCounter = 0;

// TODO: Zero-initialize allocated buffer
void* SysAlloc(int size)
{
  ++ GMemAllocCounter;
  return malloc(size);
}

void SysFree(void* obj)
{
  ++ GMemFreeCounter; 
  free(obj);
}

void PrintMemStat()
{
  Print("SysAlloc: "); PrintIntNum(GMemAllocCounter); PrintNewLine();
  Print("SysFree:  "); PrintIntNum(GMemFreeCounter);  PrintNewLine();
}

#else

#define SysAlloc(size) malloc(size)
#define SysFree(obj) free(obj)
#define PrintMemStat()

#endif
