<?php
//
// File: interpreter.php
// Date: 2021-04-18
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
  // Note that there are two stacks the callstack and
  // the "parameter stack" or "data stack".
  $process = new stdClass();
  $process->callstack = [];
  $process->stackframe_index = -1;
  $process->stack = [];
  $process->funs = [];
  $process->prims = f_create_primitives();
  $process->run = TRUE;
  
  $code_list = f_parse($code);
  f_create_stackframe($process, $code_list);

  while ($process->run):
    f_run($process);
  endwhile;
}

function f_run($process)
{
  // Get current stackframe.
  $stackframe_index = $process->stackframe_index;
  $stackframe = $process->callstack[$stackframe_index];

  f_println("STACKFRAME: ".$process->stackframe_index);

  // Get current code list element.
  $code_list = $stackframe["code_list"];
  $list_index = $stackframe["list_index"];

  //f_println("LISTINDEX: ".$list_index);
  // Has the stackframe finished evaluating the code list?
  if ($list_index >= count($code_list)):
    $process->callstack[$stackframe_index] = NULL;
    $process->stackframe_index--;
  else:
    // Evaluate the current list element. Note that
    // new stackframes may be created during eval, and
    // that this will increment $process->stackframe_index.
    $element = $code_list[$list_index];
    f_eval($process, $element);

    // Increment code list index for the stackframe that was evaluated.
    $process->callstack[$stackframe_index]["list_index"]++;
  endif;

  // Was this the last stackframe?
  if ($process->stackframe_index < 0):
    f_println("PROGRAM ENDED");
    $process->run = false;
  endif;
}

// This will evaluate the list.
function f_create_stackframe($process, $code_list)
{
  // Tail call optimization. Only increment stackframe if
  // current code list has elements left.

  if ($process->stackframe_index > 0):
    f_println("probe1");
    $code_list = $process->callstack[$process->stackframe_index]["code_list"];
    $list_index = $process->callstack[$process->stackframe_index]["list_index"];

    f_printobj("CODELIST", $code_list);
    f_println("LISTINDEX: ".$list_index);
    sleep(1);
    if ($list_index < (count($code_list) - 1)):
      f_println("probe2");
      $process->stackframe_index++;
    endif;
  else:
    $process->stackframe_index++;
  endif;

  $stackframe = [];
  $stackframe["code_list"] = $code_list;
  $stackframe["list_index"] = 0;

  if ($process->stackframe_index === 0):
    $stackframe["env"] = [];
  else:
    $stackframe["env"] = $process->callstack[$process->stackframe_index - 1]["env"];
  endif;

  $process->callstack[$process->stackframe_index] = $stackframe;
}

function f_eval($process, $element)
{
  //f_println("EVALUATING: ".$element);

/*
  if (is_array($element)):
    f_create_stackframe($process, $element);
  endif;
*/

  // Numbers and lists and objects evaluate to themselves.
  if (is_numeric($element) || is_array($element) || is_object($element)):
	  f_stack_push($process, $element);
    return;
  endif;

  // Symbols might be primyitoves, functions, variables.
  // Unbound symbols evaluate to their literal value.
  if (is_string($element)):

    // Empty symbol is an error and aborts the program.
    if (strlen($element) === 0):
      f_println("ERROR: SYMBOL IS EMPTY STRING");
      exit();
    endif;

    // Check if it is a primitive.
    $fun = $process->prims[$element];
    if (isset($fun)):
      $fun($process);
      return;
    endif;

    // Check if it is a function.
    $fun = $process->callstack[$process->stackframe_index]["env"][$element];
    if (isset($fun) && is_array($fun) && ($fun[0] === "FUN")):
      f_eval_fun($process, $fun);
      return;
    endif;

    // If not any of the above, push the literal value.
    // Variables are not evaluated here. They are evaluated
    // when calling a function or performing a primitive.
    f_stack_push($process, $element);

  endif;
}

function f_stack_push($process, $element)
{
  array_push($process->stack, $element);
}

function f_stack_pop($process)
{
  return array_pop($process->stack);
}

// Pops an element of the stack and evaluates it.
function f_stack_pop_eval($process)
{
  $value = f_stack_pop($process);
  return f_eval_element($process, $value);
}


function f_eval_element($process, $element)
{
  if (is_string($element)):
    // Lookup value.
    $obj = $process->callstack[$process->stackframe_index]["env"][$element];
    if (isset($obj)):
      // It is a variable, return its value.
      return $obj;
    endif;
  endif;

  // Otherwise return the element.
  return $element;
}

// TODO: TAIL OPTIMIZATION

function f_eval_fun($process, $fun)
{
  // Create a stackframe for the function.
  f_create_stackframe($process, $fun[2]);

  // Now that we have a new stackframe, we can bind the parameters.
  // Get number of parameters.
  $local_vars = $fun[1];
  // Stack order is reverse of param order.
  $local_vars = array_reverse($local_vars);
  foreach ($local_vars as $var):
    $value = f_stack_pop_eval($process, $stack);
    $process->callstack[$process->stackframe_index]["env"][$var] = $value;
  endforeach;
}

/*

function f_set_binding($process, $symbol, $value)
{
  $process->callstack[$process->current_frame][$symbol] = $value;
}

function f_get_fun($process, $symbol)
{
  return $process->funs[$symbol];
}

function f_set_fun($process, $symbol, $fun)
{
  $process->funs[$symbol] = $fun;
}





// Eval modifies the environment and the stack.
function f_eval($process, $element)
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


// Add a native primitive.
function f_add_primitive($process, $symbol, $fun)
{
  $process->prims[$symbol] = $fun;
}
*/

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
