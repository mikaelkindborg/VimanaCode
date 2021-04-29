
/****************** PRIMFUNS ******************/

// DO evaluates a list. Other types generates an error.
void Prim_DO(Interp* interp)
{
  printf("HELLO DO\n");
  Item item = InterpPop(interp);
  printf("ITEM TYPE: %u\n", item.type);
  // If item is a list, create a stackframe and push it onto the stack.
  if (IsList(item.type))
  {
    InterpPushStackFrame(interp, item.value.list);
  }
  else
  {
    printf("ERROR: DO GOT A NON-LIST\n");
    exit(0);
  }
}

// SET a global symbol to a value.
// Example:
// 42 FOO SET FOO PRINTLN
void Prim_SET(Interp* interp)
{
  printf("HELLO SET\n");
  Item name = InterpPop(interp);
  Item value = InterpPop(interp);
  printf("NAME TYPE:  %u\n", name.type);
  printf("VALUE TYPE: %u\n", value.type);
  Index i = name.value.symbol;
  ListSet(interp->symbolValueTable, i, value);
}

void Prim_PRINTLN(Interp* interp)
{
  // TODO: Make function to get Item as string in list.h
  
  printf("HELLO PRINTLN\n");
  Item item = InterpPopEval(interp);
  // TODO: Check type.
  printf("ITEM TYPE: %u\n", item.type);
  if (IsIntNum(item.type))
  {
    printf("%li\n", item.value.intNum);
  }
  else if (IsSymbol(item.type))
  {
    char* string = InterpGetSymbolString(interp, item.value.symbol);
    if (NULL == string)
    {
      printf("ERROR: PRINTLN SYMBOL HAS NO STRING\n");
      exit(0);
    }
    printf("%s\n", string);
  }
}

void Prim_PLUS(Interp* interp)
{
  //printf("HELLO PLUS\n");
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  // TODO: Check type.
  long res = a.value.intNum + b.value.intNum;
  InterpPush(interp, ItemWithIntNum(res));
}

void InterpDefinePrimFuns(Interp* interp)
{
  InterpAddPrimFun("DO", &Prim_DO, interp);
  InterpAddPrimFun("SET", &Prim_SET, interp);
  InterpAddPrimFun("PRINTLN", &Prim_PRINTLN, interp);
  InterpAddPrimFun("+", &Prim_PLUS, interp);
}
