// This file contains code used for "compfiled functions".

// Lookup the value of a symbol (variable value).
// If the item is a symbol, evaluate it.
// Return Virgin item if no value exists.
// Evaluating a symbol means finding its value,
// if it is bound. An unbound symbol evaluates
// to itself (its literal value).
Item InterpEvalSymbol(Interp* interp, Item item)
{
  // Lookup symbol in stackframe local environment.
  if (IsLocalVar(item))
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
      if (ListLength(env) > 0)
        return ListGet(env, item.value.symbol);
      callstackIndex--;
    }
  }

  // Lookup symbol in global symbol table.
  if (IsSymbol(item))
  {
    Item value = ListGet(interp->symbolValueTable, item.value.symbol);
    if (!IsVirgin(value))
      return value;
  }

  // Otherwise return the item itself (evaluate to itself).
  return item;
}


void InterpRun(Interp* interp, List* list)
{
  // Create root context.
  InterpEvalList(interp, list);

  PrintDebug("CREATED ROOT FRAME AT INDEX: %i", interp->callstackIndex);
  
  //ListPrint(list, interp);
    
  while (interp->run)
  {
    //ListPrint(interp->stack, interp);

    // Get current context.
    Context* context = ItemContext(ListGet(interp->callstack, interp->callstackIndex));

    // Increment code pointer.
    context->codePointer ++;

    // If the code in the current context has finished executing
    // we exit the frame.
    List* code = context->code;

    if (context->codePointer >= ListLength(code))
    {
      // EXIT Context
      InterpExitContext(interp);
      //interp->callstackIndex--;
    }
    else
    {
      // Evaluate the current element. Note that new stackframes
      // may be created during evaluation, and that this will 
      // increment the stackframe index.
      Item element = ListGet(code, context->codePointer);

      //PrintDebug("EVAL GOT ELEMENT");
      //ItemPrint(element, interp);

      // If the elements points directly to a primful we run it.
      if (IsPrimFun(element))
      {
        PrintDebug("CALLING PRIMFUN");
        element.value.primFun(interp);
        goto exit;
      }

#ifdef EXCLUDE
      // If local var, get value from local environment.
      // TODO: Comment out this
      if (IsLocalVar(element))
      {
        Item value = InterpEvalSymbol(interp, element);
        if (IsCompiledFun(value))
        {
          InterpEvalCompiledFun(interp, ItemList(value));
          goto exit;
        }
        if (IsFun(value))
        {
          InterpEvalFun(interp, ItemList(value));
          goto exit;
        }
      }
#endif

      // If symbol we check the global value is primfun or fun.
      // This means that functions defined with DEF will take 
      // precedence over local variables with the same name. 
      // Furthermore, local functions need to be called with CALL.
      if (IsSymbol(element))
      {
        // Here we get the global value and check for functions
        // and primitives.
        Item value = ListGet(interp->symbolValueTable, element.value.symbol);
        if (IsPrimFun(value))
        {
          value.value.primFun(interp);
          goto exit;
        }
        if (IsFun(value))
        {
          InterpEvalFun(interp, value.value.list);
          goto exit;
        }
#ifdef EXCLUDE
        if (IsCompiledFun(value))
        {
          InterpEvalCompiledFun(interp, value.value.list);
          goto exit;
        }
#endif
      }

      // Otherwise
      PrintDebug("PUSHING ON STACK: %lu", element.type);

      ListPush(interp->stack, element);

      PrintDebug("PRINTING STACK:");
      //ListPrintItems(interp->stack, interp);
    }
exit:
    // Was this the last stackframe?
    if (interp->callstackIndex < 0)
    {
      //PrintDebug("EXIT InterpRun");
      interp->run = FALSE;
    }
  }
}

#ifdef EXCLUDE

void InterpEvalCompiledFun(Interp* interp, List* fun)
{
  // Get info for the call context of the function.
  IntNum numArgs = ItemIntNum(ListGet(fun, FunNumArgs));
  IntNum numVars = ItemIntNum(ListGet(fun, FunNumVars));
  List* funBody = ItemList(ListGet(fun, FunBody));

  // Get context for the function call. 
  Context* context = InterpEnterContext(interp, funBody, ContextResetEnv);

  // Get environment.
  List* env = context->env;

  // Set it to empty.
  env->length = 0;

  // Bind parameters.
  for (int i = numArgs - 1; i >= 0; i--)
  {
    //Item arg = InterpPopEval(interp);
    Item arg;
    InterpPopEvalSet(interp, arg);
    ListSet(env, i, arg);
  }
/*
  // Set all localvars to TypeVirgin.
  for (int i = numArgs; i < numArgs + numVars; i++)
  {
    ListSet(env, i, ItemWithVirgin());
  }
*/
}

#endif

#ifdef EXCLUDE

