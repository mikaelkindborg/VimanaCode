<?php

//
// File: interpreter.php
// Date: 2021-04-24
// Author: Mikael Kinborg
// Email: mikael@kindborg.com
// Website: https://kindborg.com
// License: Creative Commons: Attribution-ShareAlike - CC BY-SA
//

/******************************************************************

IMPLEMENTATION NOTES
====================

This version of the interpreter implements tail call optimization.

The PHP callstack is still used. There is a limit on recursive
(non-tail) calls set by PHP.

The code is a bit messy with many conditional branches in 
interp_eval_list. I decided to use a bit function for it
to reduce the number of PHP functions calls, for perfomance.

******************************************************************/

function interp_eval_string($code, $prims = NULL)
{
  $list = interp_parse($code);
  $env = [];
  $stack = [];
  if (! isset($prims)):
    $prims = interp_create_primitives();
  endif;
  interp_eval_list($list, $env, $stack, $prims);
}

// EvalList evaluates a list as program code.
// The stack is always passed by reference.
function interp_eval_list($list, $env, &$stack, $prims)
{
  // Eval of a non-list pushes the value onto the stack.
  if (!is_array($list)):
    array_push($stack, interp_eval_element($list, $env));
    return;
  endif;
  
  $loop = TRUE;
  while ($loop):
    // We assume no tailcall.
    $loop = FALSE;
    
    // Evaluate each element in the list.
    foreach ($list as $index => $element):
      // We evaluate string symbols that represent a function 
      // or a primitive.
      if (is_string($element)):
        // Empty string is an error and should not happen.
        if (strlen($element) === 0):
          interp_println("ERROR: EMPTY STRING IN EVAL");
          exit();
        endif;
        
        // Function or list to be evaluated in case of 
        // a function call or a primitve returning a list.
        $code = NULL;
        $fun = NULL;
       
        // If the element is a primitive, call it.
        if (isset($prims[$element])):
          // Primitives may return a list to be evaluated.
          $code = $prims[$element]($stack, $env, $prims);
          if ($code === NULL):
            continue;
          endif;
        endif;
        
        /*if (isset($code)):
          interp_println("CODE: ".$code);
          interp_print_array("CODE", $code);
        endif;*/
          
        // Check if we got a list to evaluate that is NOT a funcytion.
        if (isset($code) && is_array($code) && ($code[0] != "FUN")):
          //interp_print_array("CALL LIST FROM PRIM", $code);
          // If this is the last element, do a tailcall.
          if ($index === array_key_last($list)):
            // TAILCALL
            //interp_print_array("TAILCALL LIST FROM PRIM", $code);
            $list = $code;
            $loop = TRUE;
          else:
            // NOT TAILCALL
            //interp_print_array("NON TAILCALL LIST FROM PRIM", $code);
            interp_eval_list($code, $env, $stack, $prims);
          endif;
          continue;
        endif;
        
        // Check if we got a list to evaluate that IS a function.
        if (isset($code) && is_array($code) && ($code[0] === "FUN")):
          $fun = $code;
        endif;
        
        // Lookup potential function if fun is not set.
        if (! isset($fun)):
          $fun = $env[$element];
        endif;
        
        // If function then bind parametes and evaluate.
        if (isset($fun) && is_array($fun) && ($fun[0] === "FUN")):
        
          //interp_print_array("CALL FUN", $fun);
          
          // Copy the env table to not overwrite shadowed 
          // variables permanently.
          $new_env = $env;
          
          // Get the parameter list.
          $local_vars = $fun[1];
          
          // Bind parameters to values.
          // Stack order is reverse of param order.
          foreach (array_reverse($local_vars) as $var):
            //interp_println("BIND: ".$var);
            $data = array_pop($stack);
            $value = (is_string($data) && isset($env[$data])) ? 
              $env[$data] : $data;
            $new_env[$var] = $value;
          endforeach;

          // If this is the last element, do a tailcall.
          if ($index === array_key_last($list)):
            // TAILCALL
            //interp_print_array("TAILCALL FUN", $fun);
            $list = $fun[2];
            $env = $new_env;
            $loop = TRUE;
          else:
            // NOT TAILCALL
            //interp_print_array("NON TAILCALL FUN", $fun);
            interp_eval_list($fun[2], $new_env, $stack, $prims);
          endif;
    
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
  endwhile;
}

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

// Functions for debugging.

function interp_println($str)
{
  print($str."\n");
}

function interp_print_obj($str, $obj)
{
  print($str.":\n");
  print_r($obj);
  print("\n");
}

function interp_print_array($str, $array)
{
  print($str.":\n");
  print(interp_array_as_string($array));
  print("\n");
}

function interp_array_as_string($array, $indent = "")
{
  if (!is_array($array)):
    return "".$array;
  endif;
  
  foreach ($array as $key => $value):
    $value_string = $value."";
    if (is_array($value)):
      $value_string = "(".interp_array_as_string($value, $indent."  ").")";
    endif;
    if (is_numeric($key)):
      if ($key == array_key_last($array)):
        $string .= $value_string;
      else:
        $string .= $value_string." ";
      endif;
    else:
      if ($key == array_key_last($array)):
        $string .= $indent.$key.": ".$value_string;
      else:
        $string .= $indent.$key.": ".$value_string."\n";
      endif;
    endif;
  endforeach;
  
  return $string;
}
