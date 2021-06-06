function VimanaAddPrimFuns(interp)
{
  interp.addPrimFun("Print", function(interp)
  {
    let x = interp.popEval()
    interp.print(JSON.stringify(x))
  })

  interp.addPrimFun("Eval", function(interp)
  {
    let list = interp.popEval()
    if (!VimanaIsList(list))
      interp.error("Non-list in Eval")
    interp.pushContext(list)
  })

  // Set global variable
  interp.addPrimFun("SetGlobal", function(interp)
  {
    let name = interp.pop()
    let value = interp.popEval()
    interp.print("SETGLOBAL: " + name);
    interp.print("FOOBAR");
    interp.globalEnv[name] = value
    interp.print("GLOBALENV: " + JSON.stringify(interp.globalEnv));

  })
/*
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
  // Get value of element quoted by a list
  interp.addPrimFun("First", function(interp)
  {
    let list = interp.pop()
    if (!VimanaIsList(list))
      interp.error("Non-list in First")
    interp.push(list[0])
    //interp.printStack()
  })

  interp.addPrimFun("Funify", function(interp)
  {
    // Get function definition
    let list = interp.popEval()
    if (!VimanaIsList(list))
      interp.error("Non-list in Funify")
    // Create and push function object
    let fun = new VimanaFun()
    fun.code = list
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
      interp.error("DEF: Non-list or empty body")
    
    // Get function header
    let header = interp.popEval()
    if (!VimanaIsList(header) && header.list.length)
      interp.error("DEF: Non-list or empty header")

    let funName
    
    // Does header have a single element?
    if (0 === header.list.length)
    {
      funName = header.list[0]
    }
    else
    {
      funName = header.list[header.list.length - 1]
      // TODO: Search for functions
      header.list.pop()
      body.list.unshift("=>")
      body.list.unshift(header)
    }

    let fun = new VimanaFun()
    fun.code = body
    interp.globalEnv[funName] = fun
  })

  // Bind arguments on the stack to local variables.
  interp.addPrimFun("=>", function(interp)
  {
    let context = interp.callstack[interp.contextIndex]
    let env = context.env

    // Get parameter list (includes function name as last element)
    let params = interp.pop()
    if (!VimanaIsList(params))
      interp.error("Non-array in =>")

    // Pop and bind parameters
    for (let i = params.length - 1; i > -1; --i)
    {
      let param = params[i]
      let value = interp.popEval()
      env[param] = value
    }
  })
/*
  interp.addPrimFun("TRUE", function(interp)
  {
    // TODO: Create objects for true and false?
    interp.push("TRUE")
  })

  interp.addPrimFun("FALSE", function(interp)
  {
    interp.push("FALSE")
  })
*/
  interp.addPrimFun("Eq", function(interp)
  {
    let b = interp.popEval()
    let a = interp.popEval()
    if (a === b)
      interp.push("TRUE")
    else
      interp.push("FALSE")
  })

  interp.addPrimFun("IfElse", function(interp)
  {
    let branch2 = interp.popEval()
    let branch1 = interp.popEval()
    let truth = interp.popEval()
    if (truth === "TRUE")
      interp.pushContext(branch1)
    else
      interp.pushContext(branch2)
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
}
