<?php

//
// File: interpreter.php
// Date: 2021-04-20
// Author: Mikael Kinborg
// Email: mikael@kindborg.com
// Website: kindborg.com
// License: Creative Commons: Attribution-ShareAlike - CC BY-SA
//
// This file contains an interpeter for an experimental language
// named Vimana. I have created this project for my own
// purposes, simply because for the fun of it. The inspiration
// comes from Lisp and Forth. 
// I am an old Lisp programmer who did computer science research 
// using MacLisp, MuLisp, Interlisp-D, Smalltalk, and HyperCard 
// in the 1980s. 
//
// I love Lisp, and I miss the simplicity of MuLisp, MacLisp 
// and systems like Apple Logo. Vimana is in the spirit
// of Lisp in the 1970s and 80s. Uppercase names!
//
// The idea is that you can change everything you want yourself.
// Vimana is not for writing applications, it is for 
// experimenting with programming language design.
//
// I use PHP as the implementation language for this version, 
// just because I can :) Possibly, I will do a C-implementation
// that can also run on embedded devides.
//
// An application I have in mind is retro-style arcade games.
// I plan to do this using SDL as the graphics engine.
//

function f_eval_string($code)
{
  $list = f_parse($code);
  $env = [];
  $stack = [];
  $prims = f_create_primitives();

  f_eval_list($list, $env, $stack, $prims);
}

// For debugging.
function f_println($str)
{
  print($str."\n");
}

function f_printobj($str, $obj)
{
  print($str.": ");
  print_r($obj);
  print("\n");
}

// EvalList evaluates a list as program code.
// The stack is always passed by reference.
function f_eval_list($list, &$env, &$stack, $prims)
{
  // Eval of a non-list just pushes the value onto the stack.
  // This is a hack to allow the use of symbols in place of lists
  // in some cases like IFTRUE and IFELSE.
  if (!is_array($list)):
    array_push($stack, f_eval_element($list, $env));
    return;
  endif;
  
  // Evaluate each element in the list.
  foreach ($list as $element):
    f_eval($element, $env, $stack, $prims);
  endforeach;
}

// Eval modifies the environment and the stack.
function f_eval($element, &$env, &$stack, $prims)
{
  if (is_numeric($element) || is_array($element) || is_object($element)):
    // Numbers and lists and objects evaluate to themselves.
	  array_push($stack, $element);
  elseif (is_string($element)):
    if (strlen($element) === 0):
      return;
    endif;
    // If primitive then run it.
    if (f_is_primitive($element, $prims)):
      f_eval_primitive($element, $env, $stack, $prims);
    else:
      // It was not a primitive, is it a function?
      $obj = $env[$element];
      if (isset($obj) && f_is_fun($obj)):
        // If it is a function, evaluate it.
        // Functions evaluate their arguments.
        f_eval_fun($obj, $env, $stack, $prims);
      else:
        // If it is not a function, push the literal value.
        // Variables are not evaluated here. They are evaluated
        // when calling a function or performing a primitive.
        array_push($stack, $element);
      endif;
    endif;
  endif;
}

function f_eval_primitive($symbol, &$env, &$stack, $prims)
{
  $fun = $prims[$symbol];
  $fun($env, $stack, $prims);
}

// Copies the env table to not overwrite shadowed 
// variables permanently.
function f_eval_fun($fun, $env, &$stack, $prims)
{
  // Get number of parameters.
  $local_vars = $fun[1];
  
  // Stack order is reverse of param order.
  $local_vars = array_reverse($local_vars);
  
  // Bind parameters to values.
  foreach ($local_vars as $var):
    $value = array_pop($stack);
    $value = f_eval_element($value, $env);
    $env[$var] = $value;
  endforeach;
  
  // Evaluate the function body.
  f_eval_list($fun[2], $env, $stack, $prims);
}

function f_eval_element($element, $env)
{
  if (is_string($element)):
    // Lookup value.
    $obj = $env[$element];
    if (isset($obj)):
      // It is a variable, return its value.
      return $obj;
    endif;
  endif;
  
  // Otherwise return the element.
  return $element;
}

// Pops an element of the stack and evaluates it.
function array_pop_eval(&$stack, $env)
{
  $value = array_pop($stack);
  return f_eval_element($value, $env);
}

function f_is_primitive($symbol, $prims)
{
  return isset($prims[$symbol]);
}

function f_is_fun($fun_obj)
{
  return (is_array($fun_obj) && ($fun_obj[0] === "FUN"));
}

// Add a native primitive.
function f_add_primitive(&$prims, $symbol, $fun)
{
  $prims[$symbol] = $fun;
}

// Parse (tokenize) a string and return a list.
function f_parse($code)
{
  $code = str_replace("(", " ( ", $code);
  $code = str_replace(")", " ) ", $code);
  $tokens = preg_split("/[\s]+/", $code);
  // Remove zero-lenth strings.
  $tokens = array_filter($tokens,
    function($token) { return strlen($token) > 0; });
  $list = f_create_list($tokens);
  return $list;
}

// Recursively create the list tree structure.
function f_create_list(&$tokens)
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
      $next = f_create_list($tokens);
    endif;
    
    // Convert string to number.
    if (is_numeric($next)):
      $next = $next + 0;
    endif;
    
    array_push($list, $next);
  endwhile;
}
