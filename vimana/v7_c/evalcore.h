UNUSED

/*
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
    InterpPopEvalInto(interp, arg);
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
*/
