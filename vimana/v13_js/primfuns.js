//
// File: primfuns.js
// Vimana interpreter primitive functions
// Copyright (c) 2021-2022 Mikael Kindborg
// mikael@kindborg.com
//

function VimanaDefPrimFuns(interp)
{
  interp.defPrimFun("eval", function(interp)
  {
    let list = interp.popDataStack()
    interp.mustBeList(list, "eval: got non-list")
    interp.pushStackFrame(list)
  })

  interp.defPrimFun("drop", function(interp)
  {
    interp.popDataStack()
  })

  interp.defPrimFun("doc", function(interp)
  {
    interp.popDataStack()
  })

  // Get value of a symbol
  interp.defPrimFun("value", function(interp)
  {  
    let element = interp.popDataStack()
    interp.stack.push(interp.evalSymbol(element))
  })

/*
  interp.defPrimFun("call", function(interp)
  {
    let list = interp.popDataStack()
    interp.mustBeList(list, "call: got non-list")
    interp.pushContext(list, {})
  })
*/

  // Set global variable
  interp.defPrimFun("setGlobal", function(interp)
  {
    //interp.print("STACK: " + JSON.stringify(interp.stack));
    let name = interp.popDataStack()
    interp.mustBeList(name, "setGlobal: name must be in a list")
    let value = interp.popDataStack()
    //interp.print("SETGLOBAL: " + name);
    interp.globalEnv[name.car] = value
    //interp.print("GLOBALENV: " + JSON.stringify(interp.globalEnv));
  })
/*
  interp.defPrimFun("funify", function(interp)
  {
    // Get function definition
    let list = interp.popDataStack()
    interp.mustBeList(list, "funify: got non-list")
    // Create and push function object
    let fun = new VimanaFun(list)
    interp.stack.push(fun)
  })
*/

/*
  // Form: (name) (body...) def
  interp.defPrimFun("def", function(interp)
  {
    // Get function body
    let body = interp.popDataStack()
    if (!VimanaObjectIsList(body) && body.items.length < 1)
      interp.error("def: non-list or empty body")
    
    // Get function header
    let header = interp.popDataStack()
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
*/

/*
  interp.defPrimFun("ifelse", function(interp)
  {
    let branch2 = interp.popDataStack()
    let branch1 = interp.popDataStack()
    let truth = interp.popDataStack()
    interp.mustBeList(branch1, "ifElse: branch1 is non-list")
    interp.mustBeList(branch2, "ifElse: branch2 is non-list")
    if (truth)
      interp.pushContext(branch1)
    else
      interp.pushContext(branch2)
  })

  interp.defPrimFun("iftrue", function(interp)
  {
    let branch = interp.popDataStack()
    let truth = interp.popDataStack()
    interp.mustBeList(branch, "ifTrue: branch is non-list")
    if (truth)
      interp.pushContext(branch)
  })

  interp.defPrimFun("eq", function(interp)
  {
    let b = interp.popDataStack()
    let a = interp.popDataStack()
    interp.stack.push(a === b)
  })

  interp.defPrimFun("not", function(interp)
  {
    let a = interp.popDataStack()
    interp.stack.push(!a)
  })

  interp.defPrimFun("isSmaller", function(interp)
  {
    let b = interp.popDataStack()
    let a = interp.popDataStack()
    interp.stack.push(a > b)
  })

  interp.defPrimFun("isBigger", function(interp)
  {
    let b = interp.popDataStack()
    let a = interp.popDataStack()
    //interp.print("ISBIGGER " + a + " " + b)
    interp.stack.push(a < b)
  })
*/

  interp.defPrimFun("+", function(interp)
  {
    let b = interp.popDataStack()
    let a = interp.popDataStack()
    interp.stack.push(a + b)
  })

  interp.defPrimFun("-", function(interp)
  {
    let b = interp.popDataStack()
    let a = interp.popDataStack()
    interp.stack.push(a - b)
  })

  interp.defPrimFun("*", function(interp)
  {
    let b = interp.popDataStack()
    let a = interp.popDataStack()
    interp.stack.push(a * b)
  })

  interp.defPrimFun("/", function(interp)
  {
    let b = interp.popDataStack()
    let a = interp.popDataStack()
    interp.stack.push(a / b)
  })

  // Get random number integer between 0 and max 1 -
  interp.defPrimFun("random", function(interp)
  {
    let max = interp.popDataStack()
    interp.stack.push(Math.floor(Math.random() * max))
    //interp.printStack()
  })

  // Get first element of a list
  interp.defPrimFun("first", function(interp)
  {
    let list = interp.popDataStack()
    interp.mustBeList(list, "first: got non-list")
    interp.stack.push(list.car)
    //interp.printStack()
  })

  interp.defPrimFun("print", function(interp)
  {
    let obj = interp.popDataStack()
    interp.print(obj)
  })

  interp.defPrimFun("printstack", function(interp)
  {
    interp.print(interp.stack)
  })
  
/*
  interp.defPrimFun("toString", function(interp)
  {
    let list = interp.popDataStack()
    interp.mustBeList(list, "toString: object is non-list")
    let string = list.items.join(" ")
    interp.stack.push(string)
  })
*/
}
