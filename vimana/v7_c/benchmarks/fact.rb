# ruby fact.rb  6.38s user 0.03s system 99% cpu 6.425 total
# python fact.py  20.33s user 0.04s system 99% cpu 20.400 total
# python3 fact.py  31.47s user 0.06s system 97% cpu 32.201 total
# php fact.php  6.14s user 0.03s system 99% cpu 6.218 total
# C: ./fact  0.84s user 0.00s system 99% cpu 0.849 total

def fact(n)
  if n == 0 
    1
  else
    fact(n - 1) * n
  end
end

def fact_iter(n)
  res = 1
  while (n > 0) do
    res = res * n
    n = n - 1
  end
  res
end

# python fact.py  20.33s user 0.04s system 99% cpu 20.400 total
# python3 fact.py  31.47s user 0.06s system 97% cpu 32.201 total
# ruby fact.rb  6.38s user 0.03s system 99% cpu 6.425 total
def test_fact()
  i = 0
  while i < 10000000 do
    res = fact(20)
    i = i + 1
  end
  res
end

# python fact.py  10.27s user 0.03s system 99% cpu 10.317 total
# ruby fact.rb  4.64s user 0.03s system 99% cpu 4.684 total
def test_fact_iter()
  i = 0
  while i < 10000000 do
    res = fact_iter(20)
    i = i + 1
  end
  res
end

puts test_fact()
#puts test_fact_iter()

#puts fact(5000)

