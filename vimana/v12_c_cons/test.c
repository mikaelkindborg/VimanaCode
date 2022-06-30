
#define TRACK_MEMORY_USAGE

#include "vimana.h"

void TestPrintBinary()
{
  printf("---> TestPrintBinary\n");

  PrintBinaryUInt(0x80000000);
  PrintBinaryUInt(0x1 << 1);
  PrintBinaryUInt(0x2 >> 1);
}

void TestItemAttributes()
{
  printf("---> TestItemAttributes\n");

  VItemMemory* itemMemory = MemNew(100);

  VItem* item = MemAllocItem(itemMemory);
  ItemSetGCMark(item, 1);
  ItemSetType(item, 42);
  PrintBinaryUInt(item->type);
  printf("Type: %i\n", ItemType(item));
  printf("Mark: %i\n", ItemGCMark(item));
  ShouldHold("Item type should be equal", 42 == ItemType(item));
  ShouldHold("Item gc mark should be set", 1 == ItemGCMark(item));
  MemDeallocItem(itemMemory, item);
  
  MemFree(itemMemory);
}

VItem* AllocMaxItems(VItemMemory* itemMemory)
{
  // Alloc and cons items until out of memory

  printf("Alloc max items\n");
  VItem* first = MemAllocItem(itemMemory);
  ItemSetIntNum(first, 1);
  VItem* item = first;
  while (1)
  {
    VItem* next = MemAllocItem(itemMemory);
    if (NULL == next) break;
    ItemSetIntNum(next, item->intNum + 1);
    MemItemSetNext(itemMemory, item, next);
    item = next;
  }
  return first;
}

VItem* AllocMaxItemsUsingCons(VItemMemory* itemMemory)
{
  // Alloc and cons items until out of memory
  int counter = 1;
  VItem* first = NULL;
  while (1)
  {
    VItem item;
    ItemSetIntNum(&item, counter ++);
    VItem* cons = MemCons(itemMemory, &item, first);
    if (NULL == cons) break;
    first = cons;
  }
  return first;
}

/*
void X_DeallocItems(VItem* first, VItemMemory* itemMemory)
{
  VItem* item = first;
  while (1)
  {
    printf("DEALLOC: %li\n", item->intNum);
    VAddr nextAddr = item->next;
    MemDeallocItem(itemMemory, item);
    if (0 == nextAddr) break;
    item = MemItemPointer(itemMemory, nextAddr);
  }
}

void xPrintItems(VItem* first, VItemMemory* itemMemory)
{
  VAddr addr = MemItemAddr(itemMemory, first);
  while (addr)
  {
    VItem* item = MemItemPointer(itemMemory, addr);
    printf("%li\n", item->intNum);
    addr = item->next;
  }
}
*/

void PrintItems(VItem* first, VItemMemory* itemMemory)
{
  VItem* item = first;
  while (item)
  {
    printf("%li\n", item->intNum);
    item = MemItemNext(itemMemory, item);
  }
}

int CountItems(VItem* first, VItemMemory* itemMemory)
{
  int counter = 0;
  VAddr addr = MemItemAddr(itemMemory, first);
  while (addr)
  {
    ++ counter;
    VItem* item = MemItemPointer(itemMemory, addr);
    addr = item->next;
  }
  return counter;
}

void TestAllocDealloc()
{
  printf("---> TestAllocDealloc\n");

  VItemMemory* itemMemory = MemNew(10);

  printf("Alloc max\n");
  VItem* first = AllocMaxItems(itemMemory);
  PrintItems(first, itemMemory);
  int numItems = CountItems(first, itemMemory);
  printf("Num items: %i\n", numItems);

  MemSweep(itemMemory);

  printf("Alloc max again\n");
  first = AllocMaxItems(itemMemory);
  PrintItems(first, itemMemory);
  int numItems2 = CountItems(first, itemMemory);
  printf("Num items: %i\n", numItems2);

  MemSweep(itemMemory);

  ShouldHold("Allocated items should be equal", numItems == numItems2);

  MemFree(itemMemory);
}

