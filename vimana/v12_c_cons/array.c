/*
File: array.h
Author: Mikael Kindborg (mikael@kindborg.com)
*/

int ArrayGet(VItem* array, int index)
{
  return array[index];
}

void ArraySet(VItem* array, int index, VItem* value)
{
  array[index] = *value;
}

VItem* ArrayNew(int size)
{
  return SysAlloc(size * sizeof(VItem));
}

void ArrayFree(VItem* array)
{
  SysFree(array);
}
