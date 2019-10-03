<?php

// ============================================================================================
// UEF class
//
// http://electrem.emuunlim.com/UEFSpecs.htm
// http://beebwiki.mdfs.net/Acorn_cassette_format
//
// (2017.11.18) v1.0 First version
//

class UEF
{
	private $HEADER = "UEF File!";

	private $majorVer;
	private $minorVer;
	private $blocks = array();

	private $baudRate = 1200;


	public function __construct($filename = NULL)
	{
		$this->clear();
		if ($filename!==NULL) {
			$this->loadFromFile($filename);
		}
	}

	public function clear()
	{
		$this->version = 0;
		$this->blocks = array();
	}

	public function loadFromFile($filename)
	{
		if (($bytes=file_get_contents($filename))!==NULL) {
			$this->clear();
			$out = @gzdecode($bytes);
			if ($out!==FALSE) {
				$bytes = $out;
			}

			//Read Header
			list($magic, $this->minorVer, $this->majorVer) = array_values(unpack("A9magic/x/Cmin/Cmaj", $bytes));
			$bytes = substr($bytes, 12);
			if ($magic !== $this->HEADER) {
				return false;
			}

			//Read Blocks
			while (strlen($bytes)>0) {
				$b = NULL;
				list($id, $len) = array_values(unpack("vid/Vlen", $bytes));
				$bdata = substr($bytes, 0, $len+6);

				switch ($id) {
					//&00xx set - Content information
					case 0x0000: $b = new BlockUEF_0000($bytes); break;
					case 0x0001: $b = new BlockUEF_0001($bytes); break;
					case 0x0005: $b = new BlockUEF_0005($bytes); break;
					//&01xx set - Tape chunks
					case 0x0100: $b = new BlockUEF_0100($bytes); break;
					case 0x0110: $b = new BlockUEF_0110($bytes); break;
					case 0x0111: $b = new BlockUEF_0111($bytes); break;
					case 0x0112: $b = new BlockUEF_0112($bytes); break;
					case 0x0113: $b = new BlockUEF_0113($bytes); break;
					case 0x0114: $b = new BlockUEF_0114($bytes); break;
					case 0x0116: $b = new BlockUEF_0116($bytes); break;
					case 0x0120: $b = new BlockUEF_0120($bytes); break;
					//&02xx set - Disc chunks
					//&03xx set - ROM chunks
					//&04xx set - State snapshots
					//&FFxx set - reserved / non-emulator portable
					default:   echo "Block #".dechex($id)." UNKNOWN!!\n";
							   $bytes = substr($bytes, $len+6);
							   break;
				}
				if ($b!==NULL) {
					$this->blocks[] = $b;
					$bytes = substr($bytes, $b->getSize());
				}
			}
			return true;
		}
		return false;
	}

	public function saveToFile($filename)
	{
		file_put_contents($filename, $this->getBytes());
	}

	public function getBaudRate()
	{
		return $this->baudRate;
	}

	public function setBaudRate($value)
	{
		$this->baudRate = $value;
	}

	public function getBytes()
	{
		$bytes = $this->HEADER;
		foreach ($this->blocks as $b) {
			$bytes .= $this->HEADER . $b->getBytes();
		}
		return $bytes;
	}

	public function getNumBlocks()
	{
		return count($this->blocks);
	}

	public function getBlock($pos)
	{
		return $this->blocks[$pos];
	}

	public function getLastBlock()
	{
		return end($this->blocks);
	}

	public function addBlock($newBlock)
	{
		$this->blocks[] = $newBlock;
	}

	public function insertBlock($index, $newBlock)
	{
		if ($index>count($this->blocks)) {
			$this->addBlock($newBlock);
		} else {
			array_splice($this->blocks, $index, 0, "");
			$this->blocks[$index] = $newBlock;
		}
	}

	public function deleteBlock($index)
	{
		array_splice($this->blocks, $index, 1);
	}

