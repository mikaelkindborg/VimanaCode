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
  //ListDup(interp->stack, ListLength(interp->stack) - 1);

  List* stack = interp->stack;
  int length = stack->length + 1;
  if (length > stack->maxLength)
    ListGrow(stack, length + ListGrowIncrement);
  stack->length = length;
  Item* itemPtr2 = ListItemPtr(stack, length - 1);
  Item* itemPtr1 = itemPtr2 - 1;
  *itemPtr2 = *itemPtr1;
}

// ITEM1 ITEM2 2DUP -> ITEM1 ITEM2 ITEM1 ITEM2
void Prim_2DUP(Interp* interp)
{
  ListDup(interp->stack, ListLength(interp->stack) - 2);
  ListDup(interp->stack, ListLength(interp->stack) - 2);
}

// ITEM1 ITEM2 OVER -> ITEM1 ITEM2 ITEM1
void Prim_OVER(Interp* interp)
{
  ListDup(interp->stack, ListLength(interp->stack) - 2);
}

// ITEM1 ITEM2 SWAP -> ITEM2 ITEM1
void Prim_SWAP(Interp* interp)
{
  //ListSwap(interp->stack);

  List* stack = interp->stack;
  Item* itemPtr2 = ListItemPtr(stack, stack->length - 1);
  Item* itemPtr1 = itemPtr2 - 1;
  Item temp = *itemPtr2;
  *itemPtr2 = *itemPtr1;
  *itemPtr1 = temp;
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

// FUNIFYSPECIAL turns a list into a special function.
// LIST FUNIFYSPECIAL -> FUNOBJ
void Prim_FUNIFYSPECIAL(Interp* interp)
{
  Item list;
  InterpPopInto(interp, list);
  if (!IsList(list))
    ErrorExit("Prim_FUNIFYSPECIAL: Got a non-list!");
  list.type = list.type | TypeSpecialFun;
  InterpPush(interp, list);
}

// ITEM (SYMBOL) SETLOCAL ->
void Prim_SETLOCAL(Interp* interp)
{
  //PrintDebug("HELLO SETLOCAL");
  Item varnames, value;
  InterpPopInto(interp, varnames);
/*
  if (IsSymbol(varnames))
  {
    InterpPopInto(interp, value);
    InterpSetLocal(interp, varnames, value);
  }
  else
*/
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

// (FUNNAME) (FUNBODY) DEFSPECIAL ->
void Prim_DEFSPECIAL(Interp* interp)
{
  // DEFSPECIAL defined in the target language:
  // (FUNIFY SWAP LISTFIRST SETGLOBAL) FUNIFYSPECIAL (DEFSPECIAL) LISTFIRST SETGLOBAL
  Prim_FUNIFYSPECIAL(interp);
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
  if (IsClosure(item)) // Must test IsClosure before IsList
  {
    List* closure = ItemList(item);
    Item code = ListGet(closure, 0);
    Item env = ListGet(closure, 1);
    InterpEnterContextWithEnv(interp, ItemList(code), ItemList(env));
  }
  else
  if (IsList(item))
  {
    InterpEnterContext(interp, ItemList(item));
  }
  else
  {
    ErrorExit("Prim_EVAL got a non-list");
  }
}

// Create a closure bound to the current environment.
// Note: Use only with GC.
// LIST BIND -> CLOSURE
void Prim_BIND(Interp* interp)
{
  Item closure;
  Item code;
  List* env;

  closure = InterpListItemCreate(interp);
  closure.type = closure.type | TypeClosure;

  InterpPopInto(interp, code);
  ListPush(ItemList(closure), code);

  env = interp->currentContext->env;
  env->isShared = TRUE;
  ListPush(ItemList(closure), ItemWithList(env));

  InterpPush(interp, closure);
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
  // TODO: Enter new context with empty env.
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
    InterpEnterContext(interp, ItemList(list));
}

// FASTER
// BOOL LIST IFALSE ->
void Prim_IFFALSE(Interp* interp)
{
  List* stack = interp->stack;
  int length = stack->length - 2;
  Item* boolPtr = ListItemPtr(stack, length);
  Item* listPtr = boolPtr + 1;

  if (!IsList(*listPtr))
    ErrorExit("Prim_IFFALSE: branch is non-list");
  if (!IsBool(*boolPtr))
    ErrorExit("Prim_IFFALSE: got non-bool");

  stack->length = length;

  if (!(boolPtr->value.truth))
    InterpEnterContext(interp, ItemList(*listPtr));
}

// Just another style, but the one above feels more clean.
// BOOL LIST IFALSE ->
void X_Prim_IFFALSE(Interp* interp)
{
  List* stack = interp->stack;
  int length = stack->length;
  Item* listPtr = ListItemPtr(stack, length - 1);
  Item* boolPtr = listPtr - 1;

  if (!IsList(*listPtr))
    ErrorExit("Prim_IFFALSE: branch is non-list");
  if (!IsBool(*boolPtr))
    ErrorExit("Prim_IFFALSE: got non-bool");

  stack->length = length - 2;

  if (!(boolPtr->value.truth))
    InterpEnterContext(interp, ItemList(*listPtr));
}

// SLOWER
// BOOL LIST IFALSE ->
void ORIG_Prim_IFFALSE(Interp* interp)
{
  Item list, boolVal;

  InterpPopInto(interp, list);
  InterpPopInto(interp, boolVal);

  if (!IsList(list))
    ErrorExit("Prim_IFFALSE: branch is non-list");
  if (!IsBool(boolVal))
    ErrorExit("Prim_IFFALSE: got non-bool");
  
  if (!boolVal.value.truth)
    InterpEnterContext(interp, ItemList(list));
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
    InterpEnterContext(interp, ItemList(branch1));
  else
    InterpEnterContext(interp, ItemList(branch2));
}

// NUM NUM PLUS -> NUM
void Prim_PLUS(Interp* interp)
{
  List* stack = interp->stack;
  int length = -- stack->length;
  if (length < 1) ErrorExit("Prim_PLUS: STACK IS EMPTY");
  Item* b = stack->items + length;
  Item* a = b - 1;

  if (IsIntNum(*a) && IsIntNum(*b))
  {
    a->type = TypeIntNum;
    a->value.intNum = a->value.intNum + b->value.intNum;
  }
  else
  if (IsIntNum(*a) && IsDecNum(*b))
  {
    a->type = TypeDecNum;
    a->value.decNum = a->value.intNum + b->value.decNum;
  }
  else
  if (IsDecNum(*a) && IsIntNum(*b))
  {
    a->type = TypeDecNum;
    a->value.decNum = a->value.decNum + b->value.intNum;
  }
  else
  if (IsDecNum(*a) && IsDecNum(*b))
  {
    a->type = TypeDecNum;
    a->value.decNum = a->value.decNum + b->value.decNum;
  }
  else
    ErrorExit("Prim_PLUS: Unsupported types");
}

// NUM NUM PLUS -> NUM
void ORIG_Prim_PLUS(Interp* interp)
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
  List* stack = interp->stack;
  int length = -- stack->length; // TODO: ListDrop(stack);
  // TODO: < 2
  if (length < 1) ErrorExit("Prim_MINUS: STACK IS EMPTY");
  Item* b = stack->items + length;
  // TODO: ListItemPtr(stack, length);
  // TODO: ListLastItemPtr()
  Item* a = b - 1;
  // TODO: ListDrop(stack);
  // TODO: ListDrop2(stack);

  if (IsIntNum(*a) && IsIntNum(*b))
  {
    a->type = TypeIntNum;
    a->value.intNum = a->value.intNum - b->value.intNum;
  }
  else
  if (IsIntNum(*a) && IsDecNum(*b))
  {
    a->type = TypeDecNum;
    a->value.decNum = a->value.intNum - b->value.decNum;
  }
  else
  if (IsDecNum(*a) && IsIntNum(*b))
  {
    a->type = TypeDecNum;
    a->value.decNum = a->value.decNum - b->value.intNum;
  }
  else
  if (IsDecNum(*a) && IsDecNum(*b))
  {
    a->type = TypeDecNum;
    a->value.decNum = a->value.decNum - b->value.decNum;
  }
  else
    ErrorExit("Prim_MINUS: Unsupported types");
}

// NUM NUM MINUS -> NUM
void ORIG_Prim_MINUS(Interp* interp)
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

// NUM ADD1 -> NUM
void Prim_ADD1(Interp* interp)
{
  Item a, res;

  InterpPopInto(interp, a);

  if (IsIntNum(a))
  {
    res.type = TypeIntNum;
    res.value.intNum = a.value.intNum + 1;
  }
  else
  if (IsDecNum(a))
  {
    res.type = TypeDecNum;
    res.value.decNum = a.value.decNum + 1;
  }
  else
    ErrorExit("Prim_ADD1: Unsupported types");

  InterpPush(interp, res);
}

// NUM SUB1 -> NUM
void Prim_SUB1(Interp* interp)
{
  Item* a = interp->stack->items + (interp->stack->length - 1);

  if (IsIntNum(*a))
  {
    a->value.intNum = a->value.intNum - 1;
  }
  else
  if (IsDecNum(*a))
  {
    a->value.decNum = a->value.decNum - 1;
  }
  else
    ErrorExit("Prim_SUB1: Unsupported types");
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
  item.value.truth = FALSE;
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

// ITEM 0 EQ -> BOOL
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

// ITEM 0 > -> BOOL
void Prim_GTZERO(Interp* interp)
{
  Item item;

  InterpPopInto(interp, item);

  if (IsIntNum(item))
    item.value.truth = (0 < item.value.intNum);
  else
  if (IsDecNum(item))
    item.value.truth = (0 < item.value.decNum);
  else
    ErrorExit("Prim_GTZERO: Item is not a number");

  item.type = TypeBool;

  InterpPush(interp, item);
}

// ITEM 0 < -> BOOL
void Prim_LTZERO(Interp* interp)
{
  Item item;

  InterpPopInto(interp, item);

  if (IsIntNum(item))
    item.value.truth = (0 > item.value.intNum);
  else
  if (IsDecNum(item))
    item.value.truth = (0 > item.value.decNum);
  else
    ErrorExit("Prim_LTZERO: Item is not a number");

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
    res.value.truth = a.symbol == b.symbol;
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
    res.value.truth = StrEquals(StringStr(a.value.string), StringStr(b.value.string));
  else
    ErrorExit("Prim_EQ: Cannot compare items");

  InterpPush(interp, res);
}

// ITEM ITEM > -> BOOL
void Prim_GREATERTHAN(Interp* interp)
{
  Item a, b, res;

  InterpPopInto(interp, b);
  InterpPopInto(interp, a);

  res.type = TypeBool;

  if (IsIntNum(a) && IsIntNum(b))
    res.value.truth = a.value.intNum > b.value.intNum;
  else
  if (IsIntNum(a) && IsDecNum(b))
    res.value.truth = a.value.intNum > b.value.decNum;
  else
  if (IsDecNum(a) && IsIntNum(b))
    res.value.truth = a.value.decNum > b.value.intNum;
  else
  if (IsDecNum(a) && IsDecNum(b))
    res.value.truth = a.value.decNum > b.value.decNum;
  else
    ErrorExit("Prim_GREATERTHAN: Cannot compare items");

  InterpPush(interp, res);
}

// ITEM ITEM < -> BOOL
void Prim_LESSTHAN(Interp* interp)
{

  List* stack = interp->stack;
  int length = -- stack->length;
  if (length < 1) ErrorExit("Prim_PLUS: STACK IS EMPTY");
  Item* b = stack->items + length;
  Item* a = b - 1;

  if (IsIntNum(*a) && IsIntNum(*b))
    a->value.truth = a->value.intNum < b->value.intNum;
  else
  if (IsIntNum(*a) && IsDecNum(*b))
    a->value.truth = a->value.intNum < b->value.decNum;
  else
  if (IsDecNum(*a) && IsIntNum(*b))
    a->value.truth = a->value.decNum < b->value.intNum;
  else
  if (IsDecNum(*a) && IsDecNum(*b))
    a->value.truth = a->value.decNum < b->value.decNum;
  else
    ErrorExit("Prim_LESSTHAN: Cannot compare items");

  a->type = TypeBool;
}

// ITEM ITEM < -> BOOL
void ORIG_Prim_LESSTHAN(Interp* interp)
{
  Item a, b, res;

  InterpPopInto(interp, b);
  InterpPopInto(interp, a);

  res.type = TypeBool;

  if (IsIntNum(a) && IsIntNum(b))
    res.value.truth = a.value.intNum < b.value.intNum;
  else
  if (IsIntNum(a) && IsDecNum(b))
    res.value.truth = a.value.intNum < b.value.decNum;
  else
  if (IsDecNum(a) && IsIntNum(b))
    res.value.truth = a.value.decNum < b.value.intNum;
  else
  if (IsDecNum(a) && IsDecNum(b))
    res.value.truth = a.value.decNum < b.value.decNum;
  else
    ErrorExit("Prim_LESSTHAN: Cannot compare items");

  InterpPush(interp, res);
}

// LISTNEW -> LIST
void Prim_LISTNEW(Interp* interp)
{
  //PrintDebug("Prim_LISTNEW");
  Item list = InterpListItemCreate(interp);
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

// LIST INDEX LISTGET -> ITEM
void Prim_LISTGET(Interp* interp)
{
  Item index, list, item;
  InterpPopInto(interp, index);
  InterpPopInto(interp, list);
  if (!IsIntNum(index))
    ErrorExit("Prim_LISTGET: Got non-integer index");
  if (!IsList(list))
    ErrorExit("Prim_LISTGET: Got non-list");
  item = ListGet(ItemList(list), index.value.intNum);
  InterpPush(interp, item); // Push element
}

//mylist 2 bar setat
// LIST INDEX ITEM LISTSET ->
void Prim_LISTSET(Interp* interp)
{
  Item index, list, item;
  InterpPopInto(interp, item);
  InterpPopInto(interp, index);
  InterpPopInto(interp, list);
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


// ITEM PRINT ->
void Prim_PRINT(Interp* interp)
{
  Item item;
  InterpPopInto(interp, item);
  char* buf = ItemToString(item, interp);
  puts(buf);
  free(buf);
}

// PRINTSTACK ->
void Prim_PRINTSTACK(Interp* interp)
{
  Item stack = ItemWithList(interp->stack);
  char* buf = ItemToString(stack, interp);
  puts(buf);
  free(buf);
}

// STRING EVALFILE ->
void Prim_EVALFILE(Interp* interp)
{
  Item item;
  InterpPopInto(interp, item);
  if (!IsString(item))
    ErrorExit("Prim_EVALFILE: Got non-string");
  // TODO: Read file
}

// LIST TOSTRING ->
void Prim_TOSTRING(Interp* interp)
{
  Item item;
  InterpPopInto(interp, item);
  if (!IsList(item))
    ErrorExit("Prim_TOSTRING: Got non-list");
  char* buf = ListToString(ItemList(item), interp);
  InterpPush(interp, ItemWithString(buf));
}

/*
// UNUSED - USE LISTS TO QUOTE OBJECTS
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

void DefinePrimFuns(Interp* interp)
{
  InterpAddPrimFun("setGlobal", Prim_SETGLOBAL, interp);
  InterpAddPrimFun("drop", Prim_DROP, interp);
  InterpAddPrimFun("doc", Prim_DROP, interp);
  InterpAddPrimFun("dup", Prim_DUP, interp);
  InterpAddPrimFun("2dup", Prim_2DUP, interp);
  InterpAddPrimFun("over", Prim_OVER, interp);
  InterpAddPrimFun("swap", Prim_SWAP, interp);
  InterpAddPrimFun("funify", Prim_FUNIFY, interp);
  InterpAddPrimFun("funifySpecial", Prim_FUNIFYSPECIAL, interp);
  InterpAddPrimFun("def", Prim_DEF, interp);
  InterpAddPrimFun("defSpecial", Prim_DEFSPECIAL, interp);
  InterpAddPrimFun("set", Prim_SETLOCAL, interp);
  InterpAddPrimFun("=>", Prim_SETLOCAL, interp);
  InterpAddPrimFun("eval", Prim_EVAL, interp);
  InterpAddPrimFun("bind", Prim_BIND, interp);
  InterpAddPrimFun("value", Prim_VALUE, interp);
  InterpAddPrimFun("call", Prim_CALL, interp);
  InterpAddPrimFun("ifTrue", Prim_IFTRUE, interp);
  InterpAddPrimFun("ifFalse", Prim_IFFALSE, interp);
  InterpAddPrimFun("ifElse", Prim_IFELSE, interp);
  InterpAddPrimFun("+", Prim_PLUS, interp);
  InterpAddPrimFun("-", Prim_MINUS, interp);
  InterpAddPrimFun("*", Prim_TIMES, interp);
  InterpAddPrimFun("/", Prim_DIV, interp);
  InterpAddPrimFun("modulo", Prim_MODULO, interp);
  InterpAddPrimFun("add1", Prim_ADD1, interp);
  InterpAddPrimFun("sub1", Prim_SUB1, interp);
  InterpAddPrimFun("true", Prim_TRUE, interp);
  InterpAddPrimFun("false", Prim_FALSE, interp);
  InterpAddPrimFun("not", Prim_NOT, interp);
  InterpAddPrimFun("isZero", Prim_ISZERO, interp);
  InterpAddPrimFun("gtZero", Prim_GTZERO, interp);
  InterpAddPrimFun("ltZero", Prim_LTZERO, interp);
  InterpAddPrimFun("eq", Prim_EQ, interp);
  InterpAddPrimFun(">", Prim_GREATERTHAN, interp);
  InterpAddPrimFun("<", Prim_LESSTHAN, interp);
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
  InterpAddPrimFun("print", Prim_PRINT, interp);
  InterpAddPrimFun("printStack", Prim_PRINTSTACK, interp);
  InterpAddPrimFun("toString", Prim_TOSTRING, interp);
  InterpAddPrimFun("evalFile", Prim_EVALFILE, interp);
  //InterpAddPrimFun("Quote", Prim_QUOTE, interp);
  //InterpAddPrimFun(":", Prim_QUOTE, interp);
  //InterpAddPrimFun("PRN", Prim_PRN, interp);
  //InterpAddPrimFun("NEWLINE", Prim_NEWLINE, interp);
  //InterpAddPrimFun("SPACE", Prim_SPACE, interp);
}
