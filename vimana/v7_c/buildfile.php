<?php
function echofile($file)
{
  readfile($file);
  echo "\n";
}

echofile("base.h");
echofile("item.h");
echofile("list.h");
echofile("print.h");
echofile("interp.h");
echofile("parser.h");
echofile("primfuns.h");

?>


