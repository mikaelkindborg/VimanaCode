
#define TRACK_MEMORY_USAGE
#define DEBUG

#include "vimana.h"

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
  ItemSetGCMark(item, 1);
  ItemSetType(item, 1);
  ItemSetNext(item, 3);
  ItemSetFirst(item, 3);

  PrintBinaryULong(item->next);
  PrintBinaryULong(item->first);

  printf("Type:  %i\n",  ItemGetType(item));
  printf("Mark:  %i\n",  ItemGetGCMark(item));
  printf("Next:  %lu\n", ItemGetNext(item));
  printf("First: %lu\n", ItemGetFirst(item));

  ShouldHold("Item type should equal", 1 == ItemGetType(item));
  ShouldHold("Item gc mark should be set", 1 == ItemGetGCMark(item));
  ShouldHold("Item next should equal", 3 == ItemGetNext(item));
  ShouldHold("Item first should equal", 3 == ItemGetFirst(item));
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

  VItem* item = MemAlloc(mem);
  ItemSetGCMark(item, 1);
  ItemSetType(item, 1);
  ItemSetNext(item, 4);
  PrintBinaryUInt(item->next);
  MemDeallocItem(mem, item);
  
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

  ShouldHold("Allocated items should be equal", numItems == numItems2);

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
  printf("addr: %lu\n", addr);
  ItemSetFirst(item, addr);
  item = next;

  next = MemAlloc(mem);
  ItemSetNext(next, 3);
  addr = MemGetAddr(mem, next);
  printf("addr: %lu\n", addr);
  ItemSetFirst(item, addr);
  item = next;

  // Last item
  ItemSetFirst(item, 0);

  item = first;
  while (1)
  {
    printf("item: %lu\n", ItemGetNext(item));
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
  char* str2 = (char*) ItemGetFirst(bufferPtrItem);
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
    char* str2 = (char*) ItemGetFirst(buffer);
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

/*
  char* code = "1 2 (((3 4) 5))";
  list = Parse(code, interp);
  PrintItem(list, interp);
  PrintNewLine();

  char* empty = "";
  list = Parse(empty, interp);
  PrintItem(list, interp);
  PrintNewLine();
  //ShouldHold("first should be NULL", NULL == first);

  char* empty2 = "()";
  list = Parse(empty2, interp);
  PrintItem(list, interp);
  PrintNewLine();

  char* empty3 = "   (( ( ) ))   ";
  list = Parse(empty3, interp);
  PrintItem(list, interp);
  PrintNewLine();

  char* string = "{Hi World}";
  list = Parse(string, interp);
  PrintItem(list, interp);
  PrintNewLine();

  char* string2 = "({Hi foo}) {foo bar}";
  list = Parse(string2, interp);
  PrintItem(list, interp);
  PrintNewLine();

  char* code2 = "1 2 3.33 foo bar bar sayHi";
  list = Parse(code2, interp);
  PrintItem(list, interp);
  PrintNewLine();
*/
  InterpFree(interp);
  SymbolTableFree();
  PrimFunTableFree();
}

int main()
{
  LogTest("Welcome to VimanaCode tests");

  /*TestPrintBinary();
  TestItemAttributes();
  TestFoo();
  TestMemAlloc();
  TestAllocDealloc();
  TestArray();
  TestSymbolTable();
  TestSetFirst();
  TestStringItem();
  TestMemGetHandlePtr();
  TestArrayWithStringItems();*/
  TestParse();

  SysPrintMemStat();

  PrintNumFailedTests();

  return 0;
}

// --------------------------------------------------------

#ifdef FOOBAR

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

