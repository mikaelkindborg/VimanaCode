/*
File: primfuns.h
Author: Mikael Kindborg (mikael@kindborg.com)
*/

void PrimFun_sayHi(VInterp* interp)
{
  printf("Hi World!\n");
}

void PrimFun_print(VInterp* interp)
{
  VItem* item = InterpPop(interp);
  MemPrintItem(interp->mem, item);
  PrintNewLine();
}

void PrimFun_printStack(VInterp* interp)
{
  printf("print stack!\n");
}

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

void PrimFun_plus(VInterp* interp)
{
  VItem* b = InterpPop(interp);
  VItem* a = InterpPop(interp);
  VItem result;
  ItemInit(&result);
  result.intNum = a->intNum + b->intNum;
  ItemSetType(&result, TypeIntNum);
  InterpPush(interp, &result);
}

void PrimFun_setglobal(VInterp* interp)
{
  VItem* list = InterpPop(interp);
  VItem* value = InterpPop(interp);
  VItem* symbol = MemItemFirst(interp->mem, list);
  InterpSetGlobalVar(interp, symbol->intNum, value);
}

void PrimFun_funify(VInterp* interp)
{
  VItem* list = & (interp->dataStack[interp->dataStackTop]);
  ItemSetType(list, TypeFun);
}

typedef struct __PrimFunEntry
{
  char*       name;
  VPrimFunPtr fun;
}
PrimFunEntry;

PrimFunEntry GPrimFunTable[] = 
{
  { "sayHi", PrimFun_sayHi },
  { "print", PrimFun_print },
  { "printStack", PrimFun_printStack },
  { "+", PrimFun_plus },
  { "setglobal", PrimFun_setglobal },
  { "funify", PrimFun_funify },
  { "__sentinel__", NULL }
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
