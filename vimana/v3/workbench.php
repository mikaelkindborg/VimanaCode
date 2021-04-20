<?php
//
// File: workbench.php
// Date: 2021-04-18
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
// php workbench.php
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
// php workbench.php
//
// PHP needs to be installed on your computer.
//

require('interpreter.php');
require('primitives.php');

$code = <<< CODE

(N FACT) ((N 0 EQ) DO (1) (N 1 - FACT N *) IFELSE) DEF
(N L LOOP) ((N 0 EQ NOT) DO (L DO N 1 - L LOOP) IFTRUE) DEF
100000 (20 FACT) LOOP
(24.47s user 0.04s system 99% cpu 24.533 total) DOC

CODE;

f_eval_string($code);

/*

Nicer formatting:

(N FACT) 
  ((N 0 EQ) DO 
    (1) 
    (N 1 - FACT N *) IFELSE) DEF

(N L LOOP) 
  ((N 0 EQ NOT) DO 
    (L DO N 1 - L LOOP) IFTRUE) DEF

100000 (20 FACT) LOOP

(24.47s user 0.04s system 99% cpu 24.533 total) DOC

42 X SET
X PRINTLN
(X PRINTLN) DO
(43 X SET X PRINTLN) DO
X PRINTLN
(44 Z SET Z PRINTLN) DO
Z PRINTLN

(N TAIL) 
  ((0 N EQ NOT) DO 
    (N PRINTLN 
     N 1 - TAIL) IFTRUE) DEF
1000 TAIL

(N FOO) (N PRINTLN N BAR) DEF
(N BAR) (N PRINTLN) DEF
42 FOO


(N TAIL) (N PRINTLN 1 SLEEP N 1000 + TAIL) DEF
1000 TAIL

(N FACT) ((N 0 EQ) EVAL (1) (N 1 - FACT N *) IFELSE) DEF
30000 FACT PRINTLN


(N FOO) (N PRINTLN N BAR) DEF
(N BAR) (N PRINTLN) DEF
42 FOO


HELLO PRINTLN 1 SLEEP WORLD PRINTLN 1 GOTO

1 (X) SET

(HELLO PRINTLN) (1 GOTO)


(N L LOOP) ((N 0 EQ NOT) EVAL (L EVAL N 1 - LOOP) IFTRUE) DEF
300 (HELLO PRINTLN) LOOP


(L FOO) (L EVAL) DEF
(HELLO PRINTLN) EVAL


(N FACT) ((N 0 EQ) EVAL (1) (N 1 - FACT N *) IFELSE) DEF
300 FACT PRINTLN

(N FACT) ((N 1 EQ) EVAL (1) (N 1 - FACT) IFELSE) DEF
2 FACT 

3 INCR
(A 1 +)

(N FOO) ((N 1 EQ) EVAL PRINTSTACK (N VALUE) IFTRUE) DEF
1 FOO PRINTSTACK

T (HELLO PRINTLN) (WORLD PRINTLN) IFELSE


(
1 2 + PRINTLN
((HELLO WORLD) JOIN PRINTLN) EVAL

(X FOO) (X 1 +) DEF
PRINTENV
2 FOO PRINTLN

(N FACT) ((N 0 EQ) EVAL (1) (N 1 - FACT) IFELSE) DEF
3 FACT PRINTLN
)
DOC
*/