/*
File: primfuns.h
Author: Mikael Kindborg (mikael@kindborg.com)
*/

void PrimFun_sayHi(VInterp* interp)
{
  PrintLine("Hi World!");
}//

void PrimFun_print(VInterp* interp)
{
  VItem* item = InterpStackPop(interp);
  MemPrintItem(interp->mem, item);
  PrintNewLine();
}//

void PrimFun_printstack(VInterp* interp)
{
  MemPrintArray(interp->mem, interp->dataStack, interp->dataStackTop + 1);
  PrintNewLine();
}//

// TODO: Eval in own context? apply? funcall?
void PrimFun_eval(VInterp* interp)
{
  VItem* codeBlock = InterpStackPop(interp);
  InterpPushContext(interp, codeBlock);
}//

void PrimFun_iftrue(VInterp* interp)
{
  VItem* trueBlock = InterpStackPop(interp);
  VItem* trueOrFalse = InterpStackPop(interp);
  if (trueOrFalse->intNum)
    InterpPushContext(interp, trueBlock);
}//

void PrimFun_iffalse(VInterp* interp)
{
  VItem* falseBlock = InterpStackPop(interp);
  VItem* trueOrFalse = InterpStackPop(interp);
  if (! trueOrFalse->intNum)
    InterpPushContext(interp, falseBlock);
}//

void PrimFun_ifelse(VInterp* interp)
{
  VItem* falseBlock = InterpStackPop(interp);
  VItem* trueBlock = InterpStackPop(interp);
  VItem* trueOrFalse = InterpStackPop(interp);
  if (trueOrFalse->intNum)
    InterpPushContext(interp, trueBlock);
  else
    InterpPushContext(interp, falseBlock);
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

void PrimFun_local_0_set(VInterp* interp)
{
  VItem* a = InterpStackPop(interp);
  ContextSetLocalVar(interp->callStackTop->activeContext, 0, a);
}//

void PrimFun_local_0_get(VInterp* interp)
{
  VItem* a = ContextGetLocalVar(interp->callStackTop->activeContext, 0);
  InterpStackPush(interp, a);
}//

void PrimFun_local_1_set(VInterp* interp)
{
  VItem* a = InterpStackPop(interp);
  ContextSetLocalVar(interp->callStackTop->activeContext, 1, a);
}//

void PrimFun_local_1_get(VInterp* interp)
{
  VItem* a = ContextGetLocalVar(interp->callStackTop->activeContext, 1);
  InterpStackPush(interp, a);
}//

void PrimFun_nil(VInterp* interp)
{
}//

void PrimFun_isnil(VInterp* interp)
{
}//

void PrimFun_head(VInterp* interp)
{
  VItem* list = InterpStackTop(interp);

  if (!IsTypeList(list)) GURU(OBJECT_IS_NOT_A_LIST);

  VItem* head = MemItemFirst(interp->mem, list);
  if (head)
  {
    *list = *head;
  }
  else
  {
    list->addr = 0;
  }
}//

void PrimFun_tail(VInterp* interp)
{
  VItem* list = InterpStackTop(interp);

  if (!IsTypeList(list)) GURU(OBJECT_IS_NOT_A_LIST);

  VItem* head = MemItemFirst(interp->mem, list);
  if (head)
  {
    VItem* next = MemItemNext(interp->mem, head);
    if (next)
    {
      // TODO: ?? MemTail(interp->mem, list);
      MemItemSetFirst(interp->mem, list, next);
      return;
    }
  }

  list->addr = 0; // Empty list
}//

void PrimFun_cons(VInterp* interp)
{
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
