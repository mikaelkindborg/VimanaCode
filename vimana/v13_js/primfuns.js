//
// File: primfuns.js
// Vimana interpreter primitive functions
// Copyright (c) 2021-2022 Mikael Kindborg
// mikael@kindborg.com
//

function VimanaDefinePrimFuns(interp)
{
  // EVAL -------------------------------------------------

  // x inspect ->
  interp.defPrimFun("inspect", function(interp)
  {
    let x = interp.popStack()
    console.log(x)
  })

  // inspectglobals ->
  interp.defPrimFun("inspectglobals", function(interp)
  {
    console.log(interp.globalVars)
  })

  // list eval -> ?
  interp.defPrimFun("evalJS", function(interp)
  {
    let js = interp.popStack()
    new Function("x", "y", "return x*y;")
  })

  // string parse -> list
  interp.defPrimFun("parse", function(interp)
  {
    let string = interp.popStack()
    //TODO interp.mustBeString(string, "parse: got non-string")
    let list = interp.parse(string)
    interp.pushStack(list)
  })

  // list eval -> ?
  interp.defPrimFun("eval", function(interp)
  {
    let list = interp.popStack()
    interp.mustBeList(list, "eval: got non-list")
    interp.pushStackFrame(list)
  })

  // TODO: Push a stackframe with own enviroment
  // list call -> ?
  interp.defPrimFun("call", function(interp)
  {
    interp.pushStackFrame(list)
  })
  let call = interp.getPrimFunWithName("call")

  // list timetorun -> seconds
  interp.defPrimFun("timetorun", function(interp)
  {
    let list = interp.popStack()
    interp.mustBeList(list, "timetorun: got non-list")

    let t1 = performance.now()
    interp.eval(list)
    let t2 = performance.now()

    let seconds = (t2 - t1) / 1000
    interp.pushStack(seconds)
  })

  // STACK OPERATIONS -------------------------------------

  // a drop -> 
  interp.defPrimFun("drop", function(interp)
  {
    interp.popStack()
  })
  let drop = interp.getPrimFunWithName("drop")

  // a dup -> a a
  interp.defPrimFun("dup", function(interp)
  {
    let a = interp.popStack()
    interp.pushStack(a)
    interp.pushStack(a)
  })
  let dup = interp.getPrimFunWithName("dup")

  // a 2dup -> a a a
  interp.defPrimFun("2dup", function(interp)
  {
    dup()
    dup()
  })
  let dupdup = interp.getPrimFunWithName("2dup")

  // a b swap -> b a
  interp.defPrimFun("swap", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.pushStack(b)
    interp.pushStack(a)
  })
  let swap = interp.getPrimFunWithName("swap")

  // a b over -> a b a
  interp.defPrimFun("over", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.pushStack(a)
    interp.pushStack(b)
    interp.pushStack(a)
  })
  let over = interp.getPrimFunWithName("over")

  // ALTERNATIVE NAMES FOR STACK OPERATIONS ---------------

  interp.defPrimFun("doc", drop)
  interp.defPrimFun("[]", drop)
  interp.defPrimFun("[aa]", dup)
  interp.defPrimFun("[aaa]", dupdup)
  interp.defPrimFun("[ba]", swap)
  interp.defPrimFun("[aba]", over)
  //interp.defPrimFun("[bca]", rotate)

  // GLOBAL VARS ------------------------------------------

  // Set global variable
  // value (name) setglobal ->
  interp.defPrimFun("setglobal", function(interp)
  {
    let name = interp.popStack()
    interp.mustBeList(name, "setglobal: name must be in a list")

    let value = interp.popStack()
    interp.setGlobalVar(name.car, value)
  })
  let setglobal = interp.getPrimFunWithName("setglobal")

  // setglobal with reversed parameter order
  // (name) value defvar ->
  interp.defPrimFun("defvar", function(interp)
  {
    swap(interp)
    setglobal(interp)
  })

  // Get global variable
  // (name) getglobal -> value
  interp.defPrimFun("getglobal", function(interp)
  {
    let name = interp.popStack()
    interp.mustBeList(name, "getglobal: name must be in a list")

    interp.pushStack(interp.getGlobalVar(name.car))
  })

  // DEFINE FUNCTIONS -------------------------------------

  // list funify -> fun
  interp.defPrimFun("funify", function(interp)
  {
    let list = interp.popStack()
    interp.mustBeList(list, "funify: got non-list")

    list.type = "fun"
    interp.pushStack(list)
  })
  let funify = interp.getPrimFunWithName("funify")

  // (name) (funbody) def ->
  interp.defPrimFun("def", function(interp)
  {
    funify(interp)
    swap(interp)
    setglobal(interp)
  })

  // CONDITIONALS -----------------------------------------

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
    interp.pushStack(!a)
  })

  interp.defPrimFun("eq", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    let equals = false

    // Compare lists for equality
    if ("object" === typeof (a) && "object" === typeof (b))
    {
      equals = a.car === b.car && a.cdr === b.cdr
    }
    else
    {
      equals = a === b
    }

    interp.pushStack(equals)
  })

  // MATH FUNCTIONS ---------------------------------------

  interp.defPrimFun(">", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.pushStack(a > b)
  })

  interp.defPrimFun("<", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.pushStack(a < b)
  })

  interp.defPrimFun("+", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.pushStack(a + b)
  })

  interp.defPrimFun("-", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.pushStack(a - b)
  })

  interp.defPrimFun("*", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.pushStack(a * b)
  })

  interp.defPrimFun("/", function(interp)
  {
    let b = interp.popStack()
    let a = interp.popStack()
    interp.pushStack(a / b)
  })

  interp.defPrimFun("1+", function(interp)
  {
    let a = interp.popStack()
    interp.pushStack(a + 1)
  })

  interp.defPrimFun("2+", function(interp)
  {
    let a = interp.popStack()
    interp.pushStack(a + 2)
  })

  interp.defPrimFun("1-", function(interp)
  {
    let a = interp.popStack()
    interp.pushStack(a - 1)
  })

  interp.defPrimFun("2-", function(interp)
  {
    let a = interp.popStack()
    interp.pushStack(a - 2)
  })

  // Get random number integer between 0 and max 1 -
  interp.defPrimFun("random", function(interp)
  {
    let max = interp.popStack()
    interp.pushStack(Math.floor(Math.random() * max))
    //interp.printStack()
  })

  // LIST FUNCTIONS ---------------------------------------

  // Get first item of a list
  // list first -> item
  interp.defPrimFun("first", function(interp)
  {
    let list = interp.popStack()
    interp.mustBeList(list, "first: got non-list")
    interp.pushStack(list.car)
  })

  // Get rest of a list
  // list rest -> list
  interp.defPrimFun("rest", function(interp)
  {
    let list = interp.popStack()
    interp.mustBeList(list, "rest: got non-list")
    interp.pushStack(list.cdr)
  })

  // Cons item onto a list
  // item list cons -> list
  interp.defPrimFun("cons", function(interp)
  {
    let list = interp.popStack()
    let item = interp.popStack()
    interp.mustBeList(list, "cons: got non-list")
    let first = { car: item, cdr: list }
    interp.pushStack(first)
  })

  // Replace first element of a list
  // list item setfirst -> 
  interp.defPrimFun("setfirst", function(interp)
  {
    let item = interp.popStack()
    let list = interp.popStack()
    interp.mustBeList(list, "setfirst: got non-list")
    list.car = item
  })

  // Replace rest of a list
  // list list setrest -> 
  interp.defPrimFun("setrest", function(interp)
  {
    let rest = interp.popStack()
    let list = interp.popStack()
    interp.mustBeList(list, "setfirst: got non-list")
    list.cdr = rest
  })

  // PRINT FUNCTIONS --------------------------------------

  interp.defPrimFun("print", function(interp)
  {
    let obj = interp.popStack()
    TheVimanaUI.commandPrettyPrint(obj)
  })

  interp.defPrimFun("printstack", function(interp)
  {
    TheVimanaUI.commandPrintStack()
  })

  // EXPERIMENTAL DRAWING FUNCTIONS -----------------------

  interp.defPrimFun("draw", function(interp)
  {
    let canvas = document.getElementById("vimana-canvas")
    let surface = canvas.getContext("2d")
    surface.fillStyle = "rgb(255,255,100)"
    surface.fillRect(0, 0, 200, 200)
  })

  // x y w h fillrect
  interp.defPrimFun("fillrect", function(interp)
  {
    let h = interp.popStack()
    let w = interp.popStack()
    let y = interp.popStack()
    let x = interp.popStack()
    let canvas = document.getElementById("vimana-canvas")
    let surface = canvas.getContext("2d")
    //surface.fillStyle = "rgb(0,255,255)"
    surface.fillRect(x, y, w, h)
    //surface.fillRect(0, 0, 200, 200)
  })

  // r g b setcolor
  interp.defPrimFun("setcolor", function(interp)
  {
    let b = interp.popStack()
    let g = interp.popStack()
    let r = interp.popStack()
    let canvas = document.getElementById("vimana-canvas")
    let surface = canvas.getContext("2d")
    surface.fillStyle = "rgb(" + r + "," + g + "," + b + ")"
  })
}
