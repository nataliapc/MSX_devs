# TSXphpclass

This folder contains:
  * **tsx.php**: PHP class to read/write/manage _TSX_ and _TZX_ files.
  * **cas.php**: PHP class to read/write/manage _MSX CAS_ files.
  * **uef.php**: PHP class to read/write/manage _UEF_ files.
  * **cas2tsx.php**: PHP program to convert _MSX CAS_ files to _TSX_ format.
  * **svi2tsx.php**: PHP program to convert _SVI CAS_ files to _TSX_ format.
  * **drg2tsx.php**: PHP program to convert _DRAGON/CoCo CAS_ files to _TSX_ format.
  * **uef2tsx.php**: PHP program to convert _UEF_ files to _TSX_ format (Experimental).


## Tools syntax:
```
$> php cas2tsx.php <filename.CAS> <filename.TSX>

$> php svi2tsx.php <filename.CAS> <filename.TSX>

$> php drg2tsx.php <filename.CAS> <filename.TSX>

$> php uef2tsx.php <filename.UEF> <filename.TSX>
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

## Donations

If you've found useful this code please consider a donation:

![Donation address](http://www.eslamejor.com/Bitcoin_Address_Donaciones.png)

bitcon:1DaY35yWGPRneTK9cUuyvgYcHM9GCGa4Ag
