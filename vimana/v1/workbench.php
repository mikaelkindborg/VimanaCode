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

require('interpreter.php');
require('primitives.php');

$code = <<< CODE

(FACT (N) ((N 0 EQ) (1) (N 1 - FACT N *) IFELSE)) DEF
100000 (20 FACT) DOTIMES

CODE;

f_eval_string($code);

/*

v1 with references:
(FACT (N) ((N 0 EQ) (1) (N 1 - FACT N *) IFELSE)) DEF
100000 (20 FACT) DOTIMES
php v1/workbench.php  8.98s user 0.02s system 99% cpu 9.018 total


20 FACT PRINTLN

*/