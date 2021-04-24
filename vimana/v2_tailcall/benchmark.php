<?php

//
// File: benchmark.php
// Date: 2021-04-24
// Author: Mikael Kinborg
// Email: mikael@kindborg.com
// Website: https://kindborg.com
// License: Creative Commons: Attribution-ShareAlike - CC BY-SA
//

/******************************************************************

HOW TO USE
==========

Open a terminal window and run:

  time php benchmark.php

PHP needs to be installed on your computer.

BENCHMARK RESULTS
=================

(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF
(20 FACT) 100000 TIMESDO
php v2_tailcall/benchmark.php  5.07s user 0.02s system 99% cpu 5.092 total

(N FACT) ((N 0 EQ) DO (1) (N 1 - FACT N *) IFELSE) DEF
(20 FACT) 100000 TIMESDO
php v2_tailcall/benchmark.php  5.96s user 0.02s system 99% cpu 5.983 total

(L N LOOP) (N 0 EQ NOT (L DO L N 1 - LOOP) IFTRUE) DEF
(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF
(20 FACT) 100000 LOOP
php v2_tailcall/benchmark.php  5.37s user 0.02s system 99% cpu 5.397 total

******************************************************************/

require('interpreter.php');
require('primitives.php');

$code = <<< CODE

(L N LOOP) (N 0 EQ NOT (L DO L N 1 - LOOP) IFTRUE) DEF
(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF
(20 FACT) 100000 LOOP

CODE;

interp_eval_string($code);
