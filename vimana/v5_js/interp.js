
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
  this.items = []
  this.env = null
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
  this.callstackIndex = -1
  this.currentContext = null;
  this.contextSwitch = true
  this.speed = 0 //50 // ms delay in eval loop
}

VimanaInterp.prototype.evalSymbol = function(obj)
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

// Eval a list
VimanaInterp.prototype.eval = function(code)
{
  // Push root context
  this.pushContext(code, {})

  // Eval loop
  while (this.callstackIndex > -1)
  {
    this.evalNext()
  }
}

// Eval driven by a timer, which is slower but more resource friendly
VimanaInterp.prototype.timerEval = function(code)
{
  // Push root context
  this.pushContext(code, {})

  let vimana = this
  
  // Enter eval loop
  runTimer()

  function runTimer()
  {
    // Run 20 loops one each timer call.
    for (let i = 0; i < 20; ++i)
    {
      if (vimana.callstackIndex > -1)
        vimana.evalNext()
    }

    if (vimana.callstackIndex > -1)
      setTimeout(runTimer, vimana.speed)
    else
    {
      //console.log("EXIT")
      //vimana.printStack()
    }
  }
}

VimanaInterp.prototype.evalNext = function()
{
  let context = this.currentContext

  ++ context.codePointer
  
  if (context.codePointer >= context.code.items.length)
  {
    // Inline of popContext
    -- this.callstackIndex
    this.callstack.pop()
    this.currentContext = this.callstack[this.callstackIndex]
    return
  }

  let obj = context.code.items[context.codePointer]
  
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
    this.stack.push(value)
    return
  }

  // If not a symbol, push it onto the stack
  this.stack.push(obj)
}

VimanaInterp.prototype.addPrimFun = function(name, fun)
{
  // TODO: Specify case in interpreter settings.
  name = name.toUpperCase()
  this.primFuns[name] = fun
}

VimanaInterp.prototype.pushContext = function(code, env = null)
{
  // Set environment
  if (null === env)
  {
    env = (null !== code.env) ? code.env : this.currentContext.env
  }

  // Check tail call (note that currentContext is 
  // undefined on the first call to pushContext)
  let context = this.currentContext
  if (context && (context.codePointer + 1 >= context.code.items.length))
  {
    // Reuse current context
    context.codePointer = -1
    context.code = code
    context.env = env
    //this.print("TAILCALL: " + this.callstackIndex)
  }
  else
  {
    // Push new context
    this.currentContext = new VimanaContext(code, env)
    this.callstack.push(this.currentContext)
    ++ this.callstackIndex
  }

  //this.printContext(this.currentContext)
}

VimanaInterp.prototype.popContext = function()
{
  -- this.callstackIndex
  this.callstack.pop()
  this.currentContext = this.callstack[this.callstackIndex]
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
    
    list.items.push(next)
  }
}
