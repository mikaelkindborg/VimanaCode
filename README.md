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

- Make the language as minimalistic and consistent as possible
- Make the interpreter as simple and understandable as possible
- Make the source code for the interpreter as small as possible
- Make the language as fast as possible

These goals are sometimes in conflict with each other. Optimizing code, for example, can make it harder to understand.

Another goal is to make the language easy to understand and to use. This is however not the highest priority. Making it useful for practical purposes is also not of high priority.

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
    FACT DEF

    ((L N) =>
      N 0 EQ NOT (L DO  L N 1 - TIMESDO) IFTRUE)
    TIMESDO DEF 

    (20 FACT DROP) 10000000 TIMESDO

Lower-case would also be possible (note that symbols are case-sensitive):

    ((n) =>
      n 0 eq (1) (n 1 - fact n *) ifelse)
    fact def

    ((l n) =>
      n 0 eq not (l do  l n 1 - timesdo) iftrue)
    timesdo def 

    (20 fact drop) 10000000 timesdo

Forth-inspired version:

    (DUP 1 EQ (DROP 1) (DUP 1 - FACT *) IFELSE) 
    FACT DEF

    (DUP 0 EQ (DROP DROP) (SWAP DUP DO SWAP 1 - TIMESDO) IFELSE) 
    TIMESDO DEF

    (20 FACT DROP) 10000000 TIMESDO

The arrow symbol "=>" is also a function that binds one or more items on the stack to local variables.  It is like any other function, and is not a "reserved word" or part of any special syntax. It could be named anything. (In a sense it is like a reserved symbol, but you get the idea.)

Virtually everything happens at runtime. Very litte is done during parsing (only setting the basic types of objects). There is no compile step.

## Procedural vs Declarative Style

Program code can be interpreted declarative or procedurally. Some languages are intended to use a declarative style (for example constraint languages, and logic programming languages), others are more procedural. Many languages speak of "declarations", for example variable declarations.

Forth is very procedural in its style. You have to execute the code in your head in order to understand it.

Vimana is meant to be procedurally orinted, and there are no purley declarative statements in a program (such as variable declarations). Every primitive is a functions. There are no "notations" in the source code, so to speak.

Still, the code is meant to map visually to the data stack structure, which is not the case with Forth. 

Here is an example of a function DOUBLE, written in Forth-style:

    (DUP +) double DEF

Example of calling this function:

    21 double PRINT

Here is the same function, written in a more declarative style:

    ((x) => x x +) double DEF

In Vimana you can use both of the above styles.

The arrow function looks like it is part of the language syntax, but it is not. The parser knows nothing about it. Still, introducing the arrow function and the use of variables give the code a declarative touch to it.

The ":" function pops a value of the stack and binds it to a variable in the local environment. It can be used as an alternative to "=>", as in this example:

    (x : x x +) double DEF

An interesting observation with respect to a more declarative style can be made when we introduce more than one parameter. Here is an an example:

    ((a b) => a b -) mysub DEF

This function subtracts one number from the other:

    43 1 mysub PRINT

Here is the same function written using the ":" function:

    (b : a : a b -) mysub DEF

Here you can see that the parameters are in the reverse order, because the items are popped off the stack in that order. Stack contains:

    43 1

First 1 is popped off and bound to b. Then 43 is popped off and bound to a.

So "=>" reads more declarative since it reflects the stack order visually, but is a bit slower than ":".One might be tempted to rewrite the code in the parsing step to the slightly faster version. However, this would break the principle of coding directly in the abstract syntax tree.

The ":" could be named something else, for example "setlocal":

    (b setlocal
     a setlocal
     a b -) mysub DEF

Note how this feels more "procedural" because of the name of the function (and ":" feels more "declarative").

Just as a concluding example, here is the same function written in "Forth-style":

    (-) mysub DEF

It is truly wonderful how many aspects there are to this, and the beauty in the details of the various representations. Making your own interperer is like builing a railroad model. You can experiment with things and change small details to study what the effect is.

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

## Quoting

I did not want to introduce quoting, since that involves adding one more special character. You can however quote a symbol, for example a function symbol (preventing it from being executed), by enclosing it in a list:

    (PRINT)

Then you can pushed the value of the symbol onto the data stack (without invoking the function), by using VALUE:

    (PRINT) VALUE

You can also get the value of a value, as in this example:

    42 FOO SET
    FOO BAR SET
    BAR VALUE PRINT

(Not tested the above. VALUE is not yet in the C-version.)

There could be better ways to do this. Everything is an experiment.

## C Code Structure

As few assumptions as possible are coded into the interpreter in interp.h. Most of what defined the language is specified by primitives in primfuns.h. Many different styles are possible. Postfix operations are fundamental, this is not as easy to change, and in the end you might as well go with Lisp if you want prefix functions. (Postfix notation means that the function name is the last element in a function call.) 

Everything is just one file split up into modules in .h files.

## Code Size

Current code size (2021-05-21):

    PHP: 500 lines
    C:  1580 lines (fewer primitives than PHP)
    C:  1640 lines (with minimalistic GC)
    JS:  350 lines (fewer primitives than PHP)

## License

License: Creative Commons: Attribution-ShareAlike - CC BY-SA

(I view this as an art project and not a software project, and I enjoy being a rebel, thus my choise of license ;)
