/*** vimana.c ***

/*
Benchmarks 210507 and 210508

((N) () (
  N 0 EQ (1) (N 1 - FACT N *) IFELSE))
  FUN FACT SET

((L N) () (
  N 0 EQ NOT (L DO L N 1 - TIMESDO) IFTRUE))
  FUN TIMESDO SET 

(20 FACT DOC) 100000 TIMESDO

20 FACT 100000 iterations:

Non-optimised code:
./vimana  2.54s user 0.01s system 85% cpu 2.964 total

Reuse of stackframes (minimize malloc):
./vimana  1.57s user 0.00s system 99% cpu 1.578 total

Inlining using macros:
./vimana  1.01s user 0.00s system 72% cpu 1.388 total

Some more macro inlining:
./vimana  0.87s user 0.00s system 99% cpu 0.880 total

With compiler optimization:
cc vimana.c -Ofast -o vimana
./vimana  0.41s user 0.00s system 49% cpu 0.830 total
./vimana  0.35s user 0.00s system 99% cpu 0.360 total

10000000 interations 20 FACT:
cc vimana.c -o vimana -O3
./vimana  38.98s user 0.07s system 98% cpu 39.635 total

After further inlining 10000000 interations 20 FACT:
cc vimana.c -o vimana -Ofast
./vimana  29.47s user 0.03s system 98% cpu 29.886 total

After padding struct Item 10000000 interations 20 FACT:
cc vimana.c -o vimana -Ofast
./vimana  26.61s user 0.02s system 95% cpu 27.992 total

Compiled code size is around 70280 bytes regardless of 
compiler optimization and macro inlining.

210508: Source code 1572 lines
*/
/*
  List* list = ParseCode(interp, 
    "((L N) () ("
    " N 0 EQ NOT (L DO L N 1 - TIMESDO) IFTRUE))"
    "FUN TIMESDO SET "
    "(HELLO_WORLD DROP) 10000000 TIMESDO");
*/

// ------------------------------------------

// Benchmarks 210513 and 210514

  List* list = ParseCode(interp, 
    "((N) () ("
    "  N 0 EQ (1) (N 1 - FACT N *) IFELSE))"
    "FUN FACT SET "
    "((L N) () ("
    " N 0 EQ NOT (L DO L N 1 - TIMESDO) IFTRUE))"
    "FUN TIMESDO SET "
    "(20 FACT DROP) 10000000 TIMESDO");
                  //10000000
/*
5000000 TIMESDO

Original:
cc vimana.c -o vimana
./vimana  3.40s user 0.01s system 99% cpu 3.421 total
cc vimana.c -o vimana -O3
./vimana  0.83s user 0.00s system 91% cpu 0.913 total (-O3)

Inline in PrimFun_MINUS:
./vimana  3.31s user 0.00s system 97% cpu 3.393 total
./vimana  0.82s user 0.00s system 91% cpu 0.901 total (-O3)

10000000 TIMESDO

Original:
cc vimana.c -o vimana
./vimana  6.66s user 0.00s system 98% cpu 6.751 total
./vimana  6.80s user 0.01s system 99% cpu 6.819 total
cc vimana.c -o vimana -O3
./vimana  1.67s user 0.00s system 99% cpu 1.673 total (-O3)

Assignment of variable a in PrimFun_MINUS (inlined from ItemMinus)
./vimana  6.65s user 0.01s system 98% cpu 6.739 total
./vimana  1.66s user 0.00s system 95% cpu 1.745 total (-O3)

Assignment to res (new variable)
./vimana  6.57s user 0.00s system 98% cpu 6.660 total
./vimana  6.70s user 0.01s system 99% cpu 6.713 total
./vimana  1.65s user 0.00s system 95% cpu 1.734 total (-O3)

Original call to ItemWithIntNum inlined in PrimFun_MINUS:
./vimana  6.75s user 0.01s system 99% cpu 6.758 total
./vimana  1.63s user 0.00s system 95% cpu 1.716 total (-O3)
./vimana  1.74s user 0.01s system 98% cpu 1.781 total (-O3)

Direct access to stack in PrimFun_MINUS:
./vimana  6.52s user 0.00s system 98% cpu 6.603 total
./vimana  1.63s user 0.00s system 95% cpu 1.716 total (-O3)

Pointers in InterpEvalSymbolP
./vimana  6.51s user 0.01s system 99% cpu 6.527 total
./vimana  1.61s user 0.00s system 95% cpu 1.694 total (-O3)

Original again:
./vimana  6.69s user 0.01s system 99% cpu 6.701 total
./vimana  6.74s user 0.01s system 99% cpu 6.750 total
./vimana  7.02s user 0.03s system 99% cpu 7.109 total
./vimana  1.64s user 0.00s system 95% cpu 1.722 total (-O3)
./vimana  1.71s user 0.01s system 98% cpu 1.738 total (-O3)
./vimana  1.83s user 0.01s system 97% cpu 1.894 total (-O3)

Pointers in ItemMinusP:
./vimana  6.78s user 0.01s system 98% cpu 6.889 total
./vimana  6.67s user 0.01s system 99% cpu 6.686 total
./vimana  1.66s user 0.00s system 95% cpu 1.747 total (-O3)
./vimana  1.69s user 0.00s system 99% cpu 1.695 total (-O3)

Macro with pointers for ItemMinusP:
./vimana  6.62s user 0.01s system 98% cpu 6.715 total
./vimana  6.66s user 0.01s system 99% cpu 6.670 total
./vimana  6.76s user 0.01s system 99% cpu 6.782 total
./vimana  1.68s user 0.00s system 95% cpu 1.759 total (-O3)
./vimana  1.69s user 0.00s system 99% cpu 1.701 total (-O3)

Inlined in math in primfuns (see commit log):
./vimana  6.61s user 0.01s system 94% cpu 7.028 total
./vimana  6.70s user 0.01s system 99% cpu 6.712 total
./vimana  1.68s user 0.00s system 80% cpu 2.079 total (-O3)
./vimana  1.67s user 0.00s system 99% cpu 1.678 total (-O3)
With OPTIMIZE defined:
./vimana  3.23s user 0.01s system 89% cpu 3.611 total (OPTIMIZE)
With OPTIMIZE defined and -O3:
./vimana  1.32s user 0.00s system 75% cpu 1.758 total (-O3 + OPTIMIZE)

Macro InterpPopEvalSet used in Prim_MINUS and Prim_IFTRUE:
./vimana  3.18s user 0.00s system 88% cpu 3.605 total

Further use of InterpPopEvalSet:
./vimana  6.45s user 0.01s system 95% cpu 6.744 total
./vimana  6.44s user 0.01s system 93% cpu 6.867 total
./vimana  3.15s user 0.00s system 89% cpu 3.518 total (OPTIMIZE)
./vimana  1.26s user 0.00s system 76% cpu 1.653 total (-O3 + OPTIMIZE)

./vimana  6.54s user 0.01s system 94% cpu 6.928 total

PrimFuns in code list:
./vimana  6.40s user 0.01s system 96% cpu 6.628 total
./vimana  1.53s user 0.00s system 80% cpu 1.902 total (-O3)
./vimana  3.07s user 0.01s system 93% cpu 3.305 total (OPTIMIZE)
./vimana  1.24s user 0.01s system 79% cpu 1.568 total (-O3 + OPTIMIZE)
*/

