<?php

function countlines($file)
{
  return count(file($file));
}

$numlines = 
  countlines("base.h") +
  countlines("gurumeditation.h") +
  countlines("item.h") +
  countlines("list.h") +
  countlines("string.h") +
  countlines("list_dealloc.h") +
  countlines("print.h") +
  countlines("interp.h") +
  countlines("primfuns.h") +
  countlines("codeparser.h");
  
echo "BASE VERSION: ".$numlines." lines\n";

$numlines += 
  countlines("symboldict.h") +
  countlines("symboldict_primfuns.h") +
  countlines("sourceparser.h");

echo "FULL VERSION: ".$numlines." lines\n";

/***

Version v8_minimal:
210903: 662 lines (first working prototype)
210904: 821 lines (parsning and printing)
210904: 894 lines (added strings, this is starting to look a bit complex...)
210905: 885 lines (some cleanup)
210910 (including generated files):
  BASE VERSION: 1059 lines (without source code parser)
  FULL VERSION: 1291 lines (with source code parser)
210918:
  BASE VERSION: 1244 lines
  FULL VERSION: 1522 lines

Version v7_c:
210426: 484 lines 
210429: 775 lines
210430: 837 lines
210505: 1193 lines
210506: 1321 lines
210508: 1572 lines
210514: 1773 lines
210520: 1581 lines
210521: 1641 lines
210706: 1993 lines (more primitives)
210802: 2413 lines (GC and more primitives)
210810: 2548 lines (String object and more primitives)

***/
