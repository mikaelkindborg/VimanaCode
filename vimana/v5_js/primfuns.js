function VimanaAddPrimFuns(interp)
{
  interp.addPrimFun("eval", function(interp)
  {
    let list = interp.stack.pop()
    interp.checkList(list, "eval: got non-list")
    interp.pushContext(list)
  })

  interp.addPrimFun("drop", function(interp)
  {
    interp.stack.pop()
  })

  interp.addPrimFun("doc", function(interp)
  {
    interp.stack.pop()
  })

  interp.addPrimFun("VIMANA-USE-UPPERCASE", function(interp)
  {
  })

  // Get value of a symbol
  interp.addPrimFun("value", function(interp)
  {  
    let element = interp.stack.pop()
    interp.stack.push(interp.evalSymbol(element))
  })

  interp.addPrimFun("call", function(interp)
  {
    let list = interp.stack.pop()
    interp.checkList(list, "call: got non-list")
    interp.pushContext(list, {})
  })

  // Push a shallow copy bound to current env
  interp.addPrimFun("bind", function(interp)
  {
    let list = interp.stack.pop()
    interp.checkList(list, "bind: got non-list")
    // Copy list
    let block = new VimanaList()
    //interp.print("BIND LIST: " + JSON.stringify(list))
    block.items = list.items
    block.env = interp.currentContext.env
    //interp.print("BIND BLOCK: " + JSON.stringify(block))
    interp.stack.push(block)
  })
  
  // Set global variable
  interp.addPrimFun("setglobal", function(interp)
  {
    //interp.print("STACK: " + JSON.stringify(interp.stack));
    let name = interp.stack.pop()
    interp.checkList(name, "setglobal: name must be in a list")
    let value = interp.stack.pop()
    //interp.print("SETGLOBAL: " + name);
    interp.globalEnv[name.items[0]] = value
    //interp.print("GLOBALENV: " + JSON.stringify(interp.globalEnv));
  })

  // Get value of element quoted by a list
  interp.addPrimFun("first", function(interp)
  {
    let obj = interp.stack.pop()
    interp.checkList(list, "first: got non-list")
    interp.stack.push(obj.items[0])
    //interp.printStack()
  })

  interp.addPrimFun("funify", function(interp)
  {
    // Get function definition
    let list = interp.stack.pop()
    interp.checkList(list, "funify: got non-list")
    // Create and push function object
    let fun = new VimanaFun(list)
    interp.stack.push(fun)
  })

  // Forms:
  // (A B FOO) (A B +) DEF
  // (FOO) ((A B) => A B +) DEF
  interp.addPrimFun("def", function(interp)
  {
    // Get function body
    let body = interp.stack.pop()
    if (!VimanaIsList(body) && body.items.length < 1)
      interp.error("def: non-list or empty body")
    
    // Get function header
    let header = interp.stack.pop()
    if (!VimanaIsList(header) && header.items.length < 1)
      interp.error("def: non-list or empty header")

    let funName
    
    // Does header have a single element?
    if (0 === header.items.length)
    {
      funName = header.items[0]
    }
    else
    {
      funName = header.items[header.items.length - 1]
      header.items.pop()
      body.items.unshift("=>")
      body.items.unshift(header)
    }

    let fun = new VimanaFun(body)
    interp.globalEnv[funName] = fun
  })

  // Bind arguments on the stack to local variables.
  let setLocal = function(interp)
  {
    let context = interp.callstack[interp.callstackIndex]
    let env = context.env

    // Get parameter list (includes function name as last element)
    let params = interp.stack.pop()
    interp.checkList(params, "=> Got non-list")

    // Pop and bind parameters
    for (let i = params.items.length - 1; i > -1; --i)
    {
      let param = params.items[i]
      let value = interp.stack.pop()
      env[param] = value
      //interp.print("ENV: " + JSON.stringify(env))
    }
  }
  // Synonyms
  interp.addPrimFun("=>", setLocal)
  interp.addPrimFun("set", setLocal)

  interp.addPrimFun("ifElse", function(interp)
  {
    let branch2 = interp.stack.pop()
    let branch1 = interp.stack.pop()
    let truth = interp.stack.pop()
    //interp.checkList(branch1, "ifElse: branch1 is non-list")
    //interp.checkList(branch1, "ifElse: branch2 is non-list")
    if (truth)
      interp.pushContext(branch1)
    else
      interp.pushContext(branch2)
  })

  interp.addPrimFun("ifTrue", function(interp)
  {
    let branch = interp.stack.pop()
    let truth = interp.stack.pop()
    interp.checkList(branch, "ifTrue: branch is non-list")
    if (truth)
      interp.pushContext(branch)
  })

  interp.addPrimFun("eq", function(interp)
  {
    let b = interp.stack.pop()
    let a = interp.stack.pop()
    interp.stack.push(a === b)
  })

  interp.addPrimFun("not", function(interp)
  {
    let a = interp.stack.pop()
    interp.stack.push(!a)
  })

  interp.addPrimFun("isSmaller", function(interp)
  {
    let b = interp.stack.pop()
    let a = interp.stack.pop()
    interp.stack.push(a > b)
  })

  interp.addPrimFun("isBigger", function(interp)
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

  interp.addPrimFun("print", function(interp)
  {
    let obj = interp.stack.pop()
    interp.print(JSON.stringify(obj))
  })
}
