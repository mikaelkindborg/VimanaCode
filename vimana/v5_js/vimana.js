
window.VimanaCode = new VimanaInterp()
VimanaAddPrimFuns(window.VimanaCode)

// Evaluate a string.
function VimanaEval(string)
{
  let list = VimanaParse(string)
  window.VimanaCode.timerEval(list)
  // This is faster, but is less resource friendly and 
  // can slow down the browser
  //window.VimanaCode.eval(list)
}

// This function is used to call back from JS to Vimana
function VimanaCallFun(funName, params = [])
{
  // Push params on stack (note the order)
  for (let i = params.length - 1; i > -1; --i)
  {
    window.VimanaCode.stack.push(params[i])
  }

  // Lookup function in globalEnv
  let fun = window.VimanaCode.globalEnv[funName]

  // Call function
  window.VimanaCode.pushContext(fun.code, {})
}

// UI commands, TODO: move to another file (workbench.js - rename ui.js)
function VimanaDoEval()
{
  try
  {
    let code = document.getElementsByTagName("textarea")[0].value
    VimanaEval(code)
  }
  catch (exception)
  {
    VimanaPrint(exception)
  }
}

function VimanaPrint(obj)
{
  let output = document.getElementsByTagName("textarea")[1]
  output.insertAdjacentHTML("beforeend", obj.toString() + "\n")
}

function VimanaDoBenchmark()
{

  let code = `
  (N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF
  (L N REPEAT) (0 N ISBIGGER (L EVAL L N 1 - REPEAT) IFTRUE) DEF
  [20 FACT DROP] 100000 REPEAT
  `
  let t0 = performance.now()
  let list = VimanaParse(code)
  window.VimanaCode.eval(list)
  let t1 = performance.now()
  window.VimanaCode.print("VIMANA TIME: " + ((t1 - t0) / 1000) + "s")

/*
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
}

function VimanaDoNativeBenchmark()
{
  function fact(n)
  {
    if (n === 0)
      return 1
    else 
      return n * fact(n - 1)
  }
  let t2 = performance.now()
  for (let i = 0; i < 10000000; i++)
  {
    fact(20)
  }
  let t3 = performance.now()
  vimana.print("NATIVE TIME: " + ((t3 - t2) / 1000) + "s")
  //TIME: 0.04589500000292901s 100000 iterations
  //TIME: 2.1241850000005797s  10000000 iterations
}
