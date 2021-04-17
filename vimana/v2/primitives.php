<?php

//
// File: v2_primitives.php
// Date: 2021-04-17
// Author: Mikael Kinborg
// Email: mikael@kindborg.com
// Website: kindborg.com
// License: Creative Commons: Attribution-ShareAlike - CC BY-SA
//

// Create primitives.
function f_create_primitives()
{
  $prims = [];
  
  // Helper function.
  $add = function($symbol, $fun) use (&$prims)
  {
    $prims[$symbol] = $fun;
  };
  
  $add("EVAL", function(&$process)
  {
    $obj = f_stack_pop($process);
    f_eval_list($process, $obj);
  });
  
  $add("CALL", function(&$process)
  {
    $fun = f_stack_pop($process);
    f_eval_fun($process, $fun);
  });
  
  $add("SET", function(&$process)
  {
    // SET is single assignment
    $name = f_stack_pop($process);
    if (isset($process->callstack[$process->current_frame][$name])):
      print("ERROR: ATTEMPT TO SET ASSIGNED VARIABLE: ".$name."\n");
      exit();
    endif;
    $value = f_stack_pop_eval($process);
    f_set_binding($process, $name, $value);
  });
  
  $add("DEF", function(&$process)
  {
    $fundef = f_stack_pop($process);
    $name = $fundef[0];
    $params = $fundef[1];
    $body = $fundef[2];
    $fun = ["FUN", $params, $body];
    // The function name is defined in the global function table.
    // Use SET to define a local function, like this (however, if
    // the name clashes with a defined function the system crashes):
    //  (FUN () (HELLO PRINTLN)) FOOBAR SET
    //  FOOBAR VALUE CALL
    f_set_fun($process, $name, $fun);
  });
  
  // VALUE is special in that it gets the value of the first
  // element of a list, it does not evaluate the list.
  // VALUE is like "UNQUOTE", where a list is used to quote
  // a name. Note that lists are not evaluated per default.
  $add("VALUE", function(& $process)
  {
    $var = f_stack_pop($process);
    if (is_array($var)):
      $var = $var[0];
    endif;
    $value = f_get_binding($process, $var);
    f_stack_push($process, $value);
  });
  
  $add("DOC", function(&$process)
  {
    f_stack_pop($process);
  });
  
  $add("GET", function(&$process)
  {
    $index = f_stack_pop_eval($process);
    $list = f_stack_pop_eval($process);
    f_stack_push($process, $list[$index - 1]);
  });
  
  $add("COUNT", function(&$process)
  {
    $list = f_stack_pop_eval($process);
    f_stack_push($process, count($list));
  });
  
  $add("+", function(&$process)
  {
    $b = f_stack_pop_eval($process);
    $a = f_stack_pop_eval($process);
    $res = $a + $b;
    f_stack_push($process, $res);
  });
  
  $add("-", function(&$process)
  {
    $b = f_stack_pop_eval($process);
    $a = f_stack_pop_eval($process);
    $res = $a - $b;
    f_stack_push($process, $res);
  });
  
  $add("*", function(&$process)
  {
    $b = f_stack_pop_eval($process);
    $a = f_stack_pop_eval($process);
    $res = $a * $b;
    f_stack_push($process, $res);
  });
  
  $add("/", function(&$process)
  {
    $b = f_stack_pop_eval($process);
    $a = f_stack_pop_eval($process);
    $res = $a / $b;
    f_stack_push($process, $res);
  });
  
  $add("T", function(&$process)
  {
    $value = "T";
    f_stack_push($process, $value);
  });
  
  $add("F", function(&$process)
  {
    $value = "F";
    f_stack_push($process, $value);
  });
  
  $add("NOT", function(&$process)
  {
    $bool = f_stack_pop($process);
    if ($bool === "F"):
      $res = "T";
    else:
      $res = "F";
    endif;
    f_stack_push($process, $res);
  });
  
  $add("EQ", function(&$process)
  {
    $b = f_stack_pop_eval($process);
    $a = f_stack_pop_eval($process);
    if ($a === $b):
      $res = "T";
    else:
      $res = "F";
    endif;
    f_stack_push($process, $res);
  });
  
  $add("IFTRUE", function(&$process)
  {
    $true_branch = f_stack_pop($process);
    $condition = f_stack_pop($process);
    f_eval_list($process, $condition);
    $res = f_stack_pop($process);
    if ($res === "T"):
      f_eval_list($process, $true_branch);
    endif;
  });
  
  $add("IFELSE", function(&$process)
  {
    $else_branch = f_stack_pop($process);
    $true_branch = f_stack_pop($process);
    $condition = f_stack_pop($process);
    f_eval_list($process, $condition);
    $res = f_stack_pop($process);
    if ($res === "T"):
      f_eval_list($process, $true_branch);
    else:
      f_eval_list($process, $else_branch);
    endif;
  });
  
  $add("DOTIMES", function(&$process)
  {
    $body = f_stack_pop($process);
    $times = f_stack_pop_eval($process);
    $times--;
    for  ($i = 0; $i < $times; $i++):
      f_eval_list($process, $body);
      f_stack_pop($process);
    endfor;
    // Pushes the result of the last iteration.
    f_eval_list($process, $body);
  });
  
  $add("FETCH", function(&$process)
  {
    $url = f_stack_pop_eval($process);
    $text = file_get_contents($url);
    f_stack_push($process, $text);
  });
  
  $add("JOIN", function(&$process)
  {
    $list = f_stack_pop_eval($process);
    $string = implode (" ", $list);
    f_stack_push($process, $string);
  });
  
  $add("SPACE", function(&$process)
  {
    $value = " ";
    f_stack_push($process, $value);
  });
  
  $add("ENV", function(&$process)
  {
    f_stack_push(
      $process, 
      $process->callstack[$process->current_frame]);
  });
  
  $add("STACK", function(&$process)
  {
    f_stack_push($process, $process->stack);
  });
  
  $add("CALLSTACK", function(&$process)
  {
    f_stack_push($process, $process->callstack);
  });
  
  $add("PRINT", function(&$process)
  {
    $obj = f_stack_pop_eval($process);
    print_r($obj);
  });
  
  $add("PRINTLN", function(&$process)
  {
    $obj = f_stack_pop_eval($process);
    print_r($obj); 
    print("\n");
  });
  
  $add("PRINTENV", function(&$process)
  {
    print("PRINTENV STACKFRAME ".$process->current_frame.":\n");
    print_r($process->callstack[$process->current_frame]);
  });
  
  $add("PRINTSTACK", function(&$process)
  {
    print_r("PRINTSTACK:\n");
    print_r($process->stack);
  });
  
  $add("PRINTCALLSTACK", function(&$process)
  {
    print_r("PRINTCALLSTACK:\n");
    print("CURRENT FRAME: ".$process->current_frame."\n");
    print_r($process->callstack);
  });
  
  $add("MAKEOBJ", function(&$process)
  {
    $obj = new stdClass();
    f_stack_push($process, $obj);
  });
  
  $add("SETPROP", function(&$process)
  {
    $obj = f_stack_pop_eval($process);
    $prop_name = f_stack_pop($process);
    $prop_value = f_stack_pop_eval($process);
    $obj->$prop_name = $prop_value;
  });
  
  $add("GETPROP", function(&$process)
  {
    $obj = f_stack_pop_eval($process);
    $prop_name = f_stack_pop($process);
    f_stack_push($process, $obj->$prop_name);
  });
  
  return $prims;
}
