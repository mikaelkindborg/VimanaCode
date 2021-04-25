<?php
//
// File: debug.php
// Date: 2021-04-25
// Author: Mikael Kinborg
// Email: mikael@kindborg.com
// Website: https://kindborg.com
// License: Creative Commons: Attribution-ShareAlike - CC BY-SA

/******************************************************************

WHAT IS THIS
============

Functions for printing lists and objects. 

Useful for debugging in absence of a debugger.

******************************************************************/

// For debugging.

function interp_print_obj($str, $obj)
{
  print($str.": ");
  print_r($obj);
  print("\n");
}

function interp_print_array($str, $array)
{
  print($str.":\n");
  print(interp_array_as_string($array));
  print("\n");
}

// Works for small lists, output is not so nice for maps.
function interp_array_as_string($array, $indent = "")
{
  if (!is_array($array)):
    return "".$array;
  endif;
  
  foreach ($array as $key => $value):
    $value_string = $value."";
    if (is_array($value)):
      $value_string = "(".interp_array_as_string($value, $indent."  ").")";
    endif;
    if (is_numeric($key)):
      if ($key == array_key_last($array)):
        $string .= $value_string;
      else:
        $string .= $value_string." ";
      endif;
    else:
      if ($key == array_key_last($array)):
        $string .= $indent.$key.": ".$value_string;
      else:
        $string .= $indent.$key.": ".$value_string."\n";
      endif;
    endif;
  endforeach;
  
  return $string;
}
