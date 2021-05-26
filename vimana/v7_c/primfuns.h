//
// The functions is this file can be changed to alter
// the behaviour of the language. The interpreter in
// interp.h is realively "naked", and the idea behind
// this is to make the code more mallable.
//

// DECLARATIONS ------------------------------------------------

void Prim_LISTFIRST(Interp* interp);

// EVAL PRIMITIVES ---------------------------------------------

void PrimEval_SetGlobal(Interp* interp, Item value, Item name)
{
  if (IsSymbol(name))
    InterpSetGlobalSymbolValue(interp, name.value.symbol, value);
  else
    ErrorExit("PrimEval_SetGlobal: Got a non-symbol of type: %lu", name.type);
}

#ifdef OPTIMIZE
#define PrimEval_SetLocal(interp, name, item) \
  do { \
    if (!IsSymbol(name)) \
      ErrorExit("PrimEval_SetLocal: Got a non-symbol of type: %lu", (name).type); \
    Context* context = (interp)->currentContext; \
    while (context && (!context->hasEnv)) \
      context = context->prevContext; \
    ListAssocSetGet(context->env, (name).value.symbol, &(item)); \
  } while (0)
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

// ITEM SYMBOL SET ->
void Prim_SET(Interp* interp)
{
  // Get name and value.
  Item name, value;
  InterpPopInto(interp, name);
  //PrintDebug("  NAME TYPE:  %lu", name.type);
  InterpPopEvalInto(interp, value);
  //PrintDebug("  VALUE TYPE: %lu", value.type);
  PrimEval_SetGlobal(interp, value, name);
}

// ITEM DROP ->
void Prim_DROP(Interp* interp)
{
  ListDrop(interp->stack);
}

// ITEM DUP -> ITEM ITEM
void Prim_DUP(Interp* interp)
{
  ListDup(interp->stack, ListLength(interp->stack) - 1);
}

// ITEM1 ITEM2 OVER -> ITEM1 ITEM2 ITEM1
void Prim_OVER(Interp* interp)
{
  ListDup(interp->stack, ListLength(interp->stack) - 2);
}

// ITEM1 ITEM2 SWAP -> ITEM2 ITEM1
void Prim_SWAP(Interp* interp)
{
  ListSwap(interp->stack);
}

// FUN turns a list into a function.
// LIST FUN -> FUNOBJ
void Prim_FUN(Interp* interp)
{
  Item list;
  InterpPopEvalInto(interp, list);
  if (!IsList(list))
    ErrorExit("Prim_FUN: Got a non-list!");
  list.type = list.type | TypeFun;
  InterpPush(interp, list);
}

