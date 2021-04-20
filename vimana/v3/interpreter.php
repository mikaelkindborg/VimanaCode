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

  // Get current code list.
  $code_list = $stackframe["code_list"];

  // Increment instruction index.
  $process->callstack[$stackframe_index]["instr_index"]++;
  $instr_index = $process->callstack[$stackframe_index]["instr_index"];
  //f_println("INCR INSTR INDEX: ".$instr_index);

  // If the stackframe has finished evaluating the code
  // we exit the frame.
  if ($instr_index >= count($code_list)):
    // EXIT STACK FRAME
    // TODO: Should we set to NULL here? Reuse frame?
    $process->callstack[$stackframe_index] = NULL;
    $process->stackframe_index--;
  else:
    // Evaluate the current element. Note that
    // new stackframes may be created during eval, and
    // that this will increment $process->stackframe_index.
    $element = $code_list[$instr_index];
    //f_println("EVAL ELEMENT: ". $element);
    f_eval($process, $element);
  endif;

  // Was this the last stackframe?
  if ($process->stackframe_index < 0):
    f_println("PROGRAM ENDED");
    $process->run = false;
  endif;
}

// This will evaluate the list.
function f_create_stackframe($process, $new_code_list)
{
  // ENTER STACK FRAME

  //f_printobj("ENV IN CREATE FRAME", $process->callstack[$process->stackframe_index]["env"]);

  // Below we check for tail call optimization.

  $stackframe_index = $process->stackframe_index;
  $tail_call = FALSE;

  // Determine if tail call is possible.
  // Tail call on root frame is not meaningful 
  // (stackframe_index must be at least 1).
  if ($stackframe_index > 0):
    // If the instruction index has reached the end of the list,
    // this frame has finished executing, and we can reuse it
    // for the new code.
  $index = $stackframe_index;
    $code_list = $process->callstack[$index]["code_list"];
    $instr_index = $process->callstack[$index]["instr_index"];
    if ($instr_index + 1 >= count($code_list)):
      //f_println("STACKFRAME RESUSE IS POSSIBLE: ".$index);
      // Tail call is possible.
      $tail_call_index = $index;
      $tail_call = TRUE;
    endif;
  endif;

  // If tail call.
  if ($tail_call):
    //f_println("TAIL CALL: ".$tail_call_index);
    $process->stackframe_index = $tail_call_index;
    // TODO: 
    //$process->callstack[$tail_call_index]["env"] = 
    //  $process->callstack[$stackframe_index]["env"];
  endif;
  
  // If NOT tail call.
  if (! $tail_call):
    // ENTER NEW STACKFRAME
    $process->stackframe_index++;
    $index = $process->stackframe_index;
    //f_println("NEW STACKFRAME: ".$index);

    // Set empty environment for root stackframe.
    if ($index === 0):
      // Set initial environment to empty for first stackframe.
      $stackframe["env"] = [];
    else:
      // Copy environment from previous stackframe.
      $process->callstack[$index]["env"] = 
        $process->callstack[$index - 1]["env"];
    endif;
  endif;
  
  // Set code list and init instr index.
  $process->callstack[$index]["code_list"] = $new_code_list;
  $process->callstack[$index]["instr_index"] = -1;

  //f_printobj("CALLSTACK", $process->callstack);
  //f_printobj("STACK", $process->stack);
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
  if ($value === NULL):
    f_println("ERROR: STACK VALUE IS NULL");
    f_printobj("STACK", $process->stack);
    exit();
  endif;
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

function f_eval_fun($process, $fun)
{
  // Create a stackframe for the function.
  f_create_stackframe($process, $fun[2]);

  // Now that we have a new stackframe, we can bind the parameters.
  // Get number of parameters.
  $local_vars = $fun[1];
  // Stack order is reverse of param order.
  $local_vars = array_reverse($local_vars);
  //f_println("BIND VARS");
  foreach ($local_vars as $var):
    $value = f_stack_pop_eval($process, $stack);
    $process->callstack[$process->stackframe_index]["env"][$var] = $value;
    //f_println("BIND: ".$var." = ".$value);
  endforeach;
}

/*
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
