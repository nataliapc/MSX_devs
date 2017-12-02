<?php
	include_once "tsx.php";
	include_once "uef.php";

	if ($argc<3) {
		echo "\nSyntax:\n  php uef2tsx.php <filename.EUF> <filename.TSX>\n\n";
		exit;
	}
	if (!file_exists($argv[1])) {
		echo "\nFile not found: '$argv[1]'\n\n";
		exit;
	}

	$in = new UEF($argv[1]);
	$tsx = new TSX();

	for ($i=0; $i<$in->getNumBlocks(); $i++) {
		$bin = $in->getBlock($i);
		echo ($i+1).": Chunk &".(str_pad(dechex($bin->getId()), 4, '0', STR_PAD_LEFT))." ".strlen($bin->getData())." bytes"."\n";
		
		$btsx = NULL;
		switch ($bin->getId()) {
			case 0x0000:
				$btsx = new Block30();
				$btsx->text(rtrim($bin->getData()));
				break;
			case 0x0001:
				$btsx = new Block35();
				$btsx->key("GAME.MANUAL");
				$btsx->text(rtrim($bin->getData()));
				break;
			case 0x0005:
				$btsx = new Block35();
				$btsx->key("TARGET.MACHINE");
				$btsx->text($bin->getTargetMachine());
				$tsx->addBlock($btsx);
				//2nd block
				$btsx = new Block35();
				$btsx->key("TARGET.KEYBOARD");
				$btsx->text($bin->getTargetKeyboard());
				break;
			case 0x0100:
				$btsx = new Block4B();
				$btsx->pause(0);
				$btsx->pilotLen(0);
				$btsx->pilotPulses(0);
				$btsx->bit0len(1458);	//depends of current UEF baudRate
				$btsx->bit1len(729);	//depends of current UEF baudRate
				$btsx->bitCfg(0x24);
				$btsx->byteCfg(0x4C);
				$btsx->data($bin->getData());
				$blast = $tsx->getLastBlock();
				if ($blast->getId()==0x12) {
					$tsx->deleteBlock($tsx->getNumBlocks()-1);
					$btsx->pilotLen($blast->pulseLen());
					$btsx->pilotPulses($blast->pulseNum());
				}
				break;
			case 0x0110:
				$btsx = new Block12();
				$btsx->pulseLen(729);	//depends of current UEF baudRate
				$btsx->pulseNum($bin->cycles() * 2);
				break;
			case 0x0111:
				$btsx = new Block4B();
				$btsx->pause(0);
				$btsx->pilotLen(729);	//depends of current UEF baudRate
				$btsx->pilotPulses($bin->pilot1() * 2);
				$btsx->bit0len(1458);	//depends of current UEF baudRate
				$btsx->bit1len(729);	//depends of current UEF baudRate
				$btsx->bitCfg(0x24);
				$btsx->byteCfg(0x4C);
				$btsx->data("\xAA");
				$tsx->addBlock($btsx);
				//2nd block
				$btsx = new Block12();
				$btsx->pulseLen(729);	//depends of current UEF baudRate
				$btsx->pulseNum($bin->pilot2() * 2);
				break;
			case 0x0112:
				$btsx = new Block20();
				$btsx->pause($bin->pause());
				break;
			case 0x0113;
				$in->setBaudRate($bin->frequency());
				break;
			case 0x0114:
				$btsx = new Block12();
				$btsx->pulseLen(729);	//depends of current UEF baudRate
				$pulses = $bin->cycles() * 2;
				$pulses += $bin->first()==ord('P') ? 1 : 2;
				$pulses += $bin->last()==ord('P') ? 1 : 2;
				$btsx->pulseNum($pulses);
				break;
			case 0x0116:
				$btsx = new Block20();
				$btsx->pause(intval($bin->pause()*1000));
				break;
			case 0x0120:
				$btsx = new Block30();
				$btsx->text(rtrim($bin->getData()));
				break;
		}
		if ($btsx!==NULL) {
			$tsx->addBlock($btsx);
		}
	}

	$tsx->saveToFile($argv[2]);

?>
