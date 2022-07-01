/*
File: array.h
Author: Mikael Kindborg (mikael@kindborg.com)

Growable arrays.
*/

typedef struct __VArray
{
  int    itemSize;
  int    length;
  int    size;
  void** buffer;
}
VArray;

VArray* ArrayNew(int itemSize, int size)
{
  VArray* array = SysAlloc(sizeof(VArray) + (size * itemSize));

  array->itemSize = itemSize;
  array->size = size;
  array->length = 0;
  array->buffer = BytePtrOffset(array, sizeof(VArray));

  return array;
}

void ArrayFree(VArray* array)
{
  SysFree(array);
}

int ArrayLength(VArray* array)
{
  return array->length;
}

void* ArrayAt(VArray* array, int index)
{
  if (index >= array->size)
  {
    GURU_MEDITATION(ARRAY_OUT_OF_BOUNDS);
  }

  if (index >= array->length)
  {
    array->length = index + 1;
  }

  return BytePtrOffset(array->buffer, array->itemSize * index);
}

VArray* ArrayGrow(VArray* array, int newLength)
{
  if (newLength >= array->size)
  {
    int newSize = newLength + 10;
    int totalSize = sizeof(VArray) + (newSize * array->itemSize);
    array = realloc(array, totalSize);
    array->size = newSize;
    array->buffer = BytePtrOffset(array, sizeof(VArray));
  }

  return array;
}

// Access a char* in the array
#define ArrayStringAt(array, index) \
  (*((char**)ArrayAt(array, index)))
