
/****************** PRIMFUNS ******************/

void Prim_DROP(Interp* interp)
{
  InterpPop(interp);
}

// DO evaluates a list. Other types generates an error.
void Prim_DO(Interp* interp)
{
  //PrintDebug("HELLO DO");
  Item item = InterpPopEval(interp);
  //PrintDebug("ITEM TYPE: %u", item.type);
  // If item is a list, create a stackframe and push it onto the stack.
  if (IsList(item))
    InterpEvalList(interp, ItemList(item));
  else
    ErrorExit("DO got a non-list of type: %u", item.type);
}

void Prim_IFTRUE(Interp* interp)
{
  Item item = InterpPopEval(interp);
  Bool truth = ItemBool(InterpPopEval(interp));
  if (!IsList(item))
    ErrorExit("IFTRUE got a non-list of type: %u", item.type);
  if (truth)
    InterpEvalList(interp, ItemList(item));
}

void Prim_IFELSE(Interp* interp)
{
  Item branch2 = InterpPopEval(interp);
  Item branch1 = InterpPopEval(interp);
  Bool truth = ItemBool(InterpPopEval(interp));
  if (!(IsList(branch1) && IsList(branch2)))
    ErrorExit("IFELSE got a non-list items");
  if (truth)
    InterpEvalList(interp, ItemList(branch1));
  else
    InterpEvalList(interp, ItemList(branch2));
}

// FUN turns a list into a function.
// Example:
// ((X) () (X X +) FUN DOUBLE SET
void Prim_FUN(Interp* interp)
{
  //PrintDebug("HELLO FUN");
  Item list = InterpPopEval(interp);
  Item compiledFun = InterpCompileFun(interp, list);
  InterpPush(interp, compiledFun);
}

// SET a global symbol to a value.
// Example:
// 42 FOO SET FOO PRINTLN
void Prim_SET(Interp* interp)
{
  //PrintDebug("HELLO SET");
  
  // Get name and value.
  Item name = InterpPop(interp);
  Item value = InterpPopEval(interp);

  //PrintDebug("NAME TYPE:  %u", name.type);
  //PrintDebug("VALUE TYPE: %u", value.type);

  // Check type.
  if (IsLocalSymbol(name))
  {
    //PrintDebug("LOCAL SET");
    InterpSetLocalSymbolValue(interp, name.value.symbol, value);
  }
  else if (IsSymbol(name))
  {
    //PrintDebug("GLOBAL SET");
    InterpSetGlobalSymbolValue(interp, name.value.symbol, value);
  }
  else
  {
    ErrorExit("SET  got a non-symbol of type: %u", name.type);
  }
}

void Prim_PRINTLN(Interp* interp)
{
  //PrintDebug("HELLO PRINTLN");
  Item item = InterpPopEval(interp);
  char* buf = ItemToString(item, interp);
  puts(buf);
  free(buf);
}

void Prim_PLUS(Interp* interp)
{
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  Item res = ItemPlus(b, a);
  InterpPush(interp, res);
}

void Prim_MINUS(Interp* interp)
{
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  Item res = ItemMinus(b, a);
  InterpPush(interp, res);
}

void Prim_TIMES(Interp* interp)
{
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  Item res = ItemTimes(b, a);
  InterpPush(interp, res);
}

void Prim_DIV(Interp* interp)
{
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  Item res = ItemDiv(b, a);
  InterpPush(interp, res);
}

void Prim_MODULO(Interp* interp)
{
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  Item res = ItemModulo(b, a);
  InterpPush(interp, res);
}

void Prim_TRUE(Interp* interp)
{ 
  // TODO: New style.
  Item item;
  ItemInitBool(item, TRUE); // TODO: Macro
  InterpPush(interp, item);
}

void Prim_FALSE(Interp* interp)
{
  InterpPush(interp, ItemWithBool(FALSE));
}

void Prim_NOT(Interp* interp)
{
  Item item = InterpPopEval(interp);
  Bool x = ItemBool(item);
  InterpPush(interp, ItemWithBool(!x));
}

void Prim_EQ(Interp* interp)
{
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  Bool res = ItemEquals(a, b);
  InterpPush(interp, ItemWithBool(res));
}

void InterpDefinePrimFuns(Interp* interp)
{
  InterpAddPrimFun("DROP", &Prim_DROP, interp);
  InterpAddPrimFun("DOC", &Prim_DROP, interp);
  InterpAddPrimFun("DO", &Prim_DO, interp);
  InterpAddPrimFun("IFTRUE", &Prim_IFTRUE, interp);
  InterpAddPrimFun("IFELSE", &Prim_IFELSE, interp);
  InterpAddPrimFun("FUN", &Prim_FUN, interp);
  InterpAddPrimFun("SET", &Prim_SET, interp);
  InterpAddPrimFun("PRINTLN", &Prim_PRINTLN, interp);
  InterpAddPrimFun("PRINT", &Prim_PRINTLN, interp);
  InterpAddPrimFun("+", &Prim_PLUS, interp);
  InterpAddPrimFun("-", &Prim_MINUS, interp);
  InterpAddPrimFun("*", &Prim_TIMES, interp);
  InterpAddPrimFun("/", &Prim_DIV, interp);
  InterpAddPrimFun("MODULO", &Prim_MODULO, interp);
  InterpAddPrimFun("TRUE", &Prim_TRUE, interp);
  InterpAddPrimFun("FALSE", &Prim_FALSE, interp);
  InterpAddPrimFun("NOT", &Prim_NOT, interp);
  InterpAddPrimFun("EQ", &Prim_EQ, interp);
}
