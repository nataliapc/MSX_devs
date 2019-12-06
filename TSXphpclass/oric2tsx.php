<?php
	include_once "tsx.php";

	if ($argc<2) {
		echo "\nSyntax:\n  php oric2tsx.php <InputFile.TAP> [OutputFile.TSX]\n\n";
		exit;
	}
	if ($argc==2) {
		$argv[2] = substr($argv[1],0,strlen($argv[1])-3);
		if (substr($argv[2],-1)!=='.') {
			echo "\nBad input file extension? Must be *.tap!\n\n";
			exit;
		}
		$argv[2] .= "tsx";
	}

	//$tap = file_get_contents($argv[1]);

  $fp = fopen($argv[1], 'r');
	if (!$fp) {
    echo 'Input file not found.';
	}

	$data="";
	global $bin_data;
	$bin_data="";

	$tsx = new TSX();

    while (!feof($fp)) {
      if (ord(fgetc($fp))==0x16) {
       do {
          $valin=ord(fgetc($fp));
          if (feof($fp)) break;
        } while ($valin!=0x24);
				echo ("Oric sync found...\n");
        emit_sync();

     for ($i=0;$i<9;$i++)
		     {
            $header[$i]=ord(fgetc($fp));
						emit_byte($header[$i]);
			}
		 do {
        $i=ord(fgetc($fp));
				emit_byte($i);   /* name */
      } while (($i!=0)&&(!feof($fp)));

     emit_gap();


		 $size=($header[4]*256+$header[5])-($header[6]*256+$header[7])+1;

		 echo("Found Block....... Size: ".$size."\n");
     for ($i=0;$i<$size;$i++)
			  emit_byte(ord(fgetc($fp)));
				foreach (range(0, strlen($bin_data), 8) as $j)
	 	     {
	 	 			$byte="";
	 	 			$last_bits="";
	 	 			for ($bit=0;$bit <=7;$bit++)
	 	 			{
	 	 				if (isset($bin_data[$j+$bit]))
	 	 				{
	 	 				  $byte=$byte.$bin_data[$j+$bit];
	 	 					$last_bits=$last_bits."1";
	 	 				}
	 	 			  else {
	 	 			  	$byte=$byte."0";
	 	 					$last_bits=$last_bits."0";
	 	 				  }

	 	 			}
	 	 			$data=$data.chr(bindec($byte));
	 	 		}
				//Create the new data block
				$b = new Block14();
				//Define the 14  data block with Oric params
				$b->zeroLen(1270) ; //1270
				$b->oneLen(635) ; //635
				$b->lastBits(bindec($last_bits)) ;
				$b->pause(1000) ;
				$b->data($data);
				//Insert the new data block
				$tsx->addBlock($b);

				echo("-------------------------\n");

				$data="";
				$bin_data="";


   }
	}
	$tsx->saveToFile($argv[2]);
  fclose($fp);



function emit_level($bit)
{
	global $bin_data;
	$bin_data=$bin_data.$bit;
}


function emit_bit($bit)
{
        if ($bit) {
          emit_level(1);
        } else {
          emit_level(0);
        }

}



function emit_byte($in) {
  $out = 0;
  $parity=1;
	emit_bit(0);
  for ($i=0; $i<8; $i++,$in>>=1) {
       $parity=$parity+$in&1;
       emit_bit($in & 1);
       }
  emit_bit($parity);
	emit_bit(1);
	emit_bit(1);
	emit_bit(1);
	emit_bit(1);

	//emit_bit(1);
	//emit_bit(1);
}

function emit_sync()
{
for ($i=0;$i<256;$i++)
  emit_byte(0x16);

emit_byte(0x24);
}

function emit_gap()
{
  for ($i=0;$i<100;$i++)
	 emit_bit(1);
}

?>
