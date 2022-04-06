
#define TRACK_MEMORY_USAGE

#include "vimana.h"

void TestPrintBinary()
{
  PrintBinaryUInt(0x80000000);
  PrintBinaryUInt(0x1 << 1);
  PrintBinaryUInt(0x2 >> 1);
}

void TestItemAttributes()
{
  VMem* mem = MemNew(100);

  VItem* item = MemAllocItem(mem);
  ItemSetGCMark(item, 1);
  ItemSetType(item, 42);
  PrintBinaryUInt(item->type);
  printf("Type: %i\n", ItemType(item));
  printf("Mark: %i\n", ItemGCMark(item));
  ShouldHold("Item type should be equal", 42 == ItemType(item));
  ShouldHold("Item gc mark should be set", 1 == ItemGCMark(item));
  MemDeallocItem(mem, item);
  
  MemFree(mem);
}

VItem* AllocMaxItems(VMem* mem)
{
  // Alloc and cons items until out of memory
  VItem* first = MemAllocItem(mem);
  first->data = 1;
  VItem* item = first;
  while (1)
  {
    VItem* next = MemAllocItem(mem);
    if (NULL == next) break;
    next->data = item->data + 1;
    MemCons(mem, item, next);
    item = next;
  }
  return first;
}

void DeallocItems(VItem* first, VMem* mem)
{
  VItem* item = first;
  while (1)
  {
    printf("DEALLOC: %i\n", (int)item->data);
    VAddr nextAddr = item->next;
    MemDeallocItem(mem, item);
    if (0 == nextAddr) break;
    item = MemItemPointer(mem, nextAddr);
  }
}

void xPrintItems(VItem* first, VMem* mem)
{
  VAddr addr = MemItemAddr(mem, first);
  while (addr)
  {
    VItem* item = MemItemPointer(mem, addr);
    printf("%i\n", (int)item->data);
    addr = item->next;
  }
}

void PrintItems(VItem* first, VMem* mem)
{
  VItem* item = first;
  while (item)
  {
    printf("%i\n", (int)item->data);
    item = MemItemNext(mem, item);
  }
}

int CountItems(VItem* first, VMem* mem)
{
  int counter = 0;
  VAddr addr = MemItemAddr(mem, first);
  while (addr)
  {
    ++ counter;
    VItem* item = MemItemPointer(mem, addr);
    addr = item->next;
  }
  return counter;
}

void TestAllocDealloc()
{
  VMem* mem = MemNew(100);

  VItem* first = AllocMaxItems(mem);
  PrintItems(first, mem);
  int numItems = CountItems(first, mem);
  printf("Num items: %i\n", numItems);

  DeallocItems(first, mem);

  first = AllocMaxItems(mem);
  PrintItems(first, mem);
  int numItems2 = CountItems(first, mem);
  printf("Num items: %i\n", numItems2);
  DeallocItems(first, mem);

  ShouldHold("Allocated items should be equal", numItems == numItems2);

  MemFree(mem);
}

void PrintList(VItem* first, VMem* mem)
{
  printf("(");
  VAddr addr = MemItemAddr(mem, first);
  int printSpace = FALSE;
  while (addr)
  {
    if (printSpace) printf(" ");
    VItem* item = MemItemPointer(mem, addr);
    //printf("type: %i ", ItemType(item));
    if (TypeList == ItemType(item))
      PrintList(MemItemPointer(mem, ItemData(item)), mem);
    else if (TypeIntNum == ItemType(item))
      printf("N%i", (int)ItemData(item));
    else if (TypePrimFun == ItemType(item))
      printf("P%i", (int)ItemData(item));
    addr = ItemNext(item);
    printSpace = TRUE;
  }
  printf(")");
}

void TestParseSymbolicCode()
{
  VMem* mem = MemNew(1000);
  char* code = "N-42 N44 (P1 N2 (N3 (P4)) N5 N6) N0 (((N88";
  VItem* first = ParseSymbolicCode(code, mem);
  PrintList(first, mem);
  printf("\n");
  MemFree(mem);
}

void TestInterp()
{
  // Create interpreter
  int dataStackSize = 10;
  int callStackByteSize = 1000;
  int memByteSize = 1000;
  VInterp* interp = InterpNew(dataStackSize, callStackByteSize, memByteSize);

  // Test data stack
  VItem* item = MemAllocItem(interp->itemMem);
  ItemSetIntNum(item, 42);
  printf("item value: %i\n", (int)ItemData(item));

  InterpPush(interp, item);

  VItem* item2 = InterpPop(interp);
  printf("item value: %i\n", (int)ItemData(item2));

  // Tests for underflow/overflow
  // InterpPop(interp);
  // while (1) InterpPush(interp, item);

  // Test callstack
  InterpPushContext(interp, item);
  printf("item value: %i\n", (int)ItemData(interp->callStackTop->code));
  printf("item value: %i\n", (int)ItemData(interp->callStackTop->instruction));

  InterpPushContext(interp, item);
  printf("item value: %i\n", (int)ItemData(interp->callStackTop->code));
  printf("item value: %i\n", (int)ItemData(interp->callStackTop->instruction));

  InterpPopContext(interp);
  ShouldHold("CALLSTACK TOP SHOULD NOT BE NULL", NULL != interp->callStackTop);

  printf("item value: %i\n", (int)ItemData(interp->callStackTop->code));
  printf("item value: %i\n", (int)ItemData(interp->callStackTop->instruction));

  InterpPopContext(interp);
  ShouldHold("CALLSTACK TOP SHOULD BE NULL", NULL == interp->callStackTop);

  // Tests for underflow/overflow
  // InterpPopContext(interp);
  // while (1) InterpPushContext(interp, item);

  // Free interpreter
  InterpFree(interp);
}

int main()
{
  printf("Hi World\n");

  /*TestPrintBinary();
  TestItemAttributes();
  TestAllocDealloc();
  TestParseSymbolicCode();*/
  TestInterp();

  printf("DONE\n");

  PrintMemStat();
}
