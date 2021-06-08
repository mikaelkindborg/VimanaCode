
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
  this.speed = 0 //50 // ms delay in eval loop
}

VimanaInterp.prototype.evalSymbol = function(obj, env)
{
  // If not string don't evaluate it
  if (typeof obj !== "string")
    return obj

  // Search local environment
  if (obj in this.currentContext.env)
    return this.currentContext.env[obj]

  // Search global environment
  if (obj in this.globalEnv)
    return this.globalEnv[obj]

  // Otherwise, return the string itself
  return obj
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
  // Push root context
  this.pushContext(code, {})

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
  // Push root context
  this.pushContext(code, {})

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
    // Inline of popContext:
    -- this.contextIndex
    this.callstack.pop()
    this.currentContext = this.callstack[this.contextIndex]
    return
  }

  let obj = context.code.list[context.codePointer]
  
  if (typeof obj === "string")
  {
    let primFun = this.primFuns[obj]
    if (primFun)
    {
      //this.printFunCall(obj)
      primFun(this)
      return
    }
    
    let value = this.evalSymbol(obj)
    if (VimanaIsFun(value))
    {
      //vimana.printFunCall(obj)
      this.pushContext(value.code, {})
      return
    }

    // Push symbol
    this.push(value)
    return
  }

  // If not a symbol, push it.
  this.push(obj)
}

VimanaInterp.prototype.addPrimFun = function(name, fun)
{
  // TODO: Specify case in interpreter settings.
  name = name.toUpperCase()
  this.primFuns[name] = fun
}

VimanaInterp.prototype.pushContext = function(code, env = null)
{
  // Set env
  if (null === env)
  {
    env = (null !== code.env) ? code.env : this.currentContext.env
  }

  // Check tail call
  let context = this.currentContext
  if (context && (context.codePointer + 1 >= context.code.list.length))
  {
    // Reuse current context
    context.codePointer = -1
    context.code = code
    context.env = env
    //this.print("TAILCALL: " + this.contextIndex)
  }
  else
  {
    // Push new context
    this.currentContext = new VimanaContext(code, env)
    this.callstack.push(this.currentContext)
    ++ this.contextIndex
  }

  //this.printContext(this.currentContext)
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
  this.stack.push(obj)
  //this.printStack();
}

/*
VimanaInterp.prototype.bindIfUnbound = function(list, env)
{
  if (null === list.env)
    list.env = env
}
*/

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
  code = code.replaceAll("[", " ( ")
  code = code.replaceAll("]", " ) BIND ")
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
