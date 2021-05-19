<?php

// Program that returns the mantra of now

/*
Meditating on the Four Elements

Welcome to this meditation practice.
When you select BEGIN a mantra will be displayed.
Meditatate by repeating the mantra, quietly or aloud, with a pause in between.
Look away from the screen while doing this. 
Close your eyes if that feels good.
When you hear the birdsong, a new mantra is displayed.
Continue the practice for five to ten minutes or longer.
If you catch yourself thinking about something else, just go back to the mantra.
*/

/*
MANTRAS 
  MANTRAS LENGTH 1 - RANDOM
  LISTGET
*/

require('interpreter.php');
require('primitives.php');

VimanaCode(<<< CODE

MANTRAS
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
SET

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


MINUTE-OF-THE-HOUR 
 MANTRAS LENGTH 
 MODULO 
 MANTRAS GET
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
