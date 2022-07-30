class F
{
  static fib(n)
  {
    if (n < 2) return n
    return fib(n - 1) + fib(n - 2)
  }
}

System.print(F.fib(37))
