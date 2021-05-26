# Vimana Design Overview

This repository contains an interpeter for an experimental language named Vimana. I have created this project for my own purposes, simply for the fun of it. 

This document contains notes regarding the language design. README.md tends to be more recent, however.

## Background

The inspiration comes from Lisp and Forth. I am an old-time Lisp programmer who did computer science research using MacLisp, MuLisp, Interlisp-D, Smalltalk, and HyperCard in the 1980s. 

I love Lisp, and I miss the simplicity of MuLisp, MacLisp and systems like Apple Logo. Vimana is in the spirit of Lisp in the 1970s and 80s. Uppercase names!

The idea is that you can change everything you want yourself. Vimana is not for writing applications, it is for experimenting with programming language design.

I use PHP as the implementation language for this version, just because I can :) After the PHP implementation, I have done a versions in C and JavaScript. I find it very stimulating to do the implementation in multiple languages, and the design keeps changing because of this as new aspects are discovered.

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

I have to call a function to evaluate the code in a list, this is called EVAL (similar to EVAL in Lisp):

    (1 2 + PRINTLN) EVAL

## The Data Stack

As in Forth, a data stack is used. The data stack is used for passing function arguments and for return values from functions. 

To introduce the ideas of the data stack with an example, this program will not do anything except push the symbols onto the stack:

    A B C
    
A symbol that is not bound to a value evaluates to itself (it is a literal). You can print the stack with PRINTSTACK (currently only implemented in PHP, but easy to add):

    A B C PRINTSTACK
    
When a primitive or a function is called, the stack is read and written. This function, for example, will pop three elements off the stack and push back the result:

    ((A B C) => A B C + +) (MYADD) DEF
    
When we call:

    1 2 3 MYADD PRINTLN

The stack will be updated as follows:

    STACK: 1 2 3
    STACK: 1 5
    STACK: 6
    STACK: <IS EMPTY>
    
The plus function (+) pops two elements from the stack and pushes back the result. PRINTLN pops one item off the stack and don't push anything back.

## Parsing

The parser creates an abstract syntax tree that represents the program. This tree is the actual code that gets interpreted.

Coding directly in the syntax tree is a basic design choise for the language. It is a very fundamental principle.

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

## Functions

There are primitive functions written in the host language, and functions written in Vimana. Adding primitive functions are very straightforward, what you need to know is to how to pop and push objects on the data stack.

Vimana functions are defined using DEF. Here is an example of the factorial function:

    ((N) => N 0 EQ (1) (N 1 - FACT N *) IFELSE) (FACT) DEF

You can format the code as you want, for example like this:

    ((N) => 
      N 0 EQ 
        (1) 
        (N 1 - FACT N *) 
      IFELSE) 
    (FACT) DEF

The function "=>" is a synonym for "POP", and it binds elements on the data stack to local variables. It is not part of the parser syntax. It is just like any other function. 

IFELSE takes three items from the stack, a truth value, an if-true-branch and an else-branch.

## Procedural vs Declarative Style

Program code can be interpreted declarative or procedurally. Some languages are intended to use a declarative style (for example constraint languages, and logic programming languages), others are more procedural. Many languages speak of "declarations", for example variable declarations.

Forth is very procedural in its style. You have to execute the code in your head in order to understand it.

Vimana is meant to be procedurally orinted, and there are no purley declarative statements in a program (such as variable declarations). Every primitive is a functions. There are no "notations" in the source code, so to speak.

Still, the code is meant to map visually to the data stack structure, which is not the case with Forth. 

Here is an example of a function DOUBLE, written in Forth-style:

    (DUP +) (double) DEF

Example of calling this function:

    21 double PRINT

Here is the same function, written in a more declarative style:

    (x => x x +) (double) DEF

In Vimana you can use both of the above styles.

The function "=>" looks like it is part of the language syntax, but it is not. The parser knows nothing about it. Still, introducing the arrow function and the use of variables give the code a declarative touch to it.

This function pops a value of the stack and binds it to a variable in the local environment. As an alternative you can use the synonym "POP", as in this example:

    (x POP x x +) (double) DEF

An interesting observation with respect to a more declarative style can be made when we introduce more than one parameter. Here is an an example:

    ((a b) => a b -) (mysub) DEF

This function subtracts one number from the other:

    43 1 mysub PRINT

Here is the same function written using the "POP" function to bind local variables one at a time:

    (b POP a POP a b -) (mysub) DEF

Here you can see that the parameters are in the reverse order, because the items are popped off the stack in that order. Stack contains:

    43 1

First 1 is popped off and bound to b. Then 43 is popped off and bound to a.

As we can see, "=>" reads more declarative since it reflects the stack order visually.

<!--
"POP" is actually a synonym for "=>". It could be named something else, for example "SETLOCAL":

    (b SETLOCAL
     a SETLOCAL
     a b -) (mysub) DEF

Note how this feels more "procedural" because of the naming (and "=>" feels more "declarative").
-->

"POP" is actually a synonym for "=>". You can use POP with a symbol or a list of symbols, as in these examples:

    ((a b) POP a b -) (mysub) DEF
    (b POP a POP a b -) (mysub) DEF

