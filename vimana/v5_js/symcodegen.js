//
// File: symcodegen.js
// Generate code for Vimana minimal interpreter runnable on Arduino.
// Copyright (c) 2021 Mikael Kindborg
// mikael@kindborg.com
//

let SymbolDict = {
  symbols:  [],
  primfuns: [
    "print",
    "setglobal",
    "def",
    "+",
    "-",
    "*",
    "/"
  ]
}

// Return a string with symbolic Vimana code
function SymbolDictGenerateSymCode(source)
{
  let symcode = ""
  let i = 0

  while (i < source.length)
  {
    let c = source.charAt(i)

    if (IsWhiteSpace(c))
    {
      // Skip whitespace
      ++ i
    }
    else
    if (IsParen(c))
    {
      // Write paren
      symcode = symcode + c
      ++ i
    }
    else
    if (IsStringSeparator(c))
    {
      // Write string
      let string = SymbolDictParseString(i, source);
      symcode = symcode + string
      i = i + string.length
    }
    else 
    // Parse Token Begin
    {
      // Find next token.
      let token = SymbolDictParseToken(i, source)
      i = i + token.length
      if (IsNumber(token))
      {
        // Token is number
        symcode = symcode + "N" + token
      }
      else
      {
        // Token is primfun or symbol.
        id = SymbolDictLookupName(SymbolDict.primfuns, token);
        if (id > -1)
        {
          // Write primfun
          symcode = symcode + "P" + id
        }
        else
        {
          // Lookup the symbol
          id = SymbolDictLookupName(SymbolDict.symbols, token)
          if (id < 0)
          {
            // Symbol does not exist, add it to dictionary
            id = SymbolDictAddName(SymbolDict.symbols, token)
          }
          // Write symbol
          symcode = symcode + "S" + id
        }
      }
    } 
    // Parse Token End
  }
  // While End
  
  return symcode
}

function SymbolDictLookupName(list, name)
{
  for (let i = 0; i < list.length; ++ i)
  {
    let entry = list[i]
    if (entry === name) return i
  }
  return -1
}

function SymbolDictLookupId(list, id)
{
  return list[id]
}

function SymbolDictAddName(list, token)
{
  let i = list.length
  list[i] = token
  return i
}

function SymbolDictParseString(index, source)
{
  let i = index + 1;
  for ( ; i < source.length; ++ i)
  {
    let c = source.charAt(i)
    if (IsStringSeparator(c)) break
  }
  let string = source.substring(index, i + 1)
  return string
}

function SymbolDictParseToken(index, source)
{
  let i = index; 
  for ( ; i < source.length; ++ i)
  {
    let c = source.charAt(i)
    if (IsSeparator(c)) break
  }
  let token = source.substring(index, i)
  return token
}

function IsSeparator(c)
{
  return (IsWhiteSpace(c) || IsStringSeparator(c) || IsParen(c))
}

function IsStringSeparator(c)
{
  return ("'" === c)
}

function IsWhiteSpace(c)
{
  return (" " === c || "\t" === c || "\r" === c || "\n" === c)
}

function IsParen(c)
{
  return ("(" === c || ")" === c)
}

function IsNumber(str)
{
  return isFinite(str)
}