// ITEM SYMBOL SETLOCAL ->
void Prim_SETLOCAL(Interp* interp)
{
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

// Order is BODY NAME
// (FUNBODY) (FUNNAME) DEF ->
void Prim_DEF(Interp* interp)
{
  // (SWAP FUN SWAP FIRST SET) FUN (DEF) LISTFIRST SET
  Prim_SWAP(interp);
  Prim_FUN(interp);
  Prim_SWAP(interp);
  Prim_LISTFIRST(interp);
  Prim_SET(interp);
}

// Note that order is NAME BODY
// (FUNNAME) (FUNBODY) DEFINE ->
void Prim_DEFINE(Interp* interp)
{
  // (FUN SWAP FIRST SET) FUN (DEFINE) LISTFIRST SET
  Prim_FUN(interp);
  Prim_SWAP(interp);
  Prim_LISTFIRST(interp);
  Prim_SET(interp);
}

// EVAL evaluates a list. Other types generates an error.
// LIST EVAL ->
void Prim_EVAL(Interp* interp)
{
  //PrintDebug("Prim_EVAL");
  //ListPrint(interp->stack, interp);
  //ListPrint(interp->currentContext->env, interp);
  Item item;
  InterpPopEvalInto(interp, item);
  // If item is a list, create a stackframe and push it onto the stack.
  if (IsList(item))
    //PrimEval_EvalList(interp, ItemList(item));
    InterpEnterContext(interp, ItemList(item));
  else
    ErrorExit("Prim_EVAL got a non-list of type: %lu", item.type);
}

// 21 ((X) => X X +) CALL
// ((X) => X X +) DOUBLE SET  21 DOUBLE CALL
void Prim_CALL(Interp* interp)
{
  // Enter new context with empty env.
}

// BOOL LIST IFTRUE ->
void Prim_IFTRUE(Interp* interp)
{
  Item list, boolVal;

  InterpPopEvalInto(interp, list);
  InterpPopEvalInto(interp, boolVal);

  if (!IsList(list))
    ErrorExit("Prim_IFTRUE: branch is non-list of type: %lu", list.type);
  if (!IsBool(boolVal))
    ErrorExit("Prim_IFTRUE: got non-bool of type: %lu", boolVal.type);

  if (boolVal.value.truth)
  {
    //PrimEval_EvalList(interp, ItemList(list));
    InterpEnterContext(interp, ItemList(list));
  }
}

// BOOL LIST IFALSE ->
void Prim_IFFALSE(Interp* interp)
{
  Item list, boolVal;

  InterpPopEvalInto(interp, list);
  InterpPopEvalInto(interp, boolVal);

  if (!IsList(list))
    ErrorExit("Prim_IFFALSE: branch is non-list of type: %lu", list.type);
  if (!IsBool(boolVal))
    ErrorExit("Prim_IFFALSE: got non-bool of type: %lu", boolVal.type);

  if (!boolVal.value.truth)
  {
    //PrimEval_EvalList(interp, ItemList(list));
    InterpEnterContext(interp, ItemList(list));
  }
}

// BOOL LIST LIST IFELSE ->
void Prim_IFELSE(Interp* interp)
{
  Item branch2, branch1, boolVal;

  InterpPopEvalInto(interp, branch2);
  InterpPopEvalInto(interp, branch1);
  InterpPopEvalInto(interp, boolVal);

  if (!IsList(branch1))
    ErrorExit("Prim_IFELSE: branch1 is non-list of type: %lu", branch1.type);
  if (!IsList(branch2))
    ErrorExit("Prim_IFELSE: branch2 is non-list of type: %lu", branch2.type);
  if (!IsBool(boolVal))
    ErrorExit("Prim_IFELSE: got non-bool of type: %lu", boolVal.type);

  if (boolVal.value.truth)
  {
    //PrimEval_EvalList(interp, ItemList(branch1));
    InterpEnterContext(interp, ItemList(branch1)); 
  }
  else
  {
    //PrimEval_EvalList(interp, ItemList(branch2));
    InterpEnterContext(interp, ItemList(branch2));
  }
}

void Prim_GOTOIFTRUE(Interp* interp)
{
  Item codePointer, boolVal;

  InterpPopEvalInto(interp, codePointer);
  InterpPopEvalInto(interp, boolVal);

  if (!IsIntNum(codePointer))
    ErrorExit("Prim_GOTOIFTRUE: Expected TypeIntNum");
  if (!IsBool(boolVal))
    ErrorExit("Prim_GOTOIFTRUE: Expected TypeBool");

  if (boolVal.value.truth)
  {
    Context* context = interp->currentContext;
    context->codePointer = codePointer.value.intNum - 1;
    PrintDebug("GOTO codepointer: %i", context->codePointer);
  }
}

void Prim_GOTOIFFALSE(Interp* interp)
{
  Item codePointer, boolVal;

  InterpPopEvalInto(interp, codePointer);
  InterpPopEvalInto(interp, boolVal);

  if (!IsIntNum(codePointer))
    ErrorExit("Prim_GOTOIFFALSE: Expected TypeIntNum");
  if (!IsBool(boolVal))
    ErrorExit("Prim_GOTOIFFALSE: Expected TypeBool");

  if (!boolVal.value.truth)
  {
    Context* context = interp->currentContext;
    context->codePointer = codePointer.value.intNum - 1;
    PrintDebug("GOTO codepointer: %i", context->codePointer);
  }
}

// NUM NUM PLUS -> NUM
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
    ErrorExit("Prim_PLUS: Unsupported types");

  InterpPush(interp, res);
}

// NUM NUM MINUS -> NUM
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
    ErrorExit("Prim_MINUS: Unsupported types");

  InterpPush(interp, res);
}

// NUM NUM TIMES -> NUM
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
    ErrorExit("Prim_TIMES: Unsupported types");

  InterpPush(interp, res);
}

// NUM NUM DIV -> NUM
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
    ErrorExit("Prim_DIV: Unsupported types");

  InterpPush(interp, res);
}

// INT MODULO -> INT
void Prim_MODULO(Interp* interp)
{
  Item a, b, res;

  InterpPopEvalInto(interp, b);
  InterpPopEvalInto(interp, a);

  res.type = TypeIntNum;

  if (IsIntNum(a) && IsIntNum(b))
    res.value.intNum = a.value.intNum % b.value.intNum;
  else
    ErrorExit("Prim_MODULO: Unsupported types");

  InterpPush(interp, res);
}

// TRUE -> BOOL
void Prim_TRUE(Interp* interp)
{ 
  // TODO: Test this style.
  //Item item;
  //ItemInitBool(item, TRUE); // TODO: Macro
  //InterpPush(interp, item);
  InterpPush(interp, ItemWithBool(TRUE));
}

// FALSE -> FALSE
void Prim_FALSE(Interp* interp)
{
  InterpPush(interp, ItemWithBool(FALSE));
}