	public function getInfo()
	{
				$info = array();

		//File info
		$file = array();
		$tmp = $this->getBytes();
		$file['version'] = $this->majorVer.'.'.$this->minorVer;
		$file['fileSize'] = strlen($tmp);
		$file['blocks'] = $this->getNumBlocks();
		$file['crc32'] = hash("crc32b", $tmp);
		$file['md5'] = md5($tmp);
		$file['sha1'] = sha1($tmp);
		$info['file'] = $file;

		//Blocks
		$blocks = array();
		$pos = 0;
		foreach ($this->blocks as $b) {
			$tmp = $b->getInfo();
			if ($tmp!="") $blocks[$pos] = $tmp;
			$pos++;
		}
		$info['blocks'] = $blocks;

		return $info;
	}
}

// ============================================================================================
// Generic Abstract Block class
//
abstract class BlockUEF
{
	protected $id;
	protected $len;
	protected $pattern = "";
	protected $headSize = 0;
	protected $bytes;

	public function __construct() {}

	public function getId()
	{
		return $this->id;
	}

	public function getSize()
	{
		return strlen($this->bytes);
	}

	public function getHeadSize()
	{
		return $this->headSize;
	}

	public function getBytes()
	{
		return $this->bytes;
	}

	public function getData()
	{
		return substr($this->bytes, $this->headSize);
	}

	public function getInfo()
	{
		$id = strtoupper(str_pad(dechex($this->getId()), 4, '0', STR_PAD_LEFT));
		return array(
			'id' => $id,
			'name' => 'Block #'.$id
		);
	}

	public function len() {
		return $this->len;
	}

	abstract protected function unpack($bytes = NULL);

	abstract protected function pack();

	public static function bytes2float($number) {
		$number = str_split($number);
	    $binfinal = sprintf("%08b%08b%08b%08b", ord($number[3]), ord($number[2]), ord($number[1]), ord($number[0]));
	    $sign = substr($binfinal, 0, 1);
	    $exp = substr($binfinal, 1, 8);
	    $mantissa = "1".substr($binfinal, 9);
	    $mantissa = str_split($mantissa);
	    $exp = bindec($exp)-127;
	    $significand=0;
	    for ($i = 0; $i < 24; $i++) {
	        $significand += (1 / pow(2,$i))*$mantissa[$i];
	    }
	    return $significand * pow(2,$exp) * ($sign*-2+1);
	}

	public static function float2bytes($num) {
		if ($num == 0) return "\0\0\0\0";
		// set sign bit, and add another, higher one, which will be stripped later 
		$sign = $num < 0 ? 0x300 : 0x200;
		$significant = abs($num);
		$exponent = floor(log($significant, 2));
		// get 24 most significant binary bits before the comma: 
		$significant = round($significant / pow(2, $exponent-23));
		// exponent has exponent-bias format:
		$exponent += 127;
		// format: 1 sign bit + 8 exponent bits + 23 significant bits,
		//         without left-most "1" of significant
		$bin = substr(decbin($sign + $exponent), 1) . 
		substr(decbin($significant), 1);
		// assert that result has correct number of bits:
		if (strlen($bin) !== 32) {
			return "unexpected error";
		}
		// convert binary representation to hex, with exactly 8 digits
		$hex = str_pad(dechex(bindec($bin)), 8, "0", STR_PAD_LEFT);
		$ret =  chr(hexdec(substr($hex, 6, 2)));
		$ret .= chr(hexdec(substr($hex, 4, 2)));
		$ret .= chr(hexdec(substr($hex, 2, 2)));
		$ret .= chr(hexdec(substr($hex, 0, 2)));
		return $ret;
	}

}

// ============================================================================================
// Chunk &0000 - origin information chunk
//
class BlockUEF_0000 extends BlockUEF
{
	protected $pattern = "vid/Vlen";
	protected $pattern2 = "vV";
	protected $headSize = 6;
	
	public function __construct($bytes = NULL)
	{
		$this->id = 0x0000;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->len + 6);
		} else {
			$this->pack();
		}
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id, 
			$this->len) = array_values(unpack($this->pattern, $bytes));
	}

	protected function pack()
	{
		$this->bytes = pack($this->pattern2, 
			$this->id, 
			$this->len).$this->getData();
		return $this->bytes;
	}

	public function getInfo() {
		$data = $this->getData();
		$info = parent::getInfo();
		$info['description'] = 'Origin information chunk';
		$info['label'] = rtrim($data);
		return $info;
	}

	public function data($value=NULL) {
		if ($value===NULL) {
			return $this->getData();
		} else {
			$this->bytes = substr($this->bytes, 0, $this->headSize).substr($value, 0, 0xFFFF)."\0";
			$this->len = strlen($value) + 1;
			$this->pack();
		}
	}
}

