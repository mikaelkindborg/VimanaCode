
VimanaInterp.prototype.print = function(s)
{
  let output = document.getElementsByTagName("output")[0]
  output.insertAdjacentHTML("beforeend", s + "<br>")
}

VimanaInterp.prototype.error = function(s)
{
  let guruMeditation = "Software Failure. Guru Meditation: " + s
  this.print(guruMeditation)
  let context = this.callstack[this.contextIndex]
  let index = context.codePointer
  let list = Array.from(context.code.list)
  list.splice(index, 0, ">>>>")
  this.print(JSON.stringify(list))
  this.print(JSON.stringify(context))
  //context = this.callstack[this.contextIndex - 1]
  //this.print(JSON.stringify(context))
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
