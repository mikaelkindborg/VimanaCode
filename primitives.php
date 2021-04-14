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
  
  $add("EVAL", function(&$env, &$stack, &$prims)
  {
    $obj = array_pop($stack);
    f_eval_list($obj, $env, $stack, $prims);
  });
  
  $add("CALL", function(&$env, &$stack, &$prims)
  {
    $fun = array_pop($stack);
    f_eval_fun($fun, $env, $stack, $prims);
  });
  
  $add("SET", function(&$env, &$stack, &$prims)
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
  
  $add("DEF", function(&$env, &$stack, &$prims)
  {
    $fundef = array_pop($stack);
    $name = $fundef[0];
    $params = $fundef[1];
    $body = $fundef[2];
    print_r("DEF: ".$name."\n");
    $fun = ["FUN", $params, $body];
    $env[$name] = $fun;
  });
  
  $add("VALUE", function(&$env, &$stack, &$prims)
  {
    $var = array_pop($stack);
    if (is_array($var)):
      $var = $var[0];
    endif;
    $value = $env[$var];
    array_push($stack, $value);
  });
  
  $add("MAKEOBJ", function(&$env, &$stack, &$prims)
  {
    $obj = new stdClass();
    array_push($stack, $obj);
  });
  
  $add("SETPROP", function(&$env, &$stack, &$prims)
  {
    $obj = array_pop_eval($stack, $env);
    $prop_name = array_pop($stack);
    $prop_value = array_pop_eval($stack, $env);
    $obj->$prop_name = $prop_value;
  });
  
  $add("GETPROP", function(&$env, &$stack, &$prims)
  {
    $obj = array_pop_eval($stack, $env);
    $prop_name = array_pop($stack);
    array_push($stack, $obj->$prop_name);
  });
  
  $add("POP", function(&$env, &$stack, &$prims)
  {
    array_pop($stack);
  });
  
  $add("GET", function(&$env, &$stack, &$prims)
  {
    $index = array_pop_eval($stack, $env);
    $list = array_pop_eval($stack, $env);
    array_push($stack, $list[$index - 1]);
  });
  
  $add("COUNT", function(&$env, &$stack, &$prims)
  {
    $list = array_pop_eval($stack, $env);
    array_push($stack, count($list));
  });
  
  $add("+", function(&$env, &$stack, &$prims)
  {
    $b = array_pop_eval($stack, $env);
    $a = array_pop_eval($stack, $env);
    $res = $a + $b;
    array_push($stack, $res);
  });
  
  $add("-", function(&$env, &$stack, &$prims)
  {
    $b = array_pop_eval($stack, $env);
    $a = array_pop_eval($stack, $env);
    $res = $a - $b;
    array_push($stack, $res);
  });
  
  $add("*", function(&$env, &$stack, &$prims)
  {
    $b = array_pop_eval($stack, $env);
    $a = array_pop_eval($stack, $env);
    $res = $a * $b;
    array_push($stack, $res);
  });
  
  $add("/", function(&$env, &$stack, &$prims)
  {
    $b = array_pop_eval($stack, $env);
    $a = array_pop_eval($stack, $env);
    $res = $a / $b;
    array_push($stack, $res);
  });
  
  $add("SPACE", function(&$env, &$stack, &$prims)
  {
    array_push($stack, " ");
  });
  
  $add("T", function(&$env, &$stack, &$prims)
  {
    array_push($stack, "T");
  });
  
  $add("F", function(&$env, &$stack, &$prims)
  {
    array_push($stack, "F");
  });
  
  $add("NOT", function(&$env, &$stack, &$prims)
  {
    $bool = array_pop($stack);
    if ($bool === "F"):
      $res = "T";
    else:
      $res = "F";
    endif;
    array_push($stack, $res);
  });
  
  $add("EQ", function(&$env, &$stack, &$prims)
  {
    $b = array_pop_eval($stack, $env);
    $a = array_pop_eval($stack, $env);
    if ($a === $b):
      $res = "T";
    else:
      $res = "F";
    endif;
    array_push($stack, $res);
  });
  
  $add("IFTRUE", function(&$env, &$stack, &$prims)
  {
    $a = array_pop($stack);
    $c = array_pop($stack);
    f_eval_list($c, $env, $stack, $prims);
    $res = array_pop($stack);
    if ($res === "T"):
      f_eval_list($a, $env, $stack, $prims);
    endif;
  });
  
  $add("IFELSE", function(&$env, &$stack, &$prims)
  {
    $b = array_pop($stack);
    $a = array_pop($stack);
    $c = array_pop($stack);
    f_eval_list($c, $env, $stack, $prims);
    $res = array_pop($stack);
    if ($res === "T"):
      f_eval_list($a, $env, $stack, $prims);
    else:
      f_eval_list($b, $env, $stack, $prims);
    endif;
  });
  
  $add("FETCH", function(&$env, &$stack, &$prims)
  {
    $url = array_pop_eval($stack, $env);
    $text = file_get_contents($url);
    array_push($stack, $text);
  });
  
  $add("PRINT", function(&$env, &$stack, &$prims)
  {
    $obj = array_pop_eval($stack, $env);
    print_r($obj);
  });
  
  $add("PRINTLN", function(&$env, &$stack, &$prims)
  {
    $obj = array_pop_eval($stack, $env);
    print_r($obj); 
    print("\n");
  });
  
  $add("PRINTENV", function(&$env, &$stack, &$prims)
  {
    print_r("PRINTENV:\n");
    print_r($env);
  });
  
  $add("PRINTSTACK", function(&$env, &$stack, &$prims)
  {
    print_r("PRINTSTACK:\n");
    print_r($stack);
  });
  
  $add("SET", function(&$env, &$stack, &$prims)
  {
  });
  
  return $prims;
}
