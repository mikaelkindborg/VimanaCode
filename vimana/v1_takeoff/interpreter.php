<?php

//
// File: interpreter.php
// Date: 2021-04-23
// Author: Mikael Kinborg
// Email: mikael@kindborg.com
// Website: https://kindborg.com
// License: Creative Commons: Attribution-ShareAlike - CC BY-SA
//

/******************************************************************

IMPLEMENTATION NOTES
====================

This is the basic version of the Vimana interpreter.

The PHP callstack is used for all function invocations. 
There is a limit on recursive calls set by PHP. There is
no tail-recursion, which is meant to be the primary looping
construct, so useage is a bit limited.

The code is a bit messy with many conditional branches in 
interp_eval_list. I decided to use a big function for
performance reasons, to reduce the number of PHP functions calls.

******************************************************************/

function interp_eval_string($code, $prims = NULL)
{
  $env = [];
  $stack = [];
  if (! isset($prims)):
    $prims = interp_create_primitives();
  endif;
  $list = interp_parse($code);
  interp_eval_list($list, $env, $stack, $prims);
}

// EvalList evaluates a list as program code.
// The stack is always passed by reference.
function interp_eval_list($list, &$env, &$stack, $prims)
{
  // Eval of a non-list pushes the value onto the stack.
  if (!is_array($list)):
    array_push($stack, interp_eval_element($list, $env));
    return;
  endif;
  
  // Evaluate each element in the list.
  foreach ($list as $element):
      
    // We evaluate string symbols that represent a function 
    // or a primitive.
    if (is_string($element)):
      // Empty string is an error and should not happen.
      if (strlen($element) === 0):
        interp_println("ERROR: EMPTY STRING IN EVAL");
        exit();
      endif;

      // If the element is a primitive, call it.
      if (isset($prims[$element])):
        $prims[$element]($stack, $env, $prims);
        continue;
      endif;

      // If it is a function, evaluate it.
      $fun = $env[$element];
      if (isset($fun) && (is_array($fun) && ($fun[0] === "FUN"))):
        // Copy the env table to not overwrite shadowed 
        // variables permanently.
        $new_env = $env;
        
        // Get the parameter list.
        $local_vars = $fun[1];
        
        // Bind parameters to values.
        // Stack order is reverse of param order.
        foreach (array_reverse($local_vars) as $var):
          $element = array_pop($stack);
          $value = (is_string($element) && isset($env[$element])) ? 
            $env[$element] : $element;
          $new_env[$var] = $value;
        endforeach;
    
        // Evaluate the function body.
        interp_eval_list($fun[2], $new_env, $stack, $prims);
        
        continue;
      endif;
    endif; // is_string
    
    // Comments are not pushed onto the stack. They could also be
    // be filtered out by the parser. Or be displayed by a debugger.
    if (is_array($element) && $element[0] === "***"):
      continue;
    endif;
    
    // If it is not a primitive or a function, push the literal value.
    // Variables are not evaluated here. They are evaluated
    // when calling a function or performing a primitive.
    array_push($stack, $element);
    
  endforeach;
}

// Evaluates a symbol to its value if it is a variable,
// and returns the literal representation for everything else.
function interp_eval_element($element, $env)
{
  return (is_string($element) && isset($env[$element])) ? 
    $env[$element] : $element;
}

// Pops an element of the stack and evaluates it.
function interp_pop_eval(&$stack, $env)
{
  $element = array_pop($stack);
  return (is_string($element) && isset($env[$element])) ? 
    $env[$element] : $element;
  // The above statement is inline of:
  //return interp_eval_element($element, $env);
}

function interp_define_function(&$stack, &$env)
{
  $body = array_pop($stack);
  $fundef = array_pop($stack);
  $name = array_pop($fundef);
  $params = $fundef;
  $fun = ["FUN", $params, $body];
  $env[$name] = $fun;
}

// Add a native primitive.
function interp_add_primitive($symbol, $fun, &$prims)
{
  $prims[$symbol] = $fun;
}

// Parse (tokenize) a string and return a list.
function interp_parse($code)
{
  $code = str_replace("(", " ( ", $code);
  $code = str_replace(")", " ) ", $code);
  $tokens = preg_split("/[\s]+/", $code);
  // Remove empty strings.
  $tokens = array_filter($tokens,
    function($token) { return strlen($token) > 0; });
  $list = interp_parse_tokens($tokens);
  return $list;
}

// Recursively create the list tree structure.
function interp_parse_tokens(&$tokens)
{
  $list = [];
  
  while (TRUE):
    if (count($tokens) === 0):
      return $list;
    endif;
    
    $next = array_shift($tokens);
    
    if ($next === ")"):
      return $list;
    endif;
    
    if ($next === "("):
      $next = interp_parse_tokens($tokens);
    endif;
    
    // Convert string to number.
    if (is_numeric($next)):
      $next = $next + 0;
    endif;
    
    array_push($list, $next);
  endwhile;
}

// Print string followed by newline.
function interp_println($str)
{
  print($str."\n");
}

