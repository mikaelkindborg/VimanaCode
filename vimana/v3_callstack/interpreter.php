<?php
//
// File: interpreter.php
// Date: 2021-04-25
// Author: Mikael Kinborg
// Email: mikael@kindborg.com
// Website: https://kindborg.com
// License: Creative Commons: Attribution-ShareAlike - CC BY-SA

/******************************************************************

IMPLEMENTATION NOTES
====================

This version of the interpreter implements its own callstack. 
It does not use the PHP stack.

There is also an iterpreter object that holds the execution state.
This makes the code cleaner and makes it possible to implement
interpreters running in parallel. An intepreter then becomes
a kind of process.

TODO: 

* Optimise performance (inline frequent PHP function calls).

* Fix examples.php for v3.

* Improve MAKEOBJ and SET.

* Make the interpreter a process that can be run in a kind of
  virtual machine, with massively parallel processes (thosands).

* Message passing between processes.

******************************************************************/

// Main entry point.
function interp_eval_string($code, $prims = NULL)
{
  // Note that there are two stacks the callstack and
  // the "parameter stack" or "data stack".
  $interp = new stdClass();
  $interp->callstack = [];        // Array of stackframes
  $interp->stackframe_index = 0;  // Current stackframe index
  $interp->stackframe = NULL;     // Current stackframe
  $interp->stack = [];            // Data stack
  //$interp->funs = [];
  $interp->run = TRUE;
  
  // Create primitives.
  if (!isset($prims)):
    $prims = interp_create_primitives($prims);
  endif;
  $interp->prims = $prims;
  
  // Parse program.
  $list = interp_parse($code);
  
  // Create first stackframe.
  $stackframe = new stdClass();
  $stackframe->env = [];
  $stackframe->code_list = $list;
  $stackframe->code_pointer = -1;
  $interp->callstack[0] = $stackframe;
  $interp->stackframe = $stackframe;
  
  interp_run($interp);
}

function interp_run($interp)
{
  while ($interp->run):
    // Get current stackframe.
    //$index = $interp->stackframe_index;
    //$stackframe = $interp->callstack[$index];
    $stackframe = $interp->stackframe;

    // Increment code pointer.
    $stackframe->code_pointer++;
    
    // If the code in the stackframe has finished executing
    // we exit the frame.
    if ($stackframe->code_pointer >= count($stackframe->code_list)):
      // EXIT STACK FRAME
      //interp_println("EXIT STACKFRAME: ".$interp->stackframe_index);
      $interp->stackframe_index--;
      $interp->stackframe = $interp->callstack[$interp->stackframe_index];
    else:
      // Evaluate the current element. Note that new stackframes
      // may be created during evaluation, and that this will 
      // increment the stackframe index.
      $element = $stackframe->code_list[$stackframe->code_pointer];
      //interp_println("EVAL ELEMENT: ". $element);
      interp_eval($interp, $element);
    endif;

    // Was this the last stackframe?
    if ($interp->stackframe_index < 0):
      interp_println("PROGRAM ENDED");
      $interp->run = FALSE;
    endif;
  endwhile;
}

function interp_create_stackframe($interp, $list, $copy_env = FALSE)
{
  // ENTER STACK FRAME

  //interp_print_obj("ENV IN CREATE FRAME", $interp->callstack[$interp->stackframe_index]["env"]);

  // Below we check for tail call optimization.

  // Copy previous environment.
  //if ($copy_env):
    $new_env = $interp->stackframe->env;
  //endif;
  
  // Determine if tail call is possible.
  // Tail call on root frame (stackframe 0) is not 
  // done to prevent overwriting the global environment.
  // (stackframe_index must be at least 1).
  $index = $interp->stackframe_index;
  $tail_call = FALSE;
  if ($index > 0):
    // If the code pointer has reached the end of the list,
    // this frame has finished executing and we can reuse it.
    if (($interp->stackframe->code_pointer + 1) >= 
        count($interp->stackframe->code_list)):
      // Tail call is possible.
      //interp_println("STACKFRAME RESUSE IS POSSIBLE: ".$index);
      $tail_call = TRUE;
    endif;
  endif;

  // Check tail call.
  if ($tail_call):
    // TAIL CALL
    //interp_println("TAIL CALL: ".$index);
  else:
    // NOT TAIL CALL
    // ENTER NEW STACKFRAME
    $index++;
    $interp->stackframe_index = $index;
    //interp_println("NEW STACKFRAME: ".$index);
    
    // Create stackframe.
    if (!isset($interp->callstack[$index])):
      $interp->callstack[$index] = new stdClass();
    endif;
    
    // Set current stackframe.
    $interp->stackframe = $interp->callstack[$index];
  endif;
  
  // Initialize code list and code pointer.
  $interp->stackframe->code_list = $list;
  $interp->stackframe->code_pointer = -1;

  // Set environment.
  //if ($copy_env):
    $interp->stackframe->env = $new_env;
  //endif;

  //interp_print_obj("CALLSTACK", $interp->callstack);
  //interp_print_obj("STACK", $interp->stack);
}

function interp_eval($interp, $element)
{
  //interp_println("EVALUATING: ".$element);

/*
  if (is_array($element)):
    interp_create_stackframe($interp, $element);
  endif;
*/

  // Numbers and lists and objects evaluate to themselves.
  if (is_numeric($element) || is_array($element) || is_object($element)):
	  interp_push($interp, $element);
    return;
  endif;

  // Symbols might be primyitoves, functions, variables.
  // Unbound symbols evaluate to their literal value.
  if (is_string($element)):

    // Empty symbol is an error and aborts the program.
    if (strlen($element) === 0):
      interp_println("ERROR: SYMBOL IS EMPTY STRING");
      exit();
    endif;

    // Check if it is a primitive.
    $fun = $interp->prims[$element];
    if (isset($fun)):
      $fun($interp);
      return;
    endif;

    // Check if it is a function.
    $fun = $interp->stackframe->env[$element];
    if (isset($fun) && is_array($fun) && ($fun[0] === "FUN")):
      //interp_println("EVAL FUN: ".$element);
      interp_eval_fun($interp, $fun);
      return;
    endif;

    // If not any of the above, push the literal value.
    // Variables are not evaluated here. They are evaluated
    // when calling a function or performing a primitive.
    interp_push($interp, $element);

  endif;
}

function interp_eval_element($interp, $element)
{
  $env = $interp->stackframe->env;
  return (is_string($element) && isset($env[$element])) ? 
    $env[$element] : $element;
}

// Pops an element of the stack and evaluates it.

function interp_pop_eval($interp)
{
  $element = interp_pop($interp);
  $env = $interp->stackframe->env;
  return (is_string($element) && isset($env[$element])) ? 
    $env[$element] : $element;
}

function interp_eval_fun($interp, $fun)
{
  // Create a stackframe for the function.
  interp_create_stackframe($interp, $fun[2], TRUE);
  // Now that we have a new stackframe, we can bind the parameters.
  // Get number of parameters.
  $local_vars = $fun[1];
  // Stack order is reverse of param order.
  $local_vars = array_reverse($local_vars);
  //interp_println("BIND VARS");
  foreach ($local_vars as $var):
    $value = interp_pop_eval($interp, $stack);
    $interp->stackframe->env[$var] = $value;
    //interp_println("BIND: ".$var." = ".$value);
  endforeach;
}

function interp_define_function($interp)
{
  $body = array_pop($interp->stack);
  $fundef = array_pop($interp->stack);
  $name = array_pop($fundef);
  $params = $fundef;
  $fun = ["FUN", $params, $body];
  $interp->callstack[$interp->stackframe_index]->env[$name] = $fun;
}

function interp_push($interp, $element)
{
  array_push($interp->stack, $element);
}

function interp_pop($interp)
{
  return array_pop($interp->stack);
}

// Add a native primitive.
function interp_add_primitive($symbol, $fun, &$prims)
{
  $prims[$symbol] = $fun;
}

// Parse (tokenize) a string and return a list.
function interp_parse($string)
{
  $code = $string;
  $code = str_replace("(", " ( ", $code);
  $code = str_replace(")", " ) ", $code);
  $tokens = preg_split("/[\s]+/", $code);
  // Remove zero-lenth strings.
  $tokens = array_filter(
    $tokens,
    function($token) { return strlen($token) > 0; });
  $list = interp_create_list($tokens);
  return $list;
}

// Recursively create the list tree structure.
function interp_create_list(&$tokens)
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
      $next = interp_create_list($tokens);
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