// ITEM NOT -> BOOL
void Prim_NOT(Interp* interp)
{
  Item item;

  InterpPopEvalInto(interp, item);

  Bool x = item.value.truth;
  item.value.truth = !x;
  InterpPush(interp, item);
}

// ITEM ITEM EQ -> BOOL
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

// ITEM PRINT ->
void Prim_PRINT(Interp* interp)
{
  //PrintDebug("HELLO PRINT");
  Item item;
  InterpPopEvalInto(interp, item);
  char* buf = ItemToString(item, interp);
  puts(buf);
  free(buf);
  ItemRefCountDecr(item);
  ItemGC(item);
}

// LISTNEW -> LIST
void Prim_LISTNEW(Interp* interp)
{
  //PrintDebug("HELLO LISTNEW");
  Item list;
  list.type = TypeList | TypeDynAlloc;
  list.value.list = ListCreate();
  InterpPush(interp, list);
}

// LIST LENGTH -> NUM
void Prim_LISTLENGTH(Interp* interp)
{
  Item list, item;
  InterpPopEvalInto(interp, list);
  if (!IsList(list))
    ErrorExit("Prim_LISTLENGTH: Got non-list");
  item.type = TypeIntNum;
  item.value.intNum = ListLength(ItemList(list));
  InterpPush(interp, item); // Push length
}

// LIST LISTFIRST -> ITEM
void Prim_LISTFIRST(Interp* interp)
{
  Item item;
  InterpPopEvalInto(interp, item);
  if (!IsList(item))
    ErrorExit("Prim_LISTFIRST: Got non-list");
  item = ListGet(ItemList(item), 0);
  InterpPush(interp, item); // Push element
}

// LIST LISTLAST -> ITEM
void Prim_LISTLAST(Interp* interp)
{
  Item item;
  InterpPopEvalInto(interp, item);
  if (!IsList(item))
    ErrorExit("Prim_LISTLAST: Got non-list");
  item = ListGet(ItemList(item), ListLength(ItemList(item)) - 1);
  InterpPush(interp, item); // Push element
}

// LIST LISTPOP -> ITEM
void Prim_LISTPOP(Interp* interp)
{
  Item item;
  InterpPopEvalInto(interp, item);
  if (!IsList(item))
    ErrorExit("Prim_LISTPOP: Got non-list");
  item = ListPop(ItemList(item));
  InterpPush(interp, item); // Push element
}

// LIST ITEM LISTPUSH ->
void Prim_LISTPUSH(Interp* interp)
{
  Item item, list;
  InterpPopEvalInto(interp, item);
  InterpPopEvalInto(interp, list);
  if (!IsList(item))
    ErrorExit("Prim_LISTPUSH: Got non-list");
  ListPush(ItemList(list), item);
}

// LIST INDEX LISTGET -> ITEM
void Prim_LISTGET(Interp* interp)
{
  Item index, list, item;
  InterpPopEvalInto(interp, index);
  InterpPopEvalInto(interp, list);
  if (!IsIntNum(index))
    ErrorExit("Prim_LISTGET: Got non-integer index");
  if (!IsList(list))
    ErrorExit("Prim_LISTGET: Got non-list");
  item = ListGet(ItemList(list), index.value.intNum);
  InterpPush(interp, item); // Push element
}

// LIST INDEX ITEM LISTSET ->
void Prim_LISTSET(Interp* interp)
{
  Item index, list, item;
  InterpPopEvalInto(interp, item);
  InterpPopEvalInto(interp, index);
  InterpPopEvalInto(interp, list);
  if (!IsIntNum(index))
    ErrorExit("Prim_LISTSET: Got non-integer index");
  if (!IsList(list))
    ErrorExit("Prim_LISTSET: Got non-list");
  ListSet(ItemList(list), index.value.intNum, item);
}

// LIST LISTDUP -> 
void Prim_LISTDUP(Interp* interp)
{
  Item list;
  InterpPopEvalInto(interp, list);
  if (!IsList(list))
    ErrorExit("Prim_LISTDUP: Got non-list");
  ListDup(ItemList(list), ListLength(ItemList(list)) - 1);
}

// LIST LISTSWAP -> 
void Prim_LISTSWAP(Interp* interp)
{
  Item list;
  InterpPopEvalInto(interp, list);
  if (!IsList(list))
    ErrorExit("Prim_LISTSWAP: Got non-list");
  ListSwap(ItemList(list));
}

// LIST LISTDROP -> 
void Prim_LISTDROP(Interp* interp)
{
  Item list;
  InterpPopEvalInto(interp, list);
  if (!IsList(list))
    ErrorExit("Prim_LISTDROP: Got non-list");
  ListDrop(ItemList(list));
}

