<?php

//
// File: examples.php
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

  php examples.php

PHP needs to be installed on your computer.

******************************************************************/

require('interpreter.php');
require('primitives.php');

$code = <<< CODE

HELLO_WORLD PRINTLN

(Welcome to the wonderful world of VimanaLang) JOIN PRINTLN

(This is the result of: 1 2 +) JOIN PRINTLN
1 2 + PRINTLN

(***
 This is a documentation comment.
 
 Note that the + sign in the above code follows after the numbers. 
 This is called Reverse Poslish Notation, or Postfix Notation.

 Vimana is based on lists. Lists are enclosed by parenteses. 
 This text is a list or symbols, or words. 

 Lists and symbols are not evaluated until you call a function. 
 The function always follows after the parameters.

 Note that the lists above are followed by the function JOIN, 
 which creates a string from the symbols in the list. 
 Then the function PRINTLN prints the string and a newline. 
 ***)

(*** Here is a function that will double a number ***)

(N DOUBLE) (N N +) DEF

(We can also create a function that prints lists with just one word ***)

(L PRINTLIST) (L JOIN PRINTLN) DEF

(What is the meaning of life, the universe, and everything?) PRINTLIST
21 DOUBLE PRINTLN

(*** Now we will make a function that generates a random number for a dice ***)

(TOSSDICE) (1 6 RANDOM) DEF

(*** Here is a looping function ***)

(L N LOOP) (N 0 EQ NOT (L DO L N 1 - LOOP) IFTRUE) DEF

(*** We use LOOP to toss the dice multiple times ***)

(Toss the dice 5 times:) PRINTLIST
(FOOBAR TOSSDICE PRINTLN) 5 LOOP

(*** Here is a recursive function that computes the factorial of a number ***)

(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF

(The factorial of 20 is:) PRINTLIST
20 FACT PRINTLN

(*** You can use FETCH to get data from a server ***)

(Guru meditation of the day:) PRINTLIST
http://ancientmantras.com/mantra-of-the-day.php FETCH PRINTLN

CODE;

interp_eval_string($code);
