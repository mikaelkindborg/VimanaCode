<?php
// Run this script to print code that adds primfuns
// in file "primfuns.h"

function ReadCode()
{
  return file_get_contents(__DIR__ . "/primfuns.h");
}

/*
function SaveTable($table)
{
  file_put_contents(__DIR__ . "/filexxx.h", $table);
}
*/

function GenerateTable($code)
{
  $table = "";

  $pos1 = 0;

  while (TRUE)
  {
    $pos1 = strpos($code, "void PrimFun_", $pos1);
    if (false === $pos1) break;
    
    $pos2 = strpos($code, "(VInterp* interp)", $pos1);
    if (false === $pos2) break;

    // Get C function name
    $start = $pos1 + 5;
    $funName = substr($code, $start, $pos2 - $start);

    // Get Vimana function name
    $start = $pos1 + 13;
    $vimanaName = substr($code, $start, $pos2 - $start);

    // Get special Vimana function name
    $end = strpos($code, "\n", $pos2);
    $rest = substr($code, $pos2, $end - $pos2);
    $start = strpos($rest, "//");
    if (false !== $start)
    {
      $vimanaName = trim(substr($rest, $start + 2));
    }

    //$table .= "  { \"{$vimanaName}\", {$funName} },\n";
    $table .= "PrimFunTableAdd(\"{$vimanaName}\", {$funName});\n";

    $pos1 = $pos2;
  }

  return $table;
}

//SaveTable(GenerateTable(ReadCode()));
echo GenerateTable(ReadCode());
echo PHP_EOL;
