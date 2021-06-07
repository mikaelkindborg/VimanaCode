
// FUNCTION OBJECT ----------------------------------------

// code is a VimanaList
function VimanaFun(code)
{
  this.code = code
}

function VimanaIsFun(obj)
{
  return (obj instanceof VimanaFun)
}

// LIST OBJECT ----------------------------------------

function VimanaList()
{
  this.list = []
  this.env = null
}

VimanaList.prototype.push = function(obj)
{
  this.list.push(obj)
}

function VimanaIsList(obj)
{
  return (obj instanceof VimanaList)
}

// CONTEXT OBJECT ----------------------------------------

// code is a VimanaList
// env is a JS array
function VimanaContext(code, env)
{
  this.env = env
  this.code = code
  this.codePointer = -1
}

function VimanaIsContext(obj)
{
  return (obj instanceof VimanaContext)
}

// INTERPRETER OBJECT -------------------------------------

function VimanaInterp()
{
  this.primFuns = {}
  this.globalEnv = {}
  this.stack = []
  this.callstack = []
  this.contextIndex = -1
  this.currentContext = null;
  this.contextSwitch = true
  this.speed = 50 // ms delay in eval loop
}

VimanaInterp.prototype.evalSymbol = function(obj)
{
  // If not string don't evaluate it.
  if (typeof obj !== "string")
    return obj

  // Search local env, then global env.
  //let index = this.contextIndex
  //let context = this.callstack[index]
  //let context = this.currentContext
  if (obj in this.currentContext.env)
    return this.currentContext.env[obj]

  // Lookup symbol in global environment.
  if (obj in this.globalEnv)
    return this.globalEnv[obj]

  return obj

  //return this.evalGlobalSymbol(obj)

/*
  // Search local environment chain.
  let index = this.contextIndex
  while (index > -1)
  {
    let context = this.callstack[index]
    if (x in context.env)
      return context.env[x]
    -- index
  }
*/
}

VimanaInterp.prototype.evalGlobalSymbol = function(obj)
{
  // Lookup symbol in global environment.
  if (obj in this.globalEnv)
    return this.globalEnv[obj]
  else
    return obj
}

// Eval a list (evalList).
VimanaInterp.prototype.eval = function(code)
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
VimanaInterp.prototype.timerEval = function(code)
{
  // Push code context
  this.pushContext(code)

  let vimana = this
  
  // Enter eval loop
  runTimer()

  function runTimer()
  {
    vimana.doOneStep()

    if (vimana.contextIndex > -1)
      setTimeout(runTimer, vimana.speed)
    else
    {
      //console.log("EXIT")
      //vimana.printStack()
    }
  }
}

VimanaInterp.prototype.doOneStep = function()
{
  let context = this.currentContext

  ++ context.codePointer
  
  if (context.codePointer >= context.code.list.length)
  {
    this.popContext()
    return
  }

  //this.printStack()

  let obj = context.code.list[context.codePointer]
  
  if (typeof obj === "string")
  {
    let primFun = this.primFuns[obj]
    if (primFun)
    {
      //vimana.printFunCall(obj)
      primFun(this)
      return
    }
    
    // Only global functions are evaluated here.
    // Use CALL for local funs.
    let value = this.evalGlobalSymbol(obj)
    if (VimanaIsFun(value))
    {
      //vimana.printFunCall(obj)
      this.pushContext(value.code)
      return
    }
  }

  this.push(obj)
}

VimanaInterp.prototype.addPrimFun = function(name, fun)
{
  // TODO: Specify case in an interpreter setting.
  name = name.toUpperCase()
  this.primFuns[name] = fun
}

VimanaInterp.prototype.pushContext = function(code, env = {})
{
  this.currentContext = new VimanaContext(code, env)
  this.callstack.push(this.currentContext)
  ++ this.contextIndex
  //this.printContext(context)
}

VimanaInterp.prototype.popContext = function()
{
  -- this.contextIndex
  this.callstack.pop()
  this.currentContext = this.callstack[this.contextIndex]
}

VimanaInterp.prototype.pop = function()
{
  return this.stack.pop()
}
  
VimanaInterp.prototype.push = function(obj)
{
  return this.stack.push(obj)
}

VimanaInterp.prototype.popEval = function()
{
  let obj = this.stack.pop()
  return this.evalSymbol(obj)
}

VimanaInterp.prototype.checkList = function(list, errorMessage)
{
  if (!VimanaIsList(list))
    this.error(errorMessage)
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
  let list = new VimanaList()
  
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
    {
      next = VimanaParseTokens(tokens)
    }
    else if (isFinite(next))
    {
      next = next * 1 // Convert string to number
    }
    list.push(next)
  }
}
