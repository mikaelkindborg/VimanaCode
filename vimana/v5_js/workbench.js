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
  VimanaUIPrint("Listing Built-in Functions (some of these are very experimental):")
  let primFuns = window.VimanaCode.primFuns
  for (let key in primFuns)
    VimanaUIPrint(key)
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
    VimanaEval(code)
  }
  catch (exception)
  {
    VimanaUIPrintException(exception)
    throw execption
  }
}

function VimanaUIEvalSelection()
{
  try
  {
    let textArea = document.getElementsByTagName("textarea")[0]
    let code = textArea.value.substring(textArea.selectionStart, textArea.selectionEnd)
    VimanaEval(code, VimanaUIPrintStack)
  }
  catch (exception)
  {
    VimanaUIPrintException(exception)
    throw exception
  }
}

function VimanaUIPrint(obj)
{
  let output = document.getElementsByTagName("textarea")[1]
  if (output.value.length > 0)
    output.value = output.value + "\n" + VimanaPrettyPrint(obj) //obj.toString()
  else
    output.value = VimanaPrettyPrint(obj) //obj.toString()
  //output.insertAdjacentHTML("beforeend", obj.toString() + "\n")
  output.scrollTop = output.scrollHeight;
}

function VimanaUIPrintStack()
{
  VimanaUIPrint("STACK: " + VimanaPrettyPrint(window.VimanaCode.stack)) //JSON.stringify(window.VimanaCode.stack))
}

function VimanaUIPrintException(exception)
{
  VimanaUIPrint(exception)
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
}

function VimanaUIEvalBenchmark()
{
  VimanaUIPrint("PLEASE WAIT...")
  setTimeout(function() {
    try
    {
      let code = document.getElementsByTagName("textarea")[0].value
      let t0 = performance.now()
      VimanaEvalFast(code)
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
