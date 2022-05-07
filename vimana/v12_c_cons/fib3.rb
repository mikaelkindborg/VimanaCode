#
# ruby 2.6.3p62 (2019-04-16 revision 67580) [universal.x86_64-darwin20]
#
# ruby fib3.rb  2.21s user 0.10s system 95% cpu 2.419 total
#

def fib(n)
  if n < 2 then
    n
  else
    fib(n - 1) + fib(n - 2)
  end
end

puts fib(37)

