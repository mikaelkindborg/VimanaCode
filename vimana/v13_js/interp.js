//
// File: interp.js
// Vimana interpreter
// Copyright (c) 2021-2022 Mikael Kindborg
// mikael@kindborg.com
//

// TODO: lists, strings, token symbols, symbol table, canvas object, save cards, jsbenchmark

// INTERPRETER --------------------------------------------
  
class VimanaInterp
{
  // INTERPRETER DATA STRUCTURES --------------------------
  
  constructor() 
  {
    this.primFuns = {} // primitive functions
    this.globalVars = {} // global vars
    this.dataStack = [] // data stack
    this.stackFrame = null // top stackframe of callstack
    this.speed = 0 // ms delay in eval loop
  }

  // DEFINE PRIMITIVE FUNCTION ----------------------------
  
  // Define a primitive function
  defPrimFun(name, fun) 
  {
    this.primFuns[name] = fun
  }

  // DATA STACK -------------------------------------------
  
  // Push item onto the data stack
  pushStack(obj)
  {
    this.dataStack.push(obj)
  }

  // Pop the data stack
  popStack() 
  {
    if (0 === this.dataStack.length)
    {
      this.error("DATASTACK IS EMPTY")
    }

    return this.dataStack.pop()
  }

  // CALL STACK -------------------------------------------

  pushStackFrame(list) 
  {
    // Check tail call
    if (this.stackFrame.car === null) 
    {
      // Tailcall: Reuse current stackframe
      this.stackFrame.car = list
    }
    else 
    {
      // Push new stackframe
      this.stackFrame = { car: list, cdr: this.stackFrame }
    }
  }

  popStackFrame() 
  {
    if (null === this.stackFrame) 
    {
      this.error("CALLSTACK IS EMPTY")
    }

    this.stackFrame = this.stackFrame.cdr
  }

  // GLOBAL VARIABLES -------------------------------------
  
  // Lookup symbol in global environment
  getGlobalVar(symbol) 
  {
    if (symbol in this.globalVars)
      return this.globalVars[symbol]
    else
      return null
  }

  // Set global variable
  setGlobalVar(symbol, value) 
  {
    this.globalVars[symbol] = value
  }

  // INTERPRETER LOOP -------------------------------------
/*
  // Eval a list
  eval(list) 
  {
    // Push root stackframe
    this.pushFirstStackFrame(list)

    // Eval loop
    while (null !== this.stackFrame) 
    {
      this.evalSlice()
    }
  }
*/
  // Eval a list
  eval(list) 
  {
    let stackFrame = this.stackFrame

    // Push stackframe - no tailcall
    this.stackFrame = { car: list, cdr: this.stackFrame }

    // Eval loop
    while (stackFrame !== this.stackFrame) 
    {
      this.evalSlice()
    }
  }

  // Eval next instruction
  evalSlice() 
  {
    let stackFrame = this.stackFrame
    let instruction = stackFrame.car

    // Evaluate current instruction
    if (null !== instruction) 
    {
      // Advance instruction pointer for next loop
      stackFrame.car = instruction.cdr

      if (typeof (instruction.car) == "function") 
      {
        // Call primfun
        instruction.car(this)
      }
      else if (typeof (instruction.car) == "symbol")
      {
        let value = this.getGlobalVar(instruction.car)
        if (value != null) {
          if (value.type == "fun") 
          {
            // Call function
            this.pushStackFrame(value)
          }
          else 
          {
            // Push value onto data stack
            this.pushStack(value)
          }
        }
      }
      else 
      {
        // Push literal onto data stack
        this.pushStack(instruction.car)
      }
    }
    else 
    {
      // End of instruction list - pop stackframe
      this.popStackFrame()
    }
  }

  // Eval driven by a timer, which is slower but more resource friendly
  evalAsync(list, doneFun = null) 
  {
    let stackFrame = this.stackFrame

    // Push root stackframe
    this.stackFrame = { car: list, cdr: this.stackFrame }

    // For runTimer
    let interp = this

    // Enter eval loop
    runTimer()

    function runTimer() 
    {
      if (stackFrame !== interp.stackFrame) 
      {
        interp.evalSlice()
        setTimeout(runTimer, interp.speed)
      }
      else 
      {
        if (doneFun) doneFun()
      }
    }
  }

/*
TODO: Use eval instead of this hack!

  // CALL FUNCTION FROM JS --------------------------------
    
  // Call function in Vimana
  callFun(funName, params = [])
  {
    // Push params on stack (note the order)
    for (let i = params.length - 1; i > -1; -- i)
    {
      this.pushStack(params[i])
    }

    // Lookup function in globalVars
    let fun = this.globalVars[funName]

    // Call function - note the use of pushFirstStackFrame,
    // no other code may be evaluated at the same time!
    this.pushFirstStackFrame(fun.code)
  }
*/

  // PRINTING ---------------------------------------------

  // Returns string
  prettyPrint(obj) 
  {
    if (typeof (obj) == "object")
      return this.prettyPrintList(obj)
    else if (typeof (obj) == "function")
      return "[PrimFun]"
    else if (typeof (obj) == "symbol")
      return Symbol.keyFor(obj) //"[Symbol]"
    else
      return obj.toString()
  }

  // Returns string
  prettyPrintList(list) 
  {
    let string = "("
    let item = list

    while (item != null) 
    {
      string = string + this.prettyPrint(item.car)

      item = item.cdr

      if (item != null) 
      {
        string = string + " "
      }
    }

    return string + ")"
  }

  // Returns string
  prettyPrintStack() 
  {
    let string = ""
    for (let i = 0; i < this.dataStack.length; ++ i) 
    {
      string += this.prettyPrint(this.dataStack[i]) + " "
    }
    return string
  }

  // ERROR HANDLING ---------------------------------------

  // Error handling is simple - an error aborts execution
  error(s) 
  {
    let guruMeditation = "Guru Meditation: " + s
    throw guruMeditation
  }

  mustBeList(list, errorMessage) 
  {
    if (typeof (list) != "object")
      this.error(errorMessage)
  }
}
