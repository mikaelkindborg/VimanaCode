
// FUNCTION OBJECT ----------------------------------------

function VimanaFunction()
{
  this.code = []
}

// TODO: Inline below.
function VimanaIsFun(obj)
{
  return (obj instanceof VimanaFunction)
}

// TODO: Global var, Primfun?

// INTERPRETER OBJECT -------------------------------------

function VimanaInterpreter()
{
  this.primFuns = {}
  this.globalEnv = {}
  this.stack = []
  this.callstack = []
  this.contextIndex = -1
  this.speed = 50 // ms delay in eval loop
}

VimanaInterpreter.prototype.evalSymbol = function(x)
{
  // If not string don't evaluate it.
  if (typeof x !== "string")
    return x
  
  // Search local environment chain.
  let index = this.contextIndex
  while (index > -1)
  {
    let context = this.callstack[index]
    if (x in context.env)
      return context.env[x]
    -- index
  }

  // Finally, look in global environment.
  return this.evalGlobalSymbol(x)
}

VimanaInterpreter.prototype.evalGlobalSymbol = function(x)
{
  // Lookup symbol in global environment.
  if (x in this.globalEnv)
    return this.globalEnv[x]
  else
    return x
}

// Eval a list (evalList).
VimanaInterpreter.prototype.eval = function(code)
{
  // Push code context
  this.pushContext(code)

  // Eval loop
  while (this.contextIndex > -1)
  {
    //console.log("contextIndex: " + this.contextIndex)
    this.doOneStep()
  }
  //console.log("EXIT")
  //this.printStack()
  //console.log(this.globalEnv)
}

// Eval with timer that drives the loop.
VimanaInterpreter.prototype.timerEval = function(code)
{
  // Push code context
  this.pushContext(code)

  // Enter eval loop
  runTimer()
  
  function runTimer()
  {
    console.log("contextIndex: " + this.contextIndex)
    this.doOneStep()

    if (this.contextIndex > -1)
      setTimeout(runTimer, this.speed)
    else
    {
      console.log("EXIT")
      //this.printStack()
    }
  }
}

VimanaInterpreter.prototype.doOneStep = function()
{
  let context = this.callstack[this.contextIndex]

  ++ context.codePointer
  
  if (context.codePointer >= context.code.length)
  {
    this.popContext()
    return
  }

  //printStack()

  let x = context.code[context.codePointer]
  
  if (typeof x === "string")
  {
    let primFun = this.primFuns[x]
    if (primFun)
    {
      //printFunCall(x)
      primFun(this)
      return
    }
    
    // Only global functions are evaluated here.
    // Use CALL for local funs.
    let value = this.evalGlobalSymbol(x)
    if (VimanaIsFun(value))
    {
      //printFunCall(x)
      this.pushContext(value.code)
      return
    }
  }

  this.push(x)
}

VimanaInterpreter.prototype.addPrimFun = function(name, fun)
{
  this.primFuns[name] = fun
}

VimanaInterpreter.prototype.pushContext = function(code, env = {})
{
  let context = {}
  context.code = code
  context.codePointer = -1
  context.env = env

  this.callstack.push(context)
  ++ this.contextIndex
  //printContext(context)
}
  
VimanaInterpreter.prototype.popContext = function()
{
  this.callstack.pop()
  -- this.contextIndex
}

VimanaInterpreter.prototype.pop = function()
{
  return this.stack.pop()
}
  
VimanaInterpreter.prototype.push = function(x)
{
  return this.stack.push(x)
}
  
VimanaInterpreter.prototype.popEval = function()
{
  let x = this.stack.pop()
  return this.evalSymbol(x)
}

// PARSER -------------------------------------------------

// Parse (tokenize) a string and return a list.
function VimanaParse(code)
{
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

// Recursively create the list tree structure.
function VimanaParseTokens(tokens)
{
  let list = []
  
  while (true)
  {
    if (tokens.length === 0)
      return list
    
    next = tokens.shift()
    next = next.trim()
    
    if (next === "")
      continue

    if (next === ")")
      return list
      
    if (next === "(")
      next = VimanaParseTokens(tokens)
    else if (isFinite(next))
      next = next * 1 // Convert string to number
    
    list.push(next)
  }
}
