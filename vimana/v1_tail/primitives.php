<?php

//
// File: primitives.php
// Date: 2021-04-14
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
    f_add_primitive($prims, $symbol, $fun);
  };
  
  $add("EVAL", function(&$env, &$stack, $prims)
  {
    $obj = array_pop($stack);
    f_eval_list($obj, $env, $stack, $prims);
  });
  
  $add("CALL", function(&$env, &$stack, $prims)
  {
    $fun = array_pop($stack);
    f_eval_fun($fun, $env, $stack, $prims);
  });
  
  $add("SET", function(&$env, &$stack, $prims)
  {
    // SET is single assignment
    $name = array_pop($stack);
    if (isset($env[$name])):
      echo "ERROR: ATTEMPT TO SET ASSIGNED VARIABLE: ".$name."\n";
      exit();
    endif;
    $value = array_pop_eval($stack, $env);
    $env[$name] = $value;
  });
  
  $add("DEF", function(&$env, &$stack, $prims)
  {
    $fundef = array_pop($stack);
    $name = $fundef[0];
    $params = $fundef[1];
    $body = $fundef[2];
    $fun = ["FUN", $params, $body];
    $env[$name] = $fun;
  });
  
  $add("VALUE", function(&$env, &$stack, $prims)
  {
    $var = array_pop($stack);
    if (is_array($var)):
      $var = $var[0];
    endif;
    $value = $env[$var];
    array_push($stack, $value);
  });
  
  $add("DOC", function(&$env, &$stack, $prims)
  {
    array_pop($stack);
  });
  
  $add("GET", function(&$env, &$stack, $prims)
  {
    $index = array_pop_eval($stack, $env);
    $list = array_pop_eval($stack, $env);
    array_push($stack, $list[$index - 1]);
  });
  
  $add("COUNT", function(&$env, &$stack, $prims)
  {
    $list = array_pop_eval($stack, $env);
    array_push($stack, count($list));
  });
  
  $add("+", function(&$env, &$stack, $prims)
  {
    $b = array_pop_eval($stack, $env);
    $a = array_pop_eval($stack, $env);
    array_push($stack, $a + $b);
  });
  
  $add("-", function(&$env, &$stack, $prims)
  {
    $b = array_pop_eval($stack, $env);
    $a = array_pop_eval($stack, $env);
    array_push($stack, $a - $b);
  });
  
  $add("*", function(&$env, &$stack, $prims)
  {
    f_print_array("ENV IN MULT", $env);
    $b = array_pop_eval($stack, $env);
    $a = array_pop_eval($stack, $env);
    $res = $a * $b;
    f_println("MULT ".$a." ".$b." ".$res);
    array_push($stack, $a * $b);
    //f_print_array("STACK", $stack);
  });
  
  $add("/", function(&$env, &$stack, $prims)
  {
    $b = array_pop_eval($stack, $env);
    $a = array_pop_eval($stack, $env);
    array_push($stack, $a / $b);
  });
  
  $add("T", function(&$env, &$stack, $prims)
  {
    array_push($stack, "T");
  });
  
  $add("F", function(&$env, &$stack, $prims)
  {
    array_push($stack, "F");
  });
  
  $add("NOT", function(&$env, &$stack, $prims)
  {
    $bool = array_pop($stack);
    if ($bool === "F"):
      array_push($stack, "T");
    else:
      array_push($stack, "F");
    endif;
  });
  
  $add("EQ", function(&$env, &$stack, $prims)
  {
    f_println("EVAL EQ");
    $b = array_pop_eval($stack, $env);
    $a = array_pop_eval($stack, $env);
    f_println("A = ".$a);
    f_println("B = ".$b);
    if ($a === $b):
      f_println("PUSH T");
      array_push($stack, "T");
    else:
      f_println("PUSH F");
      array_push($stack, "F");
    endif;
    
  });
  
  $add("IFTRUE", function(&$env, &$stack, $prims)
  {
    $a = array_pop($stack);
    $c = array_pop($stack);
    f_eval_list($c, $env, $stack, $prims);
    $res = array_pop($stack);
    if ($res === "T"):
      f_eval_list($a, $env, $stack, $prims);
    endif;
  });
  
  $add("IFELSE", function(&$env, &$stack, $prims)
  {
    $b = array_pop($stack);
    $a = array_pop($stack);
    $c = array_pop($stack);
    f_eval_list($c, $env, $stack, $prims);
    $res = array_pop($stack);
    if ($res === "T"):
      f_eval_list($a, $env, $stack, $prims);
    f_print_array("IFELSE STACK A", $stack);
    else:
      f_eval_list($b, $env, $stack, $prims);
    f_print_array("IFELSE STACK B", $stack);
    endif;
  });
  
  // DOTIMES is meant for benchmarking. It pushes the last 
  // result onto othe stack.
  $add("DOTIMES", function(&$env, &$stack, $prims)
  {
    $body = array_pop($stack);
    $n = array_pop_eval($stack, $env);
    for  ($i = 0; $i < ($n - 1); $i++):
      f_eval_list($body, $env, $stack, $prims);
      array_pop($stack);
    endfor;
    // Pushes the result of the last interation.
    f_eval_list($body, $env, $stack, $prims);
  });

  $add("FETCH", function(&$env, &$stack, $prims)
  {
    $url = array_pop_eval($stack, $env);
    $text = file_get_contents($url);
    array_push($stack, $text);
  });
  
  $add("JOIN", function(&$env, &$stack, $prims)
  {
    $list = array_pop_eval($stack, $env);
    $string = implode (" ", $list);
    array_push($stack, $string);
  });
  
  $add("SPACE", function(&$env, &$stack, $prims)
  {
    array_push($stack, " ");
  });
  
  $add("PRINT", function(&$env, &$stack, $prims)
  {
    $obj = array_pop_eval($stack, $env);
    print_r($obj);
  });
  
  $add("PRINTLN", function(&$env, &$stack, $prims)
  {
    $obj = array_pop_eval($stack, $env);
    print_r($obj); 
    print("\n");
  });
  
  $add("ENV", function(&$env, &$stack, $prims)
  {
    array_push($stack, $env);
  });
  
  $add("STACK", function(&$env, &$stack, $prims)
  {
    array_push($stack, $stack);
  });
  
  $add("PRINTENV", function(&$env, &$stack, $prims)
  {
    print_r("PRINTENV:\n");
    print_r($env);
  });
  
  $add("PRINTSTACK", function(&$env, &$stack, $prims)
  {
    print_r("PRINTSTACK:\n");
    print_r($stack);
  });
  
  $add("MAKEOBJ", function(&$env, &$stack, $prims)
  {
    $obj = new stdClass();
    array_push($stack, $obj);
  });
  
  $add("SETPROP", function(&$env, &$stack, $prims)
  {
    $obj = array_pop_eval($stack, $env);
    $prop_name = array_pop($stack);
    $prop_value = array_pop_eval($stack, $env);
    $obj->$prop_name = $prop_value;
  });
  
  $add("GETPROP", function(&$env, &$stack, $prims)
  {
    $obj = array_pop_eval($stack, $env);
    $prop_name = array_pop($stack);
    array_push($stack, $obj->$prop_name);
  });
  
  return $prims;
}
