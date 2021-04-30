<?php

//TODO: Count number of lines in code files
echo "HELLO WORLD\n";

function countlines($file)
{
  return count(file($file));
}

$numlines = 
  countlines("base.h") +
  countlines("list.h") +
  countlines("interp.h") +
  countlines("parser.h") +
  countlines("primfuns.h");
  
echo "NUMBER OF LINES: ".$numlines."\n";

/***

210429: 775 lines
210430: 837 lines

(FOO (N) (VAR A B) (1 A SET N A + B SET B))

(FOO (N) 
  (VAR A B) 
  (1 A SET 
   N A + B SET 
   B)) DEF

(FUN (N) 
  (VAR A B) 
  (ENV N A B)
  (1 A SET 
   N A + B SET 
   B)) 
FOO SET


(FUN (N) 
  (VAR A B)
  (1 (VAR 1) SET 
   (VAR 0) (VAR 1) + (VAR 2) SET 
   (VAR 2))) 
   
Fun
  Params
  LocalVars
  Body

TypeLocalVar

FUN = LIST + VARS



***/
