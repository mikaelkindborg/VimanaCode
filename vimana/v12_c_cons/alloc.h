/*
File: alloc.h
Author: Mikael Kindborg (mikael@kindborg.com)

Memory allocation tracking.
*/

#ifdef TRACK_MEMORY_USAGE

int GMemAllocCounter = 0;
int GMemFreeCounter = 0;

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
  Print("SysAlloc: "); PrintNum(GMemAllocCounter); PrintNewLine();
  Print("SysFree:  "); PrintNum(GMemFreeCounter);  PrintNewLine();
}

#else

#define SysAlloc(size) malloc(size)
#define SysFree(obj) free(obj)
#define PrintMemStat()

#endif
