<?php
//
// File: workbench.php
// Date: 2021-04-18
// Author: Mikael Kinborg
// Email: mikael@kindborg.com
// Website: kindborg.com
// License: Creative Commons: Attribution-ShareAlike - CC BY-SA
//
// HOW TO USE
// ==========
//
// Open a terminal window and run:
//
// php workbench.php
//
// PHP needs to be installed on your computer.
//

require('interpreter.php');
require('primitives.php');
require('debug.php');

$code = <<< CODE

(L N LOOP) (N 0 EQ NOT (L DO L N 1 - LOOP) IFTRUE) DEF

(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF

(20 FACT PRINTLN) 10 LOOP

CODE;

interp_eval_string($code);

/*

First version of v3_callstack 2021-04-25:
(L N LOOP) (N 0 EQ NOT (L DO L N 1 - LOOP) IFTRUE) DEF
(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF
(20 FACT) 100000 LOOP
php v3_callstack/workbench.php  14.97s user 0.02s system 99% cpu 15.000 total





(L N LOOP) (N 0 EQ NOT (L DO L N 1 - LOOP) IFTRUE) DEF
(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF
(20 FACT) 100000 LOOP



(X HELLO) (HELLO_WORLD PRINT SPACE PRINT X X + PRINTLN) DEF
21 HELLO

T (TRUTH PRINTLN) IFTRUE

F NOT (TRUE_BRANCH PRINTLN) (ELSE_BRANCH PRINTLN) IFELSE

(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF
20 FACT PRINTLN
PRINTCALLSTACK



A B C PRINTSTACK

1 2 + PRINTLN


PRINTENV

PRINTPRIMS

(N FACT) ((N 0 EQ) DO (1) (N 1 - FACT N *) IFELSE) DEF

(X L LOOP) ((X 0 EQ NOT) DO (L DO X 1 - L LOOP) IFTRUE) DEF

(N BAR) ((N 0 EQ) DO (1) ( N 1 - BAR N 1 - BAR +) IFELSE) DEF

100 (10 BAR PRINTLN) LOOP

(100 (HELLO PRINTLN 5 (WORLD PRINTLN) LOOP) LOOP) DOC

(100000 (20 FACT) LOOP) DOC
(24.47s user 0.04s system 99% cpu 24.533 total) DOC



Nicer formatting:

(N FACT) 
  ((N 0 EQ) DO 
    (1) 
    (N 1 - FACT N *) IFELSE) DEF

(N L LOOP) 
  ((N 0 EQ NOT) DO 
    (L DO N 1 - L LOOP) IFTRUE) DEF

100000 (20 FACT) LOOP

(24.47s user 0.04s system 99% cpu 24.533 total) DOC

42 X SET
X PRINTLN
(X PRINTLN) DO
(43 X SET X PRINTLN) DO
X PRINTLN
(44 Z SET Z PRINTLN) DO
Z PRINTLN

(N TAIL) 
  ((0 N EQ NOT) DO 
    (N PRINTLN 
     N 1 - TAIL) IFTRUE) DEF
1000 TAIL

(N FOO) (N PRINTLN N BAR) DEF
(N BAR) (N PRINTLN) DEF
42 FOO


(N TAIL) (N PRINTLN 1 SLEEP N 1000 + TAIL) DEF
1000 TAIL

(N FACT) ((N 0 EQ) EVAL (1) (N 1 - FACT N *) IFELSE) DEF
30000 FACT PRINTLN


(N FOO) (N PRINTLN N BAR) DEF
(N BAR) (N PRINTLN) DEF
42 FOO


HELLO PRINTLN 1 SLEEP WORLD PRINTLN 1 GOTO

1 (X) SET

(HELLO PRINTLN) (1 GOTO)


(N L LOOP) ((N 0 EQ NOT) EVAL (L EVAL N 1 - LOOP) IFTRUE) DEF
300 (HELLO PRINTLN) LOOP


(L FOO) (L EVAL) DEF
(HELLO PRINTLN) EVAL


(N FACT) ((N 0 EQ) EVAL (1) (N 1 - FACT N *) IFELSE) DEF
300 FACT PRINTLN

(N FACT) ((N 1 EQ) EVAL (1) (N 1 - FACT) IFELSE) DEF
2 FACT 

3 INCR
(A 1 +)

(N FOO) ((N 1 EQ) EVAL PRINTSTACK (N VALUE) IFTRUE) DEF
1 FOO PRINTSTACK

T (HELLO PRINTLN) (WORLD PRINTLN) IFELSE


(
1 2 + PRINTLN
((HELLO WORLD) JOIN PRINTLN) EVAL

(X FOO) (X 1 +) DEF
PRINTENV
2 FOO PRINTLN

(N FACT) ((N 0 EQ) EVAL (1) (N 1 - FACT) IFELSE) DEF
3 FACT PRINTLN
)
DOC
*/
