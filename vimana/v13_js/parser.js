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
    // TODO: debug stripComments
    //code = this.stripComments(code, interp)
    this.pos = 0
    let list = this.parseList(code, interp)
    return list
  }

  parseList(code, interp) 
  {
    let head = { car: null, cdr: null } // temporary head item
    let item = head
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
      if ("(" === char)
      {
        // Parse child list
        this.pos ++
        value = this.parseList(code, interp)
      }
      else if (")" === char)
      { 
        // Done parsing child list
        this.pos ++
        break
      }
      else if ("'" === char)
      {
        // Parse string
        this.pos ++
        value = this.parseString(code)
      }
      else
      {
        // Get token as string
        let token = this.parseToken(code)

        if (isFinite(token)) 
        {
          // Set value to number (convert string to number)
          value = token * 1 
        }
        else if (interp.isPrimFun(token)) 
        {
          // Set value to primfun
          value = interp.getPrimFunWithName(token)
        }
        else
        {
          // Set value to symbol
          value = Symbol.for(token)
        }
      }

      // Add value to list
      item.cdr = { car: value, cdr: null }
      item = item.cdr
    }
    
    // Discard head item
    return head.cdr
  }

  isWhiteSpace(char) 
  {
    return " " === char || "\n" === char || "\r" === char || "\t" === char 
  }

  isWhiteSpaceOrParen(char) 
  {
    return "(" === char || ")" === char || this.isWhiteSpace(char)
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
    while (! this.isWhiteSpaceOrParen(char) && this.pos < code.length)
    {
      result += char
      this.pos ++
      char = code[this.pos]
    }

    return result
  }

  stripComments(code, interp) 
  {
    let commentLevel = 0
    let pos = 0
    let result = ""

    while (pos < code.length) 
    {
      if ("/--" === code.substr(pos, pos + 3))
      {
        commentLevel ++
        pos += 3
      }

      if ("--/" === code.substr(pos, pos + 3))
      {
        commentLevel --
        pos += 3
      }

      if (commentLevel < 0)
      {
        interp.error("Unbalanced end comment")
      }

      if (0 === commentLevel)
      {
        result += code[pos]
      }
      
      pos ++
    }

    return result
  }

/*
  // Remove /-- Vimana comments --/ from the code
  // Allow nested comments (not supported in VimanaC)
  // TODO: Rewrite/simplify
  OLD_stripComments(code) 
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
*/

}
