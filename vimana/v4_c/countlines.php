<?php

//TODO: Count number of lines in code files
echo "HELLO WORLD\n";

function countlines($file)
{
  return count(file($file));
}

$numlines = 
  countlines("base.h") +
  countlines("list.h") +
  countlines("interp.h") +
  countlines("parser.h") +
  countlines("primfuns.h");
  
echo "NUMBER OF LINES: ".$numlines."\n";
