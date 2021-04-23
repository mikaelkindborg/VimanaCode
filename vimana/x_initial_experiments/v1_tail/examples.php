<?php

//
// File: examples.php
// Date: 2021-04-14
// Author: Mikael Kinborg
// Email: mikael@kindborg.com
// Website: kindborg.com
// License: Creative Commons: Attribution-ShareAlike - CC BY-SA
//
// This file contains some code examples for VimanaLang,
// a highly experimental dynamic programming language.
// inspired by Lisp and also by Forth.
//
// Run the code in this file from the command line on
// your computer like this:
//
// php examples.php
//
// The purpose of VimanaLang is to provide a small interpreter 
// that can be experimented with and modified just for the fun of it.
// One could say that the language is as esoteric as the flying
// Vimanas of the ancient past.
//
// As it is easy to get drawn into hacking the language, it is
// good to take time off the computer and be in nature, meditate
// on your existence, practice Yoga and Yoga Nidra, and so on.
//
// The use of upper case names is for nostalgic reasons.
// This was common in programming languages in the 1970s and 1980s.
// LISP (for example MacLisp and Interlisp-D), as well as LOGO, 
// FORTH and BASIC used upper case names. Smalltalk, on the other
// hand, used mixed casing.
//
// Symbols are case sensitive. You can use any casing you wish.
// You can also modify names of primitives to use the casing 
// you prefer. VimalaLang has no standards, syntax for primitives
// is flexible as long as you follow the Reverse Polish Notation
// stack order. 
//
// Evaluation order is left to right. Symbols are pushed onto
// the stack in the order they appear. When a function symbol
// is encountered, the function is evaluated. A function pops
// its arguments off the stack.
//
// Example:
//
//   Program: 
//
//     1 2 + PRINTLN
//
//   Stack during execution:
//
//     1
//     1 2
//     1 2 + (here the function + gets called)
//     3 (+ pushes the result onto the stack)
//     3 PRINTLN (pops one element off the stack and prints it)
//     (stack is empty)
//
//   Note that functions are not pushed onto the stack. 
//   Note that when the interpreter encounters a function
//   symbol, it evaluates it, it does not get push onto the stack.
//
// Symbols that are unbound evaluate to themselves.
//
// A variable is a symbol bound to a value.
//
// Variables are single assignment. Calling SET for a
// bound variable throws an error.
//
// Functions defined using DEF may however be redfined.
//
// Lists are not evaluated unless you call EVAL or CALL.
//
// Primitives have full control of which parameters to evaluate.
//
// Syntax checking and error handling are pretty non-existent.
// Use debug printing to track down errors in your code.
//
// Use PRINTSTACK and PRINTENV for debugging. It is also very
// useful to use debug print statements inside the interpreter
// (this is a must to find bugs when modifying the interpreter).
//
// Kindly see interpreter.php and primitives.php for 
// further details.
//
// The current interpreter uses the PHP call stack for recusive calls.
// I am planning to change this and implement tail optimization.
// Objects and lightwight processes in the spirit of Erlang are
// also being planned. I am thinking to make some kind of workbench
// for making retro games using SDL, in the spirit of the Amiga.
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

(Hello World) JOIN PRINTLN

(This is a list used as comment. DOC does 
not evaluate its argument, but simply pops 
it off the stack.) DOC

(Reverse Polish Notation is used.) DOC

1 2 + PRINTLN

(Code formatting is flexible) DOC

1.1 
2.2 
+ 
PRINTLN

(Unbound variables are treated as symbols.) DOC

HELLO_WORLD PRINTLN

(Lists are not evaluated per default,
they are simply pushed onto the stack.)

(But it is good practice to clean up comments.
Here we print the stack depth and pop this and
the previous list comment off the stack. You
can also print the stack itself with STACK PRINT)

Stack_depth: PRINT 
STACK COUNT PRINTLN

DOC 
DOC

Stack_depth: PRINT 
STACK COUNT PRINTLN

(T is the symbol for TRUE and F is for FALSE) DOC

T condition_is_true IFTRUE PRINTLN
F condition_is_true condition_is_false IFELSE PRINTLN

(Here is the Factorial function. Function names are global.) DOC

(FACT (N) 
  ((N 0 EQ) 1
   (N 1 - FACT N *) IFELSE)) DEF
20 FACT PRINTLN

(Examples of EVAL, CALL and SET.) DOC

(1 2 3 + + PRINTLN) EVAL

(FUN () (HELLO PRINTLN)) CALL

2222 (FUN (X) (X 2 *)) CALL PRINTLN

(FUN (X) (X 2 *)) DOUBLE SET
4444 DOUBLE PRINTLN

123456789 X SET
Value_of_X: PRINT X PRINTLN

(Variables in the outer enviroment are accessible.) DOC

(FOO () X) DEF
FOO PRINTLN

(Parameters shadow outer variables.) DOC

(FOO (X) X) DEF
88 FOO PRINTLN

(To prevent something from being evaluated, you can put
it in a list. This is like QUOTE in Lisp.
For example, a symbol bound to a function causes
the function to evaluate. This means you cannot access
the variable value (the list that defines the function).
To do this, use a list with the function symbol and
call VALUE.) DOC

(FOO () HELLO) DEF
FOO PRINTLN
(FOO) VALUE PRINTLN

(You can use FETCH to get data from a server.) DOC

(Guru meditation of the day:) JOIN PRINTLN
http://ancientmantras.com/mantra-of-the-day.php FETCH PRINTLN

CODE;

f_eval_string($code);
