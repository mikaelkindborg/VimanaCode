# Vimana

DIY interpreter for a dynamic language influenced by Lisp and Forth.

Current implementation exists in two versions, written in PHP and in C.
(There are three versions of the PHP implementation.)

The PHP version is by far the most friendly to get started with.

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

The actual Vimana code is in vimana.c´, just as a C string. There are various snippets I have used for testing in the file. I have not yet added running Vimana code from a file.

## Playground

I created Vimana as an experiment, for the fun of it. I am on old Lisp programmer and in school I had a Hewlett & Packard calculator with Reverse Polish Notation. Sweet memories. This is a retro project.

[Watch introduction video](https://youtu.be/BE7UpUuumc4)

![Screenshot](screenshot.jpg)

## License

License: Creative Commons: Attribution-ShareAlike - CC BY-SA

(I view this as an art project and not a software project, and I enjoy being a rebel, thus my choise of license ;)
