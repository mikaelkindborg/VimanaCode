#define TRACK_MEMORY_USAGE
#define DEBUG

#include "vimana.h"

int GNumFailedTests = 0;

void ShouldHold(char* description, int condition)
{
  if (!condition) 
  {
    ++ GNumFailedTests;
    printf("[SHOULD_HOLD] %s\n", description);
  }
}

void PrintTestResult()
{
  if (GNumFailedTests > 0)
    printf("FAILED TESTS: %i\n", GNumFailedTests);
  else
    printf("ALL TESTS PASS\n");
}

void LogTest(char* testName)
{
  PrintLine("================================================");
  PrintLine(testName);
  PrintLine("================================================");
}

void TestPrintBinary()
{
  LogTest("TestPrintBinary");

  PrintBinaryUInt(0x80000000);
  PrintBinaryUInt(0x1 << 1);
  PrintBinaryUInt(0x2 >> 1);

  PrintBinaryULong(0x80000000);
  PrintBinaryULong(0x1 << 1);
  PrintBinaryULong(0x2 >> 1);
}

void TestItemAttributes()
{
  LogTest("TestItemAttributes");

  VItem theItem;
  VItem* item = &theItem;

  ItemInit(item);
  ItemGCMarkSet(item);
  ItemSetType(item, 1);
  ItemSetNext(item, 3);
  ItemSetFirst(item, 3);

  PrintBinaryULong(item->next);
  PrintBinaryULong(item->first);

  printf("Type:  %i\n",  ItemGetType(item));
  printf("Mark:  %i\n",  ItemGetGCMark(item));
  printf("Next:  %lu\n", (unsigned long) ItemGetNext(item));
  printf("First: %lu\n", (unsigned long) ItemGetFirst(item));

  ShouldHold("TestItemAttributes: Item type should equal", 1 == ItemGetType(item));
  ShouldHold("TestItemAttributes: Item gc mark should be set", 128 == ItemGetGCMark(item));
  ShouldHold("TestItemAttributes: Item next should equal", 3 == ItemGetNext(item));
  ShouldHold("TestItemAttributes: Item first should equal", 3 == ItemGetFirst(item));
}

void TestFoo()
{
  long numbers[4] = { 1, 2, 3, 4 };

  long* n1 = numbers;
  long* n2 = numbers + 1;
  long* n4 = numbers + 3;
  long p = n4 - n2;
  long* n3 = numbers + p;

  printf("Number 1: %li\n", *n1);
  printf("Number 2: %li\n", *n2);
  printf("Number 3: %li\n", *n3);
  printf("Number 4: %li\n", *n4);

  printf("Diff ptr: %li\n", n3 - n1);

  printf("Ptr 1:    %lu\n", (unsigned long)n1);
  printf("Ptr 2:    %lu\n", (unsigned long)n2);
  printf("Ptr 3:    %lu\n", (unsigned long)n3);
  printf("Ptr 4:    %lu\n", (unsigned long)n4);
/*
  PrintBinaryULong((unsigned long)n1);
  PrintBinaryULong((unsigned long)n2);
  PrintBinaryULong((unsigned long)n3);
  PrintBinaryULong((unsigned long)n4);
*/

  PrintLine("16 bit memory layout");
  PrintBinaryUShort(4);
  PrintBinaryUShort(8);
  PrintBinaryUShort(12);
  PrintBinaryUShort(16);
  PrintBinaryUShort(1024);
  PrintBinaryUShort(1024*2);
  PrintBinaryUShort(1024*4);
  PrintBinaryUShort(1024*8);
  PrintBinaryUShort(0xFFFF);

  PrintLine("32 bit memory layout");
  PrintBinaryUInt(1024*32);
  PrintBinaryUInt(1024*256);
}

void TestMemAlloc()
{
  LogTest("TestMemAlloc");

  VMem* mem = SysAlloc(MemGetByteSize(10));
  MemInit(mem, 10);

  VItem* item;

  item = MemAlloc(mem);
  ItemGCMarkSet(item);
  ItemSetType(item, 1);
  ItemSetNext(item, 4);

  item = MemAlloc(mem);
  ItemGCMarkSet(item);
  ItemSetType(item, 1);
  ItemSetNext(item, 4);

  //PrintBinaryUInt(ItemGetNext(item));
  ShouldHold("TestMemAlloc: Next should equal", 4 == ItemGetNext(item));

  MemSweep(mem);
  MemPrintAllocCounter(mem);
  ShouldHold("TestMemAlloc: Next should equal", 2 == mem->allocCounter);

  MemSweep(mem);
  MemPrintAllocCounter(mem);
  ShouldHold("TestMemAlloc: Next should equal", 0 == mem->allocCounter);

  SysFree(mem);
}

// Helper function
VItem* AllocMaxItems(VMem* mem)
{
  // Alloc items until out of memory

  PrintLine("Alloc max items");

  VItem* first = NULL;
  VItem* item;
  VItem* next;

  while (1)
  {
    next = MemAlloc(mem);

    if (NULL == next) break;

    if (NULL == first) 
    {
      first = next;
      ItemSetIntNum(first, 1);
    }
    else
    {
      ItemSetIntNum(next, ItemGetIntNum(item) + 1);
      MemSetNext(mem, item, next);
    }

    item = next;
  }
  return first;
}

// Helper function
int CountItems(VItem* first, VMem* mem)
{
  int counter = 0;
  VItem* item = first;
  while (item)
  {
    ++ counter;
    item = MemGetNext(mem, item);
  }
  return counter;
}

// Helper function
void PrintItems(VItem* first, VMem* mem)
{
  VItem* item = first;
  while (item)
  {
    printf("%li ", ItemGetIntNum(item));
    item = MemGetNext(mem, item);
  }
  PrintNewLine();
}

void TestAllocDealloc()
{
  LogTest("TestAllocDealloc");

  VMem* mem = SysAlloc(MemGetByteSize(10));
  MemInit(mem, 10);

  PrintLine("Alloc max");
  VItem* first = AllocMaxItems(mem);
  PrintItems(first, mem);
  int numItems = CountItems(first, mem);
  printf("Num items: %i\n", numItems);

  MemSweep(mem);

  PrintLine("Alloc max again");
  first = AllocMaxItems(mem);
  PrintItems(first, mem);
  int numItems2 = CountItems(first, mem);
  printf("Num items: %i\n", numItems2);

  MemSweep(mem);

  ShouldHold("TestAllocDealloc: Allocated items should be equal", numItems == numItems2);

  MemPrintAllocCounter(mem);
  SysFree(mem);
  SysPrintMemStat();
}

void TestSetFirst()
{
  LogTest("TestSetFirst");

  VItem* first;
  VItem* item;
  VItem* next;
  VAddr addr;

  VMem* mem = SysAlloc(MemGetByteSize(10));
  MemInit(mem, 10);

  first = MemAlloc(mem);
  item = first;
  ItemSetNext(item, 1); // Using next for the item value in this test

  next = MemAlloc(mem);
  ItemSetNext(next, 2);
  addr = MemGetAddr(mem, next);
  printf("addr: %lu\n", (unsigned long) addr);
  ItemSetFirst(item, addr);
  item = next;

  next = MemAlloc(mem);
  ItemSetNext(next, 3);
  addr = MemGetAddr(mem, next);
  printf("addr: %lu\n", (unsigned long) addr);
  ItemSetFirst(item, addr);
  item = next;

  // Last item
  ItemSetFirst(item, 0);

  item = first;
  while (1)
  {
    printf("item: %lu\n", (unsigned long) ItemGetNext(item));
    if (0 == ItemGetFirst(item)) break;
    item = MemGetFirst(mem, item);
  }

  MemSweep(mem);

  ShouldHold("TestSetFirst: 0 == mem->allocCounter", 0 == mem->allocCounter);

  MemPrintAllocCounter(mem);

  SysFree(mem);
}

