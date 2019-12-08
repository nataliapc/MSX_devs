<?php
	/*
		Tech info Sources:
			http://archive.worldofdragon.org/index.php?title=Tape%5CDisk_Preservation
			http://www.cs.unc.edu/~yakowenk/coco/text/tapeformat.html
	*/

	include_once "tsx.php";

	if ($argc<2) {
		echo "\nSyntax:\n  php drg2tsx.php <InputFile.CAS> [OutputFile.TSX]\n\n";
		exit;
	}
	if (!file_exists($argv[1])) {
		echo "\nInput file not found: '$argv[1]'\n\n";
		exit;
	}
	if ($argc==2) {
		$argv[2] = substr($argv[1],0,strlen($argv[1])-3);
		if (substr($argv[2],-1)!=='.') {
			echo "\nBad input file extension? Must be *.cas!\n\n";
			exit;
		}
		$argv[2] .= "tsx";
	}

	$cas = file_get_contents($argv[1]);
	$tsx = new TSX();

	$pini = 0;
	$i = 0;
	$gaps = FALSE;
	while ($i < strlen($cas) - 5) {

		//============================================================= leader block
		$pini = $i;
		while ($i<strlen($cas)-1 && ord($cas[$i]) == 0x55 && ord($cas[$i+1]) != 0x3C) $i++;
		if ($i-$pini > 1) {
			//Create the new pilot block
			$b = getNewBlock();
			$b->data(str_repeat(chr(0x55), $i-$pini));
			//Insert the new pilot block
			$tsx->addBlock($b);

			echo "Found Leader block [pos: 0x".dechex($pini)."] size: ".($i-$pini)."\n";
		}
		if ($i > strlen($cas) - 5) continue;

		//============================================================= data block
		$pini = $i;
		$b = getNewBlock();
		$type = ord($cas[$i+2]);
		$len = ord($cas[$i+3]);
		$data = substr($cas, $pini, $len + 2 + 2 + 2);	//+2 magic +2 type/len +2 checksum/magic
		$b->data($data);
		switch ($type) {
			case 0x00:				//======== namefile block
					$b->pause(500);
					$gaps = ord($cas[$i+2+2+10]) == 0xFF;
					echo "Found Filename block [pos: 0x".dechex($pini)."] size: $len\n";
					break;
			case 0x01:				//======== data block
					if ($gaps) $b->pause(3);
					echo "Found Data block [pos: 0x".dechex($pini)."] size: $len\n";
					break;
			case 0xFF:				//======== end-of-file block
					$b->pause(1000);
					echo "Found OEF block [pos: 0x".dechex($pini)."] size: $len\n";
					break;
		}
		$i += $len + 2 + 2 + 1;
		$tsx->addBlock($b);
	}

	$tsx->saveToFile($argv[2]);


	function getNewBlock() {
		$b = new Block4B();
		//Define the KCS pilot block with Dragon schema
		$b->pause(0);
		$b->pilotLen(0);
		$b->pilotPulses(0);
		$b->bit0Len(1458);
		$b->bit1Len(729);
		$b->bitCfg(0b00100010);
		$b->byteCfg(0b00000000);
		return $b;	
	}

?>