<?php

//
// File: workbench.php
// Date: 2021-04-17
// Author: Mikael Kinborg
// Email: mikael@kindborg.com
// Website: kindborg.com
// License: Creative Commons: Attribution-ShareAlike - CC BY-SA
//
// This file contains some code examples for Vimana,
// a highly experimental dynamic programming language.
// inspired by Lisp and also by Forth.
//
// Run the code in this file from the command line on
// your computer like this:
//
// php workbench.php
//
// The purpose of Vimana is to provide a small interpreter 
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

PRINTCALLSTACK

42 X SET

PRINTCALLSTACK

CODE;

f_eval_string($code);
