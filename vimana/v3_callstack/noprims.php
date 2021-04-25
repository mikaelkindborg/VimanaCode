<?php

//
// File: diy.php
// Date: 2021-04-25
// Author: Mikael Kinborg
// Email: mikael@kindborg.com
// Website: https://kindborg.com
// License: Creative Commons: Attribution-ShareAlike - CC BY-SA
//

/******************************************************************

WHAT IS THIS
============

This exampel shows what happens when you have no primitives!
It prints the data stack.

******************************************************************/

require('interpreter.php');
require('primitives.php');

$code = <<< CODE

HELLO_WORLD PRINTLN
(1 2 + 10 * 12 +) DO PRINTLN
(THIS IS A LIST. ACTUALLY THE WHOLE PROGRAM IS A LIST.) JOIN PRINTLN
(*** A COMMENT ***)
PRINT_THE_STACK

CODE;

// Empty table that holds primitive functions.
$prims = [];

// Uncomment this line to create the system primitives 
// and see what happens then.
//$prims = interp_create_primitives();

// Add a primitive that prints the stack.
interp_add_primitive("PRINT_THE_STACK", function($interp)
{
  interp_println("THIS IS THE DATA STACK:");
  print_r($interp->stack);
},
$prims);

// Eval the above CODE using the custom table.
interp_eval_string($code, $prims);
