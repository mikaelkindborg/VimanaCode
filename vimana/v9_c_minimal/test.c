#include "vimana.h"

// MAIN ------------------------------------------------

// Test of VString object
void TestString()
{
  VString* string = StringCreate();

  StringWriteChar(string, 'H');
  StringWriteChar(string, 'E');
  StringWriteChar(string, 'J');

  StringWriteStr(string, " HOPP");
  StringWriteStr(string, " HOPP");

  ShouldHold(
    "TestString failed", 
    StrEquals(
      StringGetStr(string),
      "HEJ HOPP HOPP")
    );

  printf("%s\n", StringGetStr(string));

  StringFree(string);
}

/*
void TestString2()
{
  PrintLine("\nTESTING STRING ITEM\n");
  char* myString = MemAlloc(10);
  PrintBinaryULong((VUNumber)myString);
  PrintBinaryULong((VUNumber)myString | TypeString);
  PrintBinaryULong(~TypeString);
  PrintBinaryULong((VUNumber)myString & ~TypeString);

  strcpy(myString, "FUBAR\n");
  VItem stringItem;
  ItemSetString(&stringItem, myString);
  PrintBinaryULong(stringItem.value.bits);
  char* myString2 = ItemString(&stringItem);
  PrintBinaryULong((VUNumber)myString2);
  printf("THE STRING IS: %s\n", myString2);
  MemFree(myString2);
}
*/

void TestBinary()
{
  long x = -255;
  PrintBinaryULong(15);
  PrintBinaryULong(x);
  PrintBinaryULong(x << 3);

  VItem numberItem;
  ItemSetNumber(&numberItem, x);
  PrintBinaryULong(numberItem.value.bits);
  VNumber num = ItemNumber(&numberItem);
  PrintBinaryULong(num);
  printf("Number: %ld\n", num);

  unsigned long xx = 0;
  xx = ~xx;
  PrintBinaryULong(xx);
  //xx = xx >> 1; // Generates error below
  xx = xx >> 4; // Passes below
  PrintBinaryULong(xx);
  printf("xx: %lu\n", xx);
  printf("xx: %ld\n", (long)xx);
  VItem errorTest;
  ItemSetNumber(&errorTest, xx); // Uncomment/comment above to trigger error
}

void PrintIntList(VList* list)
{
  for (int i = 0; i < ListLength(list); ++i)
  {
    PrintNum(*((int*)(ListGet(list, i)))); 
    Print(" ");
  }
  PrintLine("");
}

void TestList()
{
  VList* list = ListCreate(sizeof(int));
  
  int numbers[] = { 1, 2, 3, 4, 5, 6 };
  int* p = numbers;
  ListPush(list, p++);
  ListPush(list, p++);
  ListPush(list, p++);
  ListPush(list, p++);
  ListPush(list, p++);
  ListPush(list, p);
  //ListSet(list, -1, p);
  //ListGet(list, -1);
  //ListGet(list, 100);
  PrintIntList(list);
  
  ListSwap(list);
  PrintIntList(list);
  
  List2Dup(list);
  PrintIntList(list);
  
  ListOver(list);
  PrintIntList(list);
  
  ListDup(list);
  PrintIntList(list);
  
  ListDup(list);
  PrintIntList(list);
  
  ListFree(list);

  VItem testItem;
  ItemSetNumber(&testItem, 42);
  ShouldHold("Item must be number", IsNumber(&testItem));
  ItemSetSymbol(&testItem, 42);
  ShouldHold("Item must be symbol", IsSymbol(&testItem));
  ItemSetPrimFun(&testItem, 42);
  ShouldHold("Item must be primfun", IsPrimFun(&testItem));

  VList* funList = ListCreate(sizeof(int));
  funList->header.type = TypeFun;
  ItemSetObj(&testItem, funList);
  ShouldHold("Item must be object", IsObj(&testItem));
  ShouldHold("Item must be fun", IsFun(&testItem));
  ListFree(funList);
}

void TestInterpreter()
{
  PrintLine("\nTEST INTERPRETER\n");

  VInterp* interp = InterpCreate();
  
  VList* codeList = ListCreate(sizeof(VItem));
  VItem* codeItem;

  codeItem = ListPushNewItem(codeList);
  ItemSetNumber(codeItem, 42);
  codeItem = ListPushNewItem(codeList);
  ItemSetPrimFun(codeItem, 0);
  codeItem = ListPushNewItem(codeList);
  ItemSetNumber(codeItem, 43);
  codeItem = ListPushNewItem(codeList);
  ItemSetPrimFun(codeItem, 0);

  codeItem = ListPushNewItem(codeList);
  ItemSetNumber(codeItem, 1);
  codeItem = ListPushNewItem(codeList);
  ItemSetNumber(codeItem, 4);
  codeItem = ListPushNewItem(codeList);
  ItemSetPrimFun(codeItem, 3);
  codeItem = ListPushNewItem(codeList);
  ItemSetPrimFun(codeItem, 0);

  InterpRun(interp, codeList);
  //ListFreeDeep(codeList);

  InterpFree(interp);
}

