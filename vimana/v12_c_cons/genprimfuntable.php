<?php
// Run this script to generate file "primfuntable.h"
// The generated is included in file "primfuns.h"
// The table is a lookup table for primfuns.

function SubstituteName($name)
{
  $subst =
  [
    "plus"  => "+",
    "minus" => "-",
    "times" => "*",
    "div"   => "/",
    "1plus" => "1+",
    "1minus" => "1-",
    "2plus" => "2+",
    "2minus" => "2-",
    "lessthan" => "<",
    "greaterthan" => ">",
    /*
    "local_0_set" => "A!",
    "local_0_get" => "A",
    "local_1_set" => "B!",
    "local_1_get" => "B",
    */
  ];

  if (isset($subst[$name])):
    return $subst[$name];
  else:
    return $name;
  endif;
}

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

  preg_match_all('/void PrimFun_(.+)\(VInterp/', $code, $result);

  foreach ($result[1] as $name):
    $table .= '  { "' . SubstituteName($name) . '", PrimFun_' . $name .  " },\n";
  endforeach;
  $table .= '  { "__sentinel__", NULL }' . "\n";

  return $table;
}

SaveTable(GenerateTable(ReadCode()));
