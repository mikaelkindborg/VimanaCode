
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>

typedef unsigned long VPtr;
typedef unsigned long VFlags;

typedef struct __VCell
{
  VFlags           flags; // mark bit, 0 flag = objptr, 1 flag = other type
  VPtr             data;  // data
  struct __VCell*  next;  // next cell
}
VCell;

//#define MALLOC
#define CELLMEM

#ifdef MALLOC
void MemInit()
{
}
void MemFree()
{
}
VCell* CellAlloc()
{
  return (VCell*) malloc(sizeof(VCell));
}
void CellFree(VCell* cell)
{
  free(cell);
}
#endif

#ifdef CELLMEM

VCell* CellMem;
long CellMemSize = 50000010;
long CellMemOffset = 0;
VCell* FreeList = NULL;

void MemInit()
{
  CellMem = malloc(sizeof(VCell) * CellMemSize);
}

void MemFree()
{
  free(CellMem);
}

VCell* CellAlloc()
{
  VCell* cell;
  if (FreeList)
  {
    cell = FreeList;
    FreeList = cell->next;
  }
  else
  {
    cell = CellMem + CellMemOffset;
    ++ CellMemOffset;
  }
  return cell;
}

void CellFree(VCell* cell)
{
  cell->next = FreeList;
  FreeList = cell;
}
#endif

void TestCellAlloc()
{
  VCell* cell = CellAlloc();
  cell->data = 44;
  printf("cell.data: %lu\n", cell->data);
  CellFree(cell);
}

VCell* CreateCellList()
{
  VCell* first = CellAlloc();
  VCell* cell = first;
  long i;
  for (i = 0; i < 50000000; ++i)
  {
    cell->data = i + 1;
    VCell* next = CellAlloc();
    cell->next = next;
    cell = next;
  }
  cell->data = i + 1;
  cell->next = NULL;

  return first;
}

void PrintCellList(VCell* cell)
{
  while (cell)
  {
    long n = cell->data;
    if (n % 1000000 == 0)
    {
      printf("%lu\n", cell->data);
    }
    cell = cell->next;
  }
}

void DeallocCellList(VCell* cell)
{
  while (cell)
  {
    VCell* next = cell->next;
    CellFree(cell);
    cell = next;
  }
}

void TestCellList()
{
  VCell* cell = CreateCellList();
  PrintCellList(cell);
  DeallocCellList(cell);
}

int main()
{
  printf("Hi World\n");
  MemInit();
  TestCellAlloc();
  TestCellList();
  MemFree();
}
