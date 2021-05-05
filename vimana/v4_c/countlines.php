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
210505: 1069 lines



((N FOO) (A B) 
 (...)) DEF

((n foo) 
 (a b) 
 (n a b + +) def

((n)
 (a b)
 (n a b + +) 
 fun foo set

((n) (n n +)) list set
21 list fun call println
list fun foo set
21 foo println


((n) (n n +)) lambda foobar

(foobar (lambda (n) (+ n n)))

(* MAIN FUNCTION *) DOC
(() (A B C) (
  21 A SET
  12 B SET
  A B + C SET
  C PRINT   )
) FUN CALL


(() (A B C) (
  21 A SET
  12 B SET
  A B + C SET
  C PRINT   )
) FUN MAIN SET


((MAIN) (A B C)
 (21 A SET
  12 B SET
  A B + C SET
  C PRINT)
) DEF

MAIN


(() (foo) 
  ((hello_foo print)) fun foo set
  foo
  foo
) fun call


(() (foo) 
  (hello_foo print) foo set
  foo do
  foo do
) fun call


(MYPRIMADD (X Y) (
  POP X SET
  POP Y SET
  X Y + PUSH     )
) DEFPRIM


(MYPRIMADD (X Y)
  POP X SET
  POP Y SET
  X Y + PUSH
) DEFPRIM


((MYADD) (X Y)
 (POP X SET
  POP Y SET
  X Y + PUSH)
) DEF

((MYADD)
 (POP POP +)
) DEF

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

https://www.raspberrypi.org/magpi-issues/


***/