void TestInterpreter2()
{
  PrintLine("\nTEST INTERPRETER 2\n");

  VInterp* interp = InterpCreate();
  
  VList* codeList = ParseSymbolicCode("N8888881 P0 N33 N33 P3 P0 (S1 S2) P0 ('FOO HEJ HOPP') P0");
  PrintList(codeList);
  PrintNewLine();
  InterpRun(interp, codeList);
  //ListFreeDeep(codeList);

  //ErrorExit("Exit 1");
  //ErrorExitNum("Exit 2: ", 42);

  VSymbolDict* dict = SymbolDictCreate();
  SymbolDictAddPrimFuns(dict);

  PrintLine("Generating Symbolic Code");
  char* symbolicCode = GenerateSymbolicCode(" FOO  BAR(FOOBAR 'Hi World' 1234) 5678", dict);
  printf("%s\n", symbolicCode);
  free(symbolicCode);
  
  PrintLine("Parsing Source Code");
  VList* codeListParsed = ParseSourceCode("'Hi World' print", dict);
  PrintList(codeListParsed); 
  PrintNewLine();
  InterpRun(interp, codeListParsed);
  //ListFreeDeep(codeListParsed);

  SymbolDictFree(dict);

  InterpFree(interp);
}

void TestInterpreter3()
{
  VList* code;

  VInterp* interp = InterpCreate();
  VSymbolDict* dict = SymbolDictCreate();
  SymbolDictAddPrimFuns(dict);

  code = ParseSourceCode("1 2 + print", dict);
  PrintList(code); 
  PrintNewLine();
  InterpRun(interp, code);
  //ListFreeDeep(code);

  code = ParseSourceCode("(1 2 3) 42 42", dict);
  PrintList(code); 
  PrintNewLine();
  InterpRun(interp, code);
  code = ParseSourceCode("* print", dict);
  PrintList(code); 
  PrintNewLine();
  InterpRun(interp, code);

  code = ParseSourceCode("'HI WORLD' print", dict);
  PrintList(code); 
  PrintNewLine();
  InterpRun(interp, code);
  //ListFreeDeep(code);

  code = ParseSourceCode("(foo) ('HI WORLD 2' print) def foo", dict);
  PrintList(code); 
  PrintNewLine();
  InterpRun(interp, code);
  //ListFreeDeep(code);

  code = ParseSourceCode("(double) (2 *) def 4 double print", dict);
  PrintList(code); 
  PrintNewLine();
  InterpRun(interp, code);
  //ListFreeDeep(code);

  code = ParseSourceCode("'I am Foobar' (foobar) setglobal", dict);
  PrintList(code); 
  PrintNewLine();
  InterpRun(interp, code);

  /*
  // This should crash.
  code = ParseSourceCode("foobar foobar print", dict);
  PrintList(code); 
  PrintNewLine();
  InterpRun(interp, code);
  */

  // This should work.
  code = ParseSourceCode("foobar print", dict);
  PrintList(code); 
  PrintNewLine();
  InterpRun(interp, code);

/*
  code = ParseSourceCode("0 (foobar) setglobal", dict);
  PrintList(code); 
  PrintNewLine();
  InterpRun(interp, code);
*/

  SymbolDictFree(dict);
  InterpFree(interp);
}

int main(int numargs, char* args[])
{
  //TestString();
  //TestString2();
  //TestBinary();
  //TestList();
  //TestInterpreter();
  //TestInterpreter2();
  TestInterpreter3();
  PrintMemStat();
}

/*
Parser that takes a string and outputs a string
Parser maintains symbol table and primfun table

TYPE VALUE PAIRS INPUT STREAM

1 42 // Number 42
2 1  // PrimFun 1
3    // List Begin
4    // List End

Alternative using chars:

S1            // Symbol 1
N42           // Number 42
P1            // PrimFun 1
(             // List Begin
)             // List End
"Hi World"    // String

READ BYTE
  WHITESPACE -> SKIP
  'S' -> READ NUMBER: SYMBOL ID
  'P' -> READ NUMBER: PRIMFUN ID
  'N' -> READ NUMBER
  '(' -> LIST BEGIN
  ')' -> LIST END


(S1) (P3 N2 P4 (P3 P5 S1 P6 N2 P7 S1 P8) P9) P10

(FIB)
  (DUP 2 < (DUP SUB1 FIB SWAP 2 - FIB +) IFFALSE) DEF

(TIMESDO) 
  (DUP ISZERO 
    (DROP DROP) 
    (SWAP DUP EVAL SWAP SUB1 TIMESDO) 
  IFELSE) DEFSPECIAL

(32 FIB PRINT) 5 TIMESDO

*/
