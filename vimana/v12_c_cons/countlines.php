<?php

function countlines($file)
{
  return count(file($file));
}

$numlines = 
  countlines("base.h") +
  countlines("print.h") +
  countlines("alloc.h") +
  countlines("file.h") +
  countlines("string.h") +
  countlines("item.h") +
  countlines("mem.h") +
  countlines("array.h") +
  countlines("gurumeditation.h") +
  countlines("symbols2.h") +
  countlines("interp.h") + 
  countlines("primfuns.h") +
  countlines("primfuntable.h") +
  countlines("parser.h");

echo "BASE VERSION: ". ($numlines) . " lines\n";

/***

Version v12_c_cons:

220403:
  BASE VERSION: 412 lines (symbolic parser)
220408:
  BASE VERSION: 654 lines (work-in-progress in interpreter)
220409:
  BASE VERSION: 652 lines (work-in-progress, new parser, symbol lookup)
  BASE VERSION: 1092 lines
  BASE VERSION: 1132 lines (interpeter can add and print numbers)
220412:
  BASE VERSION: 1221 lines (global variables, print stack)
  BASE VERSION: 1240 lines (functions, tail calls)
220414:
  BASE VERSION: 1334 lines (very nice interpeter, more primitives)
  BASE VERSION: 1389 lines (with file functions)
220416:
  BASE VERSION: 1508 lines (many more primitives)
220417:
  BASE VERSION: 1604 lines (local variables/registers, /-- comments --/)


Version v11_c_minimal:

211003:
  BASE VERSION: 1766 lines (ref count gc)
  FULL VERSION: 2018 lines (ref count gc)
211004 (removed unused duplicated code):
  BASE VERSION: 1657 lines (ref count gc)
  FULL VERSION: 1931 lines (ref count gc)

Version v9_minimal:

210920 (it has grown bigger with generated code for typed list):
  BASE VERSION: 1467 lines
  FULL VERSION: 1759 lines
210921 (reference counting gc):
  BASE VERSION: 1594 lines
  FULL VERSION: 1887 lines
210925
  BASE VERSION: 1963 lines (ref count gc)
  FULL VERSION: 2257 lines (ref count gc)
  BASE VERSION: 2058 lines (mark sweep gc)
  FULL VERSION: 2352 lines (mark sweep gc)

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
