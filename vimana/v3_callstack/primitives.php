<?php

//
// File: primitives.php
// Date: 2021-04-25
// Author: Mikael Kinborg
// Email: mikael@kindborg.com
// Website: https://kindborg.com
// License: Creative Commons: Attribution-ShareAlike - CC BY-SA
//

// Create primitives.
function interp_create_primitives($prims = [])
{
  // Initialize the radom number generator.
  mt_srand();

  interp_add_primitive("DEF", function($interp)
  {
    interp_define_function($interp);
  },
  $prims);
  
  interp_add_primitive("DO", function($interp)
  {
    $obj = interp_pop_eval($interp);
    if (is_array($obj)):
      interp_create_stackframe($interp, $obj);
    else:
      interp_eval_element($interp, $obj);
    endif;
  },
  $prims);
  
  interp_add_primitive("CALL", function($interp)
  {
    $fun = interp_pop_eval($stack);
    interp_eval_fun($interp, $fun);
  },
  $prims);
  
  /*
  interp_add_primitive("GOTO", function($interp)
  {
    $instr_index = interp_pop_eval($interp);
    $stackframe_offset = interp_pop_eval($interp);
    if (!is_numeric($stackframe_offset)):
      print("ERROR: NON-NUMERIC STACKFRAME OFFSET IN GOTO: ".$stackframe_offset."\n");
      exit();
    endif;
    if (!is_numeric($instr_index)):
      print("ERROR: NON-NUMERIC INSTR INDEX IN GOTO: ".$instr_index."\n");
      exit();
    endif;
    $interp->stackframe_index = $interp->stackframe_index - $stackframe_offset;
    $interp->callstack[$interp->stackframe_index]["instr_index"] = $instr_index - 1;
  });
  */

  interp_add_primitive("SET", function($interp)
  {
    $name = interp_pop($interp);
    // OLD SINGLE ASSIGNMENT CHECK.
    /*if (isset($env[$name])):
      echo "ERROR: ATTEMPT TO SET ASSIGNED VARIABLE: ".$name."\n";
      exit();
    endif;*/
    $value = interp_pop_eval($interp);
    $interp->stackframe->env[$name] = $value;
  },
  $prims);
  
  
  // VALUE is special in that it gets the value of the first
  // element of a list, it does not evaluate the list.
  // VALUE is like "UNQUOTE", where a list is used to quote
  // a name. Note that lists are not evaluated per default.
  interp_add_primitive("VALUE", function($interp)
  {
    $var = interp_pop($interp);
    if (is_array($var)):
      $var = $var[0];
    endif;
    $value = $interp->stackframe->env[$var];
    interp_push($interp, $value);
  },
  $prims);
  
  interp_add_primitive("DOC", function($interp)
  {
    interp_pop($interp);
  },
  $prims);
  
  interp_add_primitive("+", function($interp)
  {
    $b = interp_pop_eval($interp);
    $a = interp_pop_eval($interp);
    interp_push($interp, $a + $b);
  },
  $prims);
  
  interp_add_primitive("-", function($interp)
  {
    $b = interp_pop_eval($interp);
    $a = interp_pop_eval($interp);
    interp_push($interp, $a - $b);
  },
  $prims);
  
  interp_add_primitive("*", function($interp)
  {
    $b = interp_pop_eval($interp);
    $a = interp_pop_eval($interp);
    interp_push($interp, $a * $b);
  },
  $prims);
  
  interp_add_primitive("/", function($interp)
  {
    $b = interp_pop_eval($interp);
    $a = interp_pop_eval($interp);
    interp_push($interp, $a / $b);
  },
  $prims);
  
  interp_add_primitive("RANDOM", function($interp)
  {
    $b = interp_pop_eval($interp);
    $a = interp_pop_eval($interp);
    interp_push($interp, mt_rand($a, $b));
  },
  $prims);
  
  interp_add_primitive("T", function($interp)
  {
    interp_push($interp, "T");
  },
  $prims);
  
  interp_add_primitive("F", function($interp)
  {
    interp_push($interp, "F");
  },
  $prims);
  
  interp_add_primitive("NOT", function($interp)
  {
    $bool = interp_pop($interp);
    if ($bool === "F"):
      interp_push($interp, "T");
    else:
      interp_push($interp, "F");
    endif;
  },
  $prims);
  
  interp_add_primitive("EQ", function($interp)
  {
    $b = interp_pop_eval($interp);
    $a = interp_pop_eval($interp);
    if ($a === $b):
      interp_push($interp, "T");
    else:
      interp_push($interp, "F");
    endif;
  },
  $prims);
  
  interp_add_primitive("IFTRUE", function($interp)
  {
    $true_branch = interp_pop($interp);
    $truth = interp_pop($interp);
    if ($truth === "T"):
      //interp_println("WILL TAIL CALL FOLLOW?!");
      interp_create_stackframe($interp, $true_branch);
    endif;
  },
  $prims);
  
  interp_add_primitive("IFELSE", function($interp)
  {
    $else_branch = interp_pop($interp);
    $true_branch = interp_pop($interp);
    $truth = interp_pop($interp);
    if ($truth === "T"):
      interp_create_stackframe($interp, $true_branch);
    else:
      interp_create_stackframe($interp, $else_branch);
    endif;
  },
  $prims);
  
  interp_add_primitive("SLEEP", function($interp)
  {
    $time = interp_pop_eval($interp);
    sleep($time);
  },
  $prims);
  
  /* EXAMPLE OF ERROR CHECKING
  interp_add_primitive("SLEEP", function($interp)
  {
    $time = interp_pop_eval($interp);
    if (!is_numeric($time)):
      print("ERROR: NON-NUMERIC VALUE IN SLEEP:".$time."\n");
      exit();
    endif;
    sleep($time);
  });
  */
  
  interp_add_primitive("EXIT", function($interp)
  {
    exit();
  },
  $prims);
  
  interp_add_primitive("GET", function($interp)
  {
    $index = interp_pop_eval($interp);
    $list = interp_pop_eval($interp);
    interp_push($interp, $list[$index - 1]);
  },
  $prims);
  
  interp_add_primitive("COUNT", function($interp)
  {
    $list = interp_pop_eval($interp);
    interp_push($interp, count($list));
  },
  $prims);
  
  interp_add_primitive("FETCH", function($interp)
  {
    $url = interp_pop_eval($interp);
    $text = file_get_contents($url);
    interp_push($interp, $text);
  },
  $prims);
  
  interp_add_primitive("JOIN", function($interp)
  {
    $list = interp_pop_eval($interp);
    $string = implode (" ", $list);
    interp_push($interp, $string);
  },
  $prims);
  
  interp_add_primitive("SPACE", function($interp)
  {
    $value = " ";
    interp_push($interp, $value);
  },
  $prims);
  
  interp_add_primitive("NEWLINE", function($interp)
  {
    array_push($stack, "\n");
  },
  $prims);
  
  interp_add_primitive("ENV", function($interp)
  {
    interp_push($interp, $interp->stackframe->env);
  },
  $prims);
  
  interp_add_primitive("STACK", function($interp)
  {
    interp_push($interp, $interp->stack);
  },
  $prims);
  
  interp_add_primitive("CALLSTACK", function($interp)
  {
    interp_push($interp, $interp->callstack);
  },
  $prims);
  
  interp_add_primitive("PRINT", function($interp)
  {
    $obj = interp_pop_eval($interp);
    print_r($obj);
  },
  $prims);
  
  interp_add_primitive("PRINTLN", function($interp)
  {
    $obj = interp_pop_eval($interp);
    print_r($obj); 
    print("\n");
  },
  $prims);
  
  interp_add_primitive("PRINTENV", function($interp)
  {
    print("PRINTENV STACKFRAME ".$interp->stackframe_index.":\n");
    print_r($interp->stackframe->env);
  },
  $prims);
  
  interp_add_primitive("PRINTPRIMS", function($interp)
  {
    print("PRIMITIVES:\n");
    foreach ($interp->prims as $name => $value):
      print("  ".$name."\n");
    endforeach;
  },
  $prims);
  
  interp_add_primitive("PRINTSTACK", function($interp)
  {
    print_r("PRINTSTACK:\n");
    print_r($interp->stack);
  },
  $prims);
  
  interp_add_primitive("PRINTCALLSTACK", function($interp)
  {
    print_r("PRINTCALLSTACK:\n");
    print("CURRENT FRAME: ".$interp->stackframe_index."\n");
    print_r($interp->callstack);
  },
  $prims);
  
  interp_add_primitive("PRINTSTACKFRAME", function($interp)
  {
    print("PRINTSTACKFRAME: ".$interp->stackframe_index."\n");
    print_r($interp->stackframe);
  },
  $prims);
  
  interp_add_primitive("PRINTSTACKFRAMEINDEX", function($interp)
  {
    print("PRINTSTACKFRAMEINDEX: ".$interp->stackframe_index."\n");
  },
  $prims);
  
  interp_add_primitive("MAKEOBJ", function($interp)
  {
    $obj = new stdClass();
    interp_push($interp, $obj);
  },
  $prims);
  
  interp_add_primitive("SETPROP", function($interp)
  {
    $obj = interp_pop_eval($interp);
    $prop_name = interp_pop($interp);
    $prop_value = interp_pop_eval($interp);
    $obj->$prop_name = $prop_value;
  },
  $prims);
  
  interp_add_primitive("GETPROP", function($interp)
  {
    $obj = interp_pop_eval($interp);
    $prop_name = interp_pop($interp);
    interp_push($interp, $obj->$prop_name);
  },
  $prims);
  
  return $prims;
}
