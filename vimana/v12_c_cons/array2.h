/*
File: array.h
Author: Mikael Kindborg (mikael@kindborg.com)

Fixed size array/stack.

The idea is to have some generic support for commonly used data structures.
Like the growable array in array.h, but I am moving to using fixed size 
precallocated memory for everything.

It is probaly better to write specific code for each case...
Like the code for primfuntable that uses Array, there is a certain overhead ("boilerplate code")

These might be generic cases:

DataStack - own struct?             push/pop

GlobalVars - own struct?            add + set/get index access (array of VItem)
SymbolTable - own struct and file - add + set/get index access (array of char*)

PrimFunTable - own struct and file - add + set/get index accress (array of fun/char*)
--> make two arrays - like with global vars/symbols?

These ones are specialized:

CallStack - needs to be specific - use own struct?
ItemMemory - already in place in its own file
StringMem - is specific - make own file

-----------------------------------------------------------

// TODO: Move notes somewhere else

Debugging notes:

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
  int     itemSize;  // in bytes
  int     size;      // in bytes
}
VArray;

ArrayPush
ArrayPop
ArrayGet - return void* use for get and set

void ArrayInit(VArray* array, int itemSize, int numItems)
{
  VArray* array = SysAlloc(sizeof(VArray) + (size * itemSize));

  array->itemSize = itemSize;
  array->start = PtrOffset(array, sizeof(VArray));
  array->end = array->start;
  array->size = numItems * itemSize;
}

void ArrayPushByte8(VArray* array, Byte8 data)
{
   *((Byte8*)(array->end)) = data;
   array->end += 8;
}

void ArrayPush16Byte(VArray* array, Byte16 data)
{
   VItem* item = ArrayGet(array, index);
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
