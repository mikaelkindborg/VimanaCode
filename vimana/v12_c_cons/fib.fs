
: fib ( n1 -- n2 )
  dup 1 > if
	  dup 1- recurse swap 2 - recurse +
  then ;

: fib5times ( -- )
32 fib . cr
32 fib . cr
32 fib . cr
32 fib . cr
32 fib . cr ;

fib5times

.s

bye


\ : fib ( n1 -- n2 )
\   dup 2 < 0= if
\ \ drop 1
\ \  else
\ 	  dup 1- recurse swap 2 - recurse +
\   then ;

\ 32 fib . cr
\ 32 fib . cr
\ 32 fib . cr
\ 32 fib . cr
\ 32 fib . cr

\ .s


\ Note: This is incorrect ("n fib" produces the result for fib(n+1)),
\ but we do not change it to ensure that future timing results are
\ comparable to older timing results.

\ : fib ( n1 -- n2 )
\     dup 2 < if
\ 	drop 1
\     else
\ 	dup
\ 	1- recurse
\ 	swap 2 - recurse
\ 	+
\     then ;
\ 
\ : main 34 fib drop ;
