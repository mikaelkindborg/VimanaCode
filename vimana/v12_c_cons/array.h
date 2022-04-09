/*
File: item.h
Author: Mikael Kindborg (mikael@kindborg.com)
*/

#define ArrayLength(array)  ((array)->type)
#define ArrayMaxSize(array) ((array)->next)

VItem* ArrayGet(VItem* array, int index)
{
  return array + (index + 1);
}

void ArraySet(VItem* array, int index, VItem* value)
{
  array[index + 1] = *value;
}

VItem* ArrayNew(int size)
{
  VItem* array = SysAlloc((size + 1) * sizeof(VItem));
  ArrayMaxSize(array) = size;
  ArrayLength(array) = 0;
  return array;
}

VItem* ArrayGrow(VItem* array, int newSize)
{
  if (newSize >= array->data)
  {
    array = realloc(array, (newSize + 1) * sizeof(VItem));
    ArrayMaxSize(array) = newSize;
  }
  return array;
}

void ArrayFree(VItem* array)
{
  SysFree(array);
}
