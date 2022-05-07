
-- osascript fib3.applescript  35.27s user 0.12s system 99% cpu 35.436 total

on fib(n)
  if n < 1 then
    0
  else if n < 3 then
    1
  else
    fib(n - 2) + fib(n - 1)
  end if
end fib

fib(37)

-- display dialog "Hi World" with title "AppleScript"
-- copy "Hello World!" to stdout
