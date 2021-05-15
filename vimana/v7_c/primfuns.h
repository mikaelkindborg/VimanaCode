
/****************** PRIMFUNS ******************/

// DO evaluates a list. Other types generates an error.
void Prim_DO(Interp* interp)
{
  //PrintDebug("HELLO DO");
  Item item;
  InterpPopEvalSet(interp, item);
  //Item item = InterpPopEval(interp);
  //PrintDebug("ITEM TYPE: %u", item.type);
  // If item is a list, create a stackframe and push it onto the stack.
  if (IsList(item))
    InterpEvalList(interp, ItemList(item));
  else
    ErrorExit("DO got a non-list of type: %lu", item.type);
}

// ((X) (X X +) DOUBLE DEF
void Prim_DEF(Interp* interp)
{
  Item name;
  Item value;
  InterpPopSet(interp, name);
  InterpPopEvalSet(interp, value);
  // Check that name is a symbol.
  if (!IsSymbol(name))
    ErrorExit("DEF got a non-symbol of type: %lu", name.type);
  // Check that list has two element.
  // TODO: Check that both elements are lists.
  if (!IsList(value))
    ErrorExit("DEF got a non-list of type: %lu", value.type);
  if (ListLength(ItemList(value)) != 2)
    ErrorExit("DEF got a list of length != 2");
  // Set type to TypeFun and bind global var to list.
  value.type = value.type | TypeFun;
  InterpSetGlobalSymbolValue(interp, name.value.symbol, value);
}

// 21 ((X) (X X +) CALL
// ((X) (X X +) DOUBLE SET  21 DOUBLE CALL
void Prim_CALL(Interp* interp)
{
  // Bind params and enter new context.
}

void Prim_RECUR(Interp* interp)
{
  // Enter new context with current code list.
}

// FUN turns a list into a function.
// Example:
// ((X) () (X X +) FUN DOUBLE SET
void Prim_FUN(Interp* interp)
{
  // TODO: Set type to TypeCompiledFun
  PrintDebug("HELLO FUN");
  //Item list = InterpPopEval(interp);
  Item list;
  InterpPopEvalSet(interp, list);
  Item compiledFun = InterpCompileFun(interp, list);
  InterpPush(interp, compiledFun);
}

void Prim_IFTRUE(Interp* interp)
{
  //Item item = InterpPopEval(interp);
  //Bool truth = ItemBool(InterpPopEval(interp));

  Item item;
  Item truth;

  InterpPopEvalSet(interp, item);
  InterpPopEvalSet(interp, truth);

  if (!IsList(item))
    ErrorExit("IFTRUE got a non-list of type: %lu", item.type);
  else
  if (ItemBool(truth))
    InterpEvalList(interp, ItemList(item));
}

void Prim_IFELSE(Interp* interp)
{
  //Item branch2 = InterpPopEval(interp);
  //Item branch1 = InterpPopEval(interp);
  //Bool truth = ItemBool(InterpPopEval(interp));

  Item branch2;
  Item branch1;
  Item truth;

  InterpPopEvalSet(interp, branch2);
  InterpPopEvalSet(interp, branch1);
  InterpPopEvalSet(interp, truth);

  if (IsList(branch1) && IsList(branch2))
    if (ItemBool(truth))
      InterpEvalList(interp, ItemList(branch1));
    else
      InterpEvalList(interp, ItemList(branch2));
  else
    ErrorExit("IFELSE got a non-list items");
}

// TODO: 
// Handle SET of local vars, use current environment.
// Set the first var found.
// SETG and SETL ??
//
// SET a global symbol to a value.
// Example:
// 42 FOO SET FOO PRINTLN
void Prim_SET(Interp* interp)
{
  //PrintDebug("HELLO SET");
  
  // Get name and value.
  //Item name = InterpPop(interp);
  //Item value = InterpPopEval(interp);

  Item name;
  Item value;

  InterpPopSet(interp, name);
  InterpPopEvalSet(interp, value);

  //PrintDebug("  NAME TYPE:  %lu", name.type);
  //PrintDebug("  VALUE TYPE: %lu", value.type);

  // Check type.
  if (IsLocalVar(name))
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
    ErrorExit("SET got a non-symbol of type: %lu", name.type);
}

void Prim_DROP(Interp* interp)
{
  Index length = interp->stack->length;
  length = length - 1;
  if (length < 0) 
    ErrorExit("DROP list length < 0");
  interp->stack->length = length;
}

void Prim_DUP(Interp* interp)
{
  List* stack = interp->stack;
  Item item = ListGet(stack, ListLength(stack) - 1);
  ListPush(stack, item);
}

