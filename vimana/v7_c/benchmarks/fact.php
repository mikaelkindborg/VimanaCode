<?php

/***********************************************************

Python 10.000.000 iterations:
python benchmarks/fact.py  21.38s user 0.22s system 99% cpu 21.713 total

10.000.000 iterations:
php fact.php  6.14s user 0.03s system 99% cpu 6.218 total

10.000.000 iterations:
./fact  0.84s user 0.00s system 99% cpu 0.849 total

100.000 iterations:
php v1_takeoff/benchmark.php  4.33s user 0.02s system 99% cpu 4.353 total

Datum: 2021-04-25

Test av fact(20) 10.000.000 iterationer:
C: 0.84s
PHP: 6.14s
Interpretatorn: 433s = 7 minuter(4.33s x 100)

Har bitit i äpplet nu och satt igång med en C-implementation av interpretatorn. Det första jag har gjort är att göra en monitor som kompilerar och kör programmet när jag sparar det. Som reload :) Det tog inte lång stund förrän jag blev tokig på att kompilera hela tiden. Jag kör cc direkt, har inget IDE. Använder Geany som editor, eller Visual Code.

C är så klart ruskigt snabbt, men tycker PHP står sig bra. Det är en faktor 7 ungefär. Min interpretator är en faktor 500 jämfört med C och en faktor 70 med PHP. Ja jösses.

************************************************************/

function fact($n)
{
  if ($n === 0):
    return 1;
  else:
    return fact($n - 1) * $n;
  endif;
}

function test_fact()
{
  for ($i = 0; $i < 10000000; $i++):
    $res = fact(20);
  endfor;
  return $res;
}

function fact_iter($n)
{
  $res = 1;
  while ($n > 0):
    $res = $res * $n;
    $n = $n - 1;
  endwhile;
  return $res;
}

function test_fact_iter()
{
  for ($i = 0; $i < 10000000; $i++):
    $res = fact_iter(20);
  endfor;
  return $res;
}

// .php fact.php  6.34s user 0.16s system 99% cpu 6.552 total
print(test_fact()."\n");

// php fact.php  4.12s user 0.02s system 99% cpu 4.141 total
//print(test_fact_iter()."\n");
