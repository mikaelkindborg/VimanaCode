# Vimana

DIY interpreter for a dynamic language influenced by Lisp and Forth.

Current implementation exists in two versions, written in PHP and in C.
(There are three versions of the PHP implementation.)

The PHP version is by far the most friendly to get started with.

As the next step I may do a JavaScript implementation.

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

To run the C version, do as follows (compile with cc or gcc):

    cd vimana/v4_c
    cc vimana.c -o vimana
    ./vimana

The actual Vimana code is in vimana.c, as a C string. There are various snippets I have used for testing in the file. I have not yet added running Vimana code from a file.

Optimizations are enabled by defining the OPTIMIZE preprocessor symbol in base.h. Compiling with the -O3 (or -Ofast in cc) gives an additional performance boost. 

Example of how to enable compiler optimizations:

    cc vimana.c -o vimana -O3

## Benchmarks

The following are the performance test results for the interpreter and native code in C, PHP and Python.

I have used factorial(20) with 10.000.000 iterations for the benchmark test.

    C:            0.84s
    PHP:          6.14s
    Python:      21.38s
    Vimana PHP: 433.00s (optimized version)
    Vimana C:   254.00s (first version)
    Vimana C:    26.61s (optimized version)

My goal as to be as fast as Python, and it is pretty close. Futher optimizations are possible.

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

## Playground

I created Vimana as an experiment, for the fun of it. I am on old Lisp programmer and in school I had a Hewlett & Packard calculator with Reverse Polish Notation. Sweet memories. This is a retro project.

[Watch introduction video](https://youtu.be/BE7UpUuumc4)

![Screenshot](screenshot.jpg)

## License

License: Creative Commons: Attribution-ShareAlike - CC BY-SA

(I view this as an art project and not a software project, and I enjoy being a rebel, thus my choise of license ;)
