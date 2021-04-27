
/****************** INTERPRETER ******************/

typedef struct MyInterp
{
  List* symbolTable;     // List of global names (prims, funs, symbols)
  List* stack;           // The data stack
  List* callstack;       // Callstack with stack frames
  int   stackframeIndex; // Index of current stack frame
  Bool  run;             // Run flag
}
Interp;

typedef struct MyStackFrame
{
  List* env; // symbolIndex -> value
  List* codeList;
  int   codePointer;
}
StackFrame;

Interp* InterpCreate()
{
  Interp* interp = malloc(sizeof(Interp));
  interp->symbolTable = ListCreate();
  interp->stack = ListCreate();
  interp->callstack = ListCreate();
  interp->stackframeIndex = -1;
  interp->run = TRUE;
  return interp;
}

void InterpFree(Interp* interp)
{
  ListFree(interp->symbolTable, ListFreeDeep);
  ListFree(interp->stack, ListFreeDeeper);
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

Index InterpLookupSymbol(Interp* interp, char* symbol)
{
  // Lookup the symbol.
  List* symbolTable = interp->symbolTable;
  for (int i = 0; i < symbolTable->length; i++)
  {
    Item item = ListGet(symbolTable, i);
    char* string = item.string;
    if (StringEquals(string, symbol))
    {
      // Found it.
      return i;
    }
  }
  return -1; // Not found.
}

// Returns and item with info about the symbol.
Item InterpAddSymbol(Interp* interp, char* symbol)
{
  // Lookup the symbol.
  Index index = InterpLookupSymbol(interp, symbol);
  if (index > -1)
  {
    // Symbol is already added, return the symbol table entry.
    printf("SYMBOL EXISTS IN SYMTABLE: %s\n", symbol);
    Item item = ListGet(interp->symbolTable, index);
    item.symbolIndex = index;
    return item;
  }
  else
  {
    // Symbol does not exist, create it.
    char* string = malloc(strlen(symbol) + 1);
    strcpy(string, symbol);
    Item newItem = ItemWithString(string);
    Index newIndex = ListPush(interp->symbolTable, newItem);
    Item item = ListGet(interp->symbolTable, newIndex);
    item.type = TypeSymbol;
    item.symbolIndex = newIndex;
    return item;
  }
}

char* InterpGetSymbolString(Interp* interp, Index symbolIndex)
{
  List* symbolTable = interp->symbolTable;
  Item item = ListGet(symbolTable, symbolIndex);
  return item.string;
}

// Lookup the value of a symbol (variable).
// Return Virgin item if no value exists.
Item InterpLookupSymbolValue(Interp* interp, Item item)
{
  // Lookup symbol in stackframe environment.
  
  // Lookup symbol in global symbol table.
  
  return ItemWithVirgin();
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

Item InterpPopEval(Interp* interp)
{
  // If the item is a symbol, evaluate it.
  // Evaluating a symbol means finding its value,
  // if it is bound. An unbound symbol evaluates
  // to itself (its literal value).
  Item item = ListPop(interp->stack);
  if (IsSymbol(item.type))
  {
    Item value = InterpLookupSymbolValue(interp, item);
    if (TypeVirgin != value.type) return value;
  }
  
  // If no value found, return the item itself.
  return item;
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
  if (IsPrimFun(type))
  {
    //printf("PRIM FUN FOUND\n");
    element.data.primFun(interp);
  }
  else
  {
    //Push element onto the data stack
    ListPush(interp->stack, element);
    //printf("PUSH ELEMENT ONTO DATA STACK: %i\n", element.type);
  }
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
    StackFrame* stackframe = item.data.obj;

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
  // Add to symbol table.
  List* symbolTable = interp->symbolTable;
  Item item = ItemWithString(name);
  item.type = TypePrimFun;
  item.data.primFun = fun;
  item.symbolIndex = -1;
  Index index = ListPush(symbolTable, item);
}
