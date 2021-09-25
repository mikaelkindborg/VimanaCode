
#define MINIMAL
#define PLATFORM_ARDUINO
#include "vimana.h"

#define o(c) PrintChar(c);

VInterp* interp;
String input = "";

void setup() 
{
  pinMode(LED_BUILTIN, OUTPUT);    
  digitalWrite(LED_BUILTIN, LOW); 
  
  serial.begin(9600);
  delay(1000);

  WelcomeMessage();
  PrintNum(GetFreeMem()); PrintNewLine();
  CreateInterpreter();
  //TestInterpreter();
  //InterpFree(interp);
  //PrintNum(GetFreeMem()); PrintNewLine();
  //PrintSizeOfTypes();
}

void CreateInterpreter()
{
  interp = InterpCreate();
}

void EvalCode()
{
  char* buf = MemAlloc(input.length() + 1);
  input.toCharArray(buf, input.length() + 1);
  VList* code = ParseSymbolicCode(buf);
  PrintList(code);
  PrintNewLine();
  MemFree(buf);
  InterpEval(interp, code);
  ListGC(code);
  PrintNum(GetFreeMem()); PrintNewLine();
}

void loop() 
{
  // Check if incoming data is available.
  if (serial.available() > 0)
  {
    input += serial.readString();
  }
  
  // Check if done reading.
  if (input.length() > 0 && serial.available() < 1)
  {
    Print(input);
    EvalCode();
    input = "";
  }
  
  /*
  pinMode(LED_BUILTIN, OUTPUT);
  //InterpRun
  digitalWrite(LED_BUILTIN, HIGH); 
  delay(1000);                     
  digitalWrite(LED_BUILTIN, LOW); 
  delay(5000);
  */
}

void PrintSizeOfTypes()
{
  PrintStrNum("sizeof(VItem)", sizeof(VItem));
  PrintStrNum("sizeof(VList)", sizeof(VList));
  PrintStrNum("sizeof(VString)", sizeof(VString));
  PrintStrNum("sizeof(VContext)", sizeof(VContext));
  PrintStrNum("sizeof(VInterp)", sizeof(VInterp));
  PrintStrNum("sizeof(VObj)", sizeof(VObj));
  PrintStrNum("sizeof(void*)", sizeof(void*));
  PrintStrNum("sizeof(int)", sizeof(int));
  PrintStrNum("sizeof(long)", sizeof(long));
  PrintStrNum("sizeof(double)", sizeof(double));
  PrintStrNum("sizeof(float)", sizeof(float));
}

void WelcomeMessage()
{
  o('W')o('E')o('L')o('C')o('O')o('M')o('E')o(' ')
  o('T')o('O')o(' ')o('T')o('H')o('E')o(' ')
  o('W')o('O')o('N')o('D')o('E')o('R')o('F')o('U')o('L')o(' ')
  o('W')o('O')o('R')o('L')o('D')o(' ')o('O')o('F')o(' ')
  o('V')o('I')o('M')o('A')o('N')o('A')o('\n')
}

void TestInterpreter()
{
  //PrintLine("--- TestInterpreter ---");

  //VList* code = ParseSymbolicCode("'FOO HEJ HOPP' P0");
  VList* code = ParseSymbolicCode("(S0)(P17N2P8(P17N1P4S0P20N2P4S0P3)P13)P2(S1)(P17P10(P16P16)(P20P17P11P20N1P4S1)P14)P2(N18S0P0)N5S1");
  PrintList(code);
  PrintNewLine();

  PrintNum(GetFreeMem()); PrintNewLine();
  
  InterpEval(interp, code);
  
  PrintNum(GetFreeMem()); PrintNewLine();
}