void DefinePrimFuns(Interp* interp)
{
  InterpAddPrimFun("Set", Prim_SET, interp);
  InterpAddPrimFun("Drop", Prim_DROP, interp);
  InterpAddPrimFun("Doc", Prim_DROP, interp);
  InterpAddPrimFun("Dup", Prim_DUP, interp);
  InterpAddPrimFun("Over", Prim_OVER, interp);
  InterpAddPrimFun("Swap", Prim_SWAP, interp);
  InterpAddPrimFun("Fun", Prim_FUN, interp);
  InterpAddPrimFun("Def", Prim_DEF, interp);
  InterpAddPrimFun("Define", Prim_DEFINE, interp);
  InterpAddPrimFun("SetLocal", Prim_SETLOCAL, interp);
  InterpAddPrimFun("=>", Prim_SETLOCAL, interp);
  InterpAddPrimFun("Eval", Prim_EVAL, interp);
  InterpAddPrimFun("Call", Prim_CALL, interp);
  InterpAddPrimFun("IfTrue", Prim_IFTRUE, interp);
  InterpAddPrimFun("IfFalse", Prim_IFTRUE, interp);
  InterpAddPrimFun("IfElse", Prim_IFELSE, interp);
  InterpAddPrimFun("GotoIfTrue", Prim_GOTOIFTRUE, interp);
  InterpAddPrimFun("GotoIfFalse", Prim_GOTOIFFALSE, interp);
  InterpAddPrimFun("+", Prim_PLUS, interp);
  InterpAddPrimFun("-", Prim_MINUS, interp);
  InterpAddPrimFun("*", Prim_TIMES, interp);
  InterpAddPrimFun("/", Prim_DIV, interp);
  InterpAddPrimFun("Modulo", Prim_MODULO, interp);
  InterpAddPrimFun("True", Prim_TRUE, interp);
  InterpAddPrimFun("False", Prim_FALSE, interp);
  InterpAddPrimFun("Not", Prim_NOT, interp);
  InterpAddPrimFun("Eq", Prim_EQ, interp);
  InterpAddPrimFun("Print", Prim_PRINT, interp);
  InterpAddPrimFun("ListNew", Prim_LISTNEW, interp);
  InterpAddPrimFun("ListLength", Prim_LISTLENGTH, interp);
  InterpAddPrimFun("ListFirst", Prim_LISTFIRST, interp);
  InterpAddPrimFun("ListLast", Prim_LISTFIRST, interp);
  InterpAddPrimFun("ListPop", Prim_LISTPOP, interp);
  InterpAddPrimFun("ListPush", Prim_LISTPUSH, interp);
  InterpAddPrimFun("ListGet", Prim_LISTGET, interp);
  InterpAddPrimFun("ListSet", Prim_LISTSET, interp);
  InterpAddPrimFun("ListDup", Prim_LISTDUP, interp);
  InterpAddPrimFun("ListSwap", Prim_LISTSWAP, interp);
  InterpAddPrimFun("ListDrop", Prim_LISTDROP, interp);
  //InterpAddPrimFun("Recur", Prim_RECUR, interp);
  //InterpAddPrimFun("Quote", Prim_QUOTE, interp);
  //InterpAddPrimFun(":", Prim_QUOTE, interp);
  //InterpAddPrimFun("Value", Prim_VALUE, interp);
  //InterpAddPrimFun("Label", Prim_LABEL, interp);
  //InterpAddPrimFun("PRN", Prim_PRN, interp);
  //InterpAddPrimFun("NEWLINE", Prim_NEWLINE, interp);
  //InterpAddPrimFun("SPACE", Prim_SPACE, interp);
  //InterpAddPrimFun("JOIN", Prim_JOIN, interp);
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

/*
// (ITEM) VALUE -> ITEM (evaluated)
void Prim_VALUE(Interp* interp)
{
  Item item;
  InterpPopInto(interp, item);
  item = ListGet(ItemList(item), 0);
  item = PrimEval_EvalSymbol(interp, item);
  InterpPush(interp, item); // Push value
}
*/

/*
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
*/

/*
void Prim_LABEL(Interp* interp)
{
  Item symbol, item;

  InterpPopInto(interp, symbol);

  if (!IsSymbol(symbol))
    ErrorExit("Prim_LABEL: Expected TypeSymbol");

  Context* context = interp->currentContext;
  IntNum codePointer = context->codePointer + 1;
  item.type = TypeIntNum;
  item.value.intNum = codePointer;
  PrimEval_SetLocal(interp, symbol, item);
}
*/

/*
void PrimEval_EvalList(Interp* interp, List* list)
{
  // Enter new context with current env.
  InterpEnterContext(interp, list);
}
*/

/*
void PrimEval_EvalFun(Interp* interp, List* fun)
{
  // Just push it on the callstack, binding is done by primitives
  TODO: InterpEnterContext(interp, fun, ContextNewEnv);
}
*/