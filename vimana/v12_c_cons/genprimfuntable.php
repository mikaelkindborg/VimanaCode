<?php
// Run this script to generate file "primfuntable.h"
// The generated is included in file "primfuns.h"
// The table is a lookup table for primfuns.

function ReadCode()
{
  return file_get_contents(__DIR__ . "/primfuns.h");
}

function SaveTable($table)
{
  file_put_contents(__DIR__ . "/primfuntable.h", $table);
}

function GenerateTable($code)
{
  $table = "";

  preg_match_all('/PrimFun_(.+)\(VInterp/', $code, $result);

  foreach ($result[1] as $name):
    $table .= '  { "' . $name . '", PrimFun_' . $name .  " },\n";
  endforeach;
  $table .= '  { "__sentinel__", NULL }' . "\n";

  return $table;
}

SaveTable(GenerateTable(ReadCode()));