void Prim_SWAP(Interp* interp)
{
  List* stack = interp->stack;
  Index index1 = ListLength(stack) - 1;
  Index index2 = ListLength(stack) - 2;
  Item item1 = ListGet(stack, index1);
  Item item2 = ListGet(stack, index2);
  ListSet(stack, index2, item1);
  ListSet(stack, index1, item2);
}

void Prim_PRINT(Interp* interp)
{
  //PrintLine("*** HELLO PRINT ***");
  //Item item = InterpPopEval(interp);
  Item item;
  InterpPopEvalSet(interp, item);
  char* buf = ItemToString(item, interp);
  puts(buf);
  free(buf);
}

void Prim_PLUS(Interp* interp)
{
  //Item b = InterpPopEval(interp);
  //Item a = InterpPopEval(interp);
  //Item res;

  Item a;
  Item b;
  Item res;

  InterpPopEvalSet(interp, b);
  InterpPopEvalSet(interp, a);

  if (IsIntNum(a) && IsIntNum(b))
  {
    res.type = TypeIntNum;
    res.value.intNum = a.value.intNum + b.value.intNum;
  }
  else
  if (IsIntNum(a) && IsDecNum(b))
  {
    res.type = TypeDecNum;
    res.value.decNum = a.value.intNum + b.value.decNum;
  }
  else
  if (IsDecNum(a) && IsIntNum(b))
  {
    res.type = TypeDecNum;
    res.value.decNum = a.value.decNum + b.value.intNum;
  }
  else
  if (IsDecNum(a) && IsDecNum(b))
  {
    res.type = TypeDecNum;
    res.value.decNum = a.value.decNum + b.value.decNum;
  }
  else
    ErrorExit("Prim_PLUS: Unsupported item types");

  InterpPush(interp, res);
}

void Prim_MINUS(Interp* interp)
{
  //Item b = InterpPopEval(interp);
  //Item a = InterpPopEval(interp);
  //Item res;

  Item a;
  Item b;
  Item res;

  InterpPopEvalSet(interp, b);
  InterpPopEvalSet(interp, a);

  if (IsIntNum(a) && IsIntNum(b))
  {
    res.type = TypeIntNum;
    res.value.intNum = a.value.intNum - b.value.intNum;
  }
  else
  if (IsIntNum(a) && IsDecNum(b))
  {
    res.type = TypeDecNum;
    res.value.decNum = a.value.intNum - b.value.decNum;
  }
  else
  if (IsDecNum(a) && IsIntNum(b))
  {
    res.type = TypeDecNum;
    res.value.decNum = a.value.decNum - b.value.intNum;
  }
  else
  if (IsDecNum(a) && IsDecNum(b))
  {
    res.type = TypeDecNum;
    res.value.decNum = a.value.decNum - b.value.decNum;
  }
  else
    ErrorExit("Prim_MINUS: Unsupported item types");

  InterpPush(interp, res);
}

void Prim_TIMES(Interp* interp)
{
  //Item b = InterpPopEval(interp);
  //Item a = InterpPopEval(interp);
  //Item res;

  Item a;
  Item b;
  Item res;

  InterpPopEvalSet(interp, b);
  InterpPopEvalSet(interp, a);

  if (IsIntNum(a) && IsIntNum(b))
  {
    res.type = TypeIntNum;
    res.value.intNum = a.value.intNum * b.value.intNum;
  }
  else
  if (IsIntNum(a) && IsDecNum(b))
  {
    res.type = TypeDecNum;
    res.value.decNum = a.value.intNum * b.value.decNum;
  }
  else
  if (IsDecNum(a) && IsIntNum(b))
  {
    res.type = TypeDecNum;
    res.value.decNum = a.value.decNum * b.value.intNum;
  }
  else
  if (IsDecNum(a) && IsDecNum(b))
  {
    res.type = TypeDecNum;
    res.value.decNum = a.value.decNum * b.value.decNum;
  }
  else
    ErrorExit("Prim_TIMES: Unsupported item types");

  InterpPush(interp, res);
}

