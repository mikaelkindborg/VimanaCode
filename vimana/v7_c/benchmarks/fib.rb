# ruby fib.rb  1.03s user 0.04s system 97% cpu 1.096 total

def fib(n)
  if n < 2 then
    n
  else
    fib(n - 1) + fib(n - 2)
  end
end

for i in 0...5
  puts fib(32)
end
