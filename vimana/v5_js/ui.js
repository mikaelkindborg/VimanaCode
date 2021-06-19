
// THIS FILE IS NOT USED

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
