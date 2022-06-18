//
// File: parser.js
// Vimana parser
// Copyright (c) 2021-2022 Mikael Kindborg
// mikael@kindborg.com
//

// PARSER -------------------------------------------------

class VimanaParser
{
  constructor()
  {
    this.pos = 0
  }

  // Parse a string and return a list
  parse(code, interp) 
  {
    code = this.stripComments(code)
    this.pos = 0
    return this.parseList(code, interp)
  }

  parseList(code, interp) 
  {
    let first = { car: null, cdr: null } // temporary head item
    let item = first
    let value = null

    while (this.pos < code.length)
    {
      let char = code[this.pos]

      if (this.isWhiteSpace(char))
      {
        // Skip whitespace
        this.pos ++
        continue
      }
      else
      if (char == "(")
      {
        // Parse child list
        this.pos ++
        value = this.parseList(code, interp)
      }
      else if (char == ")")
      {
        break // done parsing child list
      }
      else if (char == "'")
      {
        // Parse string
        this.pos ++
        value = this.parseString(code)
      }
      else
      {
        // Get token
        value = this.parseToken(code)

        if (isFinite(value)) 
        {
          value = value * 1 // Convert string to number
        }
        else if (value in interp.primFuns) 
        {
          value = interp.primFuns[value]
        }
        else
        {
          value = Symbol.for(value)
        }
      }

      // Add value to list
      item.cdr = { car: value, cdr: null }
      item = item.cdr
    }

    return first.cdr
  }

  isWhiteSpace(char) 
  {
    return char == " " || char == "\n" || char == "\r" || char == "\t" 
  }

  parseString(code) 
  {
    let result = ""

    let char = code[this.pos]
    while (char != "'" && this.pos < code.length)
    {
      result += char
      this.pos ++
      char = code[this.pos]
    }

    this.pos ++
    return result
  }

  parseToken(code) 
  {
    let result = ""

    let char = code[this.pos]
    while (! this.isWhiteSpace(char) && this.pos < code.length)
    {
      result += char
      this.pos ++
      char = code[this.pos]
    }

    return result
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