void TestInterp()
{
  LogTest("TestInterp");

  VInterp* interp = InterpNew();

  // Test data stack
  VItem* item = MemAlloc(interp->mem);
  ItemSetIntNum(item, 42);
  printf("item value 1: %li\n", item->intNum);

  InterpStackPush(interp, item);
  VItem* item2 = InterpStackPop(interp);
  printf("item value 2: %li\n", item2->intNum);

  // Tests for underflow/overflow
  // InterpStackPop(interp);
  // while (1) InterpStackPush(interp, *item);

  // Test callstack
  InterpPushEvalStackFrame(interp, item);
  printf("code : %li\n", interp->callStackTop->code->intNum);
  printf("instr: %li\n", interp->callStackTop->instruction->intNum);

  InterpPushEvalStackFrame(interp, item);
  printf("code : %li\n", interp->callStackTop->code->intNum);
  printf("instr: %li\n", interp->callStackTop->instruction->intNum);

  InterpPopContext(interp);
  ShouldHold("CALLSTACK TOP SHOULD NOT BE NULL", NULL != interp->callStackTop);

  printf("code : %li\n", interp->callStackTop->code->intNum);
  printf("instr: %li\n", interp->callStackTop->instruction->intNum);

  InterpPopContext(interp);
  ShouldHold("CALLSTACK TOP SHOULD BE NULL", NULL == interp->callStackTop);

  // Tests for underflow/overflow
  // InterpPopContext(interp);
  // while (1) InterpPushEvalStackFrame(interp, item);

  // Free interpreter
  InterpFree(interp);
}

void TestInterpEval()
{
  LogTest("TestInterpEval");

  VInterp* interp = InterpNew();

  char* source = "1 2 3 sayHi 1 2 3 + + + + + (SUM)setglobal SUM print";
  VItem* code = ParseSourceCode(source, interp->mem);
  MemPrintList(interp->mem, code);
  PrintNewLine();

  InterpEval(interp, code);

  ShouldHold("CALLSTACK TOP SHOULD BE NULL", NULL == interp->callStackTop);

  InterpFree(interp);
}

void TestInterpEvalFun()
{
  LogTest("TestInterpEvalFun");

  VInterp* interp = InterpNew();

  //char* source = "(+ print)funify(ADD)setglobal 1 2 ADD sayHi ('My name is Ruma' print)funify(RUMA)setglobal (RUMA RUMA)funify(RUMA2)setglobal RUMA2";
  char* source = "(88 print 1 2 + print) eval";
  VItem* code = ParseSourceCode(source, interp->mem);
  MemPrintList(interp->mem, code);
  PrintNewLine();

  InterpEval(interp, code);

  ShouldHold("CALLSTACK TOP SHOULD BE NULL", NULL == interp->callStackTop);

  InterpFree(interp);
}

void TestInterpEvalFunInfiniteTail()
{
  LogTest("TestInterpEvalFunInfiniteTail");

  VInterp* interp = InterpNew();

  char* source = "('Hi Ruma' print RUMA)funify(RUMA)setglobal RUMA";
  VItem* code = ParseSourceCode(source, interp->mem);
  MemPrintList(interp->mem, code);
  PrintNewLine();

  InterpEval(interp, code);

  ShouldHold("CALLSTACK TOP SHOULD BE NULL", NULL == interp->callStackTop);

  InterpFree(interp);
}

int main()
{
  PrintLine("Welcome to the wonderful world of Vimana");

  TestPrintBinary();
  TestItemAttributes();
  TestAllocDealloc();
  TestConsDealloc();

  //TestParseSymbolicCode();
  TestSymbolTable();
  TestParseSourceCode();

  TestArray();
  TestArrayWithStrings();
  TestInterp();
  
  TestInterpEval();
  TestInterpEvalFun();
  //TestInterpEvalFunInfiniteTail();

  LogTest("DONE");

  SysPrintMemStat();
}

#endif

/*
#define ArrayStringPtrAt(array, index) \
  ((char**)ArrayAt(array, index))

int main(int numargs, char* args[])
{
  VArray* array = ArrayNew(sizeof(char*), 2);

  char* s = StrCopy("I am index zero");
  PrintLine(s);

  // *((char**)ArraySetAt(array, 0)) = s;
  //((char**)(array->buffer))[0] = s;
  //char** p = ArrayAt(array, 0);
  // *p = s;
  //PrintLine(*p);
  //PrintLine(*(char**)ArrayAt(array, 0));

  ArrayStringAt(array, 0) = s;

  array = ArrayGrow(array, 11);

  ArrayStringAt(array, 10) = StrCopy("I am index 10");

  PrintLine(ArrayStringAt(array, 0));
  PrintLine(*ArrayStringPtrAt(array, 0));
  PrintLine(ArrayStringAt(array, 10));

  printf("Array length: %i\n", array->length);


  void* pvoid = 0;
  ++ pvoid;
  printf("pvoid:   %li\n", (long)pvoid);
  printf("pvoid:   %li\n", (long)pvoid + 1);

  VInterp* pInterp = 0;
  ++ pInterp;
  printf("pInterp: %li\n", (long)pInterp);
  printf("pInterp: %li\n", (long)(pInterp + 1));
  printf("pInterp: %li\n", (long)(((void*)pInterp) + 1));
  printf("pInterp: %li\n", (long)(((unsigned long)pInterp) + 1));

  return 0;
}
*/

