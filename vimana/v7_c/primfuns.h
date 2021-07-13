//
// The functions is this file can be changed to alter
// the behaviour of the language. The interpreter in
// interp.h is realively "naked", and the idea behind
// this is to make the code more mallable.
//

// DECLARATIONS ------------------------------------------------

void Prim_LISTFIRST(Interp* interp);

// PRIMITIVE FUNCTIONS -----------------------------------------

// Set a global symbol to a value.
// Example:
// 42 (FOO) SETGLOBAL 
// FOO PRINTLN

// ITEM (SYMBOL) SETGLOBAL ->
void Prim_SETGLOBAL(Interp* interp)
{
  // Get name and value.
  Item name, value;
  InterpPopInto(interp, name);
  //PrintDebug("Prim_SETGLOBAL");
  //PrintDebug("  NAME TYPE:  %lu", name.type);
  // If name is quoted by a list we get the first element of the list.
  if (IsList(name))
    name = ListGet(ItemList(name), 0);
  // Check that name is a symbol.
  if (!IsSymbol(name))
    ErrorExit("Prim_SETGLOBAL: name is not a symbol");
  InterpPopInto(interp, value);
  //PrintDebug("  VALUE TYPE: %lu", value.type);
  InterpSetGlobal(interp, name, value);
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

// FUNIFY turns a list into a function.
// LIST FUNIFY -> FUNOBJ
void Prim_FUNIFY(Interp* interp)
{
  Item list;
  InterpPopInto(interp, list);
  if (!IsList(list))
    ErrorExit("Prim_FUNIFY: Got a non-list!");
  list.type = list.type | TypeFun;
  InterpPush(interp, list);
}

// ITEM (SYMBOL) SETLOCAL ->
void Prim_SETLOCAL(Interp* interp)
{
  //PrintDebug("HELLO SETLOCAL");
  Item varnames, value;
  InterpPopInto(interp, varnames);
  if (IsSymbol(varnames))
  {
    InterpPopInto(interp, value);
    InterpSetLocal(interp, varnames, value);
  }
  else
  if (IsList(varnames))
  {
    List* list = ItemList(varnames);
    int length = ListLength(list);
    for (int i = length - 1; i >= 0; --i)
    {
      Item name = ListGet(list, i);
      InterpPopInto(interp, value);
      InterpSetLocal(interp, name, value);
    }
  }
  else
  {
    ErrorExit("Prim_SETLOCAL: varnames not of TypeSymbol or TypeList");
  }
}

// Note that param stack order is (NAME) (BODY)
// (FUNNAME) (FUNBODY) DEF ->
void Prim_DEF(Interp* interp)
{
  // DEF defined in the target language:
  // (FUNIFY SWAP LISTFIRST SETGLOBAL) FUNIFY (DEF) LISTFIRST SETGLOBAL
  Prim_FUNIFY(interp);
  Prim_SWAP(interp);
  Prim_LISTFIRST(interp);
  Prim_SETGLOBAL(interp);
}

// EVAL evaluates a list. Other types generates an error.
// LIST EVAL ->
void Prim_EVAL(Interp* interp)
{
  //PrintDebug("Prim_EVAL");
  //ListPrint(interp->stack, interp);
  //ListPrint(interp->currentContext->env, interp);
  Item item;
  InterpPopInto(interp, item);
  // If item is a list, create a stackframe and push it onto the stack.
  if (IsList(item))
    //PrimEval_EvalList(interp, ItemList(item));
    InterpEnterContext(interp, ItemList(item));
  else
    ErrorExit("Prim_EVAL got a non-list");
}

// ITEM VALUE -> ITEM (evaluated)
void Prim_VALUE(Interp* interp)
{
  Item item;
  Item evalResult;
  InterpPopInto(interp, item);
  if (IsSymbol(item))
  {
    evalResult = InterpEvalSymbol(interp, item);
    //InterpEvalSymbolSetResult(interp, item, evalResult);
  }
  InterpPush(interp, evalResult); // Push value
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

  InterpPopInto(interp, list);
  InterpPopInto(interp, boolVal);

  if (!IsList(list))
    ErrorExit("Prim_IFTRUE: branch is non-list");
  if (!IsBool(boolVal))
    ErrorExit("Prim_IFTRUE: got non-bool");

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

  InterpPopInto(interp, list);
  InterpPopInto(interp, boolVal);

  if (!IsList(list))
    ErrorExit("Prim_IFFALSE: branch is non-list");
  if (!IsBool(boolVal))
    ErrorExit("Prim_IFFALSE: got non-bool");

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

  InterpPopInto(interp, branch2);
  InterpPopInto(interp, branch1);
  InterpPopInto(interp, boolVal);

  if (!IsList(branch1))
    ErrorExit("Prim_IFELSE: branch1 is non-list");
  if (!IsList(branch2))
    ErrorExit("Prim_IFELSE: branch2 is non-list");
  if (!IsBool(boolVal))
    ErrorExit("Prim_IFELSE: got non-bool of type");

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

// NUM NUM PLUS -> NUM
void Prim_PLUS(Interp* interp)
{
  Item a, b, res;

  InterpPopInto(interp, b);
  InterpPopInto(interp, a);

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

  InterpPopInto(interp, b);
  InterpPopInto(interp, a);

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

  InterpPopInto(interp, b);
  InterpPopInto(interp, a);

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

  InterpPopInto(interp, b);
  InterpPopInto(interp, a);

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

  InterpPopInto(interp, b);
  InterpPopInto(interp, a);

  res.type = TypeIntNum;

  if (IsIntNum(a) && IsIntNum(b))
    res.value.intNum = a.value.intNum % b.value.intNum;
  else
    ErrorExit("Prim_MODULO: Unsupported types");

  InterpPush(interp, res);
}

// TRUE -> TRUE
void Prim_TRUE(Interp* interp)
{ 
  Item item;
  item.type = TypeBool;
  item.value.truth = TRUE;
  InterpPush(interp, item);
}

// FALSE -> FALSE
void Prim_FALSE(Interp* interp)
{
  Item item;
  item.type = TypeBool;
  item.value.truth = TRUE;
  InterpPush(interp, item);
}

// ITEM NOT -> BOOL
void Prim_NOT(Interp* interp)
{
  Item item;

  InterpPopInto(interp, item);

  Bool x = item.value.truth;
  item.value.truth = !x;
  InterpPush(interp, item);
}

// ITEM ITEM EQ -> BOOL
void Prim_ISZERO(Interp* interp)
{
  Item item;

  InterpPopInto(interp, item);

  if (IsIntNum(item))
    item.value.truth = (0 == item.value.intNum);
  else
  if (IsDecNum(item))
    item.value.truth = (0 == item.value.decNum);
  else
    ErrorExit("Prim_ISZERO: Item is not a number");

  item.type = TypeBool;

  InterpPush(interp, item);
}

// ITEM ITEM EQ -> BOOL
void Prim_EQ(Interp* interp)
{
  Item a, b, res;

  InterpPopInto(interp, b);
  InterpPopInto(interp, a);

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
  //PrintDebug("Prim_PRINT");
  Item item;
  InterpPopInto(interp, item);
  char* buf = ItemToString(item, interp);
  puts(buf);
  free(buf);
}

// LISTNEW -> LIST
void Prim_LISTNEW(Interp* interp)
{
  //PrintDebug("Prim_LISTNEW");
  Item list;
  list.type = TypeList | TypeDynAlloc;
#ifdef USE_GC
  list.value.list = GCListCreate(interp->gc);
#else
  list.value.list = ListCreate();
#endif
  InterpPush(interp, list);
}

// LIST LENGTH -> NUM
void Prim_LISTLENGTH(Interp* interp)
{
  Item list, item;
  InterpPopInto(interp, list);
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
  InterpPopInto(interp, item);
  if (!IsList(item))
    ErrorExit("Prim_LISTFIRST: Got non-list");
  item = ListGet(ItemList(item), 0);
  InterpPush(interp, item); // Push element
}

// LIST LISTLAST -> ITEM
void Prim_LISTLAST(Interp* interp)
{
  Item item;
  InterpPopInto(interp, item);
  if (!IsList(item))
    ErrorExit("Prim_LISTLAST: Got non-list");
  item = ListGet(ItemList(item), ListLength(ItemList(item)) - 1);
  InterpPush(interp, item); // Push element
}

// LIST LISTPOP -> ITEM
void Prim_LISTPOP(Interp* interp)
{
  // TODO: DynAlloc only!
  Item list, item;
  InterpPopInto(interp, list);
  if (!IsList(list))
    ErrorExit("Prim_LISTPOP: Got non-list");
  ListPopInto(ItemList(list), item);
  InterpPush(interp, item); // Push element
}

// ITEM LIST LISTPUSH ->
void Prim_LISTPUSH(Interp* interp)
{
  //PrintDebug("Prim_LISTPUSH");
  Item item, list;
  InterpPopInto(interp, list);
  InterpPopInto(interp, item);
  if (!IsDynAlloc(list))
    ErrorExit("Prim_LISTPUSH: Got non-dynalloc item");
  ListPush(ItemList(list), item);
}

// INDEX LIST LISTGET -> ITEM
void Prim_LISTGET(Interp* interp)
{
  Item index, list, item;
  InterpPopInto(interp, list);
  InterpPopInto(interp, index);
  if (!IsIntNum(index))
    ErrorExit("Prim_LISTGET: Got non-integer index");
  if (!IsList(list))
    ErrorExit("Prim_LISTGET: Got non-list");
  item = ListGet(ItemList(list), index.value.intNum);
  InterpPush(interp, item); // Push element
}

// INDEX ITEM LIST LISTSET ->
void Prim_LISTSET(Interp* interp)
{
  Item index, list, item;
  InterpPopInto(interp, list);
  InterpPopInto(interp, item);
  InterpPopInto(interp, index);
  if (!IsIntNum(index))
    ErrorExit("Prim_LISTSET: Got non-integer index");
  if (!IsDynAlloc(list))
    ErrorExit("Prim_LISTSET: Got non-dynalloc item");
  ListSet(ItemList(list), index.value.intNum, item);
}

// LIST LISTDUP -> 
void Prim_LISTDUP(Interp* interp)
{
  Item list;
  InterpPopInto(interp, list);
  if (!IsDynAlloc(list))
    ErrorExit("Prim_LISTDUP: Got non-dynalloc item");
  ListDup(ItemList(list), ListLength(ItemList(list)) - 1);
}

// LIST LISTSWAP -> 
void Prim_LISTSWAP(Interp* interp)
{
  Item list;
  InterpPopInto(interp, list);
  if (!IsDynAlloc(list))
    ErrorExit("Prim_LISTSWAP: Got non-dynalloc item");
  ListSwap(ItemList(list));
}

// LIST LISTDROP -> 
void Prim_LISTDROP(Interp* interp)
{
  Item list;
  InterpPopInto(interp, list);
  if (!IsDynAlloc(list))
    ErrorExit("Prim_LISTDROP: Got non-dynalloc item");
  ListDrop(ItemList(list));
}

// GC -> 
void Prim_GC(Interp* interp)
{
#ifdef USE_GC
  InterpGC(interp);
#endif
}

void DefinePrimFuns(Interp* interp)
{
  InterpAddPrimFun("setGlobal", Prim_SETGLOBAL, interp);
  InterpAddPrimFun("drop", Prim_DROP, interp);
  InterpAddPrimFun("doc", Prim_DROP, interp);
  InterpAddPrimFun("dup", Prim_DUP, interp);
  InterpAddPrimFun("over", Prim_OVER, interp);
  InterpAddPrimFun("swap", Prim_SWAP, interp);
  InterpAddPrimFun("funify", Prim_FUNIFY, interp);
  InterpAddPrimFun("def", Prim_DEF, interp);
  InterpAddPrimFun("set", Prim_SETLOCAL, interp);
  InterpAddPrimFun("=>", Prim_SETLOCAL, interp);
  InterpAddPrimFun("eval", Prim_EVAL, interp);
  InterpAddPrimFun("value", Prim_VALUE, interp);
  InterpAddPrimFun("call", Prim_CALL, interp);
  InterpAddPrimFun("ifTrue", Prim_IFTRUE, interp);
  InterpAddPrimFun("ifFalse", Prim_IFFALSE, interp);
  InterpAddPrimFun("ifElse", Prim_IFELSE, interp);
  InterpAddPrimFun("+", Prim_PLUS, interp);
  InterpAddPrimFun("-", Prim_MINUS, interp);
  InterpAddPrimFun("*", Prim_TIMES, interp);
  InterpAddPrimFun("/", Prim_DIV, interp);
  InterpAddPrimFun("Modulo", Prim_MODULO, interp);
  InterpAddPrimFun("true", Prim_TRUE, interp);
  InterpAddPrimFun("false", Prim_FALSE, interp);
  InterpAddPrimFun("not", Prim_NOT, interp);
  InterpAddPrimFun("isZero", Prim_ISZERO, interp);
  InterpAddPrimFun("eq", Prim_EQ, interp);
  InterpAddPrimFun("print", Prim_PRINT, interp);
  InterpAddPrimFun("listNew", Prim_LISTNEW, interp);
  InterpAddPrimFun("length", Prim_LISTLENGTH, interp);
  InterpAddPrimFun("first", Prim_LISTFIRST, interp);
  InterpAddPrimFun("last", Prim_LISTLAST, interp);
  InterpAddPrimFun("pop", Prim_LISTPOP, interp);
  InterpAddPrimFun("push", Prim_LISTPUSH, interp);
  InterpAddPrimFun("getAt", Prim_LISTGET, interp);
  InterpAddPrimFun("setAt", Prim_LISTSET, interp);
  InterpAddPrimFun("listDup", Prim_LISTDUP, interp);
  InterpAddPrimFun("listSwap", Prim_LISTSWAP, interp);
  InterpAddPrimFun("listDrop", Prim_LISTDROP, interp);
  InterpAddPrimFun("gc", Prim_GC, interp);
  //InterpAddPrimFun("Recur", Prim_RECUR, interp);
  //InterpAddPrimFun("Quote", Prim_QUOTE, interp);
  //InterpAddPrimFun(":", Prim_QUOTE, interp);
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
  InterpSetLocal(interp, symbol, item);
}
*/
