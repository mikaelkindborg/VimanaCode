//
// File: interp.js
// Vimana interpreter
// Copyright (c) 2021-2022 Mikael Kindborg
// mikael@kindborg.com
//

// INTERPRETER --------------------------------------------

function VimanaInterp()
{
  this.primFuns = {}
  this.globalEnv = {}
  this.dataStack = []
  this.stackFrame = null // current stackframe
  this.speed = 10 //50 // ms delay in eval loop
  this.symbolCase = "lowercase"
}

VimanaInterp.prototype.pushDataStack = function(obj)
{
  this.stack.push(obj)
}

VimanaInterp.prototype.popDataStack = function()
{
  if (0 === this.stack.length)
    this.error("DATASTACK IS EMPTY")

  return this.stack.pop()
}

VimanaInterp.prototype.pushFirstStackFrame = function(list)
{
  this.stackFrame = { car: list, cdr: this.stackFrame }
}

VimanaInterp.prototype.pushStackFrame = function(list)
{
  // Check tail call
  if (stackFrame.car === null)
  {
    // Reuse current stackframe
    stackFrame.car = list
    this.print("TAILCALL")
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
    this.error("CALLSTACK IS EMPTY")

  this.stackFrame = this.stackFrame.cdr
}

// Lookup symbol in global environment
VimanaInterp.prototype.evalSymbol = function(obj)
{
  if (obj in this.globalEnv)
    return this.globalEnv[obj]
  else
    return obj
}

// Eval a list
VimanaInterp.prototype.eval = function(list)
{
  // Push root stackframe
  this.pushFirstStackFrame(list)

  // Eval loop
  while (this.stackframe != null)
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
  if (null != instruction)
  {
    // Advance instruction for next loop
    stackFrame.car = instruction.cdr

    if (typeof(instruction.car) == "function")
    {
      instruction.car(this)
    }
    else
    {
      this.dataStack.push(instruction.car)
    }
  }
  else
  {
    this.popStackFrame()
  }
}

// Eval driven by a timer, which is slower but more resource friendly
VimanaInterp.prototype.timerEval = function(list)
{
  // Push root stackframe
  this.pushFirstStackFrame(list)

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
  }
}

VimanaInterp.prototype.defPrimFun = function(name, fun)
{
  this.primFuns[name] = fun
}

// PARSER -------------------------------------------------

// Parse (tokenize) a string and return a list
function VimanaParse(code)
{
  code = VimanaRemoveComments(code)

  code = code.replaceAll("(", " ( ")
  code = code.replaceAll(")", " ) ")
  code = code.replaceAll("\n", " ")
  code = code.replaceAll("\r", " ")
  code = code.replaceAll("\t", " ")

  let tokens = code.split(" ")
  //$tokens = array_filter($tokens,
  //  function($token) { return strlen($token) > 0 })
  let list = VimanaParseTokens(tokens)

  return list
}

// Remove /-- Vimana comments --/ from the code
function VimanaRemoveComments(code)
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
function VimanaParseTokens(tokens)
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
      value = VimanaParseTokens(tokens)
    }
    else if (isFinite(token))
    {
      value = token * 1 // Convert string to number
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

// The idea is that you redefine this function if
// you want to output the result in your UI
VimanaInterp.prototype.print = function(s)
{
  console.log(s)
}

function VimanaPrettyPrint(obj)
{
  if (typeof(obj) == "object")
    return VimanaPrettyPrintList(obj)
  else
  if (typeof(obj) == "function")
    return "[PrimFun]"
  else
  if (typeof(obj) == "symbol")
    return "[Symbol]"
  else
    return obj.toString()
}

function VimanaPrettyPrintList(list)
{
  let string = "("
  let item = list

  while (item != null)
  {
    string = string + VimanaPrettyPrint(item.car)

    item = item.cdr

    if (item != null)
    {
      string = string + " "
    }
  }

  return string + ")"
}

function VimanaPrettyPrintStack(stack)
{
  let string = ""
  for (let i = 0; i < stack.length; ++ i)
  {
    string += VimanaPrettyPrint(stack[i]) + " "
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
  VimanaDefPrimFuns(window.VimanaCode)
}

function VimanaEval(string)
{
  let list = VimanaParse(string)
  window.VimanaCode.timerEval(list)
}

function VimanaEvalFast(string)
{
  let list = VimanaParse(string)
  // eval is much faster than timerEval, but is less resource 
  // friendly and can slow down the browser
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

  // Lookup function in globalEnv
  let fun = window.VimanaCode.globalEnv[funName]

  // Call function
  window.VimanaCode.pushContext(fun.code, {})
}
*/