/***************************************************************

"COMPILING" FUNCTIONS

"Compiling" a listy imnto a function simply means replacing
local symbols with indexes into the local enviropnment table.
The use of indexes should make the lookup faster than using
a string lookup table such as a hash map.

Before compile:

((X) (A B) 
  (1 A SET 
   N A + B SET 
   B))
   
StackFrame env becomes 0:X-value 1:A-value 2:B-value

After compile:

(1 (X A B)
  (1 (VAR 1) SET 
   (VAR 0) (VAR 1) + (VAR 2) SET 
   (VAR 2))) 

Example in lower case:

((x) (a b) 
  (1 a set
   x a + b set
   b))
   
Variation in style:

((x) (a b) 
  ((1 a set) do
   (x a + b set) do
   b))

Example of defining and calling a function:

((X) () (X X +)) FUN DOUBLE SET
21 DOUBLE PRINTLN

***************************************************************/
/*
Ways to lookup variables:

Get values from the stack and dulicate and shuffle around as needed (Forth)
Lookup in a hashtable with ints or strings O(logn)
Lookup in a tree O(logn)
Lookup in an array by index (precompiled indexes) O(1)
Lookup in an array with binary search (array must be ordered) O(logn)
Lookup in an array with linear search O(n)
*/
/*
Item
  Context
    List
    Env
    ParentEnv
    instrPointer
*/

/****************** "COMPILE" ******************/

int InterpCompileFunLookupLocalIndex(Interp* interp, List* localVars, Item symbol)
{
  for (int i = 0; i < ListLength(localVars); i++)
  {
    Item localSymbol = ListGet(localVars, i);
    if (symbol.value.symbol == localSymbol.value.symbol)
      return i;
  }
  
  // Not a local symbol.
  return -1;
}
  
Item InterpCompileFunReplaceSymbols(Interp* interp, List* localVars, List* bodyList)
{
  List* newList = ListCreate();
  
  for (int i = 0; i < ListLength(bodyList); i++)
  {
    Item item = ListGet(bodyList, i);
    if (IsList(item))
    {
      item = InterpCompileFunReplaceSymbols(interp, localVars, item.value.list);
      ListPush(newList, item);
    }
    else if (IsSymbol(item))
    {
      // Replace symbol if in localvars.
      int index = InterpCompileFunLookupLocalIndex(interp, localVars, item);
      if (index > -1)
        ListPush(newList, ItemWithLocalSymbol(index));
      else
        ListPush(newList, item);
    }
    else
    {
      ListPush(newList, item);
    }
  }
  
  return ItemWithList(newList);
}

// "Compile" the function list by replacing local var symbols 
// with indexes. This should be faster than hashtable lookups.
// Return a list with the compiled function, an item of type: TypeFun
Item InterpCompileFun(Interp* interp, Item funList)
{
  //PrintDebug("Compile Fun");
  
  if (!IsList(funList))
    ErrorExit("InterpCompileFun: funList is not a list");

  int length = ListLength(funList.value.list);

  if (3 != length)
    ErrorExit("InterpCompileFun: Wrong number of elements in funList");

  Item argList  = ListGet(ItemList(funList), 0);
  Item varList  = ListGet(ItemList(funList), 1);
  Item bodyList = ListGet(ItemList(funList), 2);
  
  // Do some basic checks.
  if (!IsList(argList))
    ErrorExit("InterpCompileFun: argList is not a list");
  if (!IsList(varList))
    ErrorExit("InterpCompileFun: varList is not a list");
  if (!IsList(bodyList))
    ErrorExit("InterpCompileFun: bodyList is not a list");
  
  // The resulting list that holds the compiled
  // function has the format: (NUMARGS LOCALVARS BODY)
  // TODO Make this a structs instead?
  
  // Create list for LOCALVARS
  List* localVars = ListCreate();
  int numArgs = ListLength(ItemList(argList));
  int numVars = ListLength(ItemList(varList));

  for (int i = 0; i < numArgs; i++)
    ListPush(localVars, ListGet(ItemList(argList), i));
  for (int i = 0; i < numVars; i++)
    ListPush(localVars, ListGet(ItemList(varList), i));

  // Recursively traverse bodyList and replace local symbols with indexes.
  Item funBody = InterpCompileFunReplaceSymbols(interp, localVars, ItemList(bodyList));
  
  // Create list for the compile function. TODO Struct?
  List* compiledFun = ListCreate();
  
  ListPush(compiledFun, ItemWithIntNum(numArgs));
  ListPush(compiledFun, ItemWithIntNum(numVars));
  ListPush(compiledFun, funBody);

  //PrintDebug("COMPILED FUNCTION:");
  //ListPrint(compiledFun, interp);
  
  // Return item with the compiled list.
  Item item;
  item.type = TypeList | TypeCompiledFun;
  item.value.list = compiledFun;
  return item;
}

#endif
