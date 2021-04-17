<?php

//
// File: v2_interpreter.php
// Date: 2021-04-17
// Author: Mikael Kinborg
// Email: mikael@kindborg.com
// Website: kindborg.com
// License: Creative Commons: Attribution-ShareAlike - CC BY-SA
//
// This file contains an interpeter for an experimental language
// named VimanaLang. I have created this project for my own
// purposes, simply because for the fun of it. The inspiration
// comes from Lisp and Forth. 
// I am an old Lisp programmer who did computer science research 
// using MacLisp, MuLisp, Interlisp-D, Smalltalk, and HyperCard 
// in the 1980s. 
//
// I love Lisp, and I miss the simplicity of MuLisp, MacLisp 
// and systems like Apple Logo. VimanaLang is in the spirit
// of Lisp in the 1970s and 80s. Uppercase names!
//
// The idea is that you can change everything you want yourself.
// VimanaLang is not for writing applications, it is for 
// experimenting with programming language design.
//
// I use PHP as the implementation language for this version, 
// just because I can :) Possibly, I will do a C-implementation
// that can also run on embedded devides.
//
// An application I have in mind is retro-style arcade games.
// I plan to do this using SDL as the graphics engine.
//

// Main entry point.
function f_eval_string($code)
{
  $list = f_parse($code);
  
  // Note that there are two stacks the callstack and
  // the "parameter stack" (look up name for this).
  $process = new stdClass();
  $process->callstack = [];
  $process->current_frame = 0;
  $process->stack = [];
  $process->funs = [];
  $process->prims = f_create_primitives();
  
  f_eval_list($process, $list);
}

function f_get_binding(&$process, &$symbol)
{
  // Search for binding.
  $n = $process->current_frame;
  while ($n > -1):
    $value = f_get_binding_in_frame($process, $n, $symbol);
    if (isset($value)):
      return $value;
    endif;
    $n--;
  endwhile;
  return $symbol;
}

function f_get_binding_in_frame(&$process, &$n, &$symbol)
{
  return $process->callstack[$n][$symbol];
}

function f_set_binding(&$process, &$symbol, &$value)
{
  $process->callstack[$process->current_frame][$symbol] = $value;
}

function f_get_fun(&$process, &$symbol)
{
  return $process->funs[$symbol];
}

function f_set_fun(&$process, &$symbol, &$fun)
{
  $process->funs[$symbol] = $fun;
}

function f_enter_stackframe(&$process)
{
  $process->current_frame++;
  $callstack[$process->current_frame] = []; // environment table
}

function f_exit_stackframe(&$process)
{
  $process->current_frame--;
}

function f_stack_push(&$process, &$element)
{
  array_push($process->stack, $element);
}

function f_stack_pop(&$process)
{
  return array_pop($process->stack);
}

// Pops an element of the stack and evaluates it.
function f_stack_pop_eval(&$process)
{
  $value = f_stack_pop($process);
  return f_eval_element($process, $value);
}

function f_is_primitive(&$process, &$symbol)
{
  return isset($process->prims[$symbol]);
}

function f_is_fun(&$fun)
{
  return (is_array($fun) && ($fun[0] === "FUN"));
}

// EvalList evaluates a list as program code.
function f_eval_list(&$process, &$something)
{
  // Eval of a non-list just pushes the value onto the stack.
  if (!is_array($something)):
    if (is_numeric($something) || is_object($something)):
      f_stack_push($process, $something);
    else:
      f_stack_push($process, f_eval_element($process, $something));
    endif;
    return;
  endif;
  
  // For lists evaluate each element in the list.
  foreach ($something as & $element):
    f_eval($process, $element);
  endforeach;
}

// Eval modifies the environment and the stack.
function f_eval(&$process, &$element)
{
  if (is_numeric($element) || is_array($element) || is_object($element)):
    // Numbers and lists and objects evaluate to themselves.
	  f_stack_push($process, $element);
  elseif (is_string($element)):
    // Evaluate symbol.
    // Empty symbol is an error.
    if (strlen($element) === 0):
      f_println("ERROR: SYMBOL IS EMPTY STRING");
      exit();
    endif;
    // If primitive then run it.
    if (f_is_primitive($process, $element)):
      f_eval_primitive($process, $element);
    else:
      // It was not a primitive, is it a function?
      $fun = f_get_fun($process, $element);
      if (isset($fun) && f_is_fun($fun)):
        // If it is a function, evaluate it.
        // Functions evaluate their arguments.
        f_eval_fun($process, $fun);
      else:
        // If it is not a function, push the literal value.
        // Variables are not evaluated here. They are evaluated
        // when calling a function or performing a primitive.
        f_stack_push($process, $element);
      endif;
    endif;
  endif;
}

function f_eval_primitive(&$process, &$symbol)
{
  $fun = $process->prims[$symbol];
  $fun($process);
}

// Copies the env table to not overwrite shadowed 
// variables permanently.
function f_eval_fun(&$process, &$fun)
{
  f_enter_stackframe($process);

  // Get number of parameters.
  $local_vars = $fun[1];
  
  // Stack order is reverse of param order.
  $local_vars = array_reverse($local_vars);
  
  // Bind parameters to values.
  foreach ($local_vars as $var):
    $value = f_stack_pop($process, $stack);
    $value = f_eval_element($process, $value);
    f_set_binding($process, $var, $value);
  endforeach;
  
  // Evaluate the function body.
  f_eval_list($process, $fun[2]);
  
  f_exit_stackframe($process);
}

function f_eval_element(&$process, &$element)
{
  if (is_string($element)):
    // Lookup value.
    $obj = f_get_binding($process, $element);
    if (isset($obj)):
      // It is a variable, return its value.
      return $obj;
    endif;
  endif;

  // Otherwise return the element.
  return $element;
}

// Add a native primitive.
function f_add_primitive(&$process, &$symbol, $fun)
{
  $process->prims[$symbol] = $fun;
}

// Parse (tokenize) a string and return a list.
function f_parse($string)
{
  $code = $string;
  $code = str_replace("(", " ( ", $code);
  $code = str_replace(")", " ) ", $code);
  $tokens = preg_split("/[\s]+/", $code);
  // Remove zero-lenth strings.
  $tokens = array_filter(
    $tokens,
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

function f_println($str)
{
  print($str."\n");
}

// For debugging.
function f_printobj($str, $obj)
{
  print($str.": ");
  print_r($obj);
  print("\n");
}
