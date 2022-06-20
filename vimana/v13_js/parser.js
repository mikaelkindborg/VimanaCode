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
    code = this.stripComments(code, interp)
    this.pos = 0
    let list = this.parseList(code, interp)
    return list
  }

  parseList(code, interp) 
  {
    let head = { car: null, cdr: null } // temporary head item
    let item = head
    let value = null
    let type = "undef"

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
        type = "list"
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
        type = "string"
      }
      else
      {
        // Get token as string
        let token = this.parseToken(code)

        if (isFinite(token)) 
        {
          // Set value to number (convert string to number)
          value = token * 1 
          type = "number"
        }
        else if (interp.isPrimFun(token)) 
        {
          // Set value to primfun
          value = interp.getPrimFunWithName(token)
          type = "primfun"
        }
        else
        {
          // Set value to symbol
          value = Symbol.for(token)
          type = "symbol"
        }
      }

      // Add value to list
      item.cdr = { car: value, cdr: null, type: type }
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
      if ("/--" === code.substring(pos, pos + 3))
      {
        commentLevel ++
        pos += 3
        result += " "
      }
      else
      if ("--/" === code.substring(pos, pos + 3))
      {
        commentLevel --
        if (commentLevel < 0)
        {
          interp.error("Unbalanced end comment")
        }
        pos += 3
        result += " "
      }
      else
      {
        if (0 === commentLevel)
        {
          result += code[pos]
        }
        pos ++
      }
    }

    return result
  }
} // class
