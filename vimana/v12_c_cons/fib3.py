#
# Python 2.7.16
# python fib3.py  9.09s user 0.07s system 99% cpu 9.253 total
#
# Python 3.9.5
# python3 fib3.py  9.08s user 0.05s system 99% cpu 9.190 total
#

def fib(n):
  if n < 2: return n
  return fib(n - 1) + fib(n - 2)

print(fib(37))
