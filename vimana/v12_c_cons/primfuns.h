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

// Eval in current context
void PrimFun_eval(VInterp* interp)
{
  VItem* codeBlock = InterpPop(interp);
  InterpPushEvalStackFrame(interp, codeBlock);
}//

// Eval in new context (function call)
void PrimFun_call(VInterp* interp)
{
  VItem* codeBlock = InterpPop(interp);
  InterpPushFunCallStackFrame(interp, codeBlock);
}//

// Eval in parent context - this can be used by "macro-like" functions that
// take block arguments (lists) that refer to the context in which the appear
void PrimFun_evalInParentContext(VInterp* interp)
{
  int          index;
  VStackFrame* stackframe;

  // Find context of current function call
  for (index = interp->callStackTop; index > -1; --index)
  {
    stackframe = InterpStackFrameAt(interp, index);
    if (stackframe->context == stackframe) break;
  }

  // We want the context of the parent of the closest function call
  if (index > 1)
  {
    stackframe = InterpStackFrameAt(interp, index - 1);
  }

  // Eval in this context
  VItem* codeBlock = InterpPop(interp);
  InterpPushStackFrameWithContext(interp, codeBlock, stackframe);  
}//

void PrimFun_iftrue(VInterp* interp)
{
  VItem* trueBlock = InterpPop(interp);
  VItem* trueOrFalse = InterpPop(interp);
  if (trueOrFalse->intNum)
    InterpPushEvalStackFrame(interp, trueBlock);
}//

void PrimFun_iffalse(VInterp* interp)
{
  VItem* falseBlock = InterpPop(interp);
  VItem* trueOrFalse = InterpPop(interp);
  if (! trueOrFalse->intNum)
    InterpPushEvalStackFrame(interp, falseBlock);
}//

void PrimFun_ifelse(VInterp* interp)
{
  VItem* falseBlock = InterpPop(interp);
  VItem* trueBlock = InterpPop(interp);
  VItem* trueOrFalse = InterpPop(interp);
  if (trueOrFalse->intNum)
    InterpPushEvalStackFrame(interp, trueBlock);
  else
    InterpPushEvalStackFrame(interp, falseBlock);
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
  VItem* list = InterpStackTop(interp);
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

void PrimFun_over(VInterp* interp)
{
  InterpPush(interp, InterpStackAt(interp, 1));
}//

void PrimFun_local_setA(VInterp* interp)
{
  InterpSetLocalVar(interp, 0, InterpPop(interp));
}//

void PrimFun_local_setAB(VInterp* interp)
{
  InterpSetLocalVar(interp, 1, InterpPop(interp));
  InterpSetLocalVar(interp, 0, InterpPop(interp));
}//

void PrimFun_local_setABC(VInterp* interp)
{
  InterpSetLocalVar(interp, 2, InterpPop(interp));
  InterpSetLocalVar(interp, 1, InterpPop(interp));
  InterpSetLocalVar(interp, 0, InterpPop(interp));
}//

void PrimFun_local_setABCD(VInterp* interp)
{
  InterpSetLocalVar(interp, 3, InterpPop(interp));
  InterpSetLocalVar(interp, 2, InterpPop(interp));
  InterpSetLocalVar(interp, 1, InterpPop(interp));
  InterpSetLocalVar(interp, 0, InterpPop(interp));
}//

void PrimFun_local_getA(VInterp* interp)
{
  InterpPush(interp, InterpGetLocalVar(interp, 0));
}//

void PrimFun_local_getB(VInterp* interp)
{
  InterpPush(interp, InterpGetLocalVar(interp, 1));
}//

void PrimFun_local_getC(VInterp* interp)
{
  InterpPush(interp, InterpGetLocalVar(interp, 2));
}//

void PrimFun_local_getD(VInterp* interp)
{
  InterpPush(interp, InterpGetLocalVar(interp, 3));
}//

// Cool languages:
// https://www.tutorialspoint.com/execute_lisp_online.php
// https://github.com/phreda4/r3d4
// https://concatenative.org/wiki/view/r3
// https://worst.mitten.party

// How other languages do it:
// (cons 1 2) => (1 2)   // NewLisp
// (cons 1 2) => (1 . 2) // Lisp
// (cons 1 nil) => (1)   // Lisp
// (car '()) => nil      // Lisp

//
// Specification of Vimana list functions:
//
// 1 2 cons => error
// 1 () cons => (1)
// 1 (2) cons => (1 2)
// () () cons => (())
// () (1) cons => (() 1)
// () first => ()
// () rest => ()
// () first () cons => (())
// () () eq => 1
// (1) (1) eq => 0
// () isempty => 1
// (isempty) (() eq) def
//

void PrimFun_first(VInterp* interp)
{
  VItem* list = InterpStackTop(interp);

  // Must be a list type
  if (!IsListType(list)) GURU(FIRST_OBJECT_IS_NOT_A_LIST);

  // Get first item
  VItem* item = MemItemFirst(interp->mem, list);

  // Leave empty list on the stack
  // () first => ()
  if (NULL == item) goto Exit;

  // Copy first item to data stack
  *list = *item;
  
Exit:;
}//

void PrimFun_rest(VInterp* interp)
{
  VItem* list = InterpStackTop(interp);

  // Must be a list type
  if (!IsListType(list)) GURU(REST_OBJECT_IS_NOT_A_LIST);

  // Get first item
  VItem* item = MemItemFirst(interp->mem, list);

  // Leave empty list on the stack
  // () rest => ()
  if (NULL == item) goto Exit;

  // Get second item in the list
  item = MemItemNext(interp->mem, item);

  // If empty tail, leave empty list on the stack
  // (1) rest => ()
  if (NULL == item)
  {
    list->addr = 0;
    goto Exit;
  }

  // Set second item as first element of the list
  MemItemSetFirst(interp->mem, list, item);

Exit:;
}//

void PrimFun_cons(VInterp* interp)
{
  // Get list and item to cons
  VItem* list = InterpPop(interp);
  VItem* item = InterpStackTop(interp);

  // Must be a list type
  if (!IsListType(list)) GURU(CONS_OBJECT_IS_NOT_A_LIST);

  // This will be the new head of the cons
  VItem newList;
  ItemInit(&newList);
  ItemSetType(&newList, ItemType(list));

  // Allocate new element
  VItem* newFirst = MemAllocItem(interp->mem);
  if (NULL == newFirst) GURU(CONS_OUT_OF_MEMORY);

  // Copy item to new element
  *newFirst = *item;

  // Get first element of the list 
  VItem* first = MemItemFirst(interp->mem, list);

  if (NULL == first)
  {
    // If empty list, the new item is the last and only element
    MemItemSetNext(interp->mem, newFirst, NULL);
  }
  else
  {
    // Link new item to the first element of the list
    MemItemSetNext(interp->mem, newFirst, first);
  }

  // Set first of list to refer to the new element
  MemItemSetFirst(interp->mem, &newList, newFirst);

  // Copy new list item to data stack
  *item = newList;
}//

void PrimFun_def(VInterp* interp)
{
  PrimFun_funify(interp);
  PrimFun_swap(interp);
  PrimFun_setglobal(interp);
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
