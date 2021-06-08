function VimanaAddPrimFuns(interp)
{
  interp.addPrimFun("Eval", function(interp)
  {
    let list = interp.popEval()
    interp.checkList(list, "Eval: Got non-list")
    interp.pushContext(list)
  })

  interp.addPrimFun("Drop", function(interp)
  {
    interp.pop()
  })

  // Get value of element
  interp.addPrimFun("Value", function(interp)
  {
    interp.push(interp.popEval())
  })

  interp.addPrimFun("Call", function(interp)
  {
    let list = interp.popEval()
    interp.checkList(list, "Call: Got non-list")
    interp.pushContext(list, {})
  })

  // Push a shallow copy bound to current env
  interp.addPrimFun("Bind", function(interp)
  {
    let list = interp.popEval()
    interp.checkList(list, "Bind: Got non-list")
    // Copy list
    let block = new VimanaList()
    //interp.print("BIND LIST: " + JSON.stringify(list))
    block.list = list.list
    block.env = interp.currentContext.env
    //interp.print("BIND BLOCK: " + JSON.stringify(block))
    interp.push(block)
  })
/*
  // Bind list to caller's env if NOT aready bound
  interp.addPrimFun("BindToCallerEnv", function(interp)
  {
    let list = interp.popEval()
    interp.checkList(list, "BindToCallerEnv: Got non-list")
    list.env = interp.callstack[interp.contextIndex - 1].env
    interp.push(list)
  })
*/
  // Set global variable
  interp.addPrimFun("SetGlobal", function(interp)
  {
    //interp.print("STACK: " + JSON.stringify(interp.stack));
    let name = interp.pop()
    let value = interp.popEval()
    //interp.print("SETGLOBAL: " + name);
    interp.globalEnv[name] = value
    //interp.print("GLOBALENV: " + JSON.stringify(interp.globalEnv));
  })

  // Get value of element quoted by a list
  interp.addPrimFun("First", function(interp)
  {
    let obj = interp.pop()
    interp.checkList(list, "First: Got non-list")
    interp.push(obj.list[0])
    //interp.printStack()
  })

  interp.addPrimFun("Funify", function(interp)
  {
    // Get function definition
    let list = interp.popEval()
    interp.checkList(list, "Funify: Got non-list")
    // Create and push function object
    let fun = new VimanaFun(list)
    interp.push(fun)
  })

  // Forms:
  // (A B FOO) (A B +) DEF
  // (FOO) ((A B) => A B +) DEF
  interp.addPrimFun("Def", function(interp)
  {
    // Get function body
    let body = interp.popEval()
    if (!VimanaIsList(body) && body.list.length < 1)
      interp.error("Def: Non-list or empty body")
    
    // Get function header
    let header = interp.popEval()
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
    let params = interp.pop()
    interp.checkList(params, "=> Got non-list")

    // Pop and bind parameters
    for (let i = params.list.length - 1; i > -1; --i)
    {
      let param = params.list[i]
      let value = interp.popEval()
      env[param] = value
      //interp.print("ENV: " + JSON.stringify(env))
    }
  })

  // Bind arguments on the stack to local variables.
  interp.addPrimFun("SetLocal", function(interp)
  {
    let context = interp.callstack[interp.contextIndex]
    let env = context.env
    let name = interp.pop()
    //interp.checkSymbol(name, "SetLocal: Name is not symbol")
    let value = interp.popEval()
    env[name] = value
  })

  interp.addPrimFun("IfElse", function(interp)
  {
    let branch2 = interp.popEval()
    let branch1 = interp.popEval()
    let truth = interp.popEval()
    interp.checkList(branch1, "IfElse: Branch1 is non-list")
    interp.checkList(branch1, "IfElse: Branch2 is non-list")
    if (truth)
      interp.pushContext(branch1)
    else
      interp.pushContext(branch2)
  })

  interp.addPrimFun("IfTrue", function(interp)
  {
    let branch = interp.popEval()
    let truth = interp.popEval()
    interp.checkList(branch, "IfTrue: Branch is non-list")
    if (truth)
      interp.pushContext(branch)
  })

  interp.addPrimFun("Eq", function(interp)
  {
    let b = interp.popEval()
    let a = interp.popEval()
    interp.push(a === b)
  })

  interp.addPrimFun("Not", function(interp)
  {
    let a = interp.popEval()
    interp.push(!a)
  })

  interp.addPrimFun("IsSmaller", function(interp)
  {
    let b = interp.popEval()
    let a = interp.popEval()
    interp.push(a > b)
  })

  interp.addPrimFun("IsBigger", function(interp)
  {
    let b = interp.popEval()
    let a = interp.popEval()
    interp.push(a < b)
  })

  interp.addPrimFun("+", function(interp)
  {
    let b = interp.popEval()
    let a = interp.popEval()
    interp.push(a + b)
  })

  interp.addPrimFun("-", function(interp)
  {
    let b = interp.popEval()
    let a = interp.popEval()
    interp.push(a - b)
  })

  interp.addPrimFun("*", function(interp)
  {
    let b = interp.popEval()
    let a = interp.popEval()
    interp.push(a * b)
  })

  interp.addPrimFun("/", function(interp)
  {
    let b = interp.popEval()
    let a = interp.popEval()
    interp.push(a / b)
  })

  interp.addPrimFun("Print", function(interp)
  {
    let obj = interp.popEval()
    interp.print(JSON.stringify(obj))
  })
}

/*
  interp.addPrimFun("TRUE", function(interp)
  {
    // TODO: Create objects for true and false?
    interp.push(true)
  })

  interp.addPrimFun("FALSE", function(interp)
  {
    interp.push(false)
  })

  // Get value of element quoted by a list
  interp.addPrimFun("VALUE", function(interp)
  {
    let element = interp.pop()
    if (Array.isArray(element))
      interp.push(interp.evalSymbol(element[0]))
    else
      interp.push(interp.evalSymbol(element))
    interp.printStack()
  })
*/
