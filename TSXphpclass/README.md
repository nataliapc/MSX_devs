# TSXphpclass

This folder contains:
  * **tsx.php**: PHP class to read/write/manage _TSX_ and _TZX_ files.
  * **cas.php**: PHP class to read/write/manage _CAS_ files.
  * **cas2tsx.php**: PHP program to convert _CAS_ files to _TSX_ format.


## Program **cas2tsx.php** syntax:
```
$> php cas2tsx.php <filename.CAS> <filename.TSX>
```

## Example **tsx.php** use:
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

By NataliaPC (@ishwin74).
