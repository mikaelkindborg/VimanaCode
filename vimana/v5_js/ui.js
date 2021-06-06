
VimanaInterp.prototype.print = function(s)
{
  let output = document.getElementsByTagName("output")[0]
  output.insertAdjacentHTML("beforeend", s + "<br>")
}

VimanaInterp.prototype.error = function(s)
{
  let guruMeditation = "Software Failure. Guru Meditation: " + s
  this.print(guruMeditation)
  throw guruMeditation
}

VimanaInterp.prototype.printTrace = function(s)
{
  let stack = document.getElementsByTagName("stack")[0]
  stack.insertAdjacentHTML("beforeend", s + "<br>")
}

VimanaInterp.prototype.printStack = function()
{
  let s = JSON.stringify(this.stack)
  this.printTrace(s)
}

VimanaInterp.prototype.printFunCall = function(name)
{
  let s = "CALLING: " + name
  this.printTrace(s)
}

VimanaInterp.prototype.printContext = function(context)
{
  let callstack = document.getElementsByTagName("callstack")[0]
  let s = JSON.stringify(context)
  //callstack.innerHTML = s
  callstack.insertAdjacentHTML("beforeend", s + "<br>")
}
