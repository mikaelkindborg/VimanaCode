#define TRACK_MEMORY_USAGE
#define DEBUG

#include "vimana.h"

// -------------------------------------------------------------
// Test helpers
// -------------------------------------------------------------

static int GlobalNumFailedTests = 0;

void ShouldHold(char* description, int condition)
{
  if (!condition) 
  {
    ++ GlobalNumFailedTests;
    printf("[SHOULD_HOLD] %s\n", description);
  }
}

void PrintTestResult()
{
  if (GlobalNumFailedTests > 0)
    printf("FAILED TESTS: %i\n", GlobalNumFailedTests);
  else
    printf("ALL TESTS PASS\n");
}

void LogTest(char* testName)
{
  PrintLine("================================================");
  PrintLine(testName);
  PrintLine("================================================");
}

// -------------------------------------------------------------
// Tests
// -------------------------------------------------------------

void TestPointerArithmetic()
{
  long buf[10];
  char* c = (char*) buf;
  long* p = buf;

  printf("p start: %lu\n", (unsigned long) p);
  printf("c start: %lu\n", (unsigned long) c);

  buf[2] = 2;
  *(p + 3) = 3;

  printf("item 0: %lu\n", (unsigned long) buf[0]);
  printf("item 2: %lu\n", (unsigned long) buf[2]);
  printf("item 3: %lu\n", (unsigned long) *(p + 3));

  printf("p 3: %lu\n", (unsigned long) ((p + 3) - p));

  printf("c 3a: %lu\n", (unsigned long) ((c + 3) - (char*)p));
  printf("c 3b: %lu\n", (unsigned long) ((char*)(p + 3) - (char*)p));
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

void TestMemoryLayout()
{
  LogTest("TestMemoryLayout");

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

// Helper function
void PrintItems(VItem* first, VListMemory* mem)
{
  VAddr addr = ListMemGetAddr(mem, first);
  while (addr)
  {
    VItem* item = ListMemGet(mem, addr);
    printf("%li ", ItemGetIntNum(item));
    addr = ItemGetNext(item);
  }
  PrintNewLine();
}

void TestMemAlloc()
{
  LogTest("TestMemAlloc");

  VListMemory* mem = SysAlloc(ListMemByteSize(10));
  ListMemInit(mem, 10);

  VItem* item;

  item = ListMemAlloc(mem);
  ItemGCMarkSet(item);
  ItemSetType(item, 1);
  ItemSetNext(item, 4);

  item = ListMemAlloc(mem);
  ItemGCMarkSet(item);
  ItemSetType(item, 1);
  ItemSetNext(item, 4);

  //PrintBinaryUInt(ItemGetNext(item));
  ShouldHold("TestMemAlloc: Next should equal 4", 4 == ItemGetNext(item));

  ListMemSweep(mem);
  ListMemPrintAllocCounter(mem);
  ShouldHold("TestMemAlloc: allocCounter should equal 2", 2 == mem->allocCounter);

  ListMemSweep(mem);
  ListMemPrintAllocCounter(mem);
  ShouldHold("TestMemAlloc: allocCounter should equal 0", 0 == mem->allocCounter);

  SysFree(mem);
}

// Helper function
VItem* AllocMaxItems(VListMemory* mem)
{
  // Alloc items until out of memory

  PrintLine("Alloc max items");

  VItem* first = NULL;
  VItem* item;
  VItem* next;

  while (1)
  {
    next = ListMemAlloc(mem);

    if (NULL == next) break;

    if (NULL == first) 
    {
      first = next;
      ItemSetIntNum(first, 1);
    }
    else
    {
      ItemSetIntNum(next, ItemGetIntNum(item) + 1);
      ListMemSetNext(mem, item, next);
    }

    item = next;
  }

  return first;
}

// Helper function
int CountItems(VItem* first, VListMemory* mem)
{
  int counter = 0;
  VAddr itemAddr = ListMemGetAddr(mem, first);
  while (itemAddr)
  {
    VItem* item = ListMemGet(mem, itemAddr);
    itemAddr = ItemGetNext(item);
    ++ counter;
  }
  return counter;
}

void TestAllocDealloc()
{
  LogTest("TestAllocDealloc");

  VListMemory* mem = SysAlloc(ListMemByteSize(10));
  ListMemInit(mem, 10);

  PrintLine("Alloc max");
  VItem* first = AllocMaxItems(mem);
  //PrintItems(first, mem);
  int numItems = CountItems(first, mem);
  printf("Num items: %i\n", numItems);

  ListMemSweep(mem);

  PrintLine("Alloc max again");
  first = AllocMaxItems(mem);
  PrintItems(first, mem);
  int numItems2 = CountItems(first, mem);
  printf("Num items: %i\n", numItems2);

  ListMemSweep(mem);

  PrintLine("Alloc max yet again");
  first = AllocMaxItems(mem);
  PrintItems(first, mem);
  int numItems3 = CountItems(first, mem);
  printf("Num items: %i\n", numItems3);

  ListMemSweep(mem);

  ShouldHold("TestAllocDealloc: Allocated items should be equal", numItems == numItems3);

  ListMemPrintAllocCounter(mem);

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

  VListMemory* mem = SysAlloc(ListMemByteSize(10));
  ListMemInit(mem, 10);

  first = ListMemAlloc(mem);
  item = first;
  ItemSetNext(item, 1); // Using next for the item value in this test

  next = ListMemAlloc(mem);
  ItemSetNext(next, 2);
  addr = ListMemGetAddr(mem, next);
  printf("addr: %lu\n", (unsigned long) addr);
  ItemSetFirst(item, addr);
  item = next;

  next = ListMemAlloc(mem);
  ItemSetNext(next, 3);
  addr = ListMemGetAddr(mem, next);
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
    item = ListMemGetFirst(mem, item);
  }

  ListMemSweep(mem);

  ShouldHold("TestSetFirst: 0 == mem->allocCounter", 0 == mem->allocCounter);

  ListMemPrintAllocCounter(mem);

  SysFree(mem);
}

void TestMemGetHandlePtr()
{
  LogTest("TestMemGetHandlePtr");

  VItem* item;
  
  VListMemory* mem = SysAlloc(ListMemByteSize(10));
  ListMemInit(mem, 10);

  char* str1 = "Hi there";
  item = ListMemAllocHandle(mem, StrCopy(str1), TypeString);
  ItemSetType(item, TypeString);

  char* str2 = ListMemGetHandlePtr(mem, item);
  printf("String: %s\n", str2);

  ShouldHold("TestMemGetHandlePtr: Strings should equal", StrEquals(str1, str2));

  ListMemSweep(mem);

  ListMemPrintAllocCounter(mem);

  SysFree(mem);
}

void TestStringItem()
{
  LogTest("TestStringItem");

  VItem* item;
  
  VListMemory* mem = SysAlloc(ListMemByteSize(10));
  ListMemInit(mem, 10);

  char* str1 = "Hi there";
  item = ListMemAllocHandle(mem, StrCopy(str1), TypeString);
  ItemSetType(item, TypeString);

  VItem* bufferPtrItem = ListMemGetFirst(mem, item);
  char* str2 = (char*) ItemGetPtr(bufferPtrItem);
  printf("String: %s\n", str2);

  ShouldHold("TestStringItem: Strings should equal", StrEquals(str1, str2));

  ListMemSweep(mem);

  ListMemPrintAllocCounter(mem);

  SysFree(mem);
}

void TestArrayWithStringItems()
{
  LogTest("TestArrayWithStringItems");

  // Make room for 5 string items and 5 buffer items
  VListMemory* mem = SysAlloc(ListMemByteSize(10));
  ListMemInit(mem, 10);

  VItem* array[5];
  VItem* item;

  char* str1 = "Hi there";

  for (int i = 0; i < 5; ++ i)
  {
    item = ListMemAllocHandle(mem, StrCopy(str1), TypeString);
    ItemSetType(item, TypeString);
    array[i] = item;
  }

  for (int i = 0; i < 5; ++ i)
  {
    VItem* item = array[i];
    //printf("Item first: %lu\n", ItemGetFirst(item));
    VItem* buffer = ListMemGetFirst(mem, item);
    char* str2 = (char*) ItemGetPtr(buffer);
    ShouldHold("TestArrayWithStringItems: StrEquals(str1, str2)", StrEquals(str1, str2));
    //printf("String: %s\n", str2);
  }

  ShouldHold("TestArrayWithStringItems: 10 == mem->allocCounter", 10 == mem->allocCounter);

  ListMemSweep(mem);

  ListMemPrintAllocCounter(mem);

  ShouldHold("TestArrayWithStringItems: 0 == mem->allocCounter", 0 == mem->allocCounter);

  SysFree(mem);
}

void TestSymbolTable()
{
  LogTest("TestSymbolTable");

  char* s1 = "First";
  char* s2 = "Second";
  char* s3 = "Third";

  int index;

  void* mem = SysAlloc(SymbolTableByteSize(3));

  SymbolTableInit(mem, 3);

  SymbolTableAdd(s1);
  SymbolTableAdd(s2);
  index = SymbolTableAdd(s3);

  ShouldHold("TestSymbolTable: last add index should equal 2", 2 == index);

  index = SymbolTableFind(s3);
  char* s = SymbolTableGet(index);

  ShouldHold("TestSymbolTable: find index should equal 2", 2 == index);
  ShouldHold("TestSymbolTable: symbols shold equal", StrEquals(s3, s));

  printf("Symbol: %i %s\n", index, s);

  SysFree(mem);
}

void TestSymbolMemory()
{
  LogTest("TestSymbolMemory");

  void* mem = SysAlloc(SymbolMemByteSize(100));

  SymbolMemInit(mem, 100);

  char* s1 = SymbolMemGetNextFree(mem);
  SymbolMemWriteChar(mem, 'H');
  SymbolMemWriteChar(mem, 'I');
  SymbolMemWriteFinish(mem);

  char* s2 = SymbolMemGetNextFree(mem);
  SymbolMemWriteChar(mem, 'H');
  SymbolMemWriteChar(mem, 'I');
  SymbolMemResetPos(mem);
  SymbolMemWriteChar(mem, ' ');
  SymbolMemWriteChar(mem, 'W');
  SymbolMemWriteChar(mem, 'O');
  SymbolMemWriteChar(mem, 'R');
  SymbolMemWriteChar(mem, 'L');
  SymbolMemWriteChar(mem, 'D');
  SymbolMemWriteFinish(mem);

  ShouldHold("TestSymbolMemory: s1 shold equal", StrEquals(s1, "HI"));
  ShouldHold("TestSymbolMemory: s2 shold equal", StrEquals(s2, " WORLD"));

  printf("s1: %s%s\n", s1 , s2);

  SysFree(mem);
}

// Helper function
void CreateMachine()
{
  MachineAllocate(3992);
  MachineAddCorePrimFuns();
  MachineCreate(
    12, // symbolTableSize
    10, // dataStackSize
    10, // callStackSize
    100 // listMemorySize
    );
}

void TestMachineCreate()
{
  LogTest("TestMachineCreate");

  CreateMachine();

  MachinePrintMemoryUse();
  //MachineEval("42 print");

  MachineFree();
}

void TestParse()
{
  LogTest("TestParse");

  CreateMachine();

  VInterp* interp = MachineInterp();
  VItem* list;
  char*  code;

  code = "foo bar 8888";
  list = MachineParse(code);
  MachinePrint(list);
  PrintNewLine();

  code = "1 2 (((3 4) 5))";
  list = MachineParse(code);
  MachinePrint(list);
  PrintNewLine();

  code = "";
  list = MachineParse(code);
  MachinePrint(list);
  PrintNewLine();
  ShouldHold("TestParse: List should be empty", IsEmpty(list));
  ShouldHold("TestParse: First should be 0", 0 == ItemGetFirst(list));

  code = "()";
  list = MachineParse(code);
  MachinePrint(list);
  PrintNewLine();
  ShouldHold("TestParse: Child should be empty", IsEmpty(GetFirst(list, interp)));

  code = "   (( ( ) ))   ";
  list = MachineParse(code);
  MachinePrint(list);
  PrintNewLine();
  ShouldHold("TestParse: Innermost should be empty", 
    IsEmpty(GetFirst(GetFirst(GetFirst(list, interp), interp), interp)));

  code = "{Hi World}";
  list = MachineParse(code);
  MachinePrint(list);
  PrintNewLine();
  ShouldHold("TestParse: First should be string", IsTypeString(GetFirst(list, interp)));

  code = "{hi {bar}} foo";
  list = MachineParse(code);
  MachinePrint(list);
  PrintNewLine();
  ShouldHold("TestParse: First should be string", IsTypeString(GetFirst(list, interp)));

  code = "42";
  list = MachineParse(code);
  MachinePrint(list);
  PrintNewLine();
  ShouldHold("TestParse: First should be intnum", IsTypeIntNum(GetFirst(list, interp)));

  code = "42.2";
  list = MachineParse(code);
  MachinePrint(list);
  PrintNewLine();
  ShouldHold("TestParse: First should be decnum", IsTypeDecNum(GetFirst(list, interp)));

  code = "42.2.3";
  list = MachineParse(code);
  MachinePrint(list);
  PrintNewLine();
  ShouldHold("TestParse: First should be symbol", IsTypeSymbol(GetFirst(list, interp)));

  MachineFree();
}

void TestInterp()
{
  LogTest("TestInterp");

  CreateMachine();

  VInterp* interp = MachineInterp();
  VItem* list;
  char*  code;

  code = "1 2 3 1 2 3 + + + + + (SUM) setglobal SUM print";
  list = MachineParse(code);
  MachinePrint(list);
  PrintNewLine();

  MachineEval(list);

  PrimFun_printstack(interp);

  ShouldHold("TestInterp: callStackTop should be -1", -1 == interp->callStackTop);

  MachineFree();
}

void TestMachine()
{
  LogTest("TestMachine");

  CreateMachine();

  VInterp* interp = MachineInterp();
  char*  code;

  code = "(foo) (bar) def (foo) ({foo} print) def foo";
  MachineEvalString(code);

  PrimFun_printstack(interp);

  MachineFree();
}

void TestMachineX()
{
  LogTest("TestMachine");

  CreateMachine();

  VInterp* interp = MachineInterp();
  char*  code;

  code = "(fib) (dup 1 > (dup 1- fib swap 2- fib +) iftrue) def 5 fib";
  MachineEvalString(code);

  ShouldHold(
    "TestMachine: callStackTop should be -1", 
    -1 == interp->callStackTop);

  PrimFun_printstack(interp);

  ShouldHold(
    "TestMachine: top of stack should be 5", 
    5 == ItemGetIntNum(InterpStackTop(interp)));

  MachineFree();
}

// -------------------------------------------------------------
// Main
// -------------------------------------------------------------

int main()
{
  LogTest("Welcome to VimanaCode tests");

  TestPointerArithmetic();
  TestPrintBinary();
  TestItemAttributes();
  TestMemoryLayout();
  TestMemAlloc();
  TestAllocDealloc();
  TestSetFirst();
  TestMemGetHandlePtr();
  TestStringItem();
  TestArrayWithStringItems();
  TestSymbolTable();
  TestSymbolMemory();
  TestMachineCreate();
  TestParse();
  TestInterp();
  TestMachine();
  TestMachineX();

  SysPrintMemStat();
  PrintTestResult();

  return 0;
}

// --------------------------------------------------------

/*
/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types/_null.h:30:15: note: expanded from macro 'NULL'
#define NULL  __DARWIN_NULL
              ^~~~~~~~~~~~~
/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk/usr/include/sys/_types.h:52:23: note: expanded from macro '__DARWIN_NULL'
#define __DARWIN_NULL ((void *)0)

void TestParseSymbolicCode()
{
  VListMemory* mem = ListMemNew(1000);
  char* code = "N-42 N44 (P1 N2 (N3 (P4)) N5 N6) N0 (((N88";
  VItem* first = ParseSymbolicCode(code, mem);
  ListMemPrintList(mem, first);
  PrintNewLine();
  ListMemFree(mem);
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
*/