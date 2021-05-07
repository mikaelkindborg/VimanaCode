
/****************** PRIMFUNS ******************/

// DO evaluates a list. Other types generates an error.
void Prim_DO(Interp* interp)
{
  PrintDebug("HELLO DO");
  
  Item item = InterpPop(interp);
  PrintDebug("ITEM TYPE: %u", item.type);
  // If item is a list, create a stackframe and push it onto the stack.
  if (IsList(item))
  {
    InterpPushStackFrame(interp, ItemList(item), NULL);
  }
  else
  {
    ErrorExit("DO got a non-list of type: %u", item.type);
  }
}

// FUN turns a list into a function.
// Example:
// ((X) () (X X +) FUN DOUBLE SET
void Prim_FUN(Interp* interp)
{
  PrintDebug("HELLO FUN");
  
  Item list = InterpPopEval(interp);
  Item compiledFun = InterpCompileFun(interp, list);
  InterpPush(interp, compiledFun);
}

// SET a global symbol to a value.
// Example:
// 42 FOO SET FOO PRINTLN
void Prim_SET(Interp* interp)
{
  PrintDebug("HELLO SET");
  
  // Get name and value.
  Item name = InterpPop(interp);
  Item value = InterpPopEval(interp);

  PrintDebug("NAME TYPE:  %u", name.type);
  PrintDebug("VALUE TYPE: %u", value.type);

  // Check type.
  if (IsLocalSymbol(name))
  {
    PrintDebug("LOCAL SET");
    InterpSetLocalSymbolValue(interp, name.value.symbol, value);
  }
  else if (IsSymbol(name))
  {
    PrintDebug("GLOBAL SET");
    InterpSetGlobalSymbolValue(interp, name.value.symbol, value);
  }
  else
  {
    ErrorExit("SET  got a non-symbol of type: %u", name.type);
  }
}

void Prim_PRINTLN(Interp* interp)
{
  // TODO: Make function to get Item as string in list.h
  
  PrintDebug("HELLO PRINTLN");
  Item item = InterpPopEval(interp);
  char* buf = ItemToString(item, interp);
  puts(buf);
  free(buf);
}

void Prim_PLUS(Interp* interp)
{
  char buf[128];
  //PrintDebug("HELLO PLUS\n");
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  Item res = ItemAdd(a, b);
  InterpPush(interp, res);
  
  //long res = a.value.intNum + b.value.intNum;
  //InterpPush(interp, ItemWithIntNum(res));
}

void Prim_TRUE(Interp* interp)
{
  InterpPush(interp, ItemWithBool(TRUE));
}

void Prim_FALSE(Interp* interp)
{
  InterpPush(interp, ItemWithBool(FALSE));
}

void Prim_EQ(Interp* interp)
{
  // TODO
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
  InterpAddPrimFun("TRUE", &Prim_TRUE, interp);
  InterpAddPrimFun("FALSE", &Prim_FALSE, interp);
}
