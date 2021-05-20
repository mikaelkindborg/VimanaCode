
// EVAL PRIMS --------------------------------------------------

//
// The following functions can be changed to alter
// the behaviour of the language. The interpreter in
// interp.h is realively "naked" and the idea behind
// this is to make the code more modular.
//

void PrimEval_EvalList(Interp* interp, List* list)
{
  // Enter new context with current env.
  InterpEnterContext(interp, list, ContextCurrentEnv);
}

void PrimEval_EvalFun(Interp* interp, List* fun)
{
  // Just push it on the callstack, binding is done by primitives
  InterpEnterContext(interp, fun, ContextNewEnv);
}

void PrimEval_SetGlobal(Interp* interp, Item value, Item name)
{
  if (IsSymbol(name))
    InterpSetGlobalSymbolValue(interp, name.value.symbol, value);
  else
    ErrorExit("SET: Got a non-symbol of type: %lu", name.type);
}

void PrimEval_SetLocal(Interp* interp, Item value, Item name)
{
  if (!IsSymbol(name))
    ErrorExit("SETLOCAL: Got a non-symbol of type: %lu", name.type);

  // Get first context that has an environment.
  Context* context = interp->currentContext;
  while (context && (!context->hasEnv))
  {
    PrintDebug("Lookup prev context");
    context = context->prevContext;
  }

  // Error checking.
  if (!context)
    ErrorExit("SETLOCAL: Context not found");
  if (!context->hasEnv)
    ErrorExit("SETLOCAL: Context has no environment");

  // Set symbol value.
  ListAssocSet(context->env, name.value.symbol, &value);
  PrintDebug("SETLOCAL: PRINTING ENV");
  ListPrint(context->env, interp);
}

Item PrimEval_EvalSymbol(Interp* interp, Item item)
{
  // Lookup local symbol.
  //if (IsSymbol(item) && IsLocalVar(item))
  if (IsSymbol(item))
  {
    // Get first context that has an environment.
    Context* context = interp->currentContext;
    while (context && (!context->hasEnv))
    {
      context = context->prevContext;
    }

    if (context && context->hasEnv)
    {
      PrintDebug("PrimEval_EvalSymbol: PRINTING ENV");
      ListPrint(context->env, interp);
      Item* value = ListAssocGet(context->env, item.value.symbol);
      if (value)
        return *value;
    }
  }

  // Lookup global symbol.
  //if (IsSymbol(item) && !IsLocalVar(item))
  if (IsSymbol(item))
  {
    Item value = ListGet(interp->globalValueTable, item.value.symbol);
    if (TypeVirgin != value.type) 
      return value;
  }

  // Item not a variable, evaluates to itself.
  return item;
}

// PRIM FUNS ---------------------------------------------------