The spirit of "=>" is to provide a nicer notation that looks similar to other languages, like arrow functions JavaScript. 

Note how "POP" feels more "procedural" because of the naming (and "=>" feels more "declarative").

Just as a concluding example, here is the same function written in "Forth-style":

    (-) (mysub) DEF

It is truly wonderful how many aspects there are to this, and the beauty in the details of the various representations. Making your own interperer is like builing a railroad model. You can experiment with things and change small details to study what the effect is.

## Quoting

Quoting is used when you want to prevent a function from being evaluated. One way to quote a symbol is by enclosing it in a list:

    (DOUBLE)

This will prevent the function DOUBLE from being evaluated. 

Quoting is useful when you want to redefine a function. First time you defined a function, its name is unbound, so there exists no function yet. That is why you can write:

    (DUP +) DOUBLE DEF

However, if you attempt to redefine the function, DOUBLE will get evaluated before DEF is called, which is not what we want. This example will therefore NOT work:

    (2 *) DOUBLE DEF <-- Will NOT work to redefine like this

You can use this pattern to redefine a function:

    (2 *) (DOUBLE) FIRST DEF

That will push the symbol "DOUBLE" onto the stack without evaluating it, before calling DEF.

As this is a bit cumbersome to write, I introduced new primitive "QUOTE". This is different from all other functions and primitives in that it comes before the symbol it operates on. It is the only prefix function in the entire language. 

QUOTE takes the next symbol from the list being evaluated and pushes it into the data stack.

Here is how QUOTE is used:

    (2 *) QUOTE DOUBLE DEF

By using the symbol ":" as a synonym for QUOTE, we get a nicer notation:

    (2 *) : DOUBLE DEF

I really did not want to introduce special symbols or prefix operations for quoting, but this is so much easier to write and read than using the "(DOUBLE) FIRST" notation.

However, shortly after implementing QUOTE, I changed my mind, and instead updated DEF to accept a list as a parameter. Then you use the list to quote the symbol (becuase whan is in a list is not evaluated by default).

Now the syntax for DEF looks like this:

    (2 *) (DOUBLE) DEF

Originally, I did not want to use the parens around the function name, because it is easy to forget them and it looks a bit weird, perhaps. Furthermore, you don't use parens wehn calling the function, so the ":" notation is more visually similar to the actual function call.

But, to stick with the principle of avoiding prefix operations, I changed my mind and now use parens to quote the name given to DEF.

## Data Types and Variables

Vimana is a highly dynamic language, but it is strongly typed. Don't confuse this with static typing. Variables in Vimana are dynamically typed, but values are strongly typed. this is similar to Smalltalk, and unlike for example JavaScript.

However, depending on the host language this can differ. You get loose typing with VimanaJS, for example.

The design philosopy is to integrate with the host language, and implementations are not entirely consistent. This means that Vimana may not be portable between implementations. Then the language also changes as the project progresses. It is an onging experiment.

## List Style

Lisp uses linked list with CONS-cells, and Vimana uses arrays for lists. However, Vimana could use CONS-cells to.

Here is an example of what the function LENGTH looks like in MacLisp:

    (DEFUN LENGTH (L)
      (COND ((NULL L) 0)
            (T (+ 1 (LENGTH (CDR L))))))

Same function in Vimana could look like this (not implemented):

    (LENGTH) ((L) => 
      L NULL (0) (L CDR LENGTH 1 +) IFELSE)
    DEFUN

These examples illustrate the characteristics of the two languages.

## Doing the Opposite

Vimana is in a sense the reverse of Lisp. Function calls use postfix notation instead of prefix. Lists are not evaluated by default, but are quoted by default.

I follow the postfix notation very strictly in the design of the language. I introduced a QUOTE function the be able to prevent functions from being evaluated, but now I am simply using parens to quote names.

There are several similarities with Lisp. Code and data have the exact same representation. There is function application and recursion.

The postfix syntax is inspired by Forth. It is even more strictly followed than in Forth. And unlike Forth there are no predefined words. There are primitives implemented in the host language, but that is another thing as I view it, they are not "predefined" in the sense of being part of the language syntax (the parser have no idea about them).

Conditionals, for example, are not in the interpreter, but defined as primitives, which are completely customizable.

Primitives do not evaluate their arguments, which means that one can do somehat similar things as with Lisp macros. You can easily introduce new constructs with their own syntax.

The postfix notation is deeply rooted in the interpreter, and even though you can create "syntactic sugar" to make it look more "prefix", I have avoided this. It is unusual outside of the Forth language family to use Reverse Polish Notation, but it is an interesting experiment to take this to its full extent.

## Having Fun With Your DIY Interpreter

Remember that the purpose with ythis project is to experiment with an interpreter for a minimal language. This is not a production language, it exists for the fun of it.

You can do similar projects yourself, by building on my code or some other small interpreter. There are several ones for Lisp, for example.

I am constantly amazed by the beuaty of the details in designing a language. It is a bit like planting seeds in a garden, and watching them grow. With time, you make adjustments and the plants now take on new shapes and colours, often ones you did not anticipate originally.

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