// ============================================================================================
// Chunk &0001 - game instructions / manual or URL 
//
// In URL case the first five characters should be "URL: " ("URL:" followed by a single space) followed by the URL.
//
class BlockUEF_0001 extends BlockUEF_0000
{
	public function __construct($bytes = NULL)
	{
		$this->id = 0x0001;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->len + 6);
		} else {
			$this->pack();
		}
	}

	public function getInfo() {
		$data = $this->getData();
		$info = parent::getInfo();
		$info['description'] = 'Game instructions/manual/URL chunk';
		$info['label'] = rtrim($data);
		return $info;
	}
}

// ============================================================================================
// Chunk &0003 - inlay scan 
//

// ============================================================================================
// Chunk &0005 - target machine chunk
//
// This chunk is exactly 1 byte long. In that byte, the most significant nibble holds one of
// the following values:
//   0 - this file is aimed at a BBC Model A
//   1 - this file is aimed at an Electron
//   2 - this file is aimed at a BBC Model B
//   3 - this file is aimed at a BBC Master
//   4 - this file is aimed at an Atom
//
// The least significant nibble holds one of the following values:
//   0 - this file will work well with any keyboard layout, or a layout preference is not specified
//   1 - this file will work best if all keys are left in the same places relative to each other as
//       on the emulated machine (e.g. the IBM PC key physically above '/' produces ':' as per the
//       original hardware, even though it has a ' on it on UK keyboards)
//   2 - this file will work best with a keyboard mapped as per the emulating computer's (e.g. on a
//       UK keyboard pressing shift+0 on a keyboard will produce ')', rather than '@' as on a BBC
//       or Electron)
//
class BlockUEF_0005 extends BlockUEF
{
	protected $pattern = "vid/Vlen/Ctarget";
	protected $pattern2 = "vVC";
	protected $headSize = 7;
	
	protected $target = 0;

	public function __construct($bytes = NULL)
	{
		$this->id = 0x0005;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->len + 6);
		} else {
			$this->pack();
		}
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id, 
			$this->len,
			$this->target) = array_values(unpack($this->pattern, $bytes));
	}

	protected function pack()
	{
		$this->bytes = pack($this->pattern2, 
			$this->id, 
			$this->len,
			$this->target).$this->getData();
		return $this->bytes;
	}

	public function getInfo() {
		$data = $this->getData();
		$info = parent::getInfo();
		$info['description'] = 'Target machine chunk';
		$info['machine'] = $this->getTargetMachine();
		$info['keyboard'] = $this->getTargetKeyboard();
		return $info;
	}

	public function target($value=NULL) {
		if ($value===NULL) {
			return $this->target;
		} else {
			$this->target = $value;
			$this->pack();
		}
	}
	public function getTargetMachine() {
		switch ($this->target & 0xf0) {
			case 0x00: return "BBC Model A";
			case 0x10: return "Electron";
			case 0x20: return "BBC Model B";
			case 0x30: return "BBC Master";
			case 0x40: return "Atom";
		}
		return "Unknown";
	}
	public function getTargetKeyboard() {
		switch ($this->target & 0x0f) {
			case 0x00: return "No preference";
			case 0x01: return "Keyboard original";
			case 0x02: return "Keyboard mapped";
		}
		return "Unknown";
	}
}


// ============================================================================================
// Chunk &0006 - bit multiplexing information 
//

// ============================================================================================
// Chunk &0007 - extra palette 
//

// ============================================================================================
// Chunk &0008 - ROM hint 
//

// ============================================================================================
// Chunk &0009 - short title 
//

// ============================================================================================
// Chunk &000a - visible area
//

