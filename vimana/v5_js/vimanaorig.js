/*
function clone(obj)
{
  this.foo = obj.foo;
  this.bar = obj.bar;
}
var obj2 = new clone(obj);
*/

let interp = InterpCreate()
InterpAddPrimFuns(interp)

function doEval()
{
  //let code = "(HELLO PRINT) DO  21 X SET  X PRINT  X 4 + 5 6 + + PRINT"
  let code = document.getElementsByTagName("textarea")[0].value
  console.log("EVAL: " + code)
  let list = interp.parse(code)
  console.log("LIST: " + code)
  interp.eval(list)
}

/*
let code = "((N) (N 0 EQ (1) (N 1 - FACT N *) IFELSE)) FUN FACT SET 20 FACT"
let list = interp.parse(code)
let t0 = performance.now()
for (let i = 0; i < 100000; i++)
{
  interp.eval(list)
}
let t1 = performance.now()
console.log("TIME: " + ((t1 - t0) / 1000) + "s")
//TIME: 4.252065000000584s
*/

/*
function fact(n)
{
  if (n === 0)
    return 1
  else 
    return n * fact(n - 1)
}
let t0 = performance.now()
for (let i = 0; i < 10000000; i++)
{
  fact(20)
}
let t1 = performance.now()
console.log("TIME: " + ((t1 - t0) / 1000) + "s")
//TIME: 0.04589500000292901s 100000 iterations
//TIME: 2.1241850000005797s  10000000 iterations
*/

/*****************************************************/
/*
Eval without timer.
  function Eval(code)
  {
    // Push root context
    PushContext(CreateContext(code))

    while (interp.contextIndex > -1)
    {

      let context = GetContext(interp.contextIndex)

      context.codePointer ++
      
      if (context.codePointer >= context.code.length)
      {
        PopContext()
        continue
      }
      
      PrintStack()
      
      let x = context.code[context.codePointer]
      
      let primFun = interp.primFuns[x]
      if (primFun)
      {
        PrintFunCall(x)
        primFun(interp)
        continue
      }
      
      let value = EvalSymbol(x)
      if (IsFun(value))
      {
        PrintFunCall(x)
        EvalFun(interp, value)
        continue
      }
      
      Push(x)
    }
  }
*/


/*
OLD VERSION
  function EvalListWithTimer(list)
  {
    let codePointer = -1

    function RunTimer()
    {
      Run()
      if (codePointer < list.length)
        setTimeout(RunTimer, 1000)
    }

    function Run()
    {
      codePointer ++
      
      if (codePointer >= list.length)
        return
      
      let x = list[codePointer]
      
      let primFun = interp.primFuns[x]
      if (primFun)
      {
        PrintFunCall(x)
        primFun(interp)
        PrintTrace("PRIMFUN")
        PrintStack()
        return
      }
      
      let value = EvalSymbol(x)
      if (IsFun(value))
      {
        PrintFunCall(x)
        EvalFun(interp, value)
        PrintTrace("FUN")
        PrintStack()
        return
      }
      
      Push(x)
      PrintTrace("PUSH")
      PrintStack()
    }

    RunTimer()
  }
*/
