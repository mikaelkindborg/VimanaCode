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
echofile("primfuns.h");
echofile("parser.h");
echofile("vimana.h");
?>
