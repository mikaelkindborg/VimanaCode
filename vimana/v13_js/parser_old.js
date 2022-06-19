//
// File: parser.js
// Vimana parser
// Copyright (c) 2021-2022 Mikael Kindborg
// mikael@kindborg.com
//

// PARSER -------------------------------------------------

class VimanaParser
{
  // Parse a string and return a list
  parse(code, interp) 
  {
    code = this.stripComments(code)

    code = code.replaceAll("(", " ( ")
    code = code.replaceAll(")", " ) ")
    code = code.replaceAll("\n", " ")
    code = code.replaceAll("\r", " ")
    code = code.replaceAll("\t", " ")

    let tokens = code.split(" ")
    console.log(tokens)
    //$tokens = array_filter($tokens,
    //  function($token) { return strlen($token) > 0 })
    let list = this.parseTokens(tokens, interp)
    console.log(list)
    return list
  }

  // Recursively create the list tree structure
  parseTokens(tokens, interp) 
  {
    let first = { car: null, cdr: null } // temporary head item
    let item = first
    let value = null

    while (true) 
    {
      if (tokens.length === 0)
      {
        //console.log(first)
        return first.cdr
      }

      let token = tokens.shift().trim()

      if (token === "") 
      {
        continue
      }

      if (token === ")") 
      {
        //console.log(first)
        return first.cdr
      }

      if (token === "(") 
      {
        //console.log("PARSE CHILD LIST")
        value = this.parseTokens(tokens, interp)
      }
      else if (isFinite(token)) 
      {
        value = token * 1 // Convert string to number
      }
      else if (token in interp.primFuns) 
      {
        value = interp.primFuns[token]
      }
      else 
      {
        value = token
      }

      item.cdr = { car: value, cdr: null }
      item = item.cdr
    }
  }

  // Remove /-- Vimana comments --/ from the code
  // Allow nested comments (not supported in VimanaC)
  stripComments(code) 
  {
    let index = 0
    let start = 0
    let stop = 0
    let result = ""

    while (true) 
    {
      // Find start comment
      start = code.indexOf("/--", index)

      if (start === -1) 
      {
        // No start comment - find last end comment
        stop = code.lastIndexOf("--/")
        if (stop > -1) 
        {
          // Remove everything until last end comment
          result = result + code.slice(stop + 3)
        }
        else 
        {
          // No end comment - keep rest of string
          result = result + code.slice(index)
        }
        // Exit loop
        break
      }

      if (start > -1) 
      {
        // Find end comment
        stop = code.indexOf("--/", start)
        if (stop > -1) 
        {
          // Remove comment
          result = result + code.slice(index, start)
          index = stop + 3
        }
        else 
        {
          // No end comment - remove rest of string and exit loop
          result = result + code.slice(index, start)
          // Exit loop
          break
        }
      }
    }

    return result
  }
}
