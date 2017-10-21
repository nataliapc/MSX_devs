# TSXphpclass

PHP class to read/write/manage _TSX_ and _TZX_ files. By NataliaPC (@ishwin74).

```php
  include_once "tsx.php";
  
  $tsx = new TSX("MYGAME.TSX");
  
  $block = new Block35();
	$block->key("TSX.RIPPER");
	$block->text("makeTSX v0.8b");
	$tsx->insertBlock(0, $block);
  
  $info = $tsx->getInfo();
  
  $tsx->saveToFile("MYGAME.TSX");
```
