
let vimana = new VimanaInterp()
VimanaAddPrimFuns(vimana)

function VimanaEval()
{
  //let code = "(HELLO PRINT) EVAL  21 X SETGLOBAL  X PRINT  X 4 + 5 6 + + PRINT"
  let code = document.getElementsByTagName("textarea")[0].value
  console.log("EVAL: " + code)
  let list = VimanaParse(code)
  console.log("LIST: " + code)
  //vimana.eval(list)
  vimana.timerEval(list)
}

function VimanaBenchmark()
{
  //let code = "((N FACT) => N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF 20 FACT"
  let code = "((N FACT) => N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF"
  let list = VimanaParse(code)
  vimana.eval(list)

  code = "20 FACT"
  list = VimanaParse(code)

  let t0 = performance.now()
  for (let i = 0; i < 100000; i++)
  {
    //console.log("LOOP")
    vimana.eval(list)
  }
  let t1 = performance.now()
  vimana.print("VIMANA TIME: " + ((t1 - t0) / 1000) + "s")

  // Version 1 in Vivaldi 
  // VIMANA TIME: 4.252065000000584s

  // Version 2 in Vivaldi 
  // VIMANA TIME: 2.0683400000016263s

  // With DEF called only once in Vivaldi
  // VIMANA TIME: 1.9142899999969814s

  // Version 2 in Safari 
  // VIMANA TIME: 5.48s
}


/*
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
console.log("NATIVE TIME: " + ((t3 - t2) / 1000) + "s")
//TIME: 0.04589500000292901s 100000 iterations
//TIME: 2.1241850000005797s  10000000 iterations
*/