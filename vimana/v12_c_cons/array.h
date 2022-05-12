/*
File: array.h
Author: Mikael Kindborg (mikael@kindborg.com)

Growable arrays.
*/

// TODO
typedef struct __VArray
{
  int   itemSize;
  int   arraySize;
  int   arrayMaxSize;
  void* arrayBuffer;
}
VArray;

// TODO: Generalize with header:
//  itemsize
//  size // array size
//  maxsize // max array size
// ArrayNew(int itemSize, int initialArraySize)
// ArrayFree = SysFree
// ArrayGrow(VArray* array, int newSize)
// void* ArrayAt(VArray* array, int index)
// Get: item = ArrayAt(items, 10);
// Set: *((VItem*)ArrayAt(items, 10)) = *item;
// void ArraySetItem(array, index) ...

// First item in the array is a header with 
// current length and max size
#define ArrayLength(array)  ((array)->type)
#define ArrayMaxSize(array) ((array)->next)

VItem* ArrayNew(int size)
{
  VItem* array = SysAlloc((size + 1) * sizeof(VItem));
  ArrayMaxSize(array) = size;
  ArrayLength(array) = 0;
  return array;
}

void ArrayFree(VItem* array)
{
  SysFree(array);
}

VItem* ArrayGet(VItem* array, int index)
{
  return array + (index + 1);
}

VItem* ArrayGrow(VItem* array, int newSize)
{
  if (newSize >= ArrayMaxSize(array))
  {
    array = realloc(array, (newSize + 1) * sizeof(VItem));
    ArrayMaxSize(array) = newSize;
  }

  return array;
}

void ArraySet(VItem* array, int index, VItem* value)
{
  if (index + 1 > ArrayLength(array))
  {
    ArrayLength(array) = index + 1;
  }
  
  array[index + 1] = *value;
}
