<?php

//
// File: benchmark.php
// Date: 2021-04-25
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

VERSION 3:

First version of v3_callstack 2021-04-25:
(L N LOOP) (N 0 EQ NOT (L DO L N 1 - LOOP) IFTRUE) DEF
(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF
(20 FACT) 100000 LOOP
php v3_callstack/benchmark.php  15.03s user 0.04s system 99% cpu 15.104 total
php v3_callstack/benchmark.php  14.93s user 0.03s system 99% cpu 14.972 total

VERSION 2:

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

VERSION 1:

(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF
(20 FACT) 100000 TIMESDO
php v1_takeoff/benchmark.php  4.33s user 0.02s system 99% cpu 4.353 total

******************************************************************/

require('interpreter.php');
require('primitives.php');

$code = <<< CODE

(L N LOOP) (N 0 EQ NOT (L DO L N 1 - LOOP) IFTRUE) DEF
(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF
(20 FACT) 100000 LOOP

CODE;

interp_eval_string($code);
