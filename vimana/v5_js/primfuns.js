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
    if (!Array.isArray(list))
      interp.error("Non-array in Eval")
    interp.pushContext(list)
  })

  // Set global variable
  interp.addPrimFun("SetGlobal", function(interp)
  {
    let name = interp.pop()
    let value = interp.popEval()
    interp.globalEnv[name] = value
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
    if (!Array.isArray(list))
      interp.error("NON-ARRAY IN First")
    interp.push(list[0])
    //interp.printStack()
  })

  interp.addPrimFun("Fun", function(interp)
  {
    // Get function definition
    let list = interp.popEval()
    if (!Array.isArray(list))
      interp.error("NON-ARRAY IN Fun")
    // Create and push function object
    let fun = new VimanaFunction()
    fun.code = list
    interp.push(fun)
  })

  interp.addPrimFun("Def", function(interp)
  {
    // Get function definition
    let list = interp.popEval()
    if (!Array.isArray(list))
      interp.error("NON-ARRAY IN Def")
    
    // Get first element, this must be a list with params and the 
    // function name as last element.
    let params = list[0]
    let funName = params[params.length - 1]

    // Create and set function object
    let fun = new VimanaFunction()
    fun.code = list
    interp.globalEnv[funName] = fun
  })

  // Bind arguments on the stack to local variables.
  interp.addPrimFun("=>", function(interp)
  {
    let context = interp.callstack[interp.contextIndex]
    let env = context.env

    // Get parameter list (includes function name as last element)
    let params = interp.pop()
    if (!Array.isArray(params))
    interp.error("NON-ARRAY IN =>")

    // Pop and bind parameters (skip last element)
    for (let i = params.length - 2; i > -1; --i)
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
