\
\ gforth 0.7.3
\
\ gforth fib3.fs  2.60s user 0.01s system 99% cpu 2.611 total
\

: fib ( n1 -- n2 )
  dup 1 > if
	  dup 1- recurse swap 2 - recurse +
  then ;

37 fib . cr

bye
