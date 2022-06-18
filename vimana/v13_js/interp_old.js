//
// File: interp.js
// Vimana interpreter
// Copyright (c) 2021-2022 Mikael Kindborg
// mikael@kindborg.com
//

// TODO: lists, timetorun, strings, token symbols, symbol table, canvas object, save cards, jsbenchmark

// INTERPRETER --------------------------------------------

function VimanaInterp()
{
  this.primFuns = {}     // primitive functions
  this.globalVars = {}   // global vars
  this.stack = []        // data stack
  this.stackFrame = null // current stackframe
  this.speed = 0         // ms delay in eval loop
}

/*
VimanaInterp.prototype.pushStack = function(obj)
{
  this.stack.push(obj)
}
*/

VimanaInterp.prototype.popStack = function()
{
  if (0 === this.stack.length)
    this.error("DATASTACK IS EMPTY")

  return this.stack.pop()
}

VimanaInterp.prototype.pushFirstStackFrame = function(list)
{
  //this.stackFrame = { car: list, cdr: this.stackFrame }
  this.stackFrame = { car: list, cdr: null }
}

VimanaInterp.prototype.pushStackFrame = function(list)
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

VimanaInterp.prototype.popStackFrame = function()
{
  if (null === this.stackFrame)
  {
    this.error("CALLSTACK IS EMPTY")
  }

  this.stackFrame = this.stackFrame.cdr
}

// Lookup symbol in global environment
VimanaInterp.prototype.getGlobalVar = function(symbol)
{
  if (symbol in this.globalVars)
    return this.globalVars[symbol]
  else
    return null //symbol
}

// Eval a list
VimanaInterp.prototype.eval = function(list)
{
  // Push root stackframe
  this.pushFirstStackFrame(list)

  // Eval loop
  while (null !== this.stackFrame)
  {
    this.evalSlice()
  }
}

// Eval next instruction
VimanaInterp.prototype.evalSlice = function()
{
  let stackFrame = this.stackFrame
  let instruction = stackFrame.car

  // Evaluate current instruction
  if (null !== instruction)
  {
    // Advance instruction for next loop
    stackFrame.car = instruction.cdr

    if (typeof(instruction.car) == "function")
    {
      // Call primfun
      instruction.car(this)
    }
    else
    if (typeof(instruction.car) == "string") //"symbol")
    {
      let value = this.evalSymbol(instruction.car)
      if (value != null)
      {
        if (value.type == "fun")
        {
          // Call function
          this.pushStackFrame(value)
        }
        else
        {
          // Push value onto data stack
          this.stack.push(value)
        }
      }
    }
    else
    {
      // Push literal onto data stack
      this.stack.push(instruction.car)
    }
  }
  else
  {
    // End of instruction list - pop stackframe
    this.popStackFrame()
  }
}

