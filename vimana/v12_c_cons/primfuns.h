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
  VItem* item = InterpPop(interp);
  MemPrintItem(interp->mem, item);
  PrintNewLine();
}//

void PrimFun_printstack(VInterp* interp)
{
  Print("STACK: ");
  MemPrintArray(interp->mem, interp->dataStack, interp->dataStackTop + 1);
  PrintNewLine();
}//

// TODO: Eval in own context? apply? funcall?
void PrimFun_eval(VInterp* interp)
{
  VItem* codeBlock = InterpPop(interp);
  InterpPushStackFrame(interp, codeBlock);
}//

void PrimFun_iftrue(VInterp* interp)
{
  VItem* trueBlock = InterpPop(interp);
  VItem* trueOrFalse = InterpPop(interp);
  if (trueOrFalse->intNum)
    InterpPushStackFrame(interp, trueBlock);
}//

void PrimFun_iffalse(VInterp* interp)
{
  VItem* falseBlock = InterpPop(interp);
  VItem* trueOrFalse = InterpPop(interp);
  if (! trueOrFalse->intNum)
    InterpPushStackFrame(interp, falseBlock);
}//

void PrimFun_ifelse(VInterp* interp)
{
  VItem* falseBlock = InterpPop(interp);
  VItem* trueBlock = InterpPop(interp);
  VItem* trueOrFalse = InterpPop(interp);
  if (trueOrFalse->intNum)
    InterpPushStackFrame(interp, trueBlock);
  else
    InterpPushStackFrame(interp, falseBlock);
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
  VItem* b = InterpPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum += b->intNum;
}//

void PrimFun_minus(VInterp* interp)
{
  VItem* b = InterpPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum -= b->intNum;
}//

void PrimFun_times(VInterp* interp)
{
  VItem* b = InterpPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum *= b->intNum;
}//

void PrimFun_div(VInterp* interp)
{
  VItem* b = InterpPop(interp);
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
  VItem* b = InterpPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum = a->intNum < b->intNum;
}//

void PrimFun_greaterthan(VInterp* interp)
{
  VItem* b = InterpPop(interp);
  VItem* a = InterpStackTop(interp);
  a->intNum = a->intNum > b->intNum;
}//

void PrimFun_eq(VInterp* interp)
{
  VItem* b = InterpPop(interp);
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

void PrimFun_drop(VInterp* interp)
{
  InterpPop(interp);
}//

void PrimFun_dup(VInterp* interp)
{
  VItem* a = InterpStackTop(interp);
  InterpPush(interp, a);
}//

void PrimFun_swap(VInterp* interp)
{
  VItem* a = InterpStackAt(interp, 0);
  VItem* b = InterpStackAt(interp, 1);
  VItem temp = *a;
  *a = *b;
  *b = temp;
}//

void PrimFun_local_setA(VInterp* interp)
{
  InterpPushContext(interp);

  VItem* itemA = InterpPop(interp);
  InterpContextSetLocalVar(interp, 0, itemA);
}//

void PrimFun_local_setAB(VInterp* interp)
{
  InterpPushContext(interp);

  VItem* itemB = InterpPop(interp);
  InterpContextSetLocalVar(interp, 1, itemB);

  VItem* itemA = InterpPop(interp);
  InterpContextSetLocalVar(interp, 0, itemA);
}//

void PrimFun_local_getA(VInterp* interp)
{
  VItem* item = InterpContextGetLocalVar(interp, 0);
  InterpPush(interp, item);
}//

void PrimFun_local_getB(VInterp* interp)
{
  VItem* item = InterpContextGetLocalVar(interp, 1);
  InterpPush(interp, item);
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