// ------------------------------------------

// Various code snippets for testing

  //List* list = ParseCode(interp, "HELLO_WORLD PRINTLN (1.7 2.2 3 + +) DO PRINTLN 1 2 +");

  //List* list = ParseCode(interp, "((HELLO_WORLD PRINTLN) DO) DO");

  //List* list = ParseCode(interp, "(() () (HELLO_FUN_WORLD PRINTLN)) FUN HELLO SET HELLO");

  //List* list = ParseCode(interp, "((A) () (A PRINTLN)) FUN HELLO SET (WELCOME TO VIMANA) HELLO");

  //List* list = ParseCode(interp, "HELLO1 PRINTLN ((HELLO2 PRINTLN) DO) DO");

  //List* list = ParseCode(interp, "(HELLO_WORLD PRINTLN) LIST SET LIST DO");

  //List* list = ParseCode(interp, "42 21 2 * EQ (IT_IS_TRUE PRINT) IFTRUE");
  
  //List* list = ParseCode(interp, "41 21 2 * EQ (IT_IS_TRUE PRINT) (IT_IS_FALSE PRINT) IFELSE");

  //List* list = ParseCode(interp, "88 FOO SET TRUE (FOO) (42) IFELSE 2 * PRINT");

  //List* list = ParseCode(interp, "((A) (B) (A A + B SET  (B B + B SET) DO  B PRINTLN)) FUN DOUBLE-PRINT SET 42 DOUBLE-PRINT");
  
/*
  List* list = ParseCode(interp, 
    "(() (F) ("
    "  (() () ("
    "    HELLO_INNER_FUN PRINTLN"
    "  )) FUN F SET "
    "  F"
    ")) FUN MYFUN SET "
    "MYFUN");
*/
  //List* list = ParseCode(interp, "2 2 EQ PRINTLN");

  //List* list = ParseCode(interp, "42 FOO SET FOO PRINTLN 888888888 FOO SET FOO PRINTLN HELLO_FOO FOO SET FOO PRINTLN");
  
  //List* list = ParseCode(interp, "((A) (B C) (A B C HELLO PRINTLN)) FUN PRINTLN");
/*
  List* list = ParseCode(interp, 
    "((N) () ("
    "  N 0 EQ (1) (N 1 - FACT N *) IFELSE))"
    "FUN FACT SET "
    "3 FACT PRINT");
*/

// ------------------------------------------

/*** interp.c ***/

/*
// Pop item off the data stack.
#ifdef OPTIMIZE
#define InterpPop(interp) ListPop((interp)->stack)
#else
Item InterpPop(Interp* interp)
{
  //PrintDebug("INTERP POP");
  //PrintDebug("PRINTING STACK:");
  //ListPrintItems(interp->stack, interp);

  return ListPop(interp->stack);
}
#endif
*/

