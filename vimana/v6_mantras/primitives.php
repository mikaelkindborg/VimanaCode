<?php

//
// File: primitives.php
// Date: 2021-04-24
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

  interp_add_primitive("DEF", function(&$stack, &$env, $prims)
  {
    interp_define_function($stack, $env);
    return NULL;
  },
  $prims);
  
  interp_add_primitive("DO", function(&$stack, &$env, $prims)
  {
    $obj = interp_pop_eval($stack, $env);
    return $obj;
  },
  $prims);
  
  interp_add_primitive("CALL", function(&$stack, &$env, $prims)
  {
    $fun = interp_pop_eval($stack);
    return $fun;
  },
  $prims);
  
  interp_add_primitive("SET", function(&$stack, &$env, $prims)
  {
    $name = array_pop($stack);
    $value = interp_pop_eval($stack, $env);
    $env[$name] = $value;
    return NULL;
  },
  $prims);
  
  interp_add_primitive("VALUE", function(&$stack, &$env, $prims)
  {
    $var = array_pop($stack);
    if (is_array($var)):
      $var = $var[0];
    endif;
    $value = $env[$var];
    array_push($stack, $value);
    return NULL;
  },
  $prims);
  
  interp_add_primitive("DOC", function(&$stack, &$env, $prims)
  {
    array_pop($stack);
    return NULL;
  },
  $prims);
  
  interp_add_primitive("GET", function(&$stack, &$env, $prims)
  {
    $index = interp_pop_eval($stack, $env);
    $list = interp_pop_eval($stack, $env);
    array_push($stack, $list[$index]);
    return NULL;
  },
  $prims);
  
  interp_add_primitive("LENGTH", function(&$stack, &$env, $prims)
  {
    $list = interp_pop_eval($stack, $env);
    array_push($stack, count($list));
    return NULL;
  },
  $prims);
  
  interp_add_primitive("+", function(&$stack, &$env, $prims)
  {
    $b = interp_pop_eval($stack, $env);
    $a = interp_pop_eval($stack, $env);
    array_push($stack, $a + $b);
  },
  $prims);
  
  interp_add_primitive("-", function(&$stack, &$env, $prims)
  {
    $b = interp_pop_eval($stack, $env);
    $a = interp_pop_eval($stack, $env);
    array_push($stack, $a - $b);
    return NULL;
  },
  $prims);
  
  interp_add_primitive("*", function(&$stack, &$env, $prims)
  {
    $b = interp_pop_eval($stack, $env);
    $a = interp_pop_eval($stack, $env);
    array_push($stack, $a * $b);
    return NULL;
  },
  $prims);
  
  interp_add_primitive("/", function(&$stack, &$env, $prims)
  {
    $b = interp_pop_eval($stack, $env);
    $a = interp_pop_eval($stack, $env);
    array_push($stack, $a / $b);
    return NULL;
  },
  $prims);
  
  interp_add_primitive("MODULO", function(&$stack, &$env, $prims)
  {
    $b = interp_pop_eval($stack, $env);
    $a = interp_pop_eval($stack, $env);
    array_push($stack, $a % $b);
    return NULL;
  },
  $prims);
  
  interp_add_primitive("RANDOM", function(&$stack, &$env, $prims)
  {
    $b = interp_pop_eval($stack, $env);
    $a = interp_pop_eval($stack, $env);
    array_push($stack, mt_rand($a, $b));
    return NULL;
  },
  $prims);
  
  interp_add_primitive("T", function(&$stack, &$env, $prims)
  {
    array_push($stack, "T");
    return NULL;
  },
  $prims);
  
  interp_add_primitive("F", function(&$stack, &$env, $prims)
  {
    array_push($stack, "F");
    return NULL;
  },
  $prims);
  
  interp_add_primitive("NOT", function(&$stack, &$env, $prims)
  {
    $bool = array_pop($stack);
    if ($bool === "F"):
      array_push($stack, "T");
    else:
      array_push($stack, "F");
    endif;
    return NULL;
  },
  $prims);
  
  interp_add_primitive("EQ", function(&$stack, &$env, $prims)
  {
    $b = interp_pop_eval($stack, $env);
    $a = interp_pop_eval($stack, $env);
    if ($a === $b):
      array_push($stack, "T");
    else:
      array_push($stack, "F");
    endif;
    return NULL;
  },
  $prims);
  
  interp_add_primitive("IFTRUE", function(&$stack, &$env, $prims)
  {
    $true_branch = array_pop($stack);
    $truth = array_pop($stack);
    if ($truth === "T"):
      return $true_branch;
    else:
      return NULL;
    endif;
  },
  $prims);
  
  interp_add_primitive("IFELSE", function(&$stack, &$env, $prims)
  {
    $else_branch = array_pop($stack);
    $true_branch = array_pop($stack);
    $truth = array_pop($stack);
    if ($truth === "T"):
      return $true_branch;
    else:
      return $else_branch;
    endif;
  },
  $prims);
  
  // TIMESDO was introduced for benchmarking.
  interp_add_primitive("TIMESDO", function(&$stack, &$env, $prims)
  {
    $n = interp_pop_eval($stack, $env);
    $body = array_pop($stack);
    for  ($i = 0; $i < ($n - 1); $i++):
      interp_eval_list($body, $env, $stack, $prims);
      array_pop($stack); // This is unsafe, but prevents the stack from filling.
    endfor;
    return NULL;
  },
  $prims);

  interp_add_primitive("SLEEP", function(&$stack, &$env, $prims)
  {
    $time = interp_pop_eval($stack, $env);
    sleep($time);
    return NULL;
  },
  $prims);
  
  interp_add_primitive("EXIT", function(&$stack, &$env, $prims)
  {
    exit();
  },
  $prims);
  
  interp_add_primitive("FETCH", function(&$stack, &$env, $prims)
  {
    $url = interp_pop_eval($stack, $env);
    $text = file_get_contents($url);
    array_push($stack, $text);
    return NULL;
  },
  $prims);
  
  interp_add_primitive("JOIN", function(&$stack, &$env, $prims)
  {
    $list = interp_pop_eval($stack, $env);
    $string = implode (" ", $list);
    array_push($stack, $string);
    return NULL;
  },
  $prims);
  
  interp_add_primitive("SPACE", function(&$stack, &$env, $prims)
  {
    array_push($stack, " ");
    return NULL;
  },
  $prims);
  
  interp_add_primitive("NEWLINE", function(&$stack, &$env, $prims)
  {
    array_push($stack, "\n");
    return NULL;
  },
  $prims);
  
  interp_add_primitive("PRINT", function(&$stack, &$env, $prims)
  {
    $obj = interp_pop_eval($stack, $env);
    print_r($obj);
    return NULL;
  },
  $prims);
  
  interp_add_primitive("PRINTLN", function(&$stack, &$env, $prims)
  {
    $obj = interp_pop_eval($stack, $env);
    print_r($obj); 
    print("\n");
    return NULL;
  },
  $prims);
  
  interp_add_primitive("ENV", function(&$stack, &$env, $prims)
  {
    array_push($stack, $env);
    return NULL;
  },
  $prims);
  
  interp_add_primitive("STACK", function(&$stack, &$env, $prims)
  {
    array_push($stack, $stack);
  },
  $prims);
  
  interp_add_primitive("PRINTENV", function(&$stack, &$env, $prims)
  {
    print_r("PRINTENV:\n");
    print_r($env);
    return NULL;
  },
  $prims);
  
  interp_add_primitive("PRINTSTACK", function(&$stack, &$env, $prims)
  {
    print_r("PRINTSTACK:\n");
    print_r($stack);
    return NULL;
  },
  $prims);
  
  interp_add_primitive("MAKEOBJ", function(&$stack, &$env, $prims)
  {
    $obj = new stdClass();
    array_push($stack, $obj);
    return NULL;
  },
  $prims);
  
  interp_add_primitive("SETPROP", function(&$stack, &$env, $prims)
  {
    $obj = interp_pop_eval($stack, $env);
    $prop_name = array_pop($stack);
    $prop_value = interp_pop_eval($stack, $env);
    $obj->$prop_name = $prop_value;
    return NULL;
  },
  $prims);
  
  interp_add_primitive("GETPROP", function(&$stack, &$env, $prims)
  {
    $obj = interp_pop_eval($stack, $env);
    $prop_name = array_pop($stack);
    array_push($stack, $obj->$prop_name);
    return NULL;
  },
  $prims);
  
  return $prims;
}