void TestConsDealloc()
{
  printf("---> TestConsDealloc\n");

  VItemMemory* itemMemory = MemNew(10);

  VItem* first = AllocMaxItemsUsingCons(itemMemory);
  printf("ONE\n");
  printf("ONE NEXT: %i\n", (int)first->next);
  printf("ONE ADDR: %i\n", (int)first->addr);
  PrintItems(first, itemMemory);
  int numItems = CountItems(first, itemMemory);
  printf("Num items: %i\n", numItems);

  MemSweep(itemMemory);

  first = AllocMaxItems(itemMemory);
  PrintItems(first, itemMemory);
  int numItems2 = CountItems(first, itemMemory);
  printf("Num items: %i\n", numItems2);

  MemSweep(itemMemory);

  ShouldHold("Allocated items should be equal", numItems == numItems2);

  MemFree(itemMemory);
}

/*
void TestParseSymbolicCode()
{
  VItemMemory* itemMemory = MemNew(1000);
  char* code = "N-42 N44 (P1 N2 (N3 (P4)) N5 N6) N0 (((N88";
  VItem* first = ParseSymbolicCode(code, itemMemory);
  MemPrintList(itemMemory, first);
  printf("\n");
  MemFree(itemMemory);
}
*/

void TestParseSourceCode()
{
  printf("---> TestParseSourceCode\n");

  GSymbolTableInit();

  VItemMemory* itemMemory = MemNew(1000);
  VItem* list;

  char* code = "1 2 (((3 4) 5))";
  list = ParseSourceCode(code, itemMemory);
  MemPrintItem(itemMemory, list);
  printf("\n");

  char* empty = "";
  list = ParseSourceCode(empty, itemMemory);
  MemPrintItem(itemMemory, list);
  printf("\n");
  //ShouldHold("first should be NULL", NULL == first);

  char* empty2 = "()";
  list = ParseSourceCode(empty2, itemMemory);
  MemPrintItem(itemMemory, list);
  printf("\n");

  char* empty3 = "   (( ( ) ))   ";
  list = ParseSourceCode(empty3, itemMemory);
  MemPrintItem(itemMemory, list);
  printf("\n");

  char* string = "'Hi World'";
  list = ParseSourceCode(string, itemMemory);
  MemPrintItem(itemMemory, list);
  printf("\n");

  char* string2 = "('Hi foo') 'foo bar'";
  list = ParseSourceCode(string2, itemMemory);
  MemPrintItem(itemMemory, list);
  printf("\n");

  char* code2 = "1 2 3.33 foo bar bar sayHi";
  list = ParseSourceCode(code2, itemMemory);
  MemPrintItem(itemMemory, list);
  printf("\n");

  /*
  MemMark(itemMemory, list);
  MemSweep(itemMemory);
  MemMark(itemMemory, list);
  MemSweep(itemMemory);
  */
  MemSweep(itemMemory);
  MemSweep(itemMemory);

  MemFree(itemMemory);

  GSymbolTableRelease();
}

void TestSymbolTable()
{
  printf("---> TestSymbolTable\n");

  char* s1 = "First";
  char* s2 = "Second";
  char* s3 = "Third";

  VItem* symbols = SymbolTableNew();
  
  SymbolTableFindAdd(symbols, s1);
  SymbolTableFindAdd(symbols, s2);
  SymbolTableFindAdd(symbols, s3);

  int index = SymbolTableFindAdd(symbols, s3);
  char* s = SymbolTableGetString(symbols, 1);

  printf("Symbol index: %i %s\n", index, s);

  SymbolTableFree(symbols);
}

void TestArray()
{
  printf("---> TestArray\n");

  VItem item;

  VItem* array = ArrayNew(5);

  for (int i = 0; i < 20; ++ i)
  {
    item.intNum = i + 1;
    array = ArrayGrow(array, i + 5);
    ArraySet(array, i, &item);
  }

  for (int i = 0; i < ArrayLength(array); ++ i)
  {
    VItem* item = ArrayGet(array, i);
    printf("value: %li\n", item->intNum);
  }

  ArrayFree(array);
}