/*
#define X_InterpPopEvalSet(interp, item) \
  ((item) = ListPop((interp)->stack), \
  ((item) = (IsGlobalVar(item) || IsLocalVar(item)) ? \
    InterpEvalSymbol(interp, item) : \
    item))
*/

// NOTE: This function is inlined in InterpRun.
/*
void InterpEval(Interp* interp, Item element)
{
  // Optimize for primfun lookup.
  if (IsSymbol(element))
  {
    Item value = ListGet(interp->symbolValueTable, element.value.symbol);
    
    // If primitive function, evaluate it.
    if (IsPrimFun(value))
    {
      //PrintDebug("PRIM FOUND: %i", element.value.symbol);
      // Call the primitive.
      value.value.primFun(interp);
      return;
    }
    
    if (IsFun(value))
    {
      //PrintDebug("FUN FOUND: %i", element.value.symbol);
      // Call the function.
      InterpEvalFun(interp, value.value.list);
      return;
    }
  }

  if (IsLocalVar(element))
  {
    // Evaluate symbol to see if it is bound to a function.
    Item value = InterpEvalSymbol(interp, element);
    
    if (IsFun(value))
    {
      //PrintDebug("FUN FOUND: %i", element.value.symbol);
      // Call the function.
      InterpEvalFun(interp, ItemList(value));
      return;
    }
  }

  // Otherwise push element onto the data stack (not evaluated).
  ListPush(interp->stack, element);
  //PrintDebug("PUSH ELEMENT ONTO DATA STACK TYPE: %u", element.type);
}
*/


/*
// EXPERIMENT
Item* InterpEvalSymbolP(Interp* interp, Item* item)
{
  if (IsLocalVar(*item))
  {
    List* env = InterpGetLocalEnv(interp);
    if (env)
    {
      Item* value = &(env->items[item->value.symbol]);
      if (TypeVirgin != value->type) 
        return value;
    }
    else
      ErrorExit("InterpEvalSymbol: Local environment not found");
  }

  // Lookup symbol in global symbol table.
  if (IsSymbol(*item))
  {
    Item* value = &(interp->symbolValueTable->items[item->value.symbol]);
    if (TypeVirgin != value->type) 
      return value;
  }

  return item;
}
*/


// Pop item off the data stack and evaluate it
// if it is a symbol.
/*
#ifdef OPTIMIZE
#define InterpPopEval(interp) InterpEvalSymbol(interp, ListPop((interp)->stack))
#else
Item InterpPopEval(Interp* interp)
{
  //PrintDebug("INTERP POPEVAL");
  //PrintDebug("PRINTING STACK:");
  //ListPrintItems(interp->stack, interp);

  Item item = ListPop(interp->stack);

  //PrintDebug("ITEM TYPE: %lu", item.type);

  if (IsGlobalVar(item) || IsLocalVar(item))
    return InterpEvalSymbol(interp, item);
  else 
    return item;
}
#endif
*/

// ------------------------------------------

/*** primfuns.h ***/

/*
void Prim_PLUS(Interp* interp)
{
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  Item res = ItemPlus(b, a);
  InterpPush(interp, res);
}

void Prim_MINUS(Interp* interp)
{
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  Item res = ItemMinus(b, a);
  InterpPush(interp, res);
}

void Prim_TIMES(Interp* interp)
{
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  Item res = ItemTimes(b, a);
  InterpPush(interp, res);
}

void Prim_DIV(Interp* interp)
{
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  Item res = ItemDiv(b, a);
  InterpPush(interp, res);
}

void Prim_MODULO(Interp* interp)
{
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  Item res = ItemModulo(b, a);
  InterpPush(interp, res);
}

// EXPERIMENT
void Prim_MINUS(Interp* interp)
{
  Item a = InterpPopEval(interp);
  Item b = InterpPopEval(interp);
  Item res;
  ItemMinusP(&b, &a, &res);
  InterpPush(interp, res);
}

// EXPERIMENT
void Prim_MINUS(Interp* interp)
{
  //Item b = InterpPopEval(interp);
  //Item a = InterpPopEval(interp);
  //Item res;

  //PrintDebug("Prim_MINUS item a type: %lu", a.type);
  //PrintDebug("Prim_MINUS item b type: %lu", a.type);

  List* stack = interp->stack;
  Item* items = stack->items;
  Index last = stack->length - 1;
  Index nextlast = last - 1;
  Item* a = InterpEvalSymbolP(interp, &(items[nextlast]));
  Item* b = InterpEvalSymbolP(interp, &(items[last]));

  if (IsIntNum(*a) && IsIntNum(*a))
  {
    IntNum res = a->value.intNum - b->value.intNum;
    items[nextlast].value.intNum = res;
    items[nextlast].type = TypeIntNum;
    stack->length = last;

    //res.value.intNum = (a.value.intNum - b.value.intNum);
    //res.type = TypeIntNum;
    //InterpPush(interp, res);

    //PrintDebug("ITEM TYPE: %lu", item.type);

    //InterpPush(interp, ItemWithIntNum(a.value.intNum - b.value.intNum));
    return;
  }

  ErrorExit("ItemMinus: Unsupported item types");
}
*/
