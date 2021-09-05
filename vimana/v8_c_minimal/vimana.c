#include "vimana.h"

// MAIN ------------------------------------------------

// TODO: Make file with item tests.

void PrintIntList(VList* list)
{
  for (int i = 0; i < ListLength(list); ++i)
  {
    PrintNum(*((int*)(ListGet(list, i)))); 
    Print(" ");
  }
  PrintLine("");
}

int main(int numargs, char* args[])
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
  funList->type = TypeFun;
  ItemSetObj(&testItem, funList);
  ShouldHold("Item must be object", IsObj(&testItem));
  ShouldHold("Item must be fun", IsFun(&testItem));
  ListFree(funList);
  
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

  PrintLine("\nTESTING INTERPRETER\n");
  VInterp* interp = InterpCreate();
  
  VList* codeList = ListCreate(sizeof(VItem));
  VItem* codeItem;

  codeItem = ListPushNewItem(codeList);
  ItemSetNumber(codeItem, 42);
  codeItem = ListPushNewItem(codeList);
  ItemSetPrimFun(codeItem, 1);
  codeItem = ListPushNewItem(codeList);
  ItemSetNumber(codeItem, 43);
  codeItem = ListPushNewItem(codeList);
  ItemSetPrimFun(codeItem, 1);

  codeItem = ListPushNewItem(codeList);
  ItemSetNumber(codeItem, 1);
  codeItem = ListPushNewItem(codeList);
  ItemSetNumber(codeItem, 2);
  codeItem = ListPushNewItem(codeList);
  ItemSetPrimFun(codeItem, 2);
  codeItem = ListPushNewItem(codeList);
  ItemSetPrimFun(codeItem, 1);
  
  InterpRun(interp, codeList);

  InterpRun(interp, codeList);

  ListFreeDeep(codeList);
  
  VList* codeList2 = ParseCode("N8888881 P1 N33 N33 P2 P1 (S1 S2) P1 ('FOO HEJ HOPP') P1");

  PrintList(codeList2);
  PrintNewLine();

  InterpRun(interp, codeList2);

  ListFreeDeep(codeList2);

  InterpFree(interp);

  PrintMemStat();
  
  //ErrorExit("Exit 1");
  //ErrorExitNum("Exit 2: ", 42);
  
  
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
