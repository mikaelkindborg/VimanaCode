
// EVAL PRIMITIVES ---------------------------------------------

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

#ifdef OPTIMIZE
#define PrimEval_SetLocal(interp, name, value) \
do { \
  if (!IsSymbol(name)) \
    ErrorExit("PrimEval_SetLocal: Got a non-symbol of type: %lu", (name).type); \
  Context* context = (interp)->currentContext; \
  while (context && (!context->hasEnv)) \
    context = context->prevContext; \
  ListAssocSetGet(context->env, (name).value.symbol, &(value)); \
} while(0)
#else
void PrimEval_SetLocal(Interp* interp, Item name, Item value)
{
  if (!IsSymbol(name))
    ErrorExit("PrimEval_SetLocal: Got a non-symbol of type: %lu", name.type);

  // Get first context that has an environment.
  Context* context = interp->currentContext;
  while (context && (!context->hasEnv))
  {
    PrintDebug("Lookup prev context");
    context = context->prevContext;
  }

  // Error checking.
  if (!context)
    ErrorExit("PrimEval_SetLocal: Context not found");
  if (!context->hasEnv)
    ErrorExit("PrimEval_SetLocal: Context has no environment");

  // Set symbol value.
  //ListAssocSet(context->env, name.value.symbol, &value);
  ListAssocSetGet(context->env, name.value.symbol, &value);

  //PrintDebug("PrimEval_SetLocal: PRINTING ENV");
  //ListPrint(context->env, interp);
}
#endif

Item PrimEval_EvalSymbol(Interp* interp, Item item)
{
#ifndef OPTIMIZE
  // Non-symbols evaluates to themselves.
  if (!IsSymbol(item))
    return item;
#endif

  // Lookup local symbol.
  //if (IsLocalVar(item))
  //{
    // Search contexts for the symbol.
    Context* context = interp->currentContext;
    while (context)
    {
      if (context && context->hasEnv)
      {
        //PrintDebug("PrimEval_EvalSymbol: PRINTING ENV");
        //ListPrint(context->env, interp);
        //Item* value = ListAssocGet(context->env, item.value.symbol);
        Item* value = ListAssocSetGet(context->env, item.value.symbol, NULL);
        if (value)
          return *value;
      }
      context = context->prevContext;
    }
  //}

  // Lookup global symbol.
  Item value = ListGet(interp->globalValueTable, item.value.symbol);
  if (TypeVirgin != value.type) 
    return value;

  // Symbol is unbound and evaluates to itself.
  return item;
}

// PRIMITIVE FUNCTIONS -----------------------------------------

// SET a global symbol to a value.
// Example:
// 42 FOO SET  
// FOO PRINTLN
void Prim_SET(Interp* interp)
{
  //PrintDebug("HELLO SET");
  
  // Get name and value.
  Item name, value;
  InterpPopInto(interp, name);
  //PrintDebug("  NAME TYPE:  %lu", name.type);
  InterpPopEvalInto(interp, value);
  //PrintDebug("  VALUE TYPE: %lu", value.type);
  PrimEval_SetGlobal(interp, value, name);
}

void Prim_DROP(Interp* interp)
{
  Index length = interp->stack->length;
  length = length - 1;
  if (length < 0) 
    ErrorExit("DROP: list length < 0");
  ItemRefCountDecr(interp->stack->items[length]);
  ItemGC(interp->stack->items[length]);
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
  Item item1 = stack->items[index1];
  stack->items[index1] = stack->items[index2];
  stack->items[index2] = item1;
}

// FUN turns a list into a function.
// Examples/ideas:
// (X SETLOCAL X X +) FUN DOUBLE SET
// (X PARAM X X +) FUN DOUBLE SET
// ((X Y) SETLOCAL X Y +) FUN MYADD SET
// ((X Y) => X Y +) FUN MYADD SET
// ((X Y) : X Y +) FUN MYADD SET
// ((X Y) (X Y +)) FUN MYADD SET // This one requires bindning by EvalCore_EvalFun()
// (Y : X : X Y +) FUN MYADD SET
// (Y PARAM X PARAM X Y +) FUN MYADD SET
void Prim_FUN(Interp* interp)
{
  //PrintDebug("HELLO FUN");
  Item list;
  InterpPopEvalInto(interp, list);
  if (!IsList(list))
    ErrorExit("FUN: Got a non-list!");
  list.type = list.type | TypeFun;
  InterpPush(interp, list);
}

