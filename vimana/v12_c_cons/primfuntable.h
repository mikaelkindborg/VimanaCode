  { "sayHi", PrimFun_sayHi },
  { "print", PrimFun_print },
  { "printstack", PrimFun_printstack },
  { "eval", PrimFun_eval },
  { "call", PrimFun_call },
  { "evalInParentContext", PrimFun_evalInParentContext },
  { "iftrue", PrimFun_iftrue },
  { "iffalse", PrimFun_iffalse },
  { "ifelse", PrimFun_ifelse },
  { "setglobal", PrimFun_setglobal },
  { "funify", PrimFun_funify },
  { "readfile", PrimFun_readfile },
  { "parse", PrimFun_parse },
  { "+", PrimFun_plus },
  { "-", PrimFun_minus },
  { "*", PrimFun_times },
  { "/", PrimFun_div },
  { "1+", PrimFun_1plus },
  { "1-", PrimFun_1minus },
  { "2+", PrimFun_2plus },
  { "2-", PrimFun_2minus },
  { "<", PrimFun_lessthan },
  { ">", PrimFun_greaterthan },
  { "eq", PrimFun_eq },
  { "iszero", PrimFun_iszero },
  { "not", PrimFun_not },
  { "drop", PrimFun_drop },
  { "dup", PrimFun_dup },
  { "swap", PrimFun_swap },
  { "over", PrimFun_over },
  { "[A]", PrimFun_local_setA },
  { "[AB]", PrimFun_local_setAB },
  { "[ABC]", PrimFun_local_setABC },
  { "[ABCD]", PrimFun_local_setABCD },
  { "A", PrimFun_local_getA },
  { "B", PrimFun_local_getB },
  { "C", PrimFun_local_getC },
  { "D", PrimFun_local_getD },
  { "nil", PrimFun_nil },
  { "isnil", PrimFun_isnil },
  { "first", PrimFun_first },
  { "rest", PrimFun_rest },
  { "cons", PrimFun_cons },
  { "def", PrimFun_def },
  { "__sentinel__", NULL }