void TestStringItem()
{
  LogTest("TestStringItem");

  VItem* item;
  
  VMem* mem = SysAlloc(MemGetByteSize(10));
  MemInit(mem, 10);

  char* str1 = "Hi there";
  item = MemAllocHandle(mem, StrCopy(str1), TypeString);
  ItemSetType(item, TypeString);

  VItem* bufferPtrItem = MemGetFirst(mem, item);
  char* str2 = (char*) ItemGetPtr(bufferPtrItem);
  printf("String: %s\n", str2);

  ShouldHold("TestStringItem: Strings should equal", StrEquals(str1, str2));

  MemSweep(mem);

  MemPrintAllocCounter(mem);
  SysFree(mem);
}

void TestMemGetHandlePtr()
{
  LogTest("TestMemGetHandlePtr");

  VItem* item;
  
  VMem* mem = SysAlloc(MemGetByteSize(10));
  MemInit(mem, 10);

  char* str1 = "Hi there";
  item = MemAllocHandle(mem, StrCopy(str1), TypeString);
  ItemSetType(item, TypeString);

  char* str2 = MemGetHandlePtr(mem, item);
  printf("String: %s\n", str2);

  ShouldHold("TestMemGetHandlePtr: Strings should equal", StrEquals(str1, str2));

  MemSweep(mem);

  MemPrintAllocCounter(mem);
  SysFree(mem);
}

void TestArray()
{
  LogTest("TestArray");

  VItem item;

  VArray* array = ArrayNew(sizeof(VItem), 5);

  for (int i = 0; i < 20; ++ i)
  {
    item.intNum = i + 1;
    array = ArrayGrow(array, i + 5);
    VItem* p = ArrayAt(array, i);
    *p = item;
  }

  for (int i = 0; i < ArrayLength(array); ++ i)
  {
    VItem* item = ArrayAt(array, i);
    printf("value: %li\n", item->intNum);
  }

  ArrayFree(array);
}

void TestSymbolTable()
{
  LogTest("TestSymbolTable");

  char* s1 = "First";
  char* s2 = "Second";
  char* s3 = "Third";

  SymbolTableCreate();

  SymbolTableFindAdd(s1);
  SymbolTableFindAdd(s2);
  SymbolTableFindAdd(s3);

  int index = SymbolTableFindAdd(s3);
  char* s = SymbolTableGet(index);

  printf("Symbol: %i %s\n", index, s);

  SymbolTableFree();
}

void TestArrayWithStringItems()
{
  LogTest("TestArrayWithStringItems");

  // Make room for 5 string items and 5 buffer items
  VMem* mem = SysAlloc(MemGetByteSize(10));
  MemInit(mem, 10);

  VItem* item;

  VArray* array = ArrayNew(sizeof(VItem*), 5);

  char* str1 = "Hi there";

  for (int i = 0; i < 5; ++ i)
  {
    array = ArrayGrow(array, i + 1);

    item = MemAllocHandle(mem, StrCopy(str1), TypeString);
    ItemSetType(item, TypeString);

    VItem* p = ArrayAt(array, i);
    *p = *item;
  }

  for (int i = 0; i < ArrayLength(array); ++ i)
  {
    VItem* item = ArrayAt(array, i);
    //printf("Item first: %lu\n", ItemGetFirst(item));
    VItem* buffer = MemGetFirst(mem, item);
    char* str2 = (char*) ItemGetPtr(buffer);
    ShouldHold("TestArrayWithStringItems: StrEquals(str1, str2)", StrEquals(str1, str2));
    //printf("String: %s\n", str2);
  }

  ShouldHold("TestArrayWithStringItems: 10 == mem->allocCounter", 10 == mem->allocCounter);

  MemSweep(mem);

  ShouldHold("TestArrayWithStringItems: 0 == mem->allocCounter", 0 == mem->allocCounter);

  ArrayFree(array);

  SysFree(mem);
}

