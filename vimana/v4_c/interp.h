
/****************** INTERPRETER ******************/

typedef struct MyInterp
{
  List* symbolTable;      // List of global names
  List* symbolValueTable; // List of global values (primfuns, funs, ...)
  List* stack;            // The data stack
  List* callstack;        // Callstack with stack frames
  int   stackframeIndex;  // Index of current stack frame
  Bool  run;              // Run flag
}
Interp;

typedef struct MyStackFrame
{
  List* env; // index -> value
  List* codeList;
  int   codePointer;
}
StackFrame;

Interp* InterpCreate()
{
  Interp* interp = malloc(sizeof(Interp));
  interp->symbolTable = ListCreate();
  interp->symbolValueTable = ListCreate();
  interp->stack = ListCreate();
  interp->callstack = ListCreate();
  interp->stackframeIndex = -1;
  interp->run = TRUE;
  return interp;
}

void InterpFree(Interp* interp)
{
  // TODO: Do custom free functions for each type.
  ListFree(interp->symbolTable, ListFreeDeep);
  ListFree(interp->symbolValueTable, ListFreeShallow);
  ListFree(interp->stack, ListFreeShallow);
  ListFree(interp->callstack, ListFreeDeep);
}

StackFrame* StackFrameCreate(List* codeList)
{
  StackFrame* stackframe = malloc(sizeof(StackFrame));
  stackframe->env = ListCreate();
  stackframe->codeList = codeList;
  stackframe->codePointer = -1;
  return stackframe;
}

void StackFrameFree(StackFrame* stackframe)
{
  ListFree(stackframe->env, ListFreeShallow);
  free(stackframe);
}

// Lookup the symbol string in the symbol table.
Index InterpLookupSymbolIndex(Interp* interp, char* symbol)
{
  List* symbolTable = interp->symbolTable;
  for (int i = 0; i < symbolTable->length; i++)
  {
    Item item = ListGet(symbolTable, i);
    char* string = item.value.string;
    if (StringEquals(string, symbol))
    {
      // Found it.
      return i;
    }
  }
  return -1; // Not found.
}

// Add a symbol to the symbol table and return an
// item with the entry.
Item InterpAddSymbol(Interp* interp, char* symbol)
{
  // Lookup the symbol.
  Index index = InterpLookupSymbolIndex(interp, symbol);
  if (index > -1)
  {
    // Symbol is already added, return an item with 
    // the symbol index.
    printf("SYMBOL EXISTS IN SYMTABLE: %s\n", symbol);
    Item item = ItemWithSymbol(index);
    return item;
  }
  else
  {
    // Symbol does not exist, create it.
    Item newItem = ItemWithString(symbol);
    Index newIndex = ListPush(interp->symbolTable, newItem);
    ListPush(interp->symbolValueTable, ItemWithVirgin());
    // Make a symbol item and return it.
    Item item = ItemWithSymbol(newIndex);
    return item;
  }
}

char* InterpGetSymbolString(Interp* interp, Index symbolIndex)
{
  Item item = ListGet(interp->symbolTable, symbolIndex);
  return item.value.string;
}

// Lookup the value of a symbol (variable value).
// Return Virgin item if no value exists.
Item InterpEvalSymbol(Interp* interp, Item item)
{
  // Lookup symbol in global symbol table.
  if (IsSymbol(item.type))
  {
    Item value =  ListGet(interp->symbolValueTable, item.value.symbol);
    if (TypeVirgin != value.type) return value;
  }
  
  // TODO: Lookup symbol in stackframe local environment.
  
  // Otherwise return the item itself (it evaluates to itself).
  return item;
}

// Associative list
Item ListLookup(List* list, Index symbolIndex)
{
  // TODO
  return ItemWithVirgin();
}

void InterpPush(Interp* interp, Item item)
{
  ListPush(interp->stack, item);
}

Item InterpPop(Interp* interp)
{
  return ListPop(interp->stack);
}

// TODO: Debug.
Item InterpPopEval(Interp* interp)
{
  // If the item is a symbol, evaluate it.
  // Evaluating a symbol means finding its value,
  // if it is bound. An unbound symbol evaluates
  // to itself (its literal value).
  Item item = ListPop(interp->stack);
  return InterpEvalSymbol(interp, item);
}

void InterpPushStackFrame(Interp* interp, List* list)
{
  StackFrame* stackframe = StackFrameCreate(list);
  Item item = ItemWithStackFrame(stackframe);
  interp->stackframeIndex = ListPush(interp->callstack, item);
  printf("PUSHED STACKFRAME AT INDEX: %i\n", interp->stackframeIndex);
}

void InterpEval(Interp* interp, Item element)
{
  Type type = element.type;
  if (IsSymbol(type))
  {
    Item item = ListGet(interp->symbolValueTable, element.value.symbol);
    if (IsPrimFun(item.type))
    {
      printf("PRIM FUN FOUND: %i\n", element.value.symbol);
      item.value.primFun(interp);
      return;
    }
  }

  //Push element onto the data stack
  ListPush(interp->stack, element);
  printf("PUSH ELEMENT ONTO DATA STACK TYPE: %u\n", element.type);
}

void InterpRun(Interp* interp, List* list)
{
  // Push root stackframe.
  InterpPushStackFrame(interp, list);
  printf("CREATED ROOT FRAME AT INDEX: %i\n", interp->stackframeIndex);
  
  while (interp->run)
  {
    // Get current stackframe.
    Item item = ListGet(interp->callstack, interp->stackframeIndex);
    StackFrame* stackframe = item.value.obj;

    // Increment code pointer.
    stackframe->codePointer++;
    
    // If the code in the stackframe has finished executing
    // we exit the frame.
    if (stackframe->codePointer >= stackframe->codeList->length)
    {
      // EXIT STACK FRAME
      printf("EXIT STACKFRAME: %i\n", interp->stackframeIndex);
      interp->stackframeIndex--;
      ListPop(interp->callstack);
    }
    else
    {
      // Evaluate the current element. Note that new stackframes
      // may be created during evaluation, and that this will 
      // increment the stackframe index.
      Item element = ListGet(stackframe->codeList, stackframe->codePointer);
      InterpEval(interp, element);
    }

    // Was this the last stackframe?
    if (interp->stackframeIndex < 0)
    {
      printf("EXIT InterpRun\n");
      interp->run = FALSE;
    }
  }
}

void InterpAddPrimFun(char* name, PrimFun fun, Interp* interp)
{
  // Add name to symbol table.
  ListPush(interp->symbolTable, ItemWithString(name));
  
  // Add function to symbol value table.
  ListPush(interp->symbolValueTable, ItemWithPrimFun(fun));
}