// ============================================================================================
// Chunk &0100 - implicit start/stop bit tape data block 
//
// Output for every byte:
// 	- Start bit (#0)
// 	- Byte LSB format
// 	- Stop bit (#1)
//
class BlockUEF_0100 extends BlockUEF
{
	protected $pattern = "vid/Vlen";
	protected $pattern2 = "vV";
	protected $headSize = 6;

	public function __construct($bytes = NULL)
	{
		$this->id = 0x0100;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->len + 6);
		} else {
			$this->pack();
		}
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id, 
			$this->len) = array_values(unpack($this->pattern, $bytes));
	}

	protected function pack()
	{
		$this->bytes = pack($this->pattern2, 
			$this->id, 
			$this->len).$this->getData();
		return $this->bytes;
	}

	public function getInfo() {
		$data = $this->getData();
		$info = parent::getInfo();
		$info['description'] = 'Implicit start/stop bit tape data block ';
		$info['bytesLength'] = strlen($data);
		$info['crc32'] = hash("crc32b", $data);
		$info['md5'] = md5($data);
		$info['sha1'] = sha1($data);
		return $info;
	}

	public function data($value=NULL) {
		if ($value===NULL) {
			return $this->getData();
		} else {
			$this->bytes = substr($this->bytes, 0, $this->headSize) . substr($value, 0xFFFFFFFF);
			$this->len = strlen($value) & 0xFFFFFFFF;
			$this->pack();
		}
	}
}

// ============================================================================================
// Chunk &0101 - multiplexed data block
//

// ============================================================================================
// Chunk &0102 - explicit tape data block 
//

// ============================================================================================
// Chunk &0104 - defined tape format data block 
//
// This chunk holds byte data with specified non-standard start/stop/parity bits.
/*class BlockUEF_0104 extends BlockUEF
{

}*/

// ============================================================================================
// Chunk &0110 - carrier tone (previously referred to as 'high tone') 
//
// A run of carrier tone (i.e. cycles with a frequency of twice the base frequency), with a
// running length described in cycles by the first two bytes.
//
class BlockUEF_0110 extends BlockUEF
{
	protected $pattern = "vid/Vlen/vcycles";
	protected $pattern2 = "vVv";
	protected $headSize = 8;
	
	protected $cycles;

	public function __construct($bytes = NULL)
	{
		$this->id = 0x0110;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->len + 6);
		} else {
			$this->pack();
		}
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id, 
			$this->len,
			$this->cycles) = array_values(unpack($this->pattern, $bytes));
	}

	protected function pack()
	{
		$this->bytes = pack($this->pattern2, 
			$this->id, 
			$this->len,
			$this->cycles).$this->getData();
		return $this->bytes;
	}

	public function getInfo() {
		$data = $this->getData();
		$info = parent::getInfo();
		$info['description'] = 'Carrier tone chunk';
		$info['cycles'] = $this->cycles;
		return $info;
	}

	public function cycles($value=NULL) {
		if ($value===NULL) {
			return $this->cycles;
		} else {
			$this->cycles = int_val($value) & 0xFFFF;
			$this->pack();
		}
	}
}


// ============================================================================================
// Chunk &0111 - carrier tone (previously 'high tone') with dummy byte 
//
// This four byte chunk is composed of two sets of two bytes - the first two describe the
// number of cycles in the tone before the dummy byte, and the second two describe the number
// of cycles in the tone after the dummy byte. The dummy byte always has value &AA.
//
class BlockUEF_0111 extends BlockUEF
{
	protected $pattern = "vid/Vlen/vpilot1/vpilot2";
	protected $pattern2 = "vVvv";
	protected $headSize = 0x0a;
	
	protected $pilot1 = 0;
	protected $pilot2 = 0;

	public function __construct($bytes = NULL)
	{
		$this->id = 0x0111;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->len + 6);
		} else {
			$this->pack();
		}
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id, 
			$this->len,
			$this->pilot1,
			$this->pilot2) = array_values(unpack($this->pattern, $bytes));
	}

	protected function pack()
	{
		$this->bytes = pack($this->pattern2, 
			$this->id, 
			$this->len, 
			$this->pilot1, 
			$this->pilot2).$this->getData();
		return $this->bytes;
	}

	public function getInfo() {
		$info = parent::getInfo();
		$info['description'] = 'Carrier tone with dummy byte';
		$info['pilot1'] = $this->pilot1;
		$info['pilot2'] = $this->pilot2;
		return $info;
	}

	public function pilot1($value=NULL) {
		if ($value===NULL) {
			return $this->pilot1;
		} else {
			$this->pilot1 = $value;
			$this->pack();
		}
	}
	public function pilot2($value=NULL) {
		if ($value===NULL) {
			return $this->pilot2;
		} else {
			$this->pilot2 = $value;
			$this->pack();
		}
	}
}

