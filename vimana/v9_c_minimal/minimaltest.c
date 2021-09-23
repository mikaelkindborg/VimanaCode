
//#define MINIMAL
#include "vimana.h"

void TestSymbolicParser()
{
  PrintLine("--- TestSymbolicParser ---");

  VList* code = ParseSymbolicCode("N8888881 P0 N33 N33 P3 P0 (S1 S2) P0 ('FOO HEJ HOPP') P0");
  PrintList(code);
  PrintNewLine();
  ListGC(code);
}

void TestInterpreter()
{
  PrintLine("--- TestInterpreter ---");

  VInterp* interp = InterpCreate();
  VList* code;
  
  code = ParseSymbolicCode("(S0)(P17N2P8(P17N1P4S0P20N2P4S0P3)P13)P2(S1)(P17P10(P16P16)(P20P17P11P20N1P4S1)P14)P2(N32S0P0)N5S1");
  PrintList(code);
  PrintNewLine();
  InterpEval(interp, code);

  code = ParseSymbolicCode("'FOO HEJ HOPP' P0");
  PrintList(code);
  PrintNewLine();
  InterpEval(interp, code);

  InterpFree(interp);
}

// MAIN --------------------------------------------------------

int main(int numargs, char* args[])
{
  //TestSymbolicParser();
  TestInterpreter();
  PrintMemStat();
}
