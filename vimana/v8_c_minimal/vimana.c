#include "vimana.h"

// MAIN ------------------------------------------------

// TODO: Make file with item tests.

void PrintIntList(VmList* list)
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
  VmItem item = ItemWithNumber(x);
  PrintBinaryULong(item.value.bits);
  VmNumber num = ItemNumber(item);
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
  VmItem errorTest = ItemWithNumber(xx);

  VmList* list = ListCreate(sizeof(int));
  
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

  ShouldHold("Item must be number", IsNumber(ItemWithNumber(42)));
  ShouldHold("Item must be symbol", IsSymbol(ItemWithSymbol(42)));
  ShouldHold("Item must be primfun", IsPrimFun(ItemWithPrimFun(42)));

  VmList* funList = ListCreate(sizeof(int));
  funList->type = TypeFun;
  ShouldHold("Item must be pointer", IsPointer(ItemWithPointer(funList)));
  ShouldHold("Item must be fun", IsFun(ItemWithPointer(funList)));
  ListFree(funList);
  
  
  VmInterp* interp = InterpCreate();
  
  VmList* codeList = ListCreate(sizeof(VmItem));
  
  VmItem codeItem;
  codeItem = ItemWithNumber(42);
  ListPush(list, &codeItem);
  codeItem = ItemWithNumber(1);
  ListPush(list, &codeItem);
  codeItem = ItemWithNumber(43);
  ListPush(list, &codeItem);
  codeItem = ItemWithNumber(1);
  ListPush(list, &codeItem);
  
  InterpRun(interp, codeList);
  
  ListFree(codeList);
  
  InterpFree(interp);
  
  PrintMemStat();
  
  //ErrorExit("Exit 1");
  //ErrorExitNum("Exit 2: ", 42);
  
  
}
