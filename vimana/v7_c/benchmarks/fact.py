
# python fact.py  20.33s user 0.04s system 99% cpu 20.400 total
# php fact.php  6.14s user 0.03s system 99% cpu 6.218 total
# C: ./fact  0.84s user 0.00s system 99% cpu 0.849 total

def fact(n):
  if n == 0:
    return 1
  else:
    return fact(n - 1) * n

def fact_iter(n):
  res = 1
  while (n > 0):
    res = res * n
    n = n - 1
  return res

# python fact.py  20.33s user 0.04s system 99% cpu 20.400 total
# python3 fact.py  31.47s user 0.06s system 97% cpu 32.201 total
def test_fact_1():
  i = 0
  while i < 10000000:
    res = fact(20)
    i = i + 1
  return res

def test_fact100():
  i = 0
  while i < 100000:
    res = fact(100)
    i = i + 1
  return res

# python fact.py  20.35s user 0.04s system 99% cpu 20.422 total
def test_fact_2():
  i = 0
  while i < 10000000:
    res = fact(20)
    i += 1
  return res

# python fact.py  20.65s user 0.16s system 99% cpu 20.891 total
def test_fact_3():
  for xi in range(0, 10000000):
    res = fact(20)
  return res

# Iterative version is much faster.
# python fact.py  10.27s user 0.03s system 99% cpu 10.317 total
def test_fact_iter():
  i = 0
  while i < 10000000:
    res = fact_iter(20)
    i = i + 1
  return res

print(test_fact_1())

#print(fact(500))

#print(test_fact100())
