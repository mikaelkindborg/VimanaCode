<?php

//
// File: examples.php
// Date: 2021-04-23
// Author: Mikael Kinborg
// Email: mikael@kindborg.com
// Website: https://kindborg.com
// License: Creative Commons: Attribution-ShareAlike - CC BY-SA
//

//
// HOW TO USE
// ==========
//
// Open a terminal window and run:
//
// php examples.php
//
// PHP needs to be installed on your computer.
//

require('interpreter.php');
require('primitives.php');

$code = <<< CODE

HELLO_WORLD PRINTLN

(Welcome to the wonderful world of VimanaLang.) JOIN PRINTLN

(This is the result of: 1 2 +) JOIN PRINTLN
1 2 + PRINTLN

(Note that the + sign follows after the numbers. This is called Reverse Poslish Notation, or Postfix Notation.

Vimana is based on lists. Lists are enclosed by parenteses. This text is a list or symbols, or words. 

Lists and symbols are not evaluated until you call a function. The function always follows after the parameters.

Note that the lists above are followed by the function JOIN, which creates a string from the symbols in the list. Then the function PRINTLN prints the string and a newline. 

This list however, is a documentation comment, and it does not get printed. Note the function DOC after the list.) DOC

(Here is a function that will double a number.) DOC

(N DOUBLE) (N N +) DEF

(We can also create a function that prints lists with just one word.)

(L PRINTLIST) (L JOIN PRINTLN) DEF

(What is the meaning of life, the universe, and everything?) PRINTLIST
21 DOUBLE PRINTLN

(Now we will make a function that generates a random number for a dice.) DOC

(TOSSDICE) (1 6 RANDOM) DEF

(Toss the dice 5 times:) PRINTLIST

(TOSSDICE PRINTLN) 5 TIMESDO

(Here is a recursive function that computes the factorial of a number.)

(N FACT) ((N 0 EQ) (1) (N 1 - FACT N *) IFELSE) DEF

(The factorial of 20 is:) PRINTLIST
20 FACT PRINTLN

(You can use FETCH to get data from a server.) DOC

(Guru meditation of the day:) PRINTLIST
http://ancientmantras.com/mantra-of-the-day.php FETCH PRINTLN

CODE;

interp_eval_string($code);
