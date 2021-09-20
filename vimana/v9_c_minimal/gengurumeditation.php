<?php /* Comment below is for the generated file gurumeditation_gen.h */ ?>
/*
GENERATED FILE.

File: gurumeditation.h
Author: Mikael Kindborg (mikael@kindborg.com)

Usage: php gengurumeditation.php > gurumeditation.h 
*/

<?php
$GuruMeditationCounter = 0;
$GuruMeditationTable = [];

GuruMeditation("GURU_MEDITATION", "Guru Meditation");

GuruMeditation("ITEM_NUMBER_TOO_LARGE", "ItemSetNumber: Number is too large"); 
GuruMeditation("ITEM_SYMBOL_TOO_LARGE", "ItemSetSymbol: Symbol id is too large"); 
GuruMeditation("ITEM_PRIMFUNID_TOO_LARGE", "ItemSetPrimFun: Primfun id is too large");
GuruMeditation("ITEM_NOT_NUMBER", "ItemNumber: Not a number");
GuruMeditation("ITEM_NOT_BOOL", "ItemBool: Not a boolean");
GuruMeditation("ITEM_NOT_SYMBOL", "ItemSymbol: Not a symbol!");
GuruMeditation("ITEM_NOT_PRIMFUN", "ItemPrimFun: Not a primfun!");
GuruMeditation("ITEM_NOT_POINTER", "ItemObj: Not a pointer!");
GuruMeditation("ITEM_NOT_STRING", "ItemString: Not a string!");

GuruMeditation("LISTGROW_OUT_OF_MEMORY", "ListGrow: Out of memory");
GuruMeditation("LISTGROW_VINDEXMAX_EXCEEDED", "ListGrow: VINDEXMAX exceeded");
GuruMeditation("LISTCHECKCAPACITY_LESS_THAN_ZERO", "ListCheckCapacity: Index < 0");
GuruMeditation("LISTCHECKCAPACITY_VINDEXMAX_EXCEEDED", "ListCheckCapacity: VINDEXMAX exceeded");
GuruMeditation("LISTGET_INDEX_OUT_OF_BOUNDS", "ListGet: Index out of bounds");
GuruMeditation("LISTPOP_CANNOT_POP_EMPTY_LIST", "ListPop: Cannot pop empty list");
GuruMeditation("LISTDROP_CANNOT_DROP_FROM_EMPTY_LIST", "ListDrop: Cannot drop from list of length < 0");
GuruMeditation("ITEMOBJASLIST_NOT_POINTER", "ItemObjAsList: Not a pointer!");

GuruMeditation("PARSESTRING_BUFFER_OVERFLOW", "ParseString: Buffer overflow");
GuruMeditation("PARSETOKEN_BUFFER_OVERFLOW", "ParseToken: Buffer overflow");

GuruMeditation("SYMBOL_UNKNOWN_TYPE", "Print: Unknown symbol type");

GenerateGuruMeditationSymbols();

GenerateGuruMeditationTable();

function GuruMeditation($name, $text)
{
  global $GuruMeditationCounter;
  global $GuruMeditationTable;

  array_push($GuruMeditationTable, ["name" => $name, "text" => $text, "id" => $GuruMeditationCounter]);

  ++ $GuruMeditationCounter;
}

function GenerateGuruMeditationSymbols()
{
  global $GuruMeditationTable;

  foreach ($GuruMeditationTable as $entry):
    echo "#define ".$entry["name"]." ".$entry["id"]."\n";
  endforeach;

  echo "\n";
}

function GenerateGuruMeditationTable()
{
  global $GuruMeditationTable;

  echo "char* GuruMeditationTable[] = { \n";

  foreach ($GuruMeditationTable as $index => $entry):
    if ($index > 0):
      echo ",";
    endif;
    echo "\"".$entry["text"]."\"\n";
  endforeach;

  echo "}; \n";
}