// ============================================================================================
// Chunk &0112 - integer gap
//
// A gap in the tape - a length of time for which no sound is on the source audio casette.
// This chunk holds a two byte rest length counted relative to the base frequency. A value 
// of n indicates a gap of 1/(2n*base frequency) seconds.
//
class BlockUEF_0112 extends BlockUEF
{
	protected $pattern = "vid/Vlen/vpause";
	protected $pattern2 = "vVv";
	protected $headSize = 0x8;
	
	protected $pause = 0;

	public function __construct($bytes = NULL)
	{
		$this->id = 0x0112;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->len + 6);
		} else {
			$this->pack();
		}
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id, 
			$this->len,
			$this->pause) = array_values(unpack($this->pattern, $bytes));
	}

	protected function pack()
	{
		$this->bytes = pack($this->pattern2, 
			$this->id, 
			$this->len, 
			$this->pause).$this->getData();
		return $this->bytes;
	}

	public function getInfo() {
		$info = parent::getInfo();
		$info['description'] = 'Integer gap';
		$info['pause'] = $this->pause;
		return $info;
	}

	public function pause($value=NULL) {
		if ($value===NULL) {
			return $this->pause;
		} else {
			$this->pause = $value;
			$this->pack();
		}
	}
}

// ============================================================================================
// Chunk &0116 - floating point gap 
//
// As per 0112, but the gap length is a floating point number measured in seconds.
//
class BlockUEF_0116 extends BlockUEF
{
	protected $pattern = "vid/Vlen/A4pause";
	protected $pattern2 = "vVA4";
	protected $headSize = 0x0A;

	protected $pause = "\x00\x00\x00\x00";

	public function __construct($bytes = NULL)
	{
		$this->id = 0x0116;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->len + 6);
		} else {
			$this->pack();
		}
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id, 
			$this->len,
			$this->pause) = array_values(unpack($this->pattern, $bytes));
	}

	protected function pack()
	{
		$this->bytes = pack($this->pattern2, 
			$this->id, 
			$this->len, 
			$this->pause).$this->getData();
		return $this->bytes;
	}

	public function getInfo() {
		$info = parent::getInfo();
		$info['description'] = 'Floating point gap';
		$info['pause'] = $this->bytes2float($this->pause);
		return $info;
	}

	public function pause($value=NULL) {
		if ($value===NULL) {
			return $this->bytes2float($this->pause);
		} else {
			$this->pause = $this->float2bytes($value);
			$this->pack();
		}
	}
}

// ============================================================================================
// Chunk &0113 - change of base frequency 
//
// The base frequency is a modal value, which is assumed to be 1200Hz when a UEF is open.
// If this chunk is encountered, the base frequency changes.
// This chunks contains a single floating point number, stating the new base frequency.
//
class BlockUEF_0113 extends BlockUEF
{
	protected $pattern = "vid/Vlen/A4freq";
	protected $pattern2 = "vVA4";
	protected $headSize = 0x0a;

	protected $freq = "\x00\x00\x96\x44";	//1200

	public function __construct($bytes = NULL)
	{
		$this->id = 0x0113;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->len + 6);
		} else {
			$this->pack();
		}
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id, 
			$this->len,
			$this->freq) = array_values(unpack($this->pattern, $bytes));
	}

	protected function pack()
	{
		$this->bytes = pack($this->pattern2, 
			$this->id, 
			$this->len, 
			$this->freq).$this->getData();
		return $this->bytes;
	}

	public function getInfo() {
		$info = parent::getInfo();
		$info['description'] = 'Floating point gap';
		$info['frequency'] = $this->bytes2float($this->freq);
		return $info;
	}

	public function frequency($value=NULL) {
		if ($value===NULL) {
			return $this->bytes2float($this->freq);
		} else {
			$this->freq = $this->float2bytes($value);
			$this->pack();
		}
	}
}

