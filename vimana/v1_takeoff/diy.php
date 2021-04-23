<?php

//
// File: diy.php
// Date: 2021-04-23
// Author: Mikael Kinborg
// Email: mikael@kindborg.com
// Website: https://kindborg.com
// License: Creative Commons: Attribution-ShareAlike - CC BY-SA
//

//
// Do It Yourself (DIY)
// ====================
//
// Here is an example of how to write your own primitives in PHP.
//
// Have fun!
//

require('interpreter.php');
require('primitives.php');

$code = <<< CODE

HELLO

a b c PRINTSTACK

CODE;

// Table that holds primitive functions.
$prims = [];

// Add a primitive.
interp_add_primitive("HELLO", function(&$stack, &$env, $prims)
{
  interp_println("Hello World");
},
$prims);

// Eval the above code using the custom table.
interp_eval_string($code, $prims);
