<?php

function interp_eval_string($code)
{
  $list = interp_parse($code);
  $env = [];
  $stack = [];
  $prims = interp_create_primitives();
  interp_eval_list($list, $env, $stack, $prims);
}

function interp_eval_list($list, &$env, &$stack, $prims)
{
  if (!is_array($list)):
    array_push($stack, interp_eval_element($list, $env));
    return;
  endif;
  
  foreach ($list as $element):
    if (is_string($element)):
      if (strlen($element) === 0):
        interp_println("ERROR: EMPTY STRING IN EVAL");
        exit();
      endif;
      
      if (isset($prims[$element])):
        $prims[$element]($stack, $env, $prims);
        continue;
      endif;

      $fun = & $env[$element];
      if (isset($fun) && (is_array($fun) && ($fun[0] === "FUN"))):
        $new_env = $env;
        $local_vars = $fun[1];
        foreach (array_reverse($local_vars) as $var):
          $element = array_pop($stack);
          $value = (is_string($element) && isset($env[$element])) ? 
            $env[$element] : $element;
          $new_env[$var] = $value;
        endforeach;
        interp_eval_list($fun[2], $new_env, $stack, $prims);
        continue;
      endif;
    endif;
    
    array_push($stack, $element);
  endforeach;
}

function interp_eval_element($element, $env)
{
  return (is_string($element) && isset($env[$element])) ? 
    $env[$element] : $element;
}

function array_pop_eval(&$stack, $env)
{
  $element = array_pop($stack);
  return (is_string($element) && isset($env[$element])) ? 
    $env[$element] : $element;
}

function interp_define_function(&$stack, &$env)
{
  $body = array_pop($stack);
  $fundef = array_pop($stack);
  $name = array_pop($fundef);
  $params = $fundef;
  $fun = ["FUN", $params, $body];
  $env[$name] = $fun;
}

function interp_parse($code)
{
  $code = str_replace("(", " ( ", $code);
  $code = str_replace(")", " ) ", $code);
  $tokens = preg_split("/[\s]+/", $code);
  // Remove empty strings.
  $tokens = array_filter($tokens,
    function($token) { return strlen($token) > 0; });
  $list = interp_create_list($tokens);
  return $list;
}

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
    if (is_numeric($next)):
      $next = $next + 0;
    endif;
    array_push($list, $next);
  endwhile;
}
