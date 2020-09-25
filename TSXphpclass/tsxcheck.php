<?php
	require_once "tsx.php";
	require_once "cas.php";

	echo "\n";

	if ($argc != 3) {
		showSyntax();
		die("##### ERROR: bad parameters number!!\n\n");
	}
	
	echo "Loading TSX file: $argv[1]\n";
	$tsx = new TSX($argv[1]);
	if ($tsx->getNumBlocks()==0) {
		die("\n##### ERROR: bad input TSX file!!\n\n");
	}

	echo "Loading CAS file: $argv[2]\n";
	$cas = new CAS($argv[2]);
	if ($cas->getNumBlocks()==0) {
		die("\n##### ERROR: bad input CAS file!!\n\n");
	}

	echo "\nComparing DATA blocks...\n\n".
		 "ID   TSX Block   CAS Block   CRC32\n".
		 "=== =========== =========== ===================================\n";

	$casIdx = 0;
loop:
	for ($i=0; $i < $tsx->getNumBlocks(); $i++) {
		$tb = $tsx->getBlock($i);
		if ($tb instanceof IBlockData) {
			$tcrc = $tb->getInfo()[BlockDataTSX::INFO_CRC32];

			$cb = $cas->getBlock($casIdx);
			$cdata = $cb->data();

			echo "#".dechex($tb->getId())."    ".($i+1)."\t   ".($casIdx+1)."\t    ";
			while (1) {
				$ccrc = hash("crc32b", $cdata);

				if ($tcrc==$ccrc) {
					echo "OK [crc:$tcrc](".$tb->len()." bytes)\n";
					break;
				} else {
					if (strlen($cdata) > $tb->len()) {
						echo "Triming last zeros in CAS... (".strlen($cdata)."->".$tb->len()." bytes)\n\t\t\t    ";
						$cdata = substr($cdata, 0, $tb->len());
					} else {
					echo "#### NO MATCH!!! [crc:$tcrc] [crc:$ccrc]\n";
						break;
					}
				}
			}

			$casIdx++;
		}
	}

	echo "\n";

	function showSyntax()
	{
		echo "TSXCHECK: Compare a TSX with a CAS block by block using data hash:\n\n".
			 "$> php tsxcheck.php <TSX_FILE> <CAS_FILE>\n\n";
	}

?>