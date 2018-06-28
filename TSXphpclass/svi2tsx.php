<?php
	include_once "tsx.php";

	if ($argc<2) {
		echo "\nSyntax:\n  php svi2tsx.php <InputFile.CAS> [OutputFile.TSX]\n\n";
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

	for ($i=0; $i<=strlen($cas); $i++) {

		if ($i==strlen($cas) || isPilotTone($cas, $i)) {

			if ($pini>0) {
				//Create the new data block
				$b = new Block4B();
				//Define the KCS data block with SVI params
				$b->pause(550);
				$b->pilotLen(0);
				$b->pilotPulses(0);
				$b->bit0Len(1458);
				$b->bit1Len(729);
				$b->bitCfg(0b00100010);
				$b->byteCfg(0b00001001);
				$b->data(substr($cas, $pini, $i-$pini));
				//Insert the new data block
				$tsx->addBlock($b);

				echo "Found block [$pini-".($i-1)."] ".strlen($b->data())." bytes\n";
			}

			if ($i<strlen($cas)) {
				//Create the new pilot block
				$b = new Block4B();
				//Define the KCS pilot block with SVI params
				$b->pause(0);
				$b->pilotLen(0);
				$b->pilotPulses(0);
				$b->bit0Len(1458);
				$b->bit1Len(729);
				$b->bitCfg(0b00100010);
				$b->byteCfg(0b00000000);
				$b->data(str_repeat(chr(0x55), 199).chr(0x7F));
				//Insert the new pilot block
				$tsx->addBlock($b);

				echo "Found block Header [$i]\n";
			}

			$pini = $i+17;
		}
	}

	$tsx->saveToFile($argv[2]);


	function isPilotTone($cas, $pos) {
		for ($i = 0; $i<16; $i++) {
			if (ord($cas[$pos+$i])!=0x55) return FALSE;
		}
		if (ord($cas[$pos+$i])!=0x7F) return FALSE;
		return TRUE;
	}

?>