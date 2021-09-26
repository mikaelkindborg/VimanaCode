<?php

function ReplaceTemplateTags($listType, $itemType, $templateCode)
{
  $code = str_replace("ListType_", $listType, $templateCode);
  $code = str_replace("ItemType", $itemType, $code);
  return $code;
}

$templateCode = file_get_contents('list_template.h');

$code = ReplaceTemplateTags("ItemList_", "VItem", $templateCode);
file_put_contents("itemlist_gen.h", $code);

$code = ReplaceTemplateTags("String_", "char", $templateCode);
file_put_contents("string_gen.h", $code);