void TestArrayWithStrings()
{
  printf("---> TestArrayWithStrings\n");

  VItemMemory* itemMemory = MemNew(1000);

  VItem* item;

  VItem* array = ArrayNew(5);

  for (int i = 0; i < 20; ++ i)
  {
    array = ArrayGrow(array, i + 5);

    char* str = "Hej";
    item = MemAllocItem(itemMemory);
    MemItemSetString(itemMemory, item, str);

    ArraySet(array, i, item);
  }

  for (int i = 0; i < ArrayLength(array); ++ i)
  {
    VItem* item = ArrayGet(array, i);
    printf("value: %s\n", MemItemString(itemMemory, item));
  }

  MemMark(itemMemory, & array[1]);
  MemMark(itemMemory, & array[2]);
  MemMark(itemMemory, & array[3]);
  MemSweep(itemMemory);
  MemSweep(itemMemory);

  ArrayFree(array);

  MemFree(itemMemory);
}

void TestInterp()
{
  printf("---> TestInterp\n");

  VInterp* interp = InterpNew();

  // Test data stack
  VItem* item = MemAllocItem(interp->itemMemory);
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
  printf("---> TestInterpEval\n");

  VInterp* interp = InterpNew();

  char* source = "1 2 3 sayHi 1 2 3 + + + + + (SUM)setglobal SUM print";
  VItem* code = ParseSourceCode(source, interp->itemMemory);
  MemPrintList(interp->itemMemory, code);
  printf("\n");

  InterpEval(interp, code);

  ShouldHold("CALLSTACK TOP SHOULD BE NULL", NULL == interp->callStackTop);

  InterpFree(interp);
}

void TestInterpEvalFun()
{
  printf("---> TestInterpEvalFun\n");

  VInterp* interp = InterpNew();

  //char* source = "(+ print)funify(ADD)setglobal 1 2 ADD sayHi ('My name is Ruma' print)funify(RUMA)setglobal (RUMA RUMA)funify(RUMA2)setglobal RUMA2";
  char* source = "(88 print 1 2 + print) eval";
  VItem* code = ParseSourceCode(source, interp->itemMemory);
  MemPrintList(interp->itemMemory, code);
  printf("\n");

  InterpEval(interp, code);

  ShouldHold("CALLSTACK TOP SHOULD BE NULL", NULL == interp->callStackTop);

  InterpFree(interp);
}

void TestInterpEvalFunInfiniteTail()
{
  printf("---> TestInterpEvalFunInfiniteTail\n");

  VInterp* interp = InterpNew();

  char* source = "('Hi Ruma' print RUMA)funify(RUMA)setglobal RUMA";
  VItem* code = ParseSourceCode(source, interp->itemMemory);
  MemPrintList(interp->itemMemory, code);
  printf("\n");

  InterpEval(interp, code);

  ShouldHold("CALLSTACK TOP SHOULD BE NULL", NULL == interp->callStackTop);

  InterpFree(interp);
}

int main()
{
  printf("Welcome to the wonderful world of Vimana\n");

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

  printf("DONE\n");

  PrintMemStat();
}


/*
#define ArrayStringPtrAt(array, index) \
  ((char**)ArrayAt(array, index))

int main(int numargs, char* args[])
{
  VArray* array = ArrayNew(sizeof(char*), 2);

  char* s = StrCopy("I am index zero");
  PrintLine(s);

  //*((char**)ArraySetAt(array, 0)) = s;
  //((char**)(array->buffer))[0] = s;
  //char** p = ArrayAt(array, 0);
  //*p = s;
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

  SymbolTableFindAddString("foo");
  SymbolTableFindAddString("bar");
  SymbolTableFindAddString("foo");

  PrintLine(SymbolTableGetString(1));

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
