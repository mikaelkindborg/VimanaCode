<?php

//
// File: benchmark.php
// Date: 2021-04-23
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

VERSION 1:

(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF
(20 FACT) 100000 TIMESDO
php v1_takeoff/benchmark.php  4.33s user 0.02s system 99% cpu 4.353 total

******************************************************************/

require('interpreter.php');
require('primitives.php');

$code = <<< CODE

(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF

(20 FACT) 100000 TIMESDO

CODE;

interp_eval_string($code);