// Eval driven by a timer, which is slower but more resource friendly
VimanaInterp.prototype.evalAsync = function(list, doneFun = null)
{
  // Push root stackframe
  this.pushFirstStackFrame(list)

  // For runTimer
  let interp = this

  // Enter eval loop
  runTimer()

  function runTimer()
  {
    if (null !== interp.stackFrame)
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

VimanaInterp.prototype.defPrimFun = function(name, fun)
{
  this.primFuns[name] = fun
}

// PARSER -------------------------------------------------

// Parse (tokenize) a string and return a list
VimanaInterp.prototype.parse = function(code)
{
  code = this.stripComments(code)

  code = code.replaceAll("(", " ( ")
  code = code.replaceAll(")", " ) ")
  code = code.replaceAll("\n", " ")
  code = code.replaceAll("\r", " ")
  code = code.replaceAll("\t", " ")

  let tokens = code.split(" ")
  //$tokens = array_filter($tokens,
  //  function($token) { return strlen($token) > 0 })
  let list = this.parseTokens(tokens)

  return list
}

// Remove /-- Vimana comments --/ from the code
VimanaInterp.prototype.stripComments = function(code)
{
  let index = 0
  let start = 0
  let stop = 0
  let result = ""

  while (true)
  {
    // Find start comment
    start = code.indexOf("/--", index)

    if (start === -1)
    {
      // No start comment - find last end comment
      stop = code.lastIndexOf("--/")
      if (stop > -1)
      {
        // Remove everything until last end comment
        result = result + code.slice(stop + 3)
      }
      else
      {
        // No end comment - keep rest of string
        result = result + code.slice(index)
      }
      // Exit loop
      break
    }

    if (start > -1)
    {
      // Find end comment
      stop = code.indexOf("--/", start)
      if (stop > -1)
      {
        // Remove comment
        result = result + code.slice(index, start)
        index = stop + 3
      }
      else
      {
        // No end comment - remove rest of string and exit loop
        result = result + code.slice(index, start)
        // Exit loop
        break
      }
    }
  }

  return result
}

// Recursively create the list tree structure
VimanaInterp.prototype.parseTokens = function(tokens)
{
  let first = { car: null, cdr: null } // Head item
  let item = first
  let value = null

  while (true)
  {
    if (tokens.length === 0)
      return first.cdr
    
    let token = tokens.shift().trim()
    
    if (token === "")
    {
      continue
    }
    
    if (token === ")")
    {
      return first.cdr
    }

    if (token === "(")
    {
      value = this.parseTokens(tokens)
    }
    else if (isFinite(token))
    {
      value = token * 1 // Convert string to number
    }
    else if (token in this.primFuns)
    {
      value = this.primFuns[token]
    }
    else
    {
      value = token
    }
    
    item.cdr = { car: value, cdr: null }
    item = item.cdr
  }
}

// PRINTING -----------------------------------------------

// The idea is that you can redefine this function if
// you want to output the result in your UI
VimanaInterp.prototype.print = function(s)
{
  console.log(s)
}

// Returns string
VimanaInterp.prototype.prettyPrint = function(obj)
{
  if (typeof(obj) == "object")
    return this.prettyPrintList(obj)
  else
  if (typeof(obj) == "function")
    return "[PrimFun]"
  else
  if (typeof(obj) == "symbol")
    return "[Symbol]"
  else
    return obj.toString()
}

// Returns string
VimanaInterp.prototype.prettyPrintList = function(list)
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
VimanaInterp.prototype.prettyPrintStack = function()
{
  let string = ""
  for (let i = 0; i < this.stack.length; ++ i)
  {
    string += this.prettyPrint(this.stack[i]) + " "
  }
  return string
}

// ERROR HANDLING -----------------------------------------

// Error handling is simple - an error aborts execution
VimanaInterp.prototype.error = function(s)
{
  let guruMeditation = "Software Failure. Guru Meditation: " + s
  throw guruMeditation
}

VimanaInterp.prototype.mustBeList = function(list, errorMessage)
{
  if (typeof(list) != "object")
    this.error(errorMessage)
}

// API FUNCTIONS ------------------------------------------

function VimanaInit()
{
  window.VimanaCode = new VimanaInterp()
  VimanaDefinePrimFuns(window.VimanaCode)
}

function VimanaEvalAsync(string, doneFun = null)
{
  let list = window.VimanaCode.parse(string)
  window.VimanaCode.evalAsync(list, doneFun)
}

function VimanaEval(string)
{
  // eval is much faster than evalAsync, but is less resource 
  // friendly and can slow down the browser
  let list = window.VimanaCode.parse(string)
  window.VimanaCode.eval(list)
}

/*
// This function is used to call back from JS to Vimana
function VimanaCallFun(funName, params = [])
{
  // Push params on stack (note the order)
  for (let i = params.length - 1; i > -1; --i)
  {
    window.VimanaCode.stack.push(params[i])
  }

  // Lookup function in globalVars
  let fun = window.VimanaCode.globalVars[funName]

  // Call function
  window.VimanaCode.pushContext(fun.code, {})
}
*/