void Prim_SYMBOL(Interp* interp)
{
  Item item;
  InterpPopInto(interp, item);
  item = ListGet(ItemList(item), 0);
  InterpPush(interp, item); // Push symbol
}

void Prim_VALUE(Interp* interp)
{
  Item item;
  InterpPopInto(interp, item);
  item = ListGet(ItemList(item), 0);
  item = PrimEval_EvalSymbol(interp, item);
  InterpPush(interp, item); // Push value
}

void Prim_SETLOCAL(Interp* interp)
{
  //PrintDebug("HELLO SETLOCAL");
  Item item, value;
  InterpPopInto(interp, item);
  if (IsSymbol(item))
  {
    InterpPopEvalInto(interp, value);
    PrimEval_SetLocal(interp, item, value);
  }
  else
  if (IsList(item))
  {
    List* list = ItemList(item);
    int length = ListLength(list);
    for (int i = length - 1; i >= 0; --i)
    {
      Item name = ListGet(list, i);
      InterpPopEvalInto(interp, value);
      PrimEval_SetLocal(interp, name, value);
    }
  }
  else
  {
    ErrorExit("Prim_SETLOCAL: VAR not of TypeSymbol or TypeList");
  }
}

// Don't evaluate next element, just push it onto the data stack.
void Prim_QUOTE(Interp* interp)
{
  //PrintDebug("HELLO QUOTE");
  
  Context* context = interp->currentContext;
  int codePointer = ++ context->codePointer;
  List* code = context->code;
  if (codePointer < ListLength(code))
  {
    // Get the next element and push it.
    Item element = ListGet(code, codePointer);
    ListPush(interp->stack, element);
  }
}

void Prim_DEF(Interp* interp)
{
  //PrintDebug("HELLO DEF");

  // (SWAP FUN SWAP SET) FUN : DEF SET
  Prim_SWAP(interp);
  Prim_FUN(interp);
  Prim_SWAP(interp);
  Prim_SET(interp);
}

// DO evaluates a list. Other types generates an error.
void Prim_DO(Interp* interp)
{
  //PrintDebug("HELLO DO");
  //ListPrint(interp->stack, interp);
  //ListPrint(interp->currentContext->env, interp);
  Item item;
  InterpPopEvalInto(interp, item);
  // If item is a list, create a stackframe and push it onto the stack.
  if (IsList(item))
    //PrimEval_EvalList(interp, ItemList(item));
    InterpEnterContext(interp, ItemList(item), ContextCurrentEnv);
  else
    ErrorExit("Prim_DO got a non-list of type: %lu", item.type);
}

// 21 ((X) => X X +) CALL
// ((X) => X X +) DOUBLE SET  21 DOUBLE CALL
void Prim_CALL(Interp* interp)
{
  // Enter new context with empty env.
}

/*
void Prim_RECUR(Interp* interp)
{
  PrintDebug("HELLO RECUR");

  // TODO: Add info to context about current function and use that code list recur.

  // Below code does not work, need to get the function context.
  // Enter new context with current code list.
  //PrimEval_EvalFun(interp, interp->currentContext->code);
}
*/

void Prim_IFTRUE(Interp* interp)
{
  Item list, boolVal;

  InterpPopEvalInto(interp, list);
  InterpPopEvalInto(interp, boolVal);

  if (!IsList(list))
    ErrorExit("IFTRUE: branch is non-list of type: %lu", list.type);
  if (!IsBool(boolVal))
    ErrorExit("IFTRUE: got non-bool of type: %lu", boolVal.type);

  if (boolVal.value.truth)
  {
    //PrimEval_EvalList(interp, ItemList(list));
    InterpEnterContext(interp, ItemList(list), ContextCurrentEnv);
  }
}

