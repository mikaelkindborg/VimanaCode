/*
File: array.h
Author: Mikael Kindborg (mikael@kindborg.com)

Fixed size array/stack.
https://sourceforge.net/projects/randompausedemo/
https://books.google.dk/books?id=8A43E1UFs_YC&hl=da
https://stackoverflow.com/questions/375913/how-can-i-profile-c-code-running-on-linux/378024#378024
https://stackoverflow.com/questions/926266/performance-optimization-strategies-of-last-resort/927773#927773



https://www.developerfiles.com/debugging-c-with-clang-compiler-and-lldb/
https://lldb.llvm.org/use/tutorial.html#controlling-your-program
https://developer.apple.com/library/archive/documentation/IDEs/Conceptual/gdb_to_lldb_transition_guide/document/lldb-command-examples.html

cc -g vimana.c
lldb a.out
process launch
ctrl-c
c (continue)

*/

typedef struct __VArray
{
  VByte*  start;
  VByte*  end;
  int     itemSize;  // byte size
  int     size;      // byte size
}
VArray;

void ArrayInit(VArray* array, int itemSize, int numItems)
{
  VArray* array = SysAlloc(sizeof(VArray) + (size * itemSize));

  array->itemSize = itemSize;
  array->start = PtrOffset(array, sizeof(VArray));
  array->end = array->start;
  array->size = numItems * itemSize;
}

void ArrayPush(VArray* array, void* ptr)
{
  if (array->end < array->start + array->size)
  copy to array? 
}

void* ArrayPop(VArray* array)
{
  VByte* top = array->end;
  if (array->end < array->start + array->size) 
    GURU_MEDITATION(ARRAY_POP_EMPTY_STACK);
  array->end -= itemSize
  return array->end;
}

void* ArrayGet(VArray* array, int index)
{
  void* pos = index * array->itemSize;

  if (index >= array->size)
  {
    GURU_MEDITATION(ARRAY_OUT_OF_BOUNDS);
  }

  if (index >= array->length)
  {
    array->length = index + 1;
  }

  return PtrOffset(array->buffer, array->itemSize * index);
}

// Access a char* in the array
#define ArrayStringAt(array, index) (*((char**)ArrayAt(array, index)))
