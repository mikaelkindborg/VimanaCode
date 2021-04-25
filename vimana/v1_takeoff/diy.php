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

CODE;

// Table that holds primitive functions.
$prims = interp_create_primitives();

// Add a primitive. Note that the primitives in primitives.php
// are in the same table, so if you use an existing name it will 
// be overwritten.
interp_add_primitive("HELLO", function(&$stack, &$env, $prims)
{
  interp_println("Hello World");
},
$prims);

// Eval the above CODE using the custom table.
interp_eval_string($code, $prims);
