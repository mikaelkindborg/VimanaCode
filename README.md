# Vimana

DIY interpreter for a dynamic language influenced by Lisp and Forth.

Current implementation exists in two versions, written in PHP and in C.
(There are three versions of the PHP implementation.)

The PHP version is by far the most friendly to get started with.

As the next step I might do a JavaScript implementation. 

UPDATE: First version of JavaScript implementation runs the factorial function. Also began doing a simple coding workbench with Amiga-inspired colors.

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

UPDATE 210514. New version in the works: vimana/v7_c

To run the C version, do as follows (compile with cc or gcc):

    cd vimana/v4_c
    cc vimana.c -o vimana
    ./vimana

The actual Vimana code is in vimana.c, as a C string. There are various snippets I have used for testing in the file. I have not yet added running Vimana code from a file.

Optimizations are enabled by defining the OPTIMIZE preprocessor symbol in base.h. Compiling with the -O3 flag gives an additional performance boost. 

Example of how to enable compiler optimizations:

    cc vimana.c -o vimana -O3

## JS-version

A version of JavaScript is now implemented, download and open this file in a web browser:

    [vimana/v5_js/vimana.html](vimana/v5_js/vimana.html)

## Mantra Meditation

Working on a web app for mantra meditation where the server code is implemented in Vimana. See folder [vimana/v6_mantras](vimana/v6_mantras).

## Benchmarks

The following are the performance test results for the interpreter and native code in C, PHP and Python.

I have used factorial(20) with 10,000,000 iterations for the benchmark test.

    C:            0.84s
    PHP:          6.14s
    Python:      21.38s
    Vimana PHP: 433.00s (optimized version)
    Vimana C:   254.00s (first version)
    Vimana C:    26.61s (optimized version v4_c)
    Vimana C:    17.79s (optimized version v7_c)
    Vimana JS:  425.00s (first version)

My goal is to be as fast as Python, and it is pretty close. Futher optimizations are possible. 

UPDATE 210514: Latest version is faster than the Python benchmark. The environment lookup is however not that flexible, will try a new model that will probably be slower. Then there is the issue of GC. However, using Forth-style parameter passing rather than named variables could provide a speedup. Still more to come!

This is the test program for the C-version:

    ((N) () (
      N 0 EQ (1) (N 1 - FACT N *) IFELSE))
      FUN FACT SET

    ((L N) () (
      N 0 EQ NOT (L DO  L N 1 - TIMESDO) IFTRUE))
      FUN TIMESDO SET 

    (20 FACT DROP) 10000000 TIMESDO

Lower-case is also possible (but note that the code is case-sensitive):

    ((n) () (
      n 0 eq (1) (n 1 - fact n *) ifelse))
      fun fact set

    ((l n) () (
      n 0 eq not (l do  l n 1 - timesdo) iftrue))
      fun timesdo set 

    (20 fact drop) 10000000 timesdo

Vimana is my personal experimental project. There are yet additional functionality to be implemented. Like garbage collection in the C-version.

Current code size (2021-05-08):

    PHP: around 500 lines
    C:  around 1500 lines
    JS:  around 350 lines (fewer primitives than PHP)

## Playground

I created Vimana as an experiment, for the fun of it. I am on old Lisp programmer and in school I had a Hewlett & Packard calculator with Reverse Polish Notation. Sweet memories. This is a retro project.

[Watch introduction video](https://youtu.be/BE7UpUuumc4)

![Screenshot](screenshot.jpg)

## License

License: Creative Commons: Attribution-ShareAlike - CC BY-SA

(I view this as an art project and not a software project, and I enjoy being a rebel, thus my choise of license ;)
