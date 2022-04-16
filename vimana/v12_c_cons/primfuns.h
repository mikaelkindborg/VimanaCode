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
  VItem* item = InterpStackPop(interp);
  MemPrintItem(interp->mem, item);
  PrintNewLine();
}//

void PrimFun_printStack(VInterp* interp)
{
  printf("print stack!\n");
}//

// TODO: Eval in own context? apply?
void PrimFun_eval(VInterp* interp)
{
  VItem* codeBlock = InterpStackPop(interp);
  InterpStackPushContext(interp, codeBlock);
}//

void PrimFun_iftrue(VInterp* interp)
{
  VItem* trueBlock = InterpStackPop(interp);
  VItem* trueOrFalse = InterpStackPop(interp);
  if (trueOrFalse->intNum)
    InterpStackPushContext(interp, trueBlock);
}//

void PrimFun_iffalse(VInterp* interp)
{
  VItem* falseBlock = InterpStackPop(interp);
  VItem* trueOrFalse = InterpStackPop(interp);
  if (! trueOrFalse->intNum)
    InterpStackPushContext(interp, falseBlock);
}//

void PrimFun_ifelse(VInterp* interp)
{
  VItem* falseBlock = InterpStackPop(interp);
  VItem* trueBlock = InterpStackPop(interp);
  VItem* trueOrFalse = InterpStackPop(interp);
  if (trueOrFalse->intNum)
    InterpStackPushContext(interp, trueBlock);
  else
    InterpStackPushContext(interp, falseBlock);
}//

void PrimFun_setglobal(VInterp* interp)
{
  VItem* list = InterpStackPop(interp);
  VItem* value = InterpStackPop(interp);
  VItem* symbol = MemItemFirst(interp->mem, list);
  InterpSetGlobalVar(interp, symbol->intNum, value);
}//

void PrimFun_funify(VInterp* interp)
{
  VItem* list = & (interp->dataStack[interp->dataStackTop]);
  ItemSetType(list, TypeFun);
}//

void PrimFun_readfile(VInterp* interp)
{
  // pop string
  // data = read file
}//

void PrimFun_parse(VInterp* interp)
{
  // pop string
  // list = parse string
}//

void PrimFun_plus(VInterp* interp)
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum += b->intNum;
}//

void PrimFun_minus(VInterp* interp)
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum -= b->intNum;
}//

void PrimFun_times(VInterp* interp)
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum *= b->intNum;
}//

void PrimFun_div(VInterp* interp)
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum /= b->intNum;
}//

void PrimFun_1plus(VInterp* interp)
{
  VItem* a = InterpStackTop(interp);
  a->intNum += 1;
}//

void PrimFun_1minus(VInterp* interp)
{
  VItem* a = InterpStackTop(interp);
  a->intNum -= 1;
}//

void PrimFun_2plus(VInterp* interp)
{
  VItem* a = InterpStackTop(interp);
  a->intNum += 2;
}//

void PrimFun_2minus(VInterp* interp)
{
  VItem* a = InterpStackTop(interp);
  a->intNum -= 2;
}//

void PrimFun_lessthan(VInterp* interp)
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum = a->intNum < b->intNum;
}//

void PrimFun_greaterthan(VInterp* interp)
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum = a->intNum > b->intNum;
}//

void PrimFun_eq(VInterp* interp)
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum = ItemEquals(a, b);
  ItemSetType(a, TypeIntNum);
}//

void PrimFun_iszero(VInterp* interp)
{
  VItem* a = InterpStackTop(interp);
  a->intNum = 0 == a->intNum;
}//

void PrimFun_not(VInterp* interp)
{
  VItem* a = InterpStackTop(interp);
  a->intNum = ! a->intNum;
}//

void PrimFun_dup(VInterp* interp)
{
  VItem* a = InterpStackTop(interp);
  InterpStackPush(interp, a);
}//

void PrimFun_swap(VInterp* interp)
{
  VItem* a = InterpStackAt(interp, 0);
  VItem* b = InterpStackAt(interp, 1);
  VItem temp = *a;
  *a = *b;
  *b = temp;
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

/*
primitive_add()
{
  VObj* num2 = InterpStackPop();
  VObj* num1 = InterpStackPop();
  VObj* result = InterpAlloc();
  int sum = num1->intNum + num2->intNum;
  ObjSetIntNum(result, sum);
  InterpStackPush(result);
}
*/
/*
PrimFun_plus(VInterp* interp)
{
  VItem* b = InterpStackPop(interp);
  VItem* a = InterpStackPop(interp);
  VItem result;
  ItemInit(&result);
  result.intNum = a->intNum + b->intNum;
  ItemSetType(&result, TypeIntNum);
  InterpStackPush(interp, &result);
}
*/
