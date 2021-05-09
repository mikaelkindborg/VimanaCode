<?php

// Program that returns the mantra of now

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
 (I sense the air that surrounds me)
 (I feel my feet on the ground)
 (What I touch is real)
 (I sense the air touching my skin))
MANTRAS SET

MANTRAS 
  MINUTE-OF-THE-HOUR 
    MANTRAS LENGTH 
    MODULO
  GET
  MANTRA-OF-NOW SET

MANTRA-OF-NOW JOIN PRINT

CODE
);

/*
In LISP:

(SET 'MANTRAS
 '((I listen)
   (I breathe in, I breathe out)
   (I experience my existence)
   (I exist here and now)
   (I am conscious of my senses)
   (I follow my breath)
   (I sense the air that surrounds me)
   (I feel my feet on the ground)
   (What I touch is real)
   (I sense the air touching my skin)))

(SET 'MANTRA-OF-NOW 
  (GET (MODULO 
         (MINUTE-OF-THE-HOUR) 
         (LENGTH MANTRAS)) 
       MANTRAS))

(PRINT (JOIN MANTRA-OF-NOW))
*/

function VimanaCode($code)
{
  $prims = interp_create_primitives();
  
  interp_add_primitive("MINUTE-OF-THE-HOUR", function(&$stack, &$env, $prims)
  {
    array_push($stack, (int)date('i'));
    return NULL;
  },
  $prims);

  interp_eval_string($code, $prims);
}