// SET a global symbol to a value.
// Example:
// 42 FOO SET  
// FOO PRINTLN
void Prim_SET(Interp* interp)
{
  //PrintDebug("HELLO SET");
  
  // Get name and value.
  Item name, value;
  InterpPopSet(interp, name);
  InterpPopEvalSet(interp, value);
  PrimEval_SetGlobal(interp, value, name);

  //PrintDebug("  NAME TYPE:  %lu", name.type);
  //PrintDebug("  VALUE TYPE: %lu", value.type);
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

// FUN turns a list into a function.
// Examples/ideas:
// (X SETLOCAL X X +) FUN DOUBLE SET
// (X PARAM X X +) FUN DOUBLE SET
// ((X Y) SETLOCAL X Y +) FUN MYADD SET
// ((X Y) => X Y +) FUN MYADD SET
// ((X Y) : X Y +) FUN MYADD SET
// ((X Y) (X Y +)) FUN MYADD SET // This one requires bindning by EvalCore_EvalFun()
// (X : Y : X Y +) FUN MYADD SET
// (X PARAM Y PARAM X Y +) FUN MYADD SET
void Prim_FUN(Interp* interp)
{
  //PrintDebug("HELLO FUN");
  Item list;
  InterpPopEvalSet(interp, list);
  list.type = TypeFun | TypeList;
  InterpPush(interp, list);
}

void Prim_SETLOCAL(Interp* interp)
{
  PrintDebug("HELLO SETLOCAL");
  Item item, name;
  InterpPopSet(interp, name);
  InterpPopEvalSet(interp, item);
  PrimEval_SetLocal(interp, item, name);
}

void Prim_DEF(Interp* interp)
{
  //PrintDebug("HELLO DEF");
  // (SWAP FUN SWAP SET) FUN DEF SET
  Prim_SWAP(interp);
  Prim_FUN(interp);
  Prim_SWAP(interp);
  Prim_SET(interp);
}

// DO evaluates a list. Other types generates an error.
void Prim_DO(Interp* interp)
{
  //PrintDebug("HELLO DO");
  Item item;
  InterpPopEvalSet(interp, item);
  // If item is a list, create a stackframe and push it onto the stack.
  if (IsList(item))
    PrimEval_EvalList(interp, ItemList(item));
  else
    ErrorExit("Prim_DO got a non-list of type: %lu", item.type);
}

// 21 ((X) => X X +) CALL
// ((X) => X X +) DOUBLE SET  21 DOUBLE CALL
void Prim_CALL(Interp* interp)
{
  // Enter new context with empty env.
}

void Prim_RECUR(Interp* interp)
{
  // Enter new context with current code list.
}

void Prim_IFTRUE(Interp* interp)
{
  Item item, truth;
  InterpPopEvalSet(interp, item);
  InterpPopEvalSet(interp, truth);

  if (!IsList(item))
    ErrorExit("IFTRUE got a non-list of type: %lu", item.type);
  else
  if (ItemBool(truth))
    PrimEval_EvalList(interp, ItemList(item));
}

void Prim_IFELSE(Interp* interp)
{
  Item branch2, branch1, truth;
  InterpPopEvalSet(interp, branch2);
  InterpPopEvalSet(interp, branch1);
  InterpPopEvalSet(interp, truth);

  if (IsList(branch1) && IsList(branch2))
    if (ItemBool(truth))
      PrimEval_EvalList(interp, ItemList(branch1));
    else
      PrimEval_EvalList(interp, ItemList(branch2));
  else
    ErrorExit("IFELSE got a non-list items");
}

void Prim_PLUS(Interp* interp)
{
  Item a, b, res;

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
  Item a, b, res;

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
  Item a, b, res;

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
  Item a, b, res;

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
  Item a, b, res;

  InterpPopEvalSet(interp, b);
  InterpPopEvalSet(interp, a);

  res.type = TypeIntNum;

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
  Item a, b, res;

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

void Prim_PRINT(Interp* interp)
{
  PrintDebug("HELLO PRINT");
  Item item;
  InterpPopEvalSet(interp, item);
  char* buf = ItemToString(item, interp);
  puts(buf);
  free(buf);
}

void DefinePrimFuns(Interp* interp)
{
  InterpAddPrimFun("SET", &Prim_SET, interp);
  InterpAddPrimFun("DROP", &Prim_DROP, interp);
  InterpAddPrimFun("DOC", &Prim_DROP, interp);
  InterpAddPrimFun("DUP", &Prim_DUP, interp);
  InterpAddPrimFun("SWAP", &Prim_SWAP, interp);
  InterpAddPrimFun("FUN", &Prim_FUN, interp);
  InterpAddPrimFun("DEF", &Prim_DEF, interp);
  InterpAddPrimFun("SETLOCAL", Prim_SETLOCAL, interp);
  InterpAddPrimFun("DO", &Prim_DO, interp);
  InterpAddPrimFun("CALL", &Prim_CALL, interp);
  InterpAddPrimFun("RECUR", &Prim_RECUR, interp);
  InterpAddPrimFun("IFTRUE", &Prim_IFTRUE, interp);
  InterpAddPrimFun("IFELSE", &Prim_IFELSE, interp);
  InterpAddPrimFun("+", &Prim_PLUS, interp);
  InterpAddPrimFun("-", &Prim_MINUS, interp);
  InterpAddPrimFun("*", &Prim_TIMES, interp);
  InterpAddPrimFun("/", &Prim_DIV, interp);
  InterpAddPrimFun("MODULO", &Prim_MODULO, interp);
  InterpAddPrimFun("TRUE", &Prim_TRUE, interp);
  InterpAddPrimFun("FALSE", &Prim_FALSE, interp);
  InterpAddPrimFun("NOT", &Prim_NOT, interp);
  InterpAddPrimFun("EQ", &Prim_EQ, interp);
  InterpAddPrimFun("PRINT", &Prim_PRINT, interp);
  //InterpAddPrimFun("PRN", &Prim_PRN, interp);
  //InterpAddPrimFun("NEWLINE", &Prim_NEWLINE, interp);
  //InterpAddPrimFun("SPACE", &Prim_SPACE, interp);
  //InterpAddPrimFun("JOIN", &Prim_JOIN, interp);
}
