
/****************** INTERPRETER ******************/

typedef struct MyInterp
{
  //List* prims;           // Pointers to functions
  List* symbolTable;     // List of global names (prims, funs, vars)
  List* stack;           // The data stack
  List* callstack;       // Callstack with stack frames
  int   stackframeIndex; // Index of current stack frame
  Bool  run;             // Run flag
}
Interp;

/*typedef struct MySymbolEntry
{
  //Type  type;
  char  symbol[32];
  //Index ref;
}
SymbolEntry;*/

typedef struct MyStackFrame
{
  List* env;
  List* codeList;
  int   codePointer;
}
StackFrame;

Interp* InterpCreate()
{
  Interp* interp = malloc(sizeof(Interp));
  //interp->prims = ListCreate();
  interp->symbolTable = ListCreate();
  interp->stack = ListCreate();
  interp->callstack = ListCreate();
  interp->stackframeIndex = -1;
  interp->run = TRUE;
  return interp;
}

void InterpFree(Interp* interp)
{
  //ListFree(interp->prims, ListFreeDeep);
  ListFree(interp->symbolTable, ListFreeDeep);
  ListFree(interp->stack, ListFreeDeeper);
  ListFree(interp->callstack, ListFreeDeep);
}

/*SymbolEntry* SymbolEntryCreate()
{
  return malloc(sizeof(SymbolEntry));
}

void SymbolEntryFree(SymbolEntry* entry)
{
  return free(entry);
}*/

StackFrame* StackFrameCreate()
{
  return malloc(sizeof(StackFrame));
}

void StackFrameFree(StackFrame* stackframe)
{
  return free(stackframe);
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
    // SymbolEntry does not exist, create it.
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

/*SymbolEntry* InterpGetSymbolEntry(Interp* interp, Index symbolIndex)
{
  List* symbolTable = interp->symbolTable;
  Item item = ListGet(symbolTable, symbolIndex);
  return (SymbolEntry*) item.data.obj;
}*/

char* InterpGetSymbol(Interp* interp, Index symbolIndex)
{
  List* symbolTable = interp->symbolTable;
  Item item = ListGet(symbolTable, symbolIndex);
  return item.string;
}

void InterpEval(Interp* interp, Item element)
{
  Type type = element.type;
  if (IsPrimFun(type))
  {
    printf("PRIM FUN FOUND\n");
    element.data.primFun(interp);
  }
  else
  {
    //Push element onto the data stack
    ListPush(interp->stack, element);
    printf("PUSH ELEMENT ONTO DATA STACK: %i\n", element.type);
  }
}

void InterpRun(Interp* interp, List* list)
{
  // Create root stackframe.
  StackFrame* stackframe = StackFrameCreate();
  stackframe->env = ListCreate();
  stackframe->codeList = list;
  stackframe->codePointer = -1;
  Item item = ItemWithObj(stackframe);
  interp->stackframeIndex = ListPush(interp->callstack, item);
  printf("Pushed root frame at index: %i\n", interp->stackframeIndex);
  
  while (interp->run)
  {
    // Get current stackframe.
    //$index = $interp->stackframe_index;
    //$stackframe = $interp->callstack[$index];
    Item item = ListGet(interp->callstack, interp->stackframeIndex);
    stackframe = item.data.obj;

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
      printf("EXIT INTERP RUN\n");
      interp->run = FALSE;
    }
  }
}
