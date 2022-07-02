//
// File: parser.js
// Vimana parser
// Copyright (c) 2021-2022 Mikael Kindborg
// mikael@kindborg.com
//

// PARSER -------------------------------------------------

// ((name {code}) (name {code}) (name {code}))

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
      else if ("{" === char)
      {
        // Parse string
        value = this.parseString(code)
      }
      else if ("}" === char)
      {
        // Unexpected!
        this.pos ++
        continue
      }
      else
      {
        // We got a non-separator char - we have a token character

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

  parseString(code) 
  {
    let result = ""
    let level = 1

    // Position is at opening curly

    // Move past opening curly
    this.pos ++

    while (this.pos < code.length)
    {
      if (this.isQuotedStringChar(code))
      {
        this.pos ++
        result += code[this.pos]
        this.pos ++
     }
     else
     {
        if ("{" === code[this.pos]) level ++
        if ("}" === code[this.pos]) level --
        if (level <= 0) break

        result += code[this.pos]
        this.pos ++
      }
    }

    // Move past closing curly
    this.pos ++

    // Position is at character after closing curly

    return result
  }

  parseToken(code) 
  {
    let result = ""

    // Position is at first token character

    while (this.pos < code.length)
    {
      if (this.isWhiteSpaceOrParen(code[this.pos])) break

      result += code[this.pos]
      this.pos ++
    }

    // Position is at character after last token character

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

  isWhiteSpace(char) 
  {
    return " " === char || "\n" === char || "\r" === char || "\t" === char 
  }

  isWhiteSpaceOrParen(char) 
  {
    return "(" === char || ")" === char || this.isWhiteSpace(char)
  }

  isQuotedStringChar(code) 
  {
    if (this.pos + 1 < code.length)
    {
      let chars = code.substring(this.pos, this.pos + 2)
      if (("\{" === chars) || ("\}" === chars))
      {
        return true
      }
    }

    return false
  }
} // class
