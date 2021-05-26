# Vimana

DIY interpreter for a dynamic language influenced by Lisp and Forth.

Current implementation exists in two versions, written in PHP and in C.
(There are three versions of the PHP implementation.)

The PHP version or JS version are by far the most friendly ones to get started with.

The JavaScript implementation is very basic, but it runs the factorial benchmark. I also started doing a simple coding workbench with Amiga-inspired colors.

## A Dynamic Language Playground 

Vimana is a personal project. I created Vimana as an experiment, for the fun of it. I am on old Lisp programmer and in school I had a Hewlett & Packard calculator with Reverse Polish Notation. Sweet memories. This is a retro project.

[Watch introduction video](https://youtu.be/BE7UpUuumc4)

![Screenshot](screenshot.jpg)

## Project Goals

The Vimana project has several goals:

- Create a language where you code directly in the abstract syntax tree (like Lisp)
- Make the language as minimalistic and consistent as possible
- Make the language as easy to implement as possible
- Make the interpreter as simple and understandable as possible
- Make the source code for the interpreter as small as possible
- Make the language as fast as possible

These goals are sometimes in conflict with each other. Optimizing code, for example, can make it harder to understand.

Another goal is to make the language easy to understand and to use. This is however not the highest priority. Making it useful for practical purposes is also not of high priority.

Also make sure to check out the document [design.md](design.md) for additional details on the design of the language.

## Hello World

Here is Hello World:

    HELLO_WORLD PRINT

Or:

    (HELLO WORLD) PRINT

## JS-version

A version of JavaScript is now implemented, download and open this file in a web browser:

    [vimana/v5_js/vimana.html](vimana/v5_js/vimana.html)

## PHP-version

See comments and code examples in file [vimana/v1_takeoff/examples.php](vimana/v1_takeoff/examples.php) for how to get started.

Also see header comment in [vimana/v1_takeoff/interpreter.php](vimana/v1_takeoff/interpreter.php).

Run on your local machine in a console window:

    php vimana/v1_takeoff/examples.php

Or go to the directory with the files and run from there:

    cd vimana/v1_takeoff/
    php examples.php

The file examples.php contains the vimana code, as a HEREDOC string. You can easily create your own PHP-files with Vimana code and run them using php myfile.php.

## Mantra Meditation

Working on a web app for mantra meditation where the server code is implemented in Vimana PHP. See folder [vimana/v6_mantras](vimana/v6_mantras).

## C-version

New version in the works: vimana/v7_c

To run the C version, do as follows (compile with cc or gcc):

    cd vimana/v7_c
    cc vimana.c -o vimana
    ./vimana

The actual Vimana code is in vimana.c, as a C string. There are various snippets I have used for testing in the file. I have not yet added running Vimana code from a file.

Optimizations are enabled by defining the OPTIMIZE preprocessor symbol in base.h. Compiling with the -O3 flag (or -Ofast) gives an additional performance boost. 

Example of how to enable compiler optimizations:

    cc vimana.c -o vimana -O3

## Benchmarks

The following are the performance test results for the interpreter and native code in C, JS, PHP and Python. JavaScript is suprisingly fast.

I have used factorial(20) with 10,000,000 iterations for the benchmark test.

    C:            0.84s (no compiler optimization)
    JS:           2.12s
    PHP:          6.14s
    Python:      21.38s
    Vimana PHP: 433.00s (optimized version)
    Vimana JS:  425.00s (first version)
    Vimana C:   254.00s (first version)
    Vimana C:    26.61s (optimized version v4_c)
    Vimana C:    17.79s (optimized version v7_c branch performance)
    Vimana C:    18.99s (optimized version v7_c generalised lookup)
    Vimana C:    19.60s (optimized version v7_c with ref counting GC)
    Vimana C:    13.17s (same benchmark using Forth-style operations)
    Vimana C:    13.72s (Forth-style with ref counting GC)

My goal has been to be as fast as the Python benchmark, and the latest version is faster than that.

The local environment lookup used in the first optimized C version was fast but not flexible. Now I am using a more conventional model that is a bit slower, but still performs well. When using Forth stack operations, no or few lookups are needed, so that performs better.

A first version of reference counting GC is now implemented. It is not fully complete with respect to nested and circular lists. Lists are the only type that can be created dynamically. Performance was not affected that badly by running with GC turned on. Note that no GC happens in the factorial bechmark, but there are still checks for reference counting that take some time.

## Code Examples and Coding Style

This is the benchmark program for the C-version:

    ((N) =>
      N 0 EQ (1) (N 1 - FACT N *) IFELSE)
    (FACT) DEF

    ((L N) =>
      N 0 EQ NOT (L EVAL  L N 1 - TIMESDO) IFTRUE)
    (TIMESDO) DEF 

    (20 FACT DROP) 10000000 TIMESDO

The symbol ":" means "quote" (it is a function just like "*", "-", and "EQ"). It is used to prevent the function symbol being evaluated (which is a problem in case you wish to redefine it, thus the convention of using ":" is used in function definitions).

Lower-case would also be possible (note that symbols are case-sensitive):

    ((n) =>
      n 0 eq (1) (n 1 - fact n *) ifelse)
    (fact) def

    ((l n) =>
      n 0 eq not (l eval  l n 1 - timesdo) iftrue)
    (timesdo) def 

    (20 fact drop) 10000000 timesdo

Forth-inspired version:

    (DUP 1 EQ (DROP 1) (DUP 1 - FACT *) IFELSE) 
    (FACT) DEF

    (DUP 0 EQ (DROP DROP) (SWAP DUP EVAL SWAP 1 - TIMESDO) IFELSE) 
    (TIMESDO) DEF

    (20 FACT DROP) 10000000 TIMESDO

The arrow symbol "=>" is also a function that binds one or more items on the stack to local variables.  It is like any other function, and is not a "reserved word" or part of any special syntax. It could be named anything. (In a sense it is like a reserved symbol, but you get the idea.) "QUOTE" is a synonym for ":".

Virtually everything happens at runtime. Very litte is done during parsing (only setting the basic types of objects). There is no compile step.

## Function Definition Syntax

I have tested a variety of different styles for function definitions. Below are some variations. Primitive "DEF" defined a global function. To follow the postfix evaluation order, it comes as the last element in the function definition (but it does not necessarily have to be like that).

Current style (the parens around the function name are needed to "quote" it):

    ((N) =>
      N 0 EQ (1) (N 1 - FACT N *) IFELSE)
    (FACT) DEF

Alternative style with symbol ":" for quoting the function name (this is a prefix operation):

    ((N) =>
      N 0 EQ (1) (N 1 - FACT N *) IFELSE)
    : FACT DEF

Function name comes first ("DEFINE" is longer than "DEF" so it stands out more when on its own at the end of the expression):

    (FACT) ((N) =>
      N 0 EQ (1) (N 1 - FACT N *) IFELSE) 
    DEFINE

Function name first and ":" used to quote the name (this looks a bit like the syntax in Forth):

    : FACT ((N) =>
      N 0 EQ (1) (N 1 - FACT N *) IFELSE)
    DEFINE

Function name part of the function list:

    (FACT (N) =>
      N 0 EQ (1) (N 1 - FACT N *) IFELSE)
    DEFINE

Here the parameter and function name are visually similar to the code for the actual function call:

    ((N FACT) =>
      N 0 EQ (1) (N 1 - FACT N *) IFELSE)
    DEFINE

And in this example, "FUNCTION" is a prefix function that also quotes the function name:

    FUNCTION FACT 
    ((N) =>
      N 0 EQ (1) (N 1 - FACT N *) IFELSE)

Personally, I am very hesitant to introduce prefix operations. That is why parens are used to quote symbols, and not ":".

Perhaps I will switch to the following style, and put the function name first to make it clearly visible. Note that DEFINE is still a postfix operation:

    (FUNNAME) (FUNBODY) DEFINE

This is the style used in the third example above. An alternative name is "DEFUN", which was used in MacLisp, so there is a nostalgic aspect to it.

Read more in [design.md](design.md) about the design of the language (will eventually move the above examples to that document).

An interesting observation with respect to postfix notation is that it is smimilar to the object-first syntax used in Smalltalk and many other object-oriented languages. Currently there is no "message dispatch" on the parameter object type(s), but this could be introduced.

So the postfix syntax may not be so strange after all, when you consider the message sensing syntax used in many languages.

## Language Grammar

Vimana has no grammar. Or rather, the grammar is really simple, and is best explained by giving some examples. 

The only reserved characters are left paren, right paren, and whitespace charaters. Line breaks are whitespace, and do not have any special meaning other than formatting to code to be more readable.

Everything in Vimana is a list of symbols. Symbols can be words or numbers or truth values. Lists can contain lists.

At the top level, parens are not used (as in the Hello World example above).

The syntax is very uniform. A program is just a stream of symbols separated by spaces. Lists are used to express "blocks" of code and to represent data. The beauty of postfix notation is that there little need to syntactically group expressions or nest functions calls. This is different from Lisp, which is a bit notorius for nested of lists that results in many parenthesis.

## Execution

Almost everything happens at runtime. The interpreter scans the program list and pushes elements onto a data stack. Elements are not evaluated in this step.

A function takes its parameters from the data stack. 

Primitive functions can decide to evaluate parameters, to find the value of a variable, for example. They can also operate directly on the "literal" values on the data stack.

User-defined functions evaluate parameters when binding values to local variables, but can also operate directly on the stack without evaluating symbols, by using "Forth-style" stack operations.

In Lisp, lists and symbols are evaluated by default. In Vimana, lists and symbols are not evaluated until a function does so. This enables the use of unbound symbols to display text. There is no string type in the language (yet). This is also a common style in Lisp (using quoted lists and symbols).

<!--
You can however quote a symbol, for example a function symbol (preventing it from being executed), by enclosing it in a list:

Then you can pushed the value of the symbol onto the data stack (without invoking the function), by using VALUE:

    (PRINT) VALUE

You can also get the value of a value, as in this example:

    42 FOO SET
    FOO BAR SET
    BAR VALUE PRINT

(Not tested the above. VALUE is not yet in the C-version.)

There could be better ways to do this. Everything is an experiment.
-->

## C Code Structure

Everything is just one file split up into modules in .h files. 

As few assumptions as possible are coded into the interpreter in interp.h. Most of what defined the language is specified by primitives in primfuns.h. Many different styles are possible. 

Postfix operations are fundamental to the language. This is not so easy to change, and in the end you might as well go with Lisp if you want prefix functions. (Postfix notation means that the function name is the last element in a function call.) 


## Code Size

Current code size (2021-05-21):

    PHP: 500 lines
    C:  1580 lines (fewer primitives than PHP)
    C:  1640 lines (with minimalistic GC)
    JS:  350 lines (fewer primitives than PHP)

## License

License: Creative Commons: Attribution-ShareAlike - CC BY-SA

(I view this as an art project and not a software project, and I enjoy being a rebel, thus my choise of license ;)
