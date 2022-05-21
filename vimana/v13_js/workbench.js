//
// File: workbench.js
// Vimana workbench UI
// Copyright (c) 2021 Mikael Kindborg
// mikael@kindborg.com
//

VimanaUIInit()

function VimanaUIInit()
{
  VimanaInit()
  VimanaInterp.prototype.print = VimanaUIPrint
  VimanaUISelectWorkspace({ target: { value: "vimana-workspace-introduction" } })
}

function VimanaUIDoMenuCommand(event)
{
  let command = event.target.value
  if ("vimana-list-primfuns" === command)
    VimanaUIListPrimFuns()
  else
  if ("vimana-save-workspace" === command)
    VimanaUISaveWorkspace()
  else
  if ("vimana-eval-benchmark" === command)
    VimanaUIEvalBenchmark()
  else
  if ("vimana-reset-workspace" === command)
    VimanaUIResetWorkspace()
  else
  if ("vimana-generate-symbolic-code" === command)
    VimanaGenerateSymbolicCode()
  else
  if ("vimana-open-github" === command)
    VimanaUIOpenGitHub()
  else
  if ("vimana-display-mantra" === command)
    VimanaUIDisplayMantra()
  else
  if ("vimana-about" === command)
    VimanaUIAbout()
  let menu = document.querySelector(".vimana-command-menu")
  menu.selectedIndex = 0
}

function VimanaUISelectWorkspace(event)
{
  window.VimanaUIWorkspace = event.target.value
  let storedWorkspaceCode = localStorage.getItem(VimanaUIWorkspace)
  let codeArea = document.getElementsByTagName("textarea")[0]
  if (storedWorkspaceCode)
    codeArea.value = storedWorkspaceCode 
  else
    codeArea.value = document.querySelector("." + VimanaUIWorkspace).textContent
  codeArea.scrollTop = 0
}

function VimanaUISaveWorkspace()
{
  let codeArea = document.getElementsByTagName("textarea")[0]
  localStorage.setItem(VimanaUIWorkspace, codeArea.value)
  VimanaUIPrint("Workspace Saved")
}

function VimanaUIResetWorkspace()
{
  let yes = window.confirm("This will cause any edits you have made in this workspace to be lost. Do you wish to proceed?")
  if (yes) 
  {
    localStorage.removeItem(VimanaUIWorkspace)
    VimanaUISelectWorkspace({ target: { value: VimanaUIWorkspace } })
  }
}

function VimanaGenerateSymbolicCode()
{
  VimanaUIPrint("CODE RUNNABLE ON ADRUINO")
  let code = document.getElementsByTagName("textarea")[0].value
  let symcode = SymbolDictGenerateSymCode(code)
  VimanaUIPrint(symcode)
  window.prompt("Copy to clipboard: Ctrl+C, Enter", symcode)
  //VimanaUIPrint(SymbolDictGenerateSymCode("1 2 3 'Hi World''Hi'(foo bar) print"))
}

function VimanaUIClearStack()
{
  window.VimanaCode.stack = []
  VimanaUIPrintStack()
}

function VimanaUIClearOutput()
{
  document.getElementsByTagName("textarea")[1].value = ""
}

function VimanaUIListPrimFuns()
{
  VimanaUIPrint("BUILT-IN FUNCTIONS:")
  let primFuns = window.VimanaCode.primFuns
  for (let key in primFuns)
  {
    VimanaUIPrint(key)
  }
}

function VimanaUIOpenGitHub()
{
  window.location.href = "https://github.com/mikaelkindborg/VimanaCode"
}

function VimanaUIDisplayMantra()
{
  VimanaEval(`
    ((I am conscious) 
     (I am present) 
     (I exist here and now)
     (I sense the air that surrounds me)
     (I follow my breath)
     (I feel my feet in contact with the ground)
     (I sense the gravity of the Earth)
     (I am aware of where I am and what time it is))
    (MANTRA-LIST) SET 
    MANTRA-LIST LENGTH RANDOM
      MANTRA-LIST GETAT 
        TOSTRING PRINT
  `)
}

function VimanaUIAbout()
{
  VimanaEval("(My name is Mikael Kindborg. I created Vimana as a hobby project. I have programmed computers for more than 35 years in 35 different programming languages. Vimana encapsulates the essense of several things I like: dynamic typing, code and data have the same format, interactive development, simplicity, few basic constructs, and an easy-to-implement intrepreter.) TOSTRING PRINT")
}

function VimanaUIEvalWorkspace()
{
  try
  {
    let code = document.getElementsByTagName("textarea")[0].value
    VimanaEvalAsync(
      code, 
      function() 
      {
        VimanaUIPrint("DONE")
      })
  }
  catch (exception)
  {
    console.log("VimanaUIEvalWorkspace")
    console.log(exception)
    VimanaUIPrintException(exception)
    throw exception
  }
}

