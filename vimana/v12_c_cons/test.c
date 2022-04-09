
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
    MemItemSetNext(mem, item, next);
    item = next;
  }
  return first;
}

VItem* AllocMaxItemsUsingCons(VMem* mem)
{
  // Alloc and cons items until out of memory
  VItem* first = MemAllocItem(mem);
  int counter = 1;
  VItem* list = NULL;
  while (1)
  {
    VItem item;
    ItemSetIntNum(&item, counter ++);
    VItem* cons = MemCons(mem, &item, list);
    if (NULL == cons) break;
    list = cons;
  }
  return list;
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

/*
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
*/

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

void TestConsDealloc()
{
  VMem* mem = MemNew(100);

  VItem* first = AllocMaxItemsUsingCons(mem);
  printf("ONE\n");
  printf("ONE NEXT: %i\n", (int)first->next);
  printf("ONE DATA: %i\n", (int)first->data);
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

/*
void TestParseSymbolicCode()
{
  VMem* mem = MemNew(1000);
  char* code = "N-42 N44 (P1 N2 (N3 (P4)) N5 N6) N0 (((N88";
  VItem* first = ParseSymbolicCode(code, mem);
  MemPrintList(mem, first);
  printf("\n");
  MemFree(mem);
}
*/

void TestParseSourceCode()
{
  VMem* mem = MemNew(1000);
  VItem* list;

  char* code = "1 2 (((3 4) 5))";
  list = ParseSourceCode(code, mem);
  MemPrintItem(mem, list);
  printf("\n");

  char* empty = "";
  list = ParseSourceCode(empty, mem);
  MemPrintItem(mem, list);
  printf("\n");
  //ShouldHold("first should be NULL", NULL == first);

  char* empty2 = "()";
  list = ParseSourceCode(empty2, mem);
  MemPrintItem(mem, list);
  printf("\n");

  char* empty3 = "   (( ( ) ))   ";
  list = ParseSourceCode(empty3, mem);
  MemPrintItem(mem, list);
  printf("\n");

  char* string = "'Hi World'";
  list = ParseSourceCode(string, mem);
  MemPrintItem(mem, list);
  printf("\n");

  char* string2 = "('Hi foo') 'foo bar'";
  list = ParseSourceCode(string2, mem);
  MemPrintItem(mem, list);
  printf("\n");

  MemFree(mem);
}

void TestInterp()
{
  // Create interpreter
  int dataStackSize = 100;
  int globalVarsSize = 100;
  int callStackSize = 100;
  int memSize = 1000;
  VInterp* interp = InterpNew(
    dataStackSize, globalVarsSize,
    callStackSize, memSize);

  // Test data stack
  VItem* item = MemAllocItem(interp->itemMem);
  ItemSetIntNum(item, 42);
  printf("item value 1: %i\n", (int)ItemData(item));

  InterpPush(interp, item);
  VItem* item2 = InterpPop(interp);
  printf("item value 2: %i\n", (int)ItemData(item2));

  // Tests for underflow/overflow
  // InterpPop(interp);
  // while (1) InterpPush(interp, item);

  // Test callstack
  InterpPushContext(interp, item);
  printf("code : %i\n", (int)ItemData(interp->callStackTop->code));
  printf("instr: %i\n", (int)ItemData(interp->callStackTop->instruction));

  InterpPushContext(interp, item);
  printf("code : %i\n", (int)ItemData(interp->callStackTop->code));
  printf("instr: %i\n", (int)ItemData(interp->callStackTop->instruction));

  InterpPopContext(interp);
  ShouldHold("CALLSTACK TOP SHOULD NOT BE NULL", NULL != interp->callStackTop);

  printf("code : %i\n", (int)ItemData(interp->callStackTop->code));
  printf("instr: %i\n", (int)ItemData(interp->callStackTop->instruction));

  InterpPopContext(interp);
  ShouldHold("CALLSTACK TOP SHOULD BE NULL", NULL == interp->callStackTop);

  // Tests for underflow/overflow
  // InterpPopContext(interp);
  // while (1) InterpPushContext(interp, item);

  // Free interpreter
  InterpFree(interp);
}

/*
void TestInterpEval()
{
  printf("TestInterpEval\n");

  // Create interpreter
  int dataStackSize = 100;
  int globalVarsSize = 100;
  int callStackSize = 100;
  int memSize = 1000;
  VInterp* interp = InterpNew(
    dataStackSize, globalVarsSize,
    callStackSize, memSize);

  char* code = "N42 P1";
  VItem* first = ParseSymbolicCode(code, interp->itemMem);
  MemPrintList(interp->itemMem, first);
  printf("\n");

  InterpEval(interp, first);

  ShouldHold("CALLSTACK TOP SHOULD BE NULL", NULL == interp->callStackTop);

  // Free interpreter
  InterpFree(interp);
}
*/

void TestSymbols()
{
  VMem* mem = MemNew(10000);

  char* s1 = "First";
  char* s2 = "Second";
  char* s3 = "Third";

  VItem* symbols = MemAllocItem(mem);
  
  SymbolFindAdd(symbols, s1, mem);
  SymbolFindAdd(symbols, s2, mem);
  SymbolFindAdd(symbols, s3, mem);

  int index = SymbolFindAdd(symbols, s3, mem);
  char* s = SymbolGetString(symbols, 1, mem);

  printf("Symbol index: %i %s\n", index, s);

  MemFree(mem);
}

int main()
{
  printf("Hi World\n");

  /*TestPrintBinary();
  TestItemAttributes();
  TestAllocDealloc();
  TestConsDealloc();
  TestParseSymbolicCode();
  TestInterp();
  TestInterpEval();*/
  
  //TestSymbols();

  TestParseSourceCode();

  printf("DONE\n");

  PrintMemStat();
}

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