void Prim_IFELSE(Interp* interp)
{
  Item branch2, branch1, boolVal;

  InterpPopEvalInto(interp, branch2);
  InterpPopEvalInto(interp, branch1);
  InterpPopEvalInto(interp, boolVal);

  if (!IsList(branch1))
    ErrorExit("IFELSE: branch1 is non-list of type: %lu", branch1.type);
  if (!IsList(branch2))
    ErrorExit("IFELSE: branch2 is non-list of type: %lu", branch2.type);
  if (!IsBool(boolVal))
    ErrorExit("IFTRUE: got non-bool of type: %lu", boolVal.type);

  if (boolVal.value.truth)
  {
    //PrimEval_EvalList(interp, ItemList(branch1));
    InterpEnterContext(interp, ItemList(branch1), ContextCurrentEnv); 
  }
  else
  {
    //PrimEval_EvalList(interp, ItemList(branch2));
    InterpEnterContext(interp, ItemList(branch2), ContextCurrentEnv);
  }
}

void Prim_PLUS(Interp* interp)
{
  Item a, b, res;

  InterpPopEvalInto(interp, b);
  InterpPopEvalInto(interp, a);

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

  InterpPopEvalInto(interp, b);
  InterpPopEvalInto(interp, a);

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

  InterpPopEvalInto(interp, b);
  InterpPopEvalInto(interp, a);

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

  InterpPopEvalInto(interp, b);
  InterpPopEvalInto(interp, a);

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

  InterpPopEvalInto(interp, b);
  InterpPopEvalInto(interp, a);

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

  InterpPopEvalInto(interp, item);

  Bool x = item.value.truth;
  item.value.truth = !x;
  InterpPush(interp, item);
}

void Prim_EQ(Interp* interp)
{
  Item a, b, res;

  InterpPopEvalInto(interp, b);
  InterpPopEvalInto(interp, a);

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
  //PrintDebug("HELLO PRINT");
  Item item;
  InterpPopEvalInto(interp, item);
  char* buf = ItemToString(item, interp);
  puts(buf);
  free(buf);
  ItemGC(item);
}

void Prim_LISTNEW(Interp* interp)
{
  PrintDebug("HELLO LISTNEW");
  Item item;
  List* list = ListCreate();
  item.type = TypeList | TypeDynAlloc;
  item.value.list = list;
  InterpPush(interp, item);
}

void DefinePrimFuns(Interp* interp)
{
  InterpAddPrimFun("SET", &Prim_SET, interp);
  InterpAddPrimFun("DROP", &Prim_DROP, interp);
  InterpAddPrimFun("DOC", &Prim_DROP, interp);
  InterpAddPrimFun("DUP", &Prim_DUP, interp);
  InterpAddPrimFun("SWAP", &Prim_SWAP, interp);
  InterpAddPrimFun("FUN", &Prim_FUN, interp);
  InterpAddPrimFun("SYMBOL", &Prim_SYMBOL, interp);
  InterpAddPrimFun("VALUE", &Prim_VALUE, interp);
  InterpAddPrimFun("DEF", &Prim_DEF, interp);
  InterpAddPrimFun("SETLOCAL", Prim_SETLOCAL, interp);
  InterpAddPrimFun("POP", Prim_SETLOCAL, interp);
  InterpAddPrimFun("=>", Prim_SETLOCAL, interp);
  InterpAddPrimFun("QUOTE", Prim_QUOTE, interp);
  InterpAddPrimFun(":", Prim_QUOTE, interp);
  InterpAddPrimFun("DO", &Prim_DO, interp);
  InterpAddPrimFun("CALL", &Prim_CALL, interp);
  //InterpAddPrimFun("RECUR", &Prim_RECUR, interp);
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
  InterpAddPrimFun("LISTNEW", &Prim_LISTNEW, interp);
  //InterpAddPrimFun("PRN", &Prim_PRN, interp);
  //InterpAddPrimFun("NEWLINE", &Prim_NEWLINE, interp);
  //InterpAddPrimFun("SPACE", &Prim_SPACE, interp);
  //InterpAddPrimFun("JOIN", &Prim_JOIN, interp);
}
