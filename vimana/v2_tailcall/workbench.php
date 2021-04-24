<?php

//
// File: workbench.php
// Date: 2021-04-24
// Author: Mikael Kinborg
// Email: mikael@kindborg.com
// Website: https://kindborg.com
// License: Creative Commons: Attribution-ShareAlike - CC BY-SA
//

/******************************************************************

HOW TO USE
==========

This file is intended as a workspace where you write your Vimana code.

The easiest way to get started is to open this file in a text editor
and open a console window where you run the program. Make sure you 
have PHP installed on your computer.

Just type your code below and run the program with the following 
command to see the output:

  php workbench.php

Explore the other files, like examples.php, to learn more. You can
also explore the PHP source code for the interpreter and add your own 
primitives.

******************************************************************/

require('interpreter.php');
require('primitives.php');

$code = <<< CODE

WELCOME_TO_VIMANA PRINTLN

CODE;

interp_eval_string($code);