function VimanaUIEvalSelection()
{
  try
  {
    let textArea = document.getElementsByTagName("textarea")[0]
    let code = textArea.value.substring(textArea.selectionStart, textArea.selectionEnd)
    VimanaEvalAsync(
      code, 
      function() 
      {
        VimanaUIPrintStack()
      })
  }
  catch (exception)
  {
    console.log("VimanaUIEvalSelection")
    console.log(exception)
    VimanaUIPrintException(exception)
    throw exception
  }
}

function VimanaUIPrint(obj)
{
  let output = document.getElementsByTagName("textarea")[1]
  if (output.value.length > 0)
  {
    output.value = output.value + "\n" + window.VimanaCode.prettyPrint(obj) //obj.toString()
  }
  else
  {
    output.value = window.VimanaCode.prettyPrint(obj) //obj.toString()
  }
  //output.insertAdjacentHTML("beforeend", obj.toString() + "\n")
  output.scrollTop = output.scrollHeight;
}

function VimanaUIPrintStack()
{
  VimanaUIPrint("STACK: " + window.VimanaCode.prettyPrintStack()) //JSON.stringify(window.VimanaCode.stack))
}

function VimanaUIPrintException(exception)
{
  VimanaUIPrint(exception)
  /*
  let interp = window.VimanaCode
  let context = interp.callstack[interp.callstackIndex]
  if (context && context.codePointer)
  {
    let index = context.codePointer
    let array = Array.from(context.code.items)
    array.splice(index, 0, "ERROR HERE >>>>")
    VimanaUIPrint("CODE: " + JSON.stringify(array))
    VimanaUIPrint("CONTEXT: " + JSON.stringify(context))
  }
  */
}

function VimanaUIEvalBenchmark()
{
  VimanaUIPrint("PLEASE WAIT...")
  setTimeout(function() {
    try
    {
      let code = document.getElementsByTagName("textarea")[0].value
      let t0 = performance.now()
      VimanaEval(code)
      let t1 = performance.now()
      VimanaUIPrint("TIME: " + ((t1 - t0) / 1000) + "s")
    }
    catch (exception)
    {
      VimanaUIPrintException(exception)
      throw exception
    }
  }, 100)

/*
  let code =
    `(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF
    (L N REPEAT) (0 N ISBIGGER (L EVAL L N 1 - REPEAT) IFTRUE) DEF
    [20 FACT DROP] 100000 REPEAT`
  let t0 = performance.now()
  let list = VimanaParse(code)
  window.VimanaCode.eval(list)
  let t1 = performance.now()
  VimanaUIPrint("VIMANA TIME (100,000 ITERATIONS): " + ((t1 - t0) / 1000) + "s")

  let code = "(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF"
  let list = VimanaParse(code)
  vimana.eval(list)
  code = "20 FACT DROP"
  list = VimanaParse(code)
  let t0 = performance.now()
  for (let i = 0; i < 100000; i++)
  {
    //console.log("LOOP")
    vimana.eval(list)
  }
  let t1 = performance.now()
  vimana.print("VIMANA TIME: " + ((t1 - t0) / 1000) + "s")
  vimana.print("STACKSIZE: " + vimana.stack.length)
*/
  // Version 1 in Vivaldi 
  // VIMANA TIME: 4.252065000000584s

  // Version 2 in Vivaldi 
  // VIMANA TIME: 2.0683400000016263s

  // Version 2 in Safari 
  // VIMANA TIME: 5.48s

  // With DEF called only once in benchmark
  // VIMANA TIME: 1.9142899999969814s

  // 210606 (branch js2)
  // Version 3 in Vivaldi:
  // 1.1230649999997695s

  // With VimanaNum object:
  // 1.2570550000000367s

  // Without number object:
  // 1.187684999999874s

  // Some inlining in interp.evalSymbol:
  // 1.1437999999998283s

  // Optimized context referencing in interpreter loop:
  // 1.0306600000003527s

  // Using JS booleans:
  // 0.9029350000000704s
  // 0.8519100000012259s

  // 210608 (branch js2)
  // Tailcall version:
  // 0.8155650000007881s

  // Some further cleanup:
  // 0.7900100000006205s

  // Removal of redundant evalSymbol:
  // 0.7702100000005885s

  // 210608 
  // New benchmark using REPEAT:
  // 0.9401600000001054s
  // I am very happy with this performance given the  
  // nature of the implementation of the interpreter
  // 210611
  // 0.9123999999985098s
  // 210620
  // 0.9001000000005588s
}

function VimanaUIRunNativeBenchmark()
{
  function fib(n)
  {
    if (n < 2)
      return n
    else 
      return fib(n - 1) + fib(n - 2)
  }
  let t1 = performance.now()
  let n = fib(37)
  let t2 = performance.now()
  VimanaUIPrint("fib(37)): " + n)
  VimanaUIPrint("NATIVE TIME fib(37)): " + ((t2 - t1) / 1000) + "s")
  //TIME: 
}

function VimanaUIRunNativeBenchmarkFact()
{
  function fact(n)
  {
    if (n === 0)
      return 1
    else 
      return n * fact(n - 1)
  }
  let t1 = performance.now()
  for (let i = 0; i < 10000000; i++)
  {
    fact(20)
  }
  let t2 = performance.now()
  VimanaUIPrint("NATIVE TIME (10,000,000 ITERATIONS): " + ((t2 - t1) / 1000) + "s")
  //TIME: 0.04589500000292901s 100000 iterations
  //TIME: 2.1241850000005797s  10000000 iterations
}

// SLOWEST
function VimanaUIRunBenchmarkConsArray()
{
  let t1 = performance.now()

  var head = new Array(2)
  head[0] = 42
  var item = head
  var n = 1000 * 1000 * 10

  for (var i = 0; i < n; ++ i)
  {
    item[1] = new Array(2)
    item = item[1];
    item[0] = tossDice()
  }
  item[1] = null

  var item = head
  for (var i = 0; i < 10; ++ i)
  {
    VimanaUIPrint("ITEM: " + item[0])
    item = item[1]
  }

  let t2 = performance.now()

  VimanaUIPrint("TIME CONS ARRAY: " + ((t2 - t1) / 1000) + "s")
}

// FASTEST
function VimanaUIRunBenchmarkConsObj()
{
  let t1 = performance.now()

  var head = { car: 42, cdr: null }
  var item = head
  var n = 1000 * 1000 * 10

  for (var i = 0; i < n; ++ i)
  {
    item.cdr = { car: tossDice(), cdr: null }
    item = item.cdr
  }

  var item = head
  for (var i = 0; i < 10; ++ i)
  {
    VimanaUIPrint("ITEM: " + item.car);
    item = item.cdr;
  }

  let t2 = performance.now()

  VimanaUIPrint("TIME CONS OBJ: " + ((t2 - t1) / 1000) + "s")
}

function VimanaUIRunBenchmarkConsFunObj()
{
  let t1 = performance.now()

  var head = new Item(42, null)
  var item = head
  var n = 1000 * 1000 * 10

  for (var i = 0; i < n; ++ i)
  {
    item.setCdr(new Item(tossDice(), null))
    item = item.cdr
  }

  var item = head
  for (var i = 0; i < 10; ++ i)
  {
    VimanaUIPrint("ITEM: " + item.car);
    item = item.cdr
  }

  let t2 = performance.now()

  VimanaUIPrint("TIME CONS FUNOBJ: " + ((t2 - t1) / 1000) + "s")
}

/*
INCREDIBLY SLOW
function VimanaUIRunBenchmarkConsMap()
{
  let t1 = performance.now()

  var head = new Map()
  head.set(0, 42)
  head.set(1, null)

  var item = head

  var n = 1000 * 1000 * 10

  for (var i = 0; i < n; ++ i)
  {
    var next = new Map()
    next.set(0, tossDice())
    next.set(1, null)
    item.set(1, next)
    item = next
  }

  var item = head
  for (var i = 0; i < 10; ++ i)
  {
    VimanaUIPrint("ITEM: " + item.get(0));
    item = item.get(1);
  }

  let t2 = performance.now()

  VimanaUIPrint("TIME CONS MAP: " + ((t2 - t1) / 1000) + "s")
}
*/

/*
INCREDIBLY SLOW
function VimanaUIRunBenchmarkConsObjNull()
{
  let t1 = performance.now()

  var head = Object.create(null)
  head.car = 42
  head.cdr = null
  var item = head
  var n = 1000 * 1000 * 10

  for (var i = 0; i < n; ++ i)
  {
    var next = Object.create(null)
    next.car = tossDice()
    next.cdr = null
    item.cdr = next
    item = next
  }

  var item = head
  for (var i = 0; i < 10; ++ i)
  {
    VimanaUIPrint("ITEM: " + item.car);
    item = item.cdr;
  }

  let t2 = performance.now()

  VimanaUIPrint("TIME CONS OBJ NULL: " + ((t2 - t1) / 1000) + "s")
}
*/

// TODO: Test performance for
//   instanceof vs 
//   typeof vs 
//   obj with type property
// And test performance for
//   symbol key vs string key

/*
Vimana types:

List (cons objects)
Function (cons with type property? custom object type (function?))
Primitive (js function)
Number (js number)
String (js string)
Symbol (js symbol)
*/

function VimanaUIRunBenchmarkPlainArray()
{
  let t1 = performance.now()

  var items = new Array()
  items[0] = 42
  var n = 1000 * 1000 * 10

  for (var i = 0; i < n; ++ i)
  {
    items[i] = tossDice()
  }

  for (var i = 0; i < 10; ++ i)
  {
    VimanaUIPrint("ITEM: " + items[i])
  }

  let t2 = performance.now()

  VimanaUIPrint("TIME PLAIN ARRAY: " + ((t2 - t1) / 1000) + "s")
}

function Item(car, cdr)
{
  this.car = car
  this.cdr = cdr
}

Item.prototype.setCdr = function(item)
{
  this.cdr = item
}

function tossDice()
{
  return Math.floor(Math.random() * 6) + 1
}
