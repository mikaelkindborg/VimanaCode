# python benchmarks/fib.py  0.47s user 0.09s system 87% cpu 0.639 total
# python benchmarks/fib.py  0.43s user 0.03s system 98% cpu 0.464 total

# python3 benchmarks/fib.py  0.57s user 0.03s system 64% cpu 0.933 total
# python3 benchmarks/fib.py  0.59s user 0.01s system 97% cpu 0.611 total

def fib(n):
  if n < 2: return n
  return fib(n - 1) + fib(n - 2)

for i in range(0, 5):
  print(fib(30))

