<?php
	require_once "tsx.php";
	require_once "cas.php";

	if ($argc<3) {
		echo "\nSyntax:\n  php cas2tsx.php <filename.CAS> <filename.TSX>\n\n";
		exit;
	}
	if (!file_exists($argv[1])) {
		echo "\nFile not found: '$argv[1]'\n\n";
		exit;
	}

	$cas = new CAS($argv[1]);
	$tsx = new TSX();

	for ($i=0; $i<$cas->getNumBlocks(); $i++) {
		$bcas = $cas->getBlock($i);
		echo "Block ".($i+1).": ".strlen($bcas->data())." bytes"."\n";
		
		$btsx = new Block4B();
		if ($bcas->isASCIIHeader() || $bcas->isBinaryHeader() || $bcas->isBasicHeader()) {
			$btsx->pilotPulses(30720);
		} else {
			$btsx->pilotPulses(7680);
		}
		$btsx->pause(1000);
		$btsx->data($bcas->data());
		$tsx->addBlock($btsx);
	}

	$tsx->saveToFile($argv[2]);

?>