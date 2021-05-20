# Vimana

DIY interpreter for a dynamic language influenced by Lisp and Forth.

Current implementation exists in two versions, written in PHP and in C.
(There are three versions of the PHP implementation.)

The PHP version or JS version are by far the most friendly ones to get started with.

The JavaScript implementation is very basic, but it runs the factorial benchmark. I also began doing a simple coding workbench with Amiga-inspired colors.

## Hello World

    HELLO_WORLD PRINT

Or:

    (HELLO WORLD) PRINT

## PHP-version

See comments and code examples in file [vimana/v1_takeoff/examples.php](vimana/v1_takeoff/examples.php) for how to get started.

Also see header comment in [vimana/v1_takeoff/interpreter.php](vimana/v1_takeoff/interpreter.php).

Run on your local machine in a console window:

    php vimana/v1_takeoff/examples.php

Or go to the directory with the files and run from there:

    cd vimana/v1_takeoff/
    php examples.php

The file examples.php conytains the vimana code, as a HEREDOC string. You can easily create your own PHP-files with Vimana code and run them using php myfile.php.

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

## JS-version

A version of JavaScript is now implemented, download and open this file in a web browser:

    [vimana/v5_js/vimana.html](vimana/v5_js/vimana.html)

## Mantra Meditation

Working on a web app for mantra meditation where the server code is implemented in Vimana. See folder [vimana/v6_mantras](vimana/v6_mantras).

## Benchmarks

The following are the performance test results for the interpreter and native code in C, JS, PHP and Python. JavaScript is suprisingly fast.

I have used factorial(20) with 10,000,000 iterations for the benchmark test.

    C:            0.84s
    JS:           2.12s
    PHP:          6.14s
    Python:      21.38s
    Vimana PHP: 433.00s (optimized version)
    Vimana C:   254.00s (first version)
    Vimana C:    26.61s (optimized version v4_c)
    Vimana C:    17.79s (optimized version v7_c branch performance)
    Vimana C:    18.99s (optimized version v7_c generalised lookup)
    Vimana C:    13.80s (same benchmark using Forth stack operations)
    Vimana JS:  425.00s (first version)

My goal has been to be as fast as the Python benchmark, and the latest version is faster than Python.

The optimized environment lookup used in the first optimized C version was fast but not flexible. Now I am using a more conventional model that is a bit slower, but still performs well. When using Forth stack operations, no or few lookups are needed, so that performs even better.

Then there is the issue of GC, closures and so on. Still more to come!

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

    (N : L : N 0 EQ NOT (L DO L N 1 - TIMESDO) IFTRUE) 
    TIMESDO DEF

    (20 FACT DROP) 10000000 TIMESDO

The ":" function pops a value of the stack and binds it to a variable in the local environment. 

The arrow symbol "=>" is also a function that binds one or more items on the stack to local variables. It is a bit slower than ":", but is easier to read since the variable order matches the order on the stack.

Both ":" and "=>" are like any other functions. They are runtime operations, and are not part of any special syntax or "reserved words". They could be named anything. (In a sense they are like reserved words, but you get the idea.)

Note that virtually everything happens at runtime. Very litte is done during parsing (only setting the basic types of objects). There is no compile step.

As few assumptions as possible are coded into the interpreter in interp.h. Most of what defined the language is specified by primitives in primfuns.h. Many different styles are possible. Postfix operations are fundamental, this is not as easy to change, and in the end you might as well go with Lisp if you want prefix functions. (Postfix notation means that the function name is the last element in a function call.) 

Everything is just one file split up into modules in .h files.

Vimana is my personal experimental project. There are yet additional functionality to be implemented. Like garbage collection in the C-version.

Current code size (2021-05-20):

    PHP: 500 lines
    C:  1580 lines (fewer primitives than PHP)
    JS:  350 lines (fewer primitives than PHP)

## Playground

I created Vimana as an experiment, for the fun of it. I am on old Lisp programmer and in school I had a Hewlett & Packard calculator with Reverse Polish Notation. Sweet memories. This is a retro project.

[Watch introduction video](https://youtu.be/BE7UpUuumc4)

![Screenshot](screenshot.jpg)

## License

License: Creative Commons: Attribution-ShareAlike - CC BY-SA

(I view this as an art project and not a software project, and I enjoy being a rebel, thus my choise of license ;)
