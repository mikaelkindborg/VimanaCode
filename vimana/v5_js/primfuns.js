//
// File: primfuns.js
// Vimana interpreter primitive functions
// Copyright (c) 2021 Mikael Kindborg
// mikael@kindborg.com
//

function VimanaAddPrimFuns(interp)
{
  interp.addPrimFun("eval", function(interp)
  {
    let list = interp.popStack()
    interp.mustBeList(list, "eval: got non-list")
    interp.pushContext(list)
  })

  interp.addPrimFun("drop", function(interp)
  {
    interp.popStack()
  })

  interp.addPrimFun("doc", function(interp)
  {
    interp.popStack()
  })

  // Get value of a symbol
  interp.addPrimFun("value", function(interp)
  {  
    let element = interp.popStack()
    interp.stack.push(interp.evalSymbol(element))
  })

  interp.addPrimFun("call", function(interp)
  {
    let list = interp.popStack()
    interp.mustBeList(list, "call: got non-list")
    interp.pushContext(list, {})
  })

  // Push a shallow copy bound to current env
  interp.addPrimFun("bind", function(interp)
  {
    let list = interp.popStack()
    interp.mustBeList(list, "bind: got non-list")
    // Copy list
    let block = new VimanaList()
    //interp.print("BIND LIST: " + JSON.stringify(list))
    block.items = list.items
    block.env = interp.currentContext.env
    //interp.print("BIND BLOCK: " + JSON.stringify(block))
    interp.stack.push(block)
  })
  
  // Set global variable
  interp.addPrimFun("setGlobal", function(interp)
  {
    //interp.print("STACK: " + JSON.stringify(interp.stack));
    let name = interp.popStack()
    interp.mustBeList(name, "setGlobal: name must be in a list")
    let value = interp.popStack()
    //interp.print("SETGLOBAL: " + name);
    interp.globalEnv[name.items[0]] = value
    //interp.print("GLOBALENV: " + JSON.stringify(interp.globalEnv));
  })

  interp.addPrimFun("funify", function(interp)
  {
    // Get function definition
    let list = interp.popStack()
    interp.mustBeList(list, "funify: got non-list")
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
    let body = interp.popStack()
    if (!VimanaObjectIsList(body) && body.items.length < 1)
      interp.error("def: non-list or empty body")
    
    // Get function header
    let header = interp.popStack()
    if (!VimanaObjectIsList(header) && header.items.length < 1)
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
    let params = interp.popStack()
    interp.mustBeList(params, "setLocal: got non-list")

    // Pop and bind parameters
    for (let i = params.items.length - 1; i > -1; --i)
    {
      let param = params.items[i]
      let value = interp.popStack()
      env[param] = value
      //interp.print("ENV: " + JSON.stringify(env))
    }
  }
  // Synonyms
  interp.addPrimFun("=>", setLocal)
  interp.addPrimFun("set", setLocal)

  interp.addPrimFun("ifElse", function(interp)
  {
    let branch2 = interp.popStack()
    let branch1 = interp.popStack()
    let truth = interp.popStack()
    interp.mustBeList(branch1, "ifElse: branch1 is non-list")
    interp.mustBeList(branch2, "ifElse: branch2 is non-list")
    if (truth)
      interp.pushContext(branch1)
    else
      interp.pushContext(branch2)
  })

  interp.addPrimFun("ifTrue", function(interp)
  {
    let branch = interp.popStack()
    let truth = interp.popStack()
    interp.mustBeList(branch, "ifTrue: branch is non-list")
    if (truth)
      interp.pushContext(branch)
  })

  interp.addPrimFun("eq", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.stack.push(a === b)
  })

  interp.addPrimFun("not", function(interp)
  {
    let a = interp.popStack()
    interp.stack.push(!a)
  })

  interp.addPrimFun("isSmaller", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.stack.push(a > b)
  })

  interp.addPrimFun("isBigger", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    //interp.print("ISBIGGER " + a + " " + b)
    interp.stack.push(a < b)
  })

  interp.addPrimFun("+", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.stack.push(a + b)
  })

  interp.addPrimFun("-", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.stack.push(a - b)
  })

  interp.addPrimFun("*", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.stack.push(a * b)
  })

  interp.addPrimFun("/", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.stack.push(a / b)
  })

  // Get random number integer between 0 and max 1 -
  interp.addPrimFun("random", function(interp)
  {
    let max = interp.popStack()
    interp.stack.push(Math.floor(Math.random() * max))
    //interp.printStack()
  })

  // Get length of a list
  interp.addPrimFun("length", function(interp)
  {
    let list = interp.popStack()
    interp.mustBeList(list, "first: got non-list")
    interp.stack.push(list.items.length)
    //interp.printStack()
  })

  // Get first element of a list
  interp.addPrimFun("first", function(interp)
  {
    let list = interp.popStack()
    interp.mustBeList(list, "first: got non-list")
    interp.stack.push(list.items[0])
    //interp.printStack()
  })

  // Get list element at index
  interp.addPrimFun("getAt", function(interp)
  {
    let list = interp.popStack()
    let index = interp.popStack()
    interp.mustBeList(list, "getAt: got non-list")
    interp.stack.push(list.items[index])
  })

  // Set list element at index
  interp.addPrimFun("setAt", function(interp)
  {
    let list = interp.popStack()
    let obj = interp.popStack()
    let index = interp.popStack()
    interp.mustBeList(list, "getAt: got non-list")
    list.items[index] = obj
  })

  // Delete list element at index
  interp.addPrimFun("deleteAt", function(interp)
  {
    let list = interp.popStack()
    let index = interp.popStack()
    interp.mustBeList(list, "getAt: got non-list")
    list.item.splice(index, 1)
  })

  interp.addPrimFun("print", function(interp)
  {
    let obj = interp.popStack()
    interp.print(obj)
  })

  interp.addPrimFun("printStack", function(interp)
  {
    interp.print(interp.stack)
  })

  interp.addPrimFun("toString", function(interp)
  {
    let list = interp.popStack()
    interp.mustBeList(list, "toString: object is non-list")
    let string = list.items.join(" ")
    interp.stack.push(string)
  })

/*
  // Experimental
  // Push next item in the code list without evaluating it
  interp.addPrimFun("quote", function(interp)
  {  
    let context = interp.currentContext
    let items = context.code.items
    let codePointer = context.codePointer + 1
    if (codePointer < items.length)
    {
      interp.stack.push(items[codePointer])
      context.codePointer = codePointer
    }
    else
      interp.error("quote: end of code list")
  })

  // Experimental
  // Push next item in the parent context code list without evaluating it
  interp.addPrimFun("nextParentItem", function(interp)
  {
    let index = interp.callstackIndex
    if (index < 1)
      interp.error("nextParentItem: no parent context")
    let context = this.callstack[index - 1]
    let items = context.code.items
    let codePointer = context.codePointer + 1
    if (codePointer < items.length)
    {
      interp.stack.push(items[codePointer])
      context.codePointer = codePointer
    }
    else
      interp.error("nextParentItem: end of code list")
  })
*/
}