/*
int main(int numargs, char* args[])
{
  SymbolTableCreate();

  SymbolTableFindAdd("foo");
  SymbolTableFindAdd("bar");
  SymbolTableFindAdd("foo");

  PrintLine(SymbolTableGet(1));

  SymbolTableFree();

  return 0;
}
*/

/*
int hash = 7;
for (int i = 0; i < strlen; i++) {
    hash = hash*31 + charAt(i);
}

unsigned int DJBHash(char* str, unsigned int len)
{
   unsigned int hash = 5381;
   unsigned int i    = 0;

   for(i = 0; i < len; str++, i++)
   {   
      hash = ((hash << 5) + hash) + (*str);
   }   

   return hash;
}
unsigned long hash = 5381;
int c;

while (c = *str++)
    hash = ((hash << 5) + hash) + c; // hash * 33 + c

http://www.cse.yorku.ca/~oz/hash.html
*/

/*
#define PROGMEM 

//const 
typedef struct __VPrimFunEntry 
{
  char *name;
  int index;
  int x;
} 
VPrimFunEntry;

VPrimFunEntry primFuns[] PROGMEM = 
{
  { "Item_one", 0, 1 },
  { "Item_two", 1, 1 },
  { "Item_three", 2, 1},
  { "Item_four", 3, 1 }
};

  //printf ("primfun: %s\n", primFuns[3].name);
*/

/*
https://forum.arduino.cc/t/call-function-pointer-from-progmem/338748

boolean fun1() {
  Serial.println(F("Function 1"));
}
boolean fun2() {
  Serial.println(F("Function 2"));
}
boolean fun3() {
  Serial.println(F("Function 3"));
}
boolean fun4() {
  Serial.println(F("Function 4"));
}

typedef boolean (*Item_Function)();

const typedef struct MenuItem_t {
  char *text;
  Item_Function func;
} MenuItem;

MenuItem firstList[4] PROGMEM = {
  { "Item_one", &fun1 }, // need to have reference to the function
  { "Item_two", &fun2 },
  { "Item_three", &fun3},
  { "Item_four", &fun4 }
};

MenuItem* itemPtr = firstList;

void setup() {

  // Here I want to use the itemPtr to call the function from the
  //      struct instance it is currently pointing to.
  // In this case it should call fun1()

  boolean (*function)(void); // function buffer

  Serial.begin(115200);
  for (byte i = 0; i < 4; i++)
  {
    Serial.println((char*)pgm_read_word(&(firstList[i].text)));
    function = (Item_Function)pgm_read_word(&(firstList[i].func)); // this needs to be assigned to another function pointer in order to use it.

    function(); // run the function.
    Serial.println();
  }

}
void loop() {}


https://www.arduino.cc/reference/en/language/variables/utilities/progmem/

https://cexamples.com/examples/using-function-pointer-from-struct-in-progmem-in-c-on-arduino

http://www.nongnu.org/avr-libc/user-manual/group__avr__pgmspace.html

https://www.arduino.cc/en/pmwiki.php?n=Reference/PROGMEM

https://www.e-tinkers.com/2020/05/do-you-know-arduino-progmem-demystified/
*/

/*
https://justine.lol/sectorlisp2/lisp.c
https://justine.lol/sectorlisp2/
https://github.com/technoblogy/ulisp/blob/master/ulisp.ino
http://www.newlisp.org/index.cgi?page=Differences_to_Other_LISPs
*/
