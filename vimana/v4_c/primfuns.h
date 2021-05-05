
/****************** PRIMFUNS ******************/

// DO evaluates a list. Other types generates an error.
void Prim_DO(Interp* interp)
{
  printf("HELLO DO\n");
  
  Item item = InterpPop(interp);
  printf("ITEM TYPE: %u\n", item.type);
  // If item is a list, create a stackframe and push it onto the stack.
  if (IsList(item))
  {
    InterpPushStackFrame(interp, item.value.list);
  }
  else
  {
    printf("ERROR: DO GOT A NON-LIST\n");
    exit(0);
  }
}

// FUN turns a list into a function.
// Example:
// ((X) () (X X +) FUN DOUBLE SET
void Prim_FUN(Interp* interp)
{
  printf("HELLO FUN\n");
  
  Item list = InterpPopEval(interp);
  Item compiledFun = InterpCompileFun(interp, list);
  InterpPush(interp, compiledFun);
}

// SET a global symbol to a value.
// Example:
// 42 FOO SET FOO PRINTLN
void Prim_SET(Interp* interp)
{
  printf("HELLO SET\n");
  
  Item name = InterpPop(interp);
  Item value = InterpPopEval(interp);
  printf("NAME TYPE:  %u\n", name.type);
  printf("VALUE TYPE: %u\n", value.type);
  Index i = name.value.symbol;
  ListSet(interp->symbolValueTable, i, value);
  
  // TODO: Set local variable
}

void Prim_PRINTLN(Interp* interp)
{
  // TODO: Make function to get Item as string in list.h
  
  printf("HELLO PRINTLN\n");
  Item item = InterpPopEval(interp);
  char buf[128];
  ItemToString(item, buf, interp);
  PrintLine("%s", buf);
}

void Prim_PLUS(Interp* interp)
{
  char buf[128];
  //printf("HELLO PLUS\n");
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  Item res = ItemAdd(a, b);
  InterpPush(interp, res);
  
  //long res = a.value.intNum + b.value.intNum;
  //InterpPush(interp, ItemWithIntNum(res));
}

void InterpDefinePrimFuns(Interp* interp)
{
  InterpAddPrimFun("DO", &Prim_DO, interp);
  InterpAddPrimFun("do", &Prim_DO, interp);
  InterpAddPrimFun("FUN", &Prim_FUN, interp);
  InterpAddPrimFun("fun", &Prim_FUN, interp);
  InterpAddPrimFun("SET", &Prim_SET, interp);
  InterpAddPrimFun("set", &Prim_SET, interp);
  InterpAddPrimFun("PRINTLN", &Prim_PRINTLN, interp);
  InterpAddPrimFun("println", &Prim_PRINTLN, interp);
  InterpAddPrimFun("+", &Prim_PLUS, interp);
}
