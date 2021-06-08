function VimanaAddPrimFuns(interp)
{
  interp.addPrimFun("Eval", function(interp)
  {
    let list = interp.stack.pop()
    interp.checkList(list, "Eval: Got non-list")
    interp.pushContext(list)
  })

  interp.addPrimFun("Drop", function(interp)
  {
    interp.stack.pop()
  })

  // Get value of element
  interp.addPrimFun("Value", function(interp)
  {
    interp.stack.push(interp.stack.pop())
  })

  interp.addPrimFun("Call", function(interp)
  {
    let list = interp.stack.pop()
    interp.checkList(list, "Call: Got non-list")
    interp.pushContext(list, {})
  })

  // Push a shallow copy bound to current env
  interp.addPrimFun("Bind", function(interp)
  {
    let list = interp.stack.pop()
    interp.checkList(list, "Bind: Got non-list")
    // Copy list
    let block = new VimanaList()
    //interp.print("BIND LIST: " + JSON.stringify(list))
    block.list = list.list
    block.env = interp.currentContext.env
    //interp.print("BIND BLOCK: " + JSON.stringify(block))
    interp.stack.push(block)
  })
/*
  // Bind list to caller's env if NOT aready bound
  interp.addPrimFun("BindToCallerEnv", function(interp)
  {
    let list = interp.stack.pop()
    interp.checkList(list, "BindToCallerEnv: Got non-list")
    list.env = interp.callstack[interp.contextIndex - 1].env
    interp.stack.push(list)
  })
*/
  // Set global variable
  interp.addPrimFun("SetGlobal", function(interp)
  {
    //interp.print("STACK: " + JSON.stringify(interp.stack));
    let name = interp.stack.pop()
    interp.checkList(name, "SetGlobal: Name must be in a list")
    let value = interp.stack.pop()
    interp.print("SETGLOBAL: " + name);
    interp.globalEnv[name.list[0]] = value
    interp.print("GLOBALENV: " + JSON.stringify(interp.globalEnv));
  })

  // Get value of element quoted by a list
  interp.addPrimFun("First", function(interp)
  {
    let obj = interp.stack.pop()
    interp.checkList(list, "First: Got non-list")
    interp.stack.push(obj.list[0])
    //interp.printStack()
  })

  interp.addPrimFun("Funify", function(interp)
  {
    // Get function definition
    let list = interp.stack.pop()
    interp.checkList(list, "Funify: Got non-list")
    // Create and push function object
    let fun = new VimanaFun(list)
    interp.stack.push(fun)
  })

  // Forms:
  // (A B FOO) (A B +) DEF
  // (FOO) ((A B) => A B +) DEF
  interp.addPrimFun("Def", function(interp)
  {
    // Get function body
    let body = interp.stack.pop()
    if (!VimanaIsList(body) && body.list.length < 1)
      interp.error("Def: Non-list or empty body")
    
    // Get function header
    let header = interp.stack.pop()
    if (!VimanaIsList(header) && header.list.length < 1)
      interp.error("Def: Non-list or empty header")

    let funName
    
    // Does header have a single element?
    if (0 === header.list.length)
    {
      funName = header.list[0]
    }
    else
    {
      funName = header.list[header.list.length - 1]
      header.list.pop()
      body.list.unshift("=>")
      body.list.unshift(header)
    }

    let fun = new VimanaFun(body)
    interp.globalEnv[funName] = fun
  })

  // Bind arguments on the stack to local variables.
  interp.addPrimFun("=>", function(interp)
  {
    let context = interp.callstack[interp.contextIndex]
    let env = context.env

    // Get parameter list (includes function name as last element)
    let params = interp.stack.pop()
    interp.checkList(params, "=> Got non-list")

    // Pop and bind parameters
    for (let i = params.list.length - 1; i > -1; --i)
    {
      let param = params.list[i]
      let value = interp.stack.pop()
      env[param] = value
      //interp.print("ENV: " + JSON.stringify(env))
    }
  })
/*
  // Bind arguments on the stack to local variable.
  interp.addPrimFun("SetLocal", function(interp)
  {
    let context = interp.callstack[interp.contextIndex]
    let env = context.env
    let name = interp.stack.pop()
    //interp.checkSymbol(name, "SetLocal: Name is not symbol")
    let value = interp.stack.pop()
    env[name] = value
  })
*/
  interp.addPrimFun("IfElse", function(interp)
  {
    let branch2 = interp.stack.pop()
    let branch1 = interp.stack.pop()
    let truth = interp.stack.pop()
    //interp.checkList(branch1, "IfElse: Branch1 is non-list")
    //interp.checkList(branch1, "IfElse: Branch2 is non-list")
    if (truth)
      interp.pushContext(branch1)
    else
      interp.pushContext(branch2)
  })

  interp.addPrimFun("IfTrue", function(interp)
  {
    let branch = interp.stack.pop()
    let truth = interp.stack.pop()
    interp.checkList(branch, "IfTrue: Branch is non-list")
    if (truth)
      interp.pushContext(branch)
  })

  interp.addPrimFun("Eq", function(interp)
  {
    let b = interp.stack.pop()
    let a = interp.stack.pop()
    interp.stack.push(a === b)
  })

  interp.addPrimFun("Not", function(interp)
  {
    let a = interp.stack.pop()
    interp.stack.push(!a)
  })

  interp.addPrimFun("IsSmaller", function(interp)
  {
    let b = interp.stack.pop()
    let a = interp.stack.pop()
    interp.stack.push(a > b)
  })

  interp.addPrimFun("IsBigger", function(interp)
  {
    let b = interp.stack.pop()
    let a = interp.stack.pop()
    //interp.print("ISBIGGER " + a + " " + b)
    interp.stack.push(a < b)
  })

  interp.addPrimFun("+", function(interp)
  {
    let b = interp.stack.pop()
    let a = interp.stack.pop()
    interp.stack.push(a + b)
  })

  interp.addPrimFun("-", function(interp)
  {
    let b = interp.stack.pop()
    let a = interp.stack.pop()
    interp.stack.push(a - b)
  })

  interp.addPrimFun("*", function(interp)
  {
    let b = interp.stack.pop()
    let a = interp.stack.pop()
    interp.stack.push(a * b)
  })

  interp.addPrimFun("/", function(interp)
  {
    let b = interp.stack.pop()
    let a = interp.stack.pop()
    interp.stack.push(a / b)
  })

  interp.addPrimFun("Print", function(interp)
  {
    let obj = interp.stack.pop()
    interp.print(JSON.stringify(obj))
  })
}

/*
  interp.addPrimFun("TRUE", function(interp)
  {
    // TODO: Create objects for true and false?
    interp.stack.push(true)
  })

  interp.addPrimFun("FALSE", function(interp)
  {
    interp.stack.push(false)
  })

  // Get value of element quoted by a list
  interp.addPrimFun("VALUE", function(interp)
  {
    let element = interp.stack.pop()
    if (Array.isArray(element))
      interp.stack.push(interp.evalSymbol(element[0]))
    else
      interp.stack.push(interp.evalSymbol(element))
    interp.printStack()
  })
*/
