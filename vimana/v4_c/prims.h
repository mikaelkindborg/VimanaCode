
/****************** PRIMS ******************/

void Prim_PRINTLN(Interp* interp)
{
  printf("HELLO PRINTLN\n");
  Item item = InterpPopEval(interp);
  // TODO: Check type.
  char* string = InterpGetSymbolString(interp, item.symbolIndex);
  printf("%s\n", string);
}

void Prim_PLUS(Interp* interp)
{
  printf("HELLO PLUS\n");
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  // TODO: Check type.
  long res = a.data.intNum + b.data.intNum;
  InterpPushIntNum(interp, res);
}

void InterpDefinePrimFuns(Interp* interp)
{
  InterpAddPrimFun("PRINTLN", &Prim_PRINTLN, interp);
  InterpAddPrimFun("+", &Prim_PLUS, interp);
}
