
/***** CORE EVALUATOR FUNCTIONS **************************/

//
// The functions in this file can be changed to alter
// the behaviour of the language. The interpreter in
// interp.h is realively "naked" and the idea behind
// this is to make the code more modular.
//

// Add a symbol to the symbol table and return an
// item with the entry. Used by the parser.
Item EvalCoreAddSymbol(Interp* interp, char* symbolString)
{
  // Lookup the symbol.
  Index index = InterpLookupSymbolIndex(interp, symbolString);
  if (index > -1)
  {
#ifdef OPTIMIZE
    // Special case for primfuns. We return the primfun item.
    Item value = ListGet(interp->symbolValueTable, index);
    if (IsPrimFun(value))
    {
      return value;
    }
#endif
    // Symbol is already added, return an item for it.
    Item item = ItemWithSymbol(index);
    return item;
  }
  else
  {
    // Symbol does not exist, create it.
    Item newItem = ItemWithString(symbolString);
    ListPush(interp->globalSymbolTable, newItem);
    ListPush(interp->globalValueTable, ItemWithVirgin());
    Index newIndex = ListLength(interp->globalSymbolTable) - 1;
    // Make a symbol item and return it.
    Item item = ItemWithSymbol(newIndex);
    return item;
  }
}

Item EvalCoreEvalSymbol(Interp* interp, Item item)
{
  // Lookup symbol.
  if (IsSymbol(item))
  {
    Item value = ListGet(interp->globalValueTable, item.value.symbol);
    if (TypeVirgin != value.type) 
      return value;
  }
  return item;
}

void EvalCoreEvalFun(Interp* interp, List* fun)
{
  // TODO
}

#ifdef EXCLUDE

// Lookup symbol in the context list
Item EvalCoreEvalSymbol(Interp* interp, Item item)
{
  // Lookup symbol.
  if (IsSymbol(item))
  {
    Context* context;
    List* env;
    List* callstack = interp->callstack;
    Index callstackIndex = interp->callstackIndex;
    while (callstackIndex > -1)
    {
      context = ItemContext(ListGet(callstack, callstackIndex));
      env = context->env;

      //PrintLine("CONTEXT in InterpEvalSymbol:");
      //ListPrint(env, interp);

      // Search environment.
      Item* value = ListLookupSymbol(env, item.value.symbol);
      if (value)
        return *value;
      callstackIndex--;
    }
  }

  // Otherwise return the item itself (evaluate to itself).
  return item;
}

// Bind stack parameters and push a context on the
// callstack with the function body.
void EvalCoreFunEvalFun(Interp* interp, List* fun)
{
  // Get args and body of the function.
  List* funArgs = ItemList(ListGet(fun, 0));
  List* funBody = ItemList(ListGet(fun, 1));

  // Get context for the function call. 
  Context* context = InterpEnterContext(interp, funBody, );

  // Get environment.
  List* env = context->env;

  // Set it to empty.
  env->length = 0;

  // Bind parameters.
  int numArgs = ListLength(funArgs);
  for (int i = numArgs - 1; i >= 0; i--)
  {
    //Item arg = InterpPopEval(interp);
    Item arg;
    InterpPopEvalSet(interp, arg);
    //ListSet(env, i, arg);
    // Push a symbol -> value pair.
    // Push symbol.
    ListPush(env, ListGet(funArgs, i));
    // Push value.
    ListPush(env, arg);
  }
}


void InterpSetLocalSymbolValue(Interp* interp, Index index, Item value)
{
  ErrorExit("InterpSetLocalSymbolValue is not implemented!");
  /*
  List* env = InterpGetLocalEnv(interp);
  if (env)
    ListSet(env, index, value);
  else
    ErrorExit("InterpSetLocalSymbolValue: Local environment not found");
  */
}

#endif