void Prim_DIV(Interp* interp)
{
  //Item b = InterpPopEval(interp);
  //Item a = InterpPopEval(interp);
  //Item res;

  Item a;
  Item b;
  Item res;

  InterpPopEvalSet(interp, b);
  InterpPopEvalSet(interp, a);

  if (IsIntNum(a) && IsIntNum(b))
  {
    res.type = TypeIntNum;
    res.value.intNum = a.value.intNum / b.value.intNum;
  }
  else
  if (IsIntNum(a) && IsDecNum(b))
  {
    res.type = TypeDecNum;
    res.value.decNum = a.value.intNum / b.value.decNum;
  }
  else
  if (IsDecNum(a) && IsIntNum(b))
  {
    res.type = TypeDecNum;
    res.value.decNum = a.value.decNum / b.value.intNum;
  }
  else
  if (IsDecNum(a) && IsDecNum(b))
  {
    res.type = TypeDecNum;
    res.value.decNum = a.value.decNum / b.value.decNum;
  }
  else
    ErrorExit("Prim_DIV: Unsupported item types");

  InterpPush(interp, res);
}

void Prim_MODULO(Interp* interp)
{
  //Item a = InterpPopEval(interp);
  //Item b = InterpPopEval(interp);

  Item a;
  Item b;
  Item res;
  res.type = TypeIntNum;

  InterpPopEvalSet(interp, b);
  InterpPopEvalSet(interp, a);

  if (IsIntNum(a) && IsIntNum(b))
    res.value.intNum = a.value.intNum % b.value.intNum;
  else
    ErrorExit("Prim_MODULO: Unsupported item types");

  InterpPush(interp, res);
}

void Prim_TRUE(Interp* interp)
{ 
  // TODO: Test this style.
  //Item item;
  //ItemInitBool(item, TRUE); // TODO: Macro
  //InterpPush(interp, item);
  InterpPush(interp, ItemWithBool(TRUE));
}

void Prim_FALSE(Interp* interp)
{
  InterpPush(interp, ItemWithBool(FALSE));
}

void Prim_NOT(Interp* interp)
{
  Item item;
  InterpPopEvalSet(interp, item);

  //Item item = InterpPopEval(interp);

  Bool x = item.value.truth;
  item.value.truth = !x;
  InterpPush(interp, item);
}

void Prim_EQ(Interp* interp)
{
  //Item a = InterpPopEval(interp);
  //Item b = InterpPopEval(interp);

  Item a;
  Item b;
  Item res;

  InterpPopEvalSet(interp, b);
  InterpPopEvalSet(interp, a);

  res.type = TypeBool;

  if (IsIntNum(a) && IsIntNum(b))
    res.value.truth = a.value.intNum == b.value.intNum;
  else
  if (IsSymbol(a) && IsSymbol(b))
    res.value.truth = a.value.symbol == b.value.symbol;
  else
  if (IsIntNum(a) && IsDecNum(b))
    res.value.truth = a.value.intNum == b.value.decNum;
  else
  if (IsDecNum(a) && IsIntNum(b))
    res.value.truth = a.value.decNum == b.value.intNum;
  else
  if (IsDecNum(a) && IsDecNum(b))
    res.value.truth = a.value.decNum == b.value.decNum;
  else
  if (IsBool(a) && IsBool(b))
    res.value.truth = a.value.truth == b.value.truth;
  else
  if (IsString(a) && IsString(b))
    res.value.truth = StringEquals(a.value.string, b.value.string);
  else
    ErrorExit("Prim_EQ: Cannot compare items");

  InterpPush(interp, res);
}

void InterpDefinePrimFuns(Interp* interp)
{
  InterpAddPrimFun("DO", &Prim_DO, interp);
  InterpAddPrimFun("FUN", &Prim_FUN, interp);
  InterpAddPrimFun("DEF", &Prim_DEF, interp);
  InterpAddPrimFun("CALL", &Prim_CALL, interp);
  InterpAddPrimFun("RECUR", &Prim_RECUR, interp);
  InterpAddPrimFun("IFTRUE", &Prim_IFTRUE, interp);
  InterpAddPrimFun("IFELSE", &Prim_IFELSE, interp);
  InterpAddPrimFun("SET", &Prim_SET, interp);
  InterpAddPrimFun("DROP", &Prim_DROP, interp);
  InterpAddPrimFun("DOC", &Prim_DROP, interp);
  InterpAddPrimFun("DUP", &Prim_DUP, interp);
  InterpAddPrimFun("SWAP", &Prim_SWAP, interp);
  InterpAddPrimFun("PRINT", &Prim_PRINT, interp);
  //InterpAddPrimFun("PRN", &Prim_PRN, interp);
  //InterpAddPrimFun("NEWLINE", &Prim_NEWLINE, interp);
  //InterpAddPrimFun("SPACE", &Prim_SPACE, interp);
  //InterpAddPrimFun("JOIN", &Prim_JOIN, interp);
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
