/*
File: primfuns.h
Author: Mikael Kindborg (mikael@kindborg.com)
*/

void PrimFun_sayHi(VInterp* interp)
{
  printf("Hi World!\n");
}//

void PrimFun_print(VInterp* interp)
{
  VItem* item = InterpPop(interp);
  MemPrintItem(interp->mem, item);
  PrintNewLine();
}//

void PrimFun_printStack(VInterp* interp)
{
  printf("print stack!\n");
}//

void PrimFun_eval(VInterp* interp)
{
  // pop list
  // push context with env flag
}//

void PrimFun_setglobal(VInterp* interp)
{
  VItem* list = InterpPop(interp);
  VItem* value = InterpPop(interp);
  VItem* symbol = MemItemFirst(interp->mem, list);
  InterpSetGlobalVar(interp, symbol->intNum, value);
}//

void PrimFun_funify(VInterp* interp)
{
  VItem* list = & (interp->dataStack[interp->dataStackTop]);
  ItemSetType(list, TypeFun);
}//

void PrimFun_loadfile(VInterp* interp)
{
  // pop string
  // data = read file
  // list = parse data
  // eval list (in own context)
}//

/*
primitive_add()
{
  VObj* num2 = InterpPop();
  VObj* num1 = InterpPop();
  VObj* result = InterpAlloc();
  int sum = num1->intNum + num2->intNum;
  ObjSetIntNum(result, sum);
  InterpPush(result);
}
*/
/*
PrimFun_plus(VInterp* interp)
{
  VItem* b = InterpPop(interp);
  VItem* a = InterpPop(interp);
  VItem result;
  ItemInit(&result);
  result.intNum = a->intNum + b->intNum;
  ItemSetType(&result, TypeIntNum);
  InterpPush(interp, &result);
}
*/

void PrimFun_plus(VInterp* interp)
{
  VItem* b = InterpPop(interp);
  VItem* a = InterpTop(interp);
  a->intNum += b->intNum;
}//

void PrimFun_minus(VInterp* interp)
{
  VItem* b = InterpPop(interp);
  VItem* a = InterpTop(interp);
  a->intNum -= b->intNum;
}//

void PrimFun_times(VInterp* interp)
{
  VItem* b = InterpPop(interp);
  VItem* a = InterpTop(interp);
  a->intNum *= b->intNum;
}//

void PrimFun_div(VInterp* interp)
{
  VItem* b = InterpPop(interp);
  VItem* a = InterpTop(interp);
  a->intNum /= b->intNum;
}//

void PrimFun_eq(VInterp* interp)
{
  VItem* b = InterpPop(interp);
  VItem* a = InterpTop(interp);
  a->intNum = ItemEquals(a, b);
  ItemSetType(a, TypeIntNum);
}//

typedef struct __PrimFunEntry
{
  char*       name;
  VPrimFunPtr fun;
}
PrimFunEntry;

PrimFunEntry GPrimFunTable[] = 
{
  #include "primfuntable.h"
};

int LookupPrimFun(char* name)
{
  for (int i = 0; ; ++ i)
  {
    PrimFunEntry* entry = & (GPrimFunTable[i]);
    if (StrEquals(entry->name, name))
      return i;
    else
    if (NULL == entry->fun)
      return -1;
  } 
}

VPrimFunPtr LookupPrimFunPtr(int index)
{
  return GPrimFunTable[index].fun;
}
