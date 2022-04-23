/*
File: array.h
Author: Mikael Kindborg (mikael@kindborg.com)

Functions for growable item arrays.
*/

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
