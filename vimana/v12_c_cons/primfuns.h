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
  VItem item = InterpPop(interp);
  MemPrintItem(interp->itemMem, &item);
  PrintNewLine();
}

void PrimFun_printStack(VInterp* interp)
{
  printf("print stack!\n");
}

void PrimFun_plus(VInterp* interp)
{
  VItem b = InterpPop(interp);
  VItem a = InterpPop(interp);
  a.data += b.data;
  InterpPush(interp, a);
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
