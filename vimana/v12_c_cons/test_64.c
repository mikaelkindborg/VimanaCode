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
  PrintNewLine();
  PrintLine(testName);
  PrintLine("------------------------------------------------");
}

// -------------------------------------------------------------
// Tests
// -------------------------------------------------------------

void TestPointerArithmetic()
{
  LogTest("TestPointerArithmetic");

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
  PrintBinaryULong(item->next);

  ItemGCMarkSet(item);
  PrintBinaryULong(item->next);

  ItemSetType(item, TypeList);
  PrintBinaryULong(item->next);

  ShouldHold("TestItemAttributes: Item type should equal TypeList", TypeList == ItemGetType(item));

  ItemSetNext(item, 3);
  PrintBinaryULong(item->next);

  ShouldHold("TestItemAttributes: Item next should equal", 3 == ItemGetNext(item));
  
  ShouldHold("TestItemAttributes: Mark bit should be set", 0 != ItemGetGCMark(item));

  ItemSetIntNum(item, 15);

  ItemGCMarkUnset(item);
  PrintBinaryULong(item->next);

  ShouldHold("TestItemAttributes: Mark bit should be unset", 0 == ItemGetGCMark(item));

  ShouldHold("TestItemAttributes: Item type should equal TypeIntNum", TypeIntNum == ItemGetType(item));

  ShouldHold("TestItemAttributes: Item value should equal 15", 15 == ItemGetIntNum(item));

  printf("Type:  %lu\n", ItemGetType(item));
  printf("Mark:  %lu\n", ItemGetGCMark(item));
  printf("Next:  %lu\n", (unsigned long) ItemGetNext(item));
  printf("First: %lu\n", (unsigned long) ItemGetFirst(item));
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
  PrintBinaryUShort(0xFFFF);

  PrintLine("32 bit memory layout");
  PrintBinaryUInt(0xFFFF);
  PrintBinaryUInt(0xFFFFFFFF);
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
  ItemSetType(item, TypeIntNum);
  ItemSetNext(item, 0);

  item = ListMemAlloc(mem);
  ItemGCMarkSet(item);
  ItemSetType(item, TypeIntNum);
  ItemSetNext(item, 0);

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

  // Link items by first pointer

  first = ListMemAlloc(mem);
  item = first;
  ItemSetNext(item, 1); // Using next for the item value in this test

  next = ListMemAlloc(mem);
  ItemSetNext(next, 2); // Using next for the item value in this test
  ItemSetFirst(item, next);
  item = next;

  next = ListMemAlloc(mem);
  ItemSetNext(next, 3); // Using next for the item value in this test
  addr = ListMemGetAddr(mem, next);
  ItemSetFirst(item, next);
  item = next;

  // Last item
  ItemSetFirst(item, NULL);

  item = first;
  while (1)
  {
    printf("item: %lu\n", (unsigned long) ItemGetNext(item));
    if (NULL == ItemGetFirst(item)) break;
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

  code = "1 2 3 1 2 3 + + + + + (SUM) setglobal SUM dup print";
  list = MachineParse(code);

  MachinePrint(list);
  PrintNewLine();

  MachineEval(list);

  PrimFun_printstack(interp);

  ShouldHold(
    "TestMachine: top of stack should be 12", 
    12 == ItemGetIntNum(InterpStackTop(interp)));

  ShouldHold(
    "TestInterp: callStackTop should be < callStack", 
    interp->callStackTop < interp->callStack);
  
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
    "TestMachine: callStackTop should be < callStack", 
    interp->callStackTop < interp->callStack);

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
  PrintLine("WELCOME TO VIMANACODE TESTS");

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

  PrintNewLine();
  SysPrintMemStat();

  PrintNewLine();
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
*/