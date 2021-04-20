<?php

//
// File: primitives.php
// Date: 2021-04-18
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
  
  $add("DO", function($process)
  {
    $obj = f_stack_pop_eval($process);
    //f_printobj("OBJ IN DO", $obj);
    if (is_array($obj)):
      f_create_stackframe($process, $obj);
      //f_printobj("PROCESS",$process);
      //exit();
    else:
      f_eval_element($process, $obj);
    endif;
  });
  
  $add("CALL", function($process)
  {
    $fun = f_stack_pop($process);
    f_eval_fun($process, $fun);
  });
  
  $add("GOTO", function($process)
  {
    $instr_index = f_stack_pop_eval($process);
    $stackframe_offset = f_stack_pop_eval($process);
    if (!is_numeric($stackframe_offset)):
      print("ERROR: NON-NUMERIC STACKFRAME OFFSET IN GOTO: ".$stackframe_offset."\n");
      exit();
    endif;
    if (!is_numeric($instr_index)):
      print("ERROR: NON-NUMERIC INSTR INDEX IN GOTO: ".$instr_index."\n");
      exit();
    endif;
    $process->stackframe_index = $process->stackframe_index - $stackframe_offset;
    $process->callstack[$process->stackframe_index]["instr_index"] = $instr_index - 1;
  });

  $add("SLEEP", function($process)
  {
    $n = f_stack_pop_eval($process);
    if (!is_numeric($n)):
      print("ERROR: NON-NUMERIC VALUE IN SLEEP:".$n."\n");
      exit();
    endif;
    sleep($n);
  });

  $add("SET", function($process)
  {
    $name = f_stack_pop($process);
    // OLD SINGLE ASSIGNMENT CHECK.
    //if (isset($process->callstack[$process->current_frame][$name])):
    //  print("ERROR: ATTEMPT TO SET ASSIGNED VARIABLE: ".$name."\n");
    //  exit();
    //endif;
    $value = f_stack_pop_eval($process);
    $process->callstack[$process->stackframe_index]["env"][$name] = $value;
  });

  $add("DEF", function($process)
  {
    $body = f_stack_pop($process);
    $fundef = f_stack_pop($process);
    $name = array_pop($fundef);
    $params = $fundef;
    //f_println("FUN NAME: ".$name);
    //f_printobj("FUN PARAMS", $params);
    $fun = ["FUN", $params, $body];
    //f_printobj("FUN", $fun);
    $process->callstack[$process->stackframe_index]["env"][$name] = $fun;
  });
  
  // VALUE is special in that it gets the value of the first
  // element of a list, it does not evaluate the list.
  // VALUE is like "UNQUOTE", where a list is used to quote
  // a name. Note that lists are not evaluated per default.
  $add("VALUE", function( $process)
  {
    $var = f_stack_pop($process);
    if (is_array($var)):
      $var = $var[0];
    endif;
    $value =  $process->callstack[$process->stackframe_index]["env"][$var];
    f_stack_push($process, $value);
  });
  
  $add("DOC", function($process)
  {
    f_stack_pop($process);
  });
  
  $add("+", function($process)
  {
    $b = f_stack_pop_eval($process);
    $a = f_stack_pop_eval($process);
    $res = $a + $b;
    f_stack_push($process, $res);
  });
  
  $add("-", function($process)
  {
    $b = f_stack_pop_eval($process);
    $a = f_stack_pop_eval($process);
    $res = $a - $b;
    f_stack_push($process, $res);
  });
  
  $add("*", function($process)
  {
    $b = f_stack_pop_eval($process);
    $a = f_stack_pop_eval($process);
    $res = $a * $b;
    f_stack_push($process, $res);
  });
  
  $add("/", function($process)
  {
    $b = f_stack_pop_eval($process);
    $a = f_stack_pop_eval($process);
    $res = $a / $b;
    f_stack_push($process, $res);
  });
  
  $add("T", function($process)
  {
    $value = "T";
    f_stack_push($process, $value);
  });
  
  $add("F", function($process)
  {
    $value = "F";
    f_stack_push($process, $value);
  });
  
  $add("NOT", function($process)
  {
    $bool = f_stack_pop($process);
    if ($bool === "F"):
      $res = "T";
    else:
      $res = "F";
    endif;
    f_stack_push($process, $res);
  });
  
  $add("EQ", function($process)
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
  
  $add("IFTRUE", function($process)
  {
    $true_branch = f_stack_pop($process);
    $truth = f_stack_pop($process);
    if ($truth === "T"):
      f_create_stackframe($process, $true_branch);
    endif;
  });
  
  $add("IFELSE", function($process)
  {
    $else_branch = f_stack_pop($process);
    $true_branch = f_stack_pop($process);
    $truth = f_stack_pop($process);
    if ($truth === "T"):
      f_create_stackframe($process, $true_branch);
    else:
      f_create_stackframe($process, $else_branch);
    endif;
  });
  /*
  $add("DOTIMES", function($process)
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
  */
  $add("GET", function($process)
  {
    $index = f_stack_pop_eval($process);
    $list = f_stack_pop_eval($process);
    f_stack_push($process, $list[$index - 1]);
  });
  
  $add("COUNT", function($process)
  {
    $list = f_stack_pop_eval($process);
    f_stack_push($process, count($list));
  });
  
  $add("FETCH", function($process)
  {
    $url = f_stack_pop_eval($process);
    $text = file_get_contents($url);
    f_stack_push($process, $text);
  });
  
  $add("JOIN", function($process)
  {
    $list = f_stack_pop_eval($process);
    $string = implode (" ", $list);
    f_stack_push($process, $string);
  });
  
  $add("SPACE", function($process)
  {
    $value = " ";
    f_stack_push($process, $value);
  });
  
  $add("ENV", function($process)
  {
    f_stack_push(
      $process, 
      $process->callstack[$process->current_frame]);
  });
  
  $add("STACK", function($process)
  {
    f_stack_push($process, $process->stack);
  });
  
  $add("CALLSTACK", function($process)
  {
    f_stack_push($process, $process->callstack);
  });
  
  $add("PRINT", function($process)
  {
    $obj = f_stack_pop_eval($process);
    print_r($obj);
  });
  
  $add("PRINTLN", function($process)
  {
    $obj = f_stack_pop_eval($process);
    print_r($obj); 
    print("\n");
  });
  
  $add("PRINTENV", function($process)
  {
    print("PRINTENV STACKFRAME ".$process->stackframe_index.":\n");
    print_r($process->callstack[$process->stackframe_index]["env"]);
  });
  
  $add("PRINTSTACK", function($process)
  {
    print_r("PRINTSTACK:\n");
    print_r($process->stack);
  });
  
  $add("PRINTCALLSTACK", function($process)
  {
    print_r("PRINTCALLSTACK:\n");
    print("CURRENT FRAME: ".$process->stackframe_index."\n");
    print_r($process->callstack);
  });
  
  $add("MAKEOBJ", function($process)
  {
    $obj = new stdClass();
    f_stack_push($process, $obj);
  });
  
  $add("SETPROP", function($process)
  {
    $obj = f_stack_pop_eval($process);
    $prop_name = f_stack_pop($process);
    $prop_value = f_stack_pop_eval($process);
    $obj->$prop_name = $prop_value;
  });
  
  $add("GETPROP", function($process)
  {
    $obj = f_stack_pop_eval($process);
    $prop_name = f_stack_pop($process);
    f_stack_push($process, $obj->$prop_name);
  });
  
  return $prims;
}
