<?php
	require_once "tsx.php";
	require_once "wav.php";

	if ($argc<3) {
		echo "\nSyntax:\n  php tsx2wav.php <filename.TSX> <filename.WAV>\n\n";
		exit;
	}
	if (!file_exists($argv[1])) {
		die("\nFile not found: '$argv[1]'\n\n");
	}

	$tsx = new TSX($argv[1]);
	$wav = new WavFromTSX($argv[2]);

	//Add start silence of 250 millis
	echo "Adding start silence...\n";
	$wav->writeSilence(250);

	for ($i=0; $i<$tsx->getNumBlocks(); $i++) {
		$b = $tsx->getBlock($i);
		echo "Block ".($i+1)." ID#".dechex($b->getId()).": ";

		switch ($b->getId()) {
			case 0x10: dump10($wav, $b); break;
			case 0x11: dump11($wav, $b); break;
			case 0x12: dump12($wav, $b); break;
			case 0x13: dump13($wav, $b); break;
			case 0x15: dump15($wav, $b); break;
			case 0x20: dump20($wav, $b); break;
			case 0x4B: dump4B($wav, $b); break;
			default:
				echo "Skip, no data block\n";
				break;
		}
	}

	//Add end silence of 250 millis
	echo "Adding end silence...\n";
	$wav->writeSilence(250);

	$wav->close();

	exit;

	//===============================================================
	// Functions

	function checkID($b, $id) {
		if ($b->getId() !== $id) {
			throw new UnexpectedValueException("Block with ID#".dechex($b->getId())." is not of type $id\n");
		}
	}

	function dump10($wav, $b) {
		checkID($b, 0x10);
		echo "Dumping Standard Data block...\n";

		$wav->setPhase(0);

		for ($i=0; $i<3223; $i++) {
			$wav->writeTurboPilot();
		}
		$wav->writeTurboSync();

		$data = $b->data();
		$size = strlen($data);
		for ($i=0; $i<$size; $i++) {
			$wav->writeTurboByte(ord($data[$i]));
		}
		if ($b->pause()!=0) {
			$wav->writePulse(2000);
		}
		$wav->writeSilence($b->pause());
	}

	function dump11($wav, $b) {
		checkID($b, 0x11);
		echo "Dumping Turbo Speed Data block...\n";

		$wav->setPhase(0);

		for ($i=0; $i<$b->pilotPulses(); $i++) {
			$wav->writeTurboPilot($b->pilotLen());
		}
		$wav->writeTurboSync($b->syncLen1(), $b->syncLen2());

		$data = $b->data();
		$size = strlen($data);
		for ($i=0; $i<$size; $i++) {
			$wav->writeTurboByte(ord($data[$i]), 8, $b->zeroLen(), $b->oneLen());
		}
		if ($b->pause()!=0) {
			$wav->writePulse(2000);
		}
		$wav->writeSilence($b->pause());
	}

	function dump12($wav, $b) {
		checkID($b, 0x12);
		echo "Dumping Pure Tone block...\n";

		$pulse = $wav->tstates2bytes($b->pulseLen());
		for ($i=0; $i<$b->pulseNum(); $i++) {
			$wav->writePulse($pulse);
		}
	}

	function dump13($wav, $b) {
		checkID($b, 0x13);
		echo "Dumping Pulses Sequence block...\n";

		$data = $b->data();
		for ($i=0; $i<$b->pulseNum(); $i++) {
			$wav->writePulse($wav->tstates2bytes(ord($data[i])));
		}
	}

	function dump15($wav, $b) {
		checkID($b, 0x15);
		echo "Dumping Direct Recording block...\n";

		$data = $b->data();
		for ($i=0; $i<strlen($data)-1; $i++) {
			for ($j=7; $j>=0; $j--) {
				$wav->writeSample($b->sampleLen(), (ord($data[$i]) >> $j) & 1 ? 127 : -127);
			}
		}
		$lastValue = ord($data[$i]);
		$lastBits = max(1, $b->lastBits());
		$lastBits = min(8, $lastBits);
		for ($j = $lastBits-1; $j>=0; $j--) {
			$wav->writeSample($b->sampleLen(), ($lastValue >> $j) & 1 ? 127 : -127);
		}
		$wav->writeSilence($b->pause());
	}

	function dump20($wav, $b) {
		checkID($b, 0x20);
		echo "Dumping Pause block...\n";

		$wav->writeSilence($b->pause());
	}

	function dump4B($wav, $b) {
		checkID($b, 0x4b);
		echo "Dumping MSX/KCS block...\n";
		
		$wav->setPhase(1);
		$wav->setSettings4B($b);
		$wav->writePilot4B();

		$data = $b->data();
		$size = strlen($data);
		for ($i=0; $i<$size; $i++) {
			$wav->writeByte4B(ord($data[$i]));
		}
		$wav->writeSilence($b->pause());
	}

?>