//
// File: primfuns.js
// Vimana interpreter primitive functions
// Copyright (c) 2021-2022 Mikael Kindborg
// mikael@kindborg.com
//

function VimanaDefinePrimFuns(interp)
{
  interp.defPrimFun("eval", function(interp)
  {
    let list = interp.popStack()
    interp.mustBeList(list, "eval: got non-list")
    interp.pushStackFrame(list)
  })

  interp.defPrimFun("call", function(interp)
  {
    let list = interp.popStack()
    interp.mustBeList(list, "call: got non-list")
    interp.pushStackFrame(list)
  })

  // a drop -> 
  let drop = function(interp)
  {
    interp.popStack()
  }

  interp.defPrimFun("drop", drop)

  // a dup -> a a
  let dup = function(interp)
  {
    let a = interp.popStack()
    interp.stack.push(a)
    interp.stack.push(a)
  }

  interp.defPrimFun("dup", dup)

  // a b swap -> b a
  let swap = function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.stack.push(b)
    interp.stack.push(a)
  }

  interp.defPrimFun("swap", swap)

  // a b over -> a b a
  let over = function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.stack.push(a)
    interp.stack.push(b)
    interp.stack.push(a)
  }

  interp.defPrimFun("over", over)

  // Get value of a symbol
  interp.defPrimFun("value", function(interp)
  {  
    let element = interp.popStack()
    interp.stack.push(interp.evalSymbol(element))
  })

  // (getglobal) (first value) def
  // (name) getglobal

  // Set global variable
  // value (name) setglobal ->
  let setglobal = function(interp)
  {
    let name = interp.popStack()
    interp.mustBeList(name, "setglobal: name must be in a list")

    let value = interp.popStack()
    interp.globalEnv[name.car] = value
  }
  
  interp.defPrimFun("setglobal", setglobal)

  // list funify -> fun
  let funify = function(interp)
  {
    let list = interp.popStack()
    interp.mustBeList(list, "funify: got non-list")

    list.type = "fun"
    interp.stack.push(list)
  }

  interp.defPrimFun("funify", funify)

  // Form: (name) (funbody) def
  interp.defPrimFun("def", function(interp)
  {
    funify(interp)
    swap(interp)
    setglobal(interp)
  })

  // setglobal with reversed parameter order
  // (name) value defval ->
  interp.defPrimFun("defval", function(interp)
  {
    swap(interp)
    setglobal(interp)
  })

  interp.defPrimFun("ifelse", function(interp)
  {
    let branch2 = interp.popStack()
    let branch1 = interp.popStack()
    let truth = interp.popStack()
    interp.mustBeList(branch1, "ifelse: branch1 is non-list")
    interp.mustBeList(branch2, "ifelse: branch2 is non-list")
    if (truth)
      interp.pushStackFrame(branch1)
    else
      interp.pushStackFrame(branch2)
  })

  interp.defPrimFun("iftrue", function(interp)
  {
    let branch = interp.popStack()
    let truth = interp.popStack()
    interp.mustBeList(branch, "iftrue: branch is non-list")
    if (truth)
      interp.pushStackFrame(branch)
  })

  interp.defPrimFun("iffalse", function(interp)
  {
    let branch = interp.popStack()
    let truth = interp.popStack()
    interp.mustBeList(branch, "iffalse: branch is non-list")
    if (!truth)
      interp.pushStackFrame(branch)
  })

  interp.defPrimFun("not", function(interp)
  {
    let a = interp.popStack()
    interp.stack.push(!a)
  })

  interp.defPrimFun("eq", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.stack.push(a === b)
  })

  interp.defPrimFun(">", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.stack.push(a > b)
  })

  interp.defPrimFun("<", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.stack.push(a < b)
  })

  interp.defPrimFun("+", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.stack.push(a + b)
  })

  interp.defPrimFun("-", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.stack.push(a - b)
  })

  interp.defPrimFun("*", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.stack.push(a * b)
  })

  interp.defPrimFun("/", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.stack.push(a / b)
  })

  interp.defPrimFun("1+", function(interp)
  {
    let a = interp.popStack()
    interp.stack.push(a + 1)
  })

  interp.defPrimFun("2+", function(interp)
  {
    let a = interp.popStack()
    interp.stack.push(a + 2)
  })

  interp.defPrimFun("1-", function(interp)
  {
    let a = interp.popStack()
    interp.stack.push(a - 1)
  })

  interp.defPrimFun("2-", function(interp)
  {
    let a = interp.popStack()
    interp.stack.push(a - 2)
  })

  // Get random number integer between 0 and max 1 -
  interp.defPrimFun("random", function(interp)
  {
    let max = interp.popStack()
    interp.stack.push(Math.floor(Math.random() * max))
    //interp.printStack()
  })

  // Get first element of a list
  interp.defPrimFun("first", function(interp)
  {
    let list = interp.popStack()
    interp.mustBeList(list, "first: got non-list")
    interp.stack.push(list.car)
    //interp.printStack()
  })

  interp.defPrimFun("print", function(interp)
  {
    let obj = interp.popStack()
    interp.print(obj)
  })

  interp.defPrimFun("printstack", function(interp)
  {
    VimanaUIPrintStack()
  })

  // Synonyms
  interp.defPrimFun("doc", drop)
  interp.defPrimFun("[]", drop)
  interp.defPrimFun("[XX]", dup)
  interp.defPrimFun("[YX]", swap)
  interp.defPrimFun("[XYX]", over)
  //interp.defPrimFun("[YZX]", rotate)

}