void TestParse()
{
  LogTest("TestParse");

  PrimFunTableCreate();
  AddCorePrimFuns();
  SymbolTableCreate();

  VInterp* interp = InterpNew();

  VItem* list;
  char*  code;

  code = "foo bar 8888";
  list = Parse(code, interp);
  PrintItem(list, interp);
  PrintNewLine();

  code = "1 2 (((3 4) 5))";
  list = Parse(code, interp);
  PrintItem(list, interp);
  PrintNewLine();

  code = "";
  list = Parse(code, interp);
  PrintItem(list, interp);
  PrintNewLine();
  ShouldHold("TestParse: List should be empty", IsEmpty(list));
  ShouldHold("TestParse: First should be 0", 0 == ItemGetFirst(list));

  code = "()";
  list = Parse(code, interp);
  PrintItem(list, interp);
  PrintNewLine();
  ShouldHold("TestParse: Child should be empty", IsEmpty(GetFirst(list, interp)));

  code = "   (( ( ) ))   ";
  list = Parse(code, interp);
  PrintItem(list, interp);
  PrintNewLine();
  ShouldHold("TestParse: Innermost should be empty", 
    IsEmpty(GetFirst(GetFirst(GetFirst(list, interp), interp), interp)));

  code = "{Hi World}";
  list = Parse(code, interp);
  PrintItem(list, interp);
  PrintNewLine();
  ShouldHold("TestParse: First should be string", IsTypeString(GetFirst(list, interp)));

  code = "{hi {bar}} foo";
  list = Parse(code, interp);
  PrintItem(list, interp);
  PrintNewLine();
  ShouldHold("TestParse: First should be string", IsTypeString(GetFirst(list, interp)));

  code = "42";
  list = Parse(code, interp);
  PrintItem(list, interp);
  PrintNewLine();
  ShouldHold("TestParse: First should be intnum", IsTypeIntNum(GetFirst(list, interp)));

  code = "42.2";
  list = Parse(code, interp);
  PrintItem(list, interp);
  PrintNewLine();
  ShouldHold("TestParse: First should be decnum", IsTypeDecNum(GetFirst(list, interp)));

  code = "42.2.3";
  list = Parse(code, interp);
  PrintItem(list, interp);
  PrintNewLine();
  ShouldHold("TestParse: First should be symbol", IsTypeSymbol(GetFirst(list, interp)));

  InterpFree(interp);
  SymbolTableFree();
  PrimFunTableFree();
}

void TestInterp()
{
  LogTest("TestInterp");

  PrimFunTableCreate();
  AddCorePrimFuns();
  SymbolTableCreate();

  VInterp* interp = InterpNew();

  VItem* list;
  char*  code;

  code = "1 2 3 1 2 3 + + + + + (SUM) setglobal SUM print";
  list = Parse(code, interp);
  PrintList(list, interp);
  PrintNewLine();

  InterpEval(interp, list);

  ShouldHold("TestInterp: callStackTop should be -1", -1 == interp->callStackTop);

  InterpFree(interp);
  SymbolTableFree();
  PrimFunTableFree();
}

int main()
{
  LogTest("Welcome to VimanaCode tests");

  TestPrintBinary();
  TestItemAttributes();
  TestFoo();
  TestMemAlloc();
  TestAllocDealloc();
  TestArray();
  TestSymbolTable();
  TestSetFirst();
  TestStringItem();
  TestMemGetHandlePtr();
  TestArrayWithStringItems();
  TestParse();
  TestInterp();

  SysPrintMemStat();
  PrintTestResult();

  printf("sizeof(VItem): %lu\n", sizeof(VItem));

  return 0;
}

// --------------------------------------------------------

/*
void TestParseSymbolicCode()
{
  VMem* mem = MemNew(1000);
  char* code = "N-42 N44 (P1 N2 (N3 (P4)) N5 N6) N0 (((N88";
  VItem* first = ParseSymbolicCode(code, mem);
  MemPrintList(mem, first);
  PrintNewLine();
  MemFree(mem);
}
*/