// ============================================================================================
// Chunk &0114 - security cycles 
//
// Security cycles are mainly found at the start of a run of carrier tone as an identification
// feature. Rarely they are at the end of a run of carrier tone. They consist of cycles of the
// base frequency and twice the base frequency and sometimes have a leading and/or trailing
// pulse.
//
class BlockUEF_0114 extends BlockUEF
{
	protected $pattern = "vid/Vlen/vcycles16/Ccycles8/Cfirst/Clast";
	protected $pattern2 = "vVvCCC";
	protected $headSize = 11;
	
	protected $cycles = 0;
	protected $first = 'W';
	protected $last = 'W';

	public function __construct($bytes = NULL)
	{
		$this->id = 0x0114;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->len + 6);
		} else {
			$this->pack();
		}
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id, 
			$this->len,
			$cycles16,
			$cycles8,
			$this->first,
			$this->last) = array_values(unpack($this->pattern, $bytes));
		$this->cycles = $cycles16 | ($cycles8 << 16);
	}

	protected function pack()
	{
		$cycles16 = $this->cycles & 0xFFFF;
		$cycles8 = $this->cycles >> 16;
		$this->bytes = pack($this->pattern2, 
			$this->id, 
			$this->len,
			$cycles16,
			$cycles8,
			$this->first,
			$this->last).$this->getData();
		return $this->bytes;
	}

	public function getInfo() {
		$data = $this->getData();
		$info = parent::getInfo();
		$info['description'] = 'Security cycles';
		$info['cycles'] = $this->cycles;
		$info['first'] = chr($this->first);
		$info['last'] = chr($this->last);
		return $info;
	}

	public function cycles($value=NULL) {
		if ($value===NULL) {
			return $this->cycles;
		} else {
			$this->cycles = intval($value) & 0xFFFFFF;
			$this->pack();
		}
	}
	public function first($value=NULL) {
		if ($value===NULL) {
			return $this->first;
		} else {
			$this->first = $value;
			$this->pack();
		}
	}
	public function last($value=NULL) {
		if ($value===NULL) {
			return $this->last;
		} else {
			$this->last = $value;
			$this->pack();
		}
	}
}

// ============================================================================================
// Chunk &0115 - phase change
//

// ============================================================================================
// Chunk &0117 - data encoding format change
//

// ============================================================================================
// Chunk &0120 - position marker
//
// This chunk contains a string offering a textual description of the significance of the
// location it sits at within the file purely for the benefit of human beings.
//
class BlockUEF_0120 extends BlockUEF_0000
{
	public function __construct($bytes = NULL)
	{
		$this->id = 0x0120;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->len + 6);
		} else {
			$this->pack();
		}
	}

	public function getInfo() {
		$data = $this->getData();
		$info = parent::getInfo();
		$info['description'] = 'Position marker chunk';
		$info['label'] = rtrim($data);
		return $info;
	}
}

// ============================================================================================
// Chunk &0130 - tape set info
//

// ============================================================================================
// Chunk &0131 - start of tape side
//

// ============================================================================================
// Chunk &0200 - disc info
//

// ============================================================================================
// Chunk &0201 - single implicit disc side
//

// ============================================================================================
// Chunk &0202 - multiplexed disc side
//

// ============================================================================================
// Chunk &0300 - standard machine rom
//

// ============================================================================================
// Chunk &0301 - multiplexed machine rom
//

// ============================================================================================
// Chunk &0400 - 6502 standard state
//

// ============================================================================================
// Chunk &0401 - Electron ULA state
//

// ============================================================================================
// Chunk &0402 - WD1770 state
//

// ============================================================================================
// Chunk &0403 - JIM paging register state
//

// ============================================================================================
// Chunk &0410 - standard memory data
//

// ============================================================================================
// Chunk &0411 - multiplexed memory data
//

// ============================================================================================
// Chunk &0412 - multiplexed (partial) 6502 state
//

// ============================================================================================
// Chunk &0420 - Slogger Master RAM Board State
//

?>
