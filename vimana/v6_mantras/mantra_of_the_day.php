<?php

// Program that returns the mantra of the day

/*
MANTRAS 
  MANTRAS LENGTH 1 - RANDOM
  LISTGET
*/

require('interpreter.php');
require('primitives.php');

VimanaCode(<<< CODE

((I listen)
 (I breathe in, I breathe out)
 (I experience my existence)
 (I exist here and now)
 (I am conscious of my senses)
 (I follow my breath)
 (I sense the air that surrounds me))
MANTRAS SET

MANTRAS 
  DAY-OF-THE-YEAR MANTRAS LENGTH MODULO
  GET
MANTRA-OF-THE-DAY SET

MANTRA-OF-THE-DAY JOIN PRINT

CODE
);

function VimanaCode($code)
{
  $prims = interp_create_primitives();

  interp_add_primitive("DAY-OF-THE-YEAR", function(&$stack, &$env, $prims)
  {
    array_push($stack, date('z'));
    return NULL;
  },
  $prims);

  interp_eval_string($code, $prims);
}
