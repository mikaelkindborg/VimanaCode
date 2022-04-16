<?php
/*
miki@mikis-Air v7_c % time ./vimana fib.vimana     
832040
832040
832040
832040
832040
./vimana fib.vimana  1.36s user 0.02s system 96% cpu 1.430 total
miki@mikis-Air v7_c % time python3 benchmarks/fib.py
832040
832040
832040
832040
832040
python3 benchmarks/fib.py  2.37s user 0.08s system 95% cpu 2.578 total
miki@mikis-Air v7_c % time python benchmarks/fib.py 
832040
832040
832040
832040
832040
python benchmarks/fib.py  1.70s user 0.07s system 95% cpu 1.848 total
miki@mikis-Air v7_c % time ./vimana fibstack.vimana 
832040
832040
832040
832040
832040
./vimana fibstack.vimana  0.86s user 0.01s system 96% cpu 0.904 total
miki@mikis-Air v7_c % time ruby benchmarks/fib.rb   
832040
832040
832040
832040
832040
ruby benchmarks/fib.rb  0.66s user 0.05s system 96% cpu 0.741 total
miki@mikis-Air v7_c % time php benchmarks/fib.php  
832040
832040
832040
832040
832040
php benchmarks/fib.php  0.62s user 0.03s system 94% cpu 0.678 total

python3 benchmarks/fib.py  2.37s user 0.08s system 95% cpu 2.578 total
python benchmarks/fib.py  1.70s user 0.07s system 95% cpu 1.848 total
ruby benchmarks/fib.rb  0.66s user 0.05s system 96% cpu 0.741 total
php benchmarks/fib.php  0.62s user 0.03s system 94% cpu 0.678 total
./vimana fib.vimana  1.36s user 0.02s system 96% cpu 1.430 total
./vimana fibstack.vimana  0.86s user 0.01s system 96% cpu 0.904 total


NEW TESTS 210814
----------------
32 fib 5 times:
php benchmarks/fib.php  1.04s user 0.02s system 99% cpu 1.068 total
ruby benchmarks/fib.rb  1.09s user 0.06s system 97% cpu 1.180 total
python benchmarks/fib.py  2.75s user 0.07s system 97% cpu 2.899 total
python3 benchmarks/fib.py  3.87s user 0.03s system 84% cpu 4.632 total
./vimana fibstack.vimana  1.37s user 0.00s system 99% cpu 1.385 total

*/

// php fib.php  0.94s user 0.03s system 98% cpu 0.990 total

function fib($n)
{
  if ($n < 2) return $n;
  return fib($n - 1) + fib($n - 2);
}

for ($i = 0; $i < 5; $i++):
  echo fib(32) . "\n";
endfor;
