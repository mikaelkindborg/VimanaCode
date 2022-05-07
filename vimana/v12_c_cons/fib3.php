<?php
/*
PHP 8.1.5 (cli) (built: Apr 16 2022 00:24:39) (NTS)
Copyright (c) The PHP Group
Zend Engine v4.1.5, Copyright (c) Zend Technologies
    with Zend OPcache v8.1.5, Copyright (c), by Zend Technologies

php fib3.php  2.00s user 0.03s system 99% cpu 2.042 total
*/

function fib($n)
{
  if ($n < 2) return $n;
  return fib($n - 1) + fib($n - 2);
}

echo fib(37) . "\n";
