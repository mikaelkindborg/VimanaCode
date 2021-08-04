# ruby benchmarks/fib.rb  0.28s user 0.08s system 83% cpu 0.432 total
# ruby benchmarks/fib.rb  0.23s user 0.03s system 96% cpu 0.271 total

def fib(n)
  if n < 2 then
    n
  else
    fib(n - 1) + fib(n - 2)
  end
end

for i in 0...5
  puts fib(30)
end
