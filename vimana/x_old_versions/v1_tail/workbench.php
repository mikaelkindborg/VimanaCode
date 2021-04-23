<?php

//
// File: workbench.php
// Date: 2021-04-14
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

require('interpreter2.php');
require('primitives2.php');

$code = <<< CODE

(N FACT) ((N 0 EQ) (1) (N 1 - FACT N *) IFELSE) DEF
(20 FACT) 100000 DOTIMES

(php v1_tail/workbench.php  4.49s user 0.01s system 99% cpu 4.516 total) DOC

CODE;

interp_eval_string($code);

/*



(FACT (N) ((N 0 EQ) 1 (N 1 - FACT N *) IFELSE)) DEF
3 FACT PRINTLN

(HELLO (N) (N PRINTLN (N 100 EQ NOT) (TEST 1 N + HELLO) IFTRUE)) DEF


(TEST () ((1 1 EQ) 
(HELLOTRUE PRINTLN) IFTRUE)) DEF


1 HELLO

100000 (20 FACT) DOTIMES

(FACT (N) ((N 0 EQ) (1) (N 1 - FACT N *) IFELSE)) DEF

3 FACT PRINTLN
100000 (20 FACT) DOTIMES

v1_plain with references:
(FACT (N) ((N 0 EQ) (1) (N 1 - FACT N *) IFELSE)) DEF
100000 (20 FACT) DOTIMES
php v1_optim/workbench.php  5.78s user 0.02s system 99% cpu 5.804 total
php v1_optim/workbench.php  5.83s user 0.02s system 99% cpu 5.857 total


20 FACT PRINTLN


Language Characteristics

As simple as possible but usable, both the language and the implementation.

Basically no syntax, only syntax is parens and whitespace.




*/
