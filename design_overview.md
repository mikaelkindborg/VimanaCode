# Vimana Design Overview

This repository contains an interpeter for an experimental language named VimanaLang. I have created this project for my own purposes, simply for the fun of it. 

This is the beginning of a design document. The text is not polished, and is best considered as notes at this point.

## Background

The inspiration comes from Lisp and Forth. I am an old-time Lisp programmer who did computer science research using MacLisp, MuLisp, Interlisp-D, Smalltalk, and HyperCard in the 1980s. 

I love Lisp, and I miss the simplicity of MuLisp, MacLisp and systems like Apple Logo. VimanaLang is in the spirit of Lisp in the 1970s and 80s. Uppercase names!

The idea is that you can change everything you want yourself. VimanaLang is not for writing applications, it is for experimenting with programming language design.

I use PHP as the implementation language for this version, just because I can :) Possibly, I will do a C-implementation that can also run on embedded devides.

## Syntax

Basically there is "no syntax", the only syntax is parens and whitespace. The following is eqivalent:

    1 2 + PRINTLN

To this:

    1
    2
    +
    PRINTLN

The program consists of a list, just as in Lisp. But the top-level parens are not written out. Here is what it looks like internally:

    (
      1 2 + PRINTLN
    )

As you can see, function calls use Reverse Polish Notation. This is like Forth, but the opposite of Lisp.

## Evaluation

Nothing is evaluated until you call a function. A list is not evaluated until you say so. So you can use lists for both data and code, just as in Lisp.

If I would write this, nothing gets evaluated:

    (1 2 + PRINTLN)

Note that the above program actually is a list in a list. With the top-level parens it looks like this:

    (
      (1 2 + PRINTLN)
    )

I have to call a function to evaluate the code in a list, this is called DO in Vimana (EVAL in Lisp):

    (1 2 + PRINTLN) DO

## The Data Stack

As in Forth, a data stack is used. The data stack is used for passing function arguments and for return values from functions. 

To introduce the ideas of the data stack with an example, this program will not do anything except push the symbols onto the stack:

    A B C
    
A symbol that is not bound to a value evaluates to itself (it is a literal). You can print the stack with PRINTSTACK:

    A B C PRINTSTACK
    
When a primitive or a function is called, the stack is read and written. This function, for example, will pop three elements off the stack and push back the result:

    (A B C ADD) (A B C + +) DEF
    
When we call:

    1 2 3 ADD PRINTLN

The stack will be updated as follows:

    STACK: 1 2 3
    STACK: 1 5
    STACK: 6
    STACK: <IS EMPTY>
    
The plus function (+) pops two elements from the stack and pushes back the result. PRINTLN pops one item off the stack and don't push anything back.


## Parsing

The parser creates an abstract syntax tree that represents the program.
This tree is the actual code that gets interpreted.

## Interpreter

The basic operation of the interpreter is to traverse a list in the syntax tree and push the elements of the list onto the data stack. Elements are not evaluated.

When a function or primitive is found it is called. This typically results in elements being popped off and pushed onto the data stack.

A primitive can freely pop and push items off and onto the stack. By convention each primitive pushes (returns) one or zero items. Functions return one or zero items.

If you would write a program that does not call any functions or primitives, all elements of the program would be pused onto the stack. 

Here is an example of this:

    A B C (D E F) PRINTSTACK
    
When this program is run it will print a stack structure that looks like this:

    A
    B
    C
    (D E F)
    
The primitive PRINTSTACK will not be on the stack since it is evaluated instead of being pushed into the stack.

## Doing the Opposite

Vimana is in a sense the reverse of Lisp. Function calls use postfix notation instead of prefix. Lists are not evaluated by default, but are quoted by default.

But there are many similarities with Lisp. Code and data have the exact same representation. There is function application and recursion.

The postfix syntax is inspired by Forth. It is even more strictly followed than in Forth. And unlike Forth there are no predefined words. 

Conditionals, for example, are not in the interpreter, but defined as primitives, which are completely customizable.

Primitives do not evaluate their arguments, which means that one can do similar things as with Lisp macros. You can easily introduce new constructs with their own syntax.

The postfix notation is deeply rooted in the interpreter, and even though you can create "syntactic sugar" to make it look more "prefix", I have avoided this. It is unusual outside of the Forth language family to use Reverse Polish Notation, but it is an interesting experiment to take this to its full extent, so to speak.

## More to Come

I will write more here about the language design and and the implementation.

Remember that my purpose is to experiment with an interpreter for a minimal language. This is not a production language, it exists for the fun of it.

<!--

An application I have in mind is retro-style arcade games. I plan to do this using SDL as the graphics engine.


Breaking rules

Licensing

Design overview

Vimana programs are lists. 

Language Characteristics

As simple as possible but usable, both the language and the implementation.

Basically no syntax, only syntax is parens and whitespace.


v1_plain with references:

    (FACT (N) ((N 0 EQ) (1) (N 1 - FACT N *) IFELSE)) DEF
    100000 (20 FACT) DOTIMES
    
php v1_optim/workbench.php  5.78s user 0.02s system 99% cpu 5.804 total
php v1_optim/workbench.php  5.83s user 0.02s system 99% cpu 5.857 total



(php v1_tail/workbench.php  4.49s user 0.01s system 99% cpu 4.516 total) DOC

-->

