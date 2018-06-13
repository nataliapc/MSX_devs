<?php

// ============================================================================================
// TSX (TZX 1.21 Implementation) class
//
// (2017.10.21) v1.0 First version
//
class TSX
{
	private $majorVer;
	private $minorVer;
	private $blocks = array();

	public function __construct($filename=NULL)
	{
		$this->clear();
		if ($filename!==NULL) {
			$this->loadFromFile($filename);
		}
	}

	public function clear()
	{
		$this->majorVer=1;
		$this->minorVer=21;
		$this->blocks = array();
	}

	public function loadFromFile($filename)
	{
		if (($bytes=file_get_contents($filename))!==NULL) {
			$this->clear();

			//Read Header
			list($magic, $this->majorVer, $this->minorVer) = array_values(unpack("A8magic/C1majorVer/C1minorVer", $bytes));
			$bytes = substr($bytes, 10);
			if ($magic !== "ZXTape!\x1a") {
				return false;
			}
			if ($this->majorVer!=1 || $this->minorVer!=21) return false;

			//Read Blocks
			while (strlen($bytes)>0) {
				$b = NULL;
				list($id) = array_values(unpack("C", $bytes));
				switch ($id) {
					case 0x10: $b = new Block10($bytes); break;
					case 0x11: $b = new Block11($bytes); break;
					case 0x12: $b = new Block12($bytes); break;
					case 0x13: $b = new Block13($bytes); break;
					//case 0x14: $b = new Block14($bytes); break;
					//case 0x15: $b = new Block15($bytes); break;
					//case 0x19: $b = new Block19($bytes); break;
					case 0x20: $b = new Block20($bytes); break;
					case 0x21: $b = new Block21($bytes); break;
					case 0x22: $b = new Block22($bytes); break;
					//case 0x23: $b = new Block23($bytes); break;
					//case 0x24: $b = new Block24($bytes); break;
					//case 0x25: $b = new Block25($bytes); break;
					//case 0x26: $b = new Block26($bytes); break;
					//case 0x27: $b = new Block27($bytes); break;
					//case 0x28: $b = new Block28($bytes); break;
					//case 0x2A: $b = new Block2A($bytes); break;
					case 0x30: $b = new Block30($bytes); break;
					case 0x31: $b = new Block31($bytes); break;
					case 0x32: $b = new Block32($bytes); break;
					//case 0x33: $b = new Block33($bytes); break;
					case 0x35: $b = new Block35($bytes); break;
					case 0x4B: $b = new Block4B($bytes); break;
					case 0x5A: $b = new Block5A($bytes); break;
					//NOT FULLY IMPLEMENTED
					//case 0x18: $b = new Block18($bytes); break;
					//case 0x2B: $b = new Block2B($bytes); break;
					//DEPRECATED
					//case 0x16: $b = new Block16($bytes); break;
					//case 0x17: $b = new Block17($bytes); break;
					//case 0x34: $b = new Block34($bytes); break;
					//case 0x40: $b = new Block40($bytes); break;
					default:   echo "Block #".dechex($id)." UNKNOWN!!\n"; exit;
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

	public function getBytes()
	{
		$bytes = "ZXTape!\x1a\x01\x15";
		foreach ($this->blocks as $b) {
			$bytes .= $b->getBytes();
		}
		return $bytes;
	}

	public function getNumBlocks()
	{
		return count($this->blocks);
	}

	public function getBlockId($pos)
	{
		return $this->blocks[$pos]->getId();
	}

	public function getBlock($pos)
	{
		return $this->blocks[$pos];
	}

	public function getLastBlock()
	{
		return end($this->blocks);
	}

	public function getBlockPosById($id, $offset = 0)
	{
		while ($offset < count($this->blocks)) {
			if ($this->blocks[$offset]->getId() == $id) {
				return $offset;
			}
			$offset++;
		}
		return -1;
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
		$file['fileSize'] = strlen($tmp);
		$file['blocks'] = $this->getNumBlocks();
		$file['crc32'] = hash("crc32b", $tmp);
		$file['md5'] = md5($tmp);
		$file['sha1'] = sha1($tmp);
		$info['file'] = $file;

		//Labels
		$pos = -1;
		$labels = array();
		while (($pos=$this->getBlockPosById(0x35, $pos+1)) >= 0) {
			$labels[$this->blocks[$pos]->key()] = $this->blocks[$pos]->text();
		}
		$info['labels'] = $labels;

		//Data info (blocks: 10, 11, 14, ¿19?, 4B)
		$data = array();
		$data['blocks'] = 0;
		$tmp = "";
		foreach ($this->blocks as $b) {
			switch ($b->getId()) {
				case 0x10:
				case 0x11:
				case 0x14:
				case 0x19:
				case 0x4B:
					$data['blocks']++;
					$tmp .= $b->getData();
					break;
			}
		}
		$data['crc32'] = hash("crc32b", $tmp);
		$data['md5'] = md5($tmp);
		$data['sha1'] = sha1($tmp);
		$info['data'] = $data;

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
abstract class BlockTSX
{
	protected $bytes = "";
	protected $pattern = "";
	protected $headSize = 0;
	protected $id = 0;

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
		$id = strtoupper(str_pad(dechex($this->getId()), 2, '0', STR_PAD_LEFT));
		return array(
			'id' => $id,
			'name' => 'Block #'.$id
		);
	}

	abstract protected function unpack($bytes = NULL);

	abstract protected function pack();
}

// ============================================================================================
// Block #10 - Standard speed data block
//	0x00 	- 	WORD 	    Pause after this block (ms.) {1000}
//	0x02 	N 	WORD 	    Length of data that follow
//	0x04 	- 	BYTE[N]     Data as in .TAP files
class Block10 extends BlockTSX
{
	protected $pattern = "Cid/vpause/vlen";
	protected $pattern2 = "Cvv";
	protected $headSize = 0x5;
	
	protected $pause = 1000;
	protected $len = 0;

	public function __construct($bytes = NULL)
	{
		$this->id = 0x10;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->headSize + $this->len);
		} else {
			$this->pack();
		}
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id, 
			$this->pause, 
			$this->len) = array_values(unpack($this->pattern, $bytes));
	}

	protected function pack()
	{
		$this->bytes = pack($this->pattern2, 
			$this->id, 
			$this->pause, 
			$this->len).$this->getData();
		return $this->bytes;
	}

	protected function getChecksum($data) {
		$checksum = 255;
		for ($i=0; $i<strlen($data); $i++) {
			$checksum ^= ord($data[$i]);
		}
		return $checksum;
	}

	public function getInfo() {
		$data = $this->getData();
		$info = parent::getInfo();
		$info['description'] = 'Standard speed data block';
		$info['bytesLength'] = strlen($data);
		$info['crc32'] = hash("crc32b", $data);
		$info['md5'] = md5($data);
		$info['sha1'] = sha1($data);
		return $info;
	}

	public function pause($value=NULL) {
		if ($value===NULL) {
			return $this->pause;
		} else {
			$this->pause = intval($value) & 0xFFFF;
			$this->pack();
		}
	}
	public function len() {
		return $this->len;
	}
	public function data($value=NULL, $addFlagAndChecksun=TRUE) {
		if ($value===NULL) {
			return $this->getData();
		} else {
			$value = chr(0xFF).$value.chr($this->getChecksum($value));
			$this->bytes = substr($this->bytes, 0, $this->headSize) . substr($value, 0, 0xFFFF);
			$this->len = strlen($value) & 0xFFFF;
			$this->pack();
		}
	}
}

// ============================================================================================
// Block #11 - Turbo speed data block
//	0x00	-	WORD		Length of PILOT pulse {2168}
//	0x02	-	WORD		Length of SYNC first pulse {667}
//	0x04	-	WORD		Length of SYNC second pulse {735}
//	0x06	-	WORD		Length of ZERO bit pulse {855}
//	0x08	-	WORD		Length of ONE bit pulse {1710}
//	0x0A	-	WORD		Length of PILOT tone (number of pulses) {8063 header (flag<128), 3223 data (flag>=128)}
//	0x0C	-	BYTE		Used bits in the last byte (other bits should be 0) {8}
//							(e.g. if this is 6, then the bits used (x) in the last byte are: xxxxxx00, where MSb is the leftmost bit, LSb is the rightmost bit)
//	0x0D	-	WORD		Pause after this block (ms.) {1000}
//	0x0F	N	BYTE[3]		Length of data that follow
//	0x12	-	BYTE[N]		Data as in .TAP files
class Block11 extends BlockTSX
{
	protected $pattern = "Cid/vpilot/vsync1/vsync2/vlen0/vlen1/vpulses/Clast/vpause/vlen16/Clen8";
	protected $pattern2 = "CvvvvvvCvvC";
	protected $headSize = 0x13;
	
	protected $pilotLen = 2168;
	protected $syncLen1 = 667;
	protected $syncLen2 = 735;
	protected $zeroLen = 855;
	protected $oneLen = 1710;
	protected $pilotPulses = 3223;
	protected $lastBits = 8;
	protected $pause = 1000;
	protected $len = 0;

	public function __construct($bytes = NULL)
	{
		$this->id = 0x11;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->headSize + $this->len);
		} else {
			$this->pack();
		}
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id, 
			$this->pilotLen,
			$this->syncLen1,
			$this->syncLen2,
			$this->zeroLen,
			$this->oneLen,
			$this->pilotPulses,
			$this->lastBits,
			$this->pause, 
			$len16, $len8) = array_values(unpack($this->pattern, $bytes));
		$this->len = $len16 | ($len8 << 16);
	}

	protected function pack()
	{
		$len16 = $this->len & 0xFFFF;
		$len8 = $this->len >> 16;
		$this->bytes = pack($this->pattern2, 
			$this->id, 
			$this->pilotLen,
			$this->syncLen1,
			$this->syncLen2,
			$this->zeroLen,
			$this->oneLen,
			$this->pilotPulses,
			$this->lastBits,
			$this->pause, 
			$len16, $len8).$this->getData();
		return $this->bytes;
	}

	public function getInfo() {
		$data = $this->getData();
		$info = parent::getInfo();
		$info['description'] = 'Turbo speed data block';
		$info['bytesLength'] = strlen($data);
		$info['crc32'] = hash("crc32b", $data);
		$info['md5'] = md5($data);
		$info['sha1'] = sha1($data);
		return $info;
	}

	public function pilotLen($value=NULL) {
		if ($value===NULL) {
			return $this->pilotLen;
		} else {
			$this->pilotLen = intval($value) & 0xFFFF;
			$this->pack();
		}
	}
	public function syncLen1($value=NULL) {
		if ($value===NULL) {
			return $this->syncLen1;
		} else {
			$this->syncLen1 = intval($value) & 0xFFFF;
			$this->pack();
		}
	}
	public function syncLen2($value=NULL) {
		if ($value===NULL) {
			return $this->syncLen2;
		} else {
			$this->syncLen2 = intval($value) & 0xFFFF;
			$this->pack();
		}
	}
	public function zeroLen($value=NULL) {
		if ($value===NULL) {
			return $this->zeroLen;
		} else {
			$this->zeroLen = intval($value) & 0xFFFF;
			$this->pack();
		}
	}
	public function oneLen($value=NULL) {
		if ($value===NULL) {
			return $this->oneLen;
		} else {
			$this->oneLen = intval($value) & 0xFFFF;
			$this->pack();
		}
	}
	public function pilotPulses($value=NULL) {
		if ($value===NULL) {
			return $this->pilotPulses;
		} else {
			$this->pilotPulses = intval($value) & 0xFFFF;
			$this->pack();
		}
	}
	public function lastBits($value=NULL) {
		if ($value===NULL) {
			return $this->lastBits;
		} else {
			$this->lastBits = intval($value) & 0xFF;
			$this->pack();
		}
	}
	public function pause($value=NULL) {
		if ($value===NULL) {
			return $this->pause;
		} else {
			$this->pause = intval($value) & 0xFFFF;
			$this->pack();
		}
	}
	public function len() {
		return $this->len;
	}
	public function data($value=NULL) {
		if ($value===NULL) {
			return $this->getData();
		} else {
			$this->bytes = substr($this->bytes, 0, $this->headSize) . substr($value, 0, 0xFFFFFF);
			$this->len = strlen($value) & 0xFFFFFF;
			$this->pack();
		}
	}
}

// ============================================================================================
// Block #12 - Pure tone
//	0x00	-	WORD		Length of one pulse in T-states
//	0x02	-	WORD		Number of pulses
class Block12 extends BlockTSX
{
	protected $pattern = "Cid/vpulseLen/vpulseNum";
	protected $pattern2 = "Cvv";
	protected $headSize = 0x5;
	
	protected $pulseLen = 0;
	protected $pulseNum = 0;

	public function __construct($bytes = NULL)
	{
		$this->id = 0x12;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->headSize);
		} else {
			$this->pack();
		}
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id, 
			$this->pulseLen,
			$this->pulseNum) = array_values(unpack($this->pattern, $bytes));
	}

	protected function pack()
	{
		$this->bytes = pack($this->pattern2, 
			$this->id, 
			$this->pulseLen,
			$this->pulseNum).$this->getData();
		return $this->bytes;
	}

	public function getInfo() {
		$info = parent::getInfo();
		$info['description'] = 'Pure tone block';
		return $info;
	}

	public function pulseLen($value=NULL) {
		if ($value===NULL) {
			return $this->pulseLen;
		} else {
			$this->pulseLen = intval($value) & 0xFFFF;
			$this->pack();
		}
	}
	public function pulseNum($value=NULL) {
		if ($value===NULL) {
			return $this->pulseNum;
		} else {
			$this->pulseNum = intval($value) & 0xFFFF;
			$this->pack();
		}
	}
}

// ============================================================================================
// Block #13 - Sequence of pulses of various lengths
//	0x00	N	BYTE		Number of pulses
//	0x01	-	WORD[N]		Pulses' lengths
class Block13 extends BlockTSX
{
	protected $pattern = "Cid/Cnum";
	protected $pattern2 = "CC";
	protected $headSize = 0x2;
	
	protected $pulseNum = 0;

	public function __construct($bytes = NULL)
	{
		$this->id = 0x13;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->headSize + $this->pulseNum * 2);
		} else {
			$this->pack();
		}
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id, 
			$this->pulseNum) = array_values(unpack($this->pattern, $bytes));
	}

	protected function pack()
	{
		$this->bytes = pack($this->pattern2, 
			$this->id, 
			$this->pulseNum).$this->getData();
		return $this->bytes;
	}

	public function getInfo() {
		$info = parent::getInfo();
		$info['description'] = 'Sequence of pulses block';
		return $info;
	}

	public function pulseNum() {
		return $this->pulseNum;
	}
	public function data($value=NULL) {	//$value must be an array(int)
		if ($value===NULL) {
			return $this->getData();
		} else {
			if (is_array($value)) {
				$this->bytes = substr($this->bytes, 0, $this->headSize) . pack("v*", $value);
				$this->pulseNum = count($value);
				$this->pack();
			}
		}
	}
}

// ============================================================================================
// Block #14 - Pure data block
//	0x00	-	WORD		Length of ZERO bit pulse
//	0x02	-	WORD		Length of ONE bit pulse
//	0x04	-	BYTE		Used bits in last byte (other bits should be 0)
//							(e.g. if this is 6, then the bits used (x) in the last byte are: xxxxxx00, where MSb is the leftmost bit, LSb is the rightmost bit)
//	0x05	-	WORD		Pause after this block (ms.)
//	0x07	N	BYTE[3] 	Length of data that follow
//	0x0A	-	BYTE[N]		Data as in .TAP files

// ============================================================================================
// Block #15 - Direct recording block
//	0x00	-	WORD 		Number of T-states per sample (bit of data)
//	0x02	-	WORD 		Pause after this block in milliseconds (ms.)
//	0x04	-	BYTE 		Used bits (samples) in last byte of data (1-8)
//							(e.g. if this is 2, only first two samples of the last byte will be played)
//	0x05	N	BYTE[3]		Length of samples' data
//	0x08	-	BYTE[N]		Samples data. Each bit represents a state on the EAR port (i.e. one sample).
//							MSb is played first.

// ============================================================================================
// Block #19 - Generalized data block
//	0x00	 -		DWORD		Block length (without these four bytes)
//	0x04	 -		WORD 		Pause after this block (ms)
//	0x06	TOTP	DWORD		Total number of symbols in pilot/sync block (can be 0)
//	0x0A	NPP		BYTE		Maximum number of pulses per pilot/sync symbol
//	0x0B	ASP		BYTE		Number of pilot/sync symbols in the alphabet table (0=256)
//	0x0C	TOTD	DWORD		Total number of symbols in data stream (can be 0)
//	0x10	NPD		BYTE		Maximum number of pulses per data symbol
//	0x11	ASD		BYTE		Number of data symbols in the alphabet table (0=256)
//	0x12	 -	 SYMDEF[ASP]	Pilot and sync symbols definition table
//								This field is present only if TOTP>0
//	0x12+		 PRLE[TOTP]		Pilot and sync data stream
//	 (2*NPP+1)*ASP				This field is present only if TOTP>0
//	0x12+		 SYMDEF[ASD] 	Data symbols definition table
//	 (TOTP>0)*((2*NPP+1)*ASP)+	This field is present only if TOTD>0
//	 TOTP*3
//	0x12+		  BYTE[DS]		Data stream
//	 (TOTP>0)*((2*NPP+1)*ASP)+	This field is present only if TOTD>0
//	 TOTP*3+
//	 (2*NPD+1)*ASD

// ============================================================================================
// Block #20 - Pause (silence) or 'Stop the tape' command
//	0x00	-	WORD		Pause duration (ms.)
class Block20 extends BlockTSX
{
	protected $pattern = "Cid/vpause";
	protected $pattern2 = "Cv";
	protected $headSize = 0x3;
	
	protected $pause = 0;

	public function __construct($bytes = NULL)
	{
		$this->id = 0x20;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->headSize);
		} else {
			$this->pack();
		}
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id, 
			$this->pause) = array_values(unpack($this->pattern, $bytes));
	}

	public function getInfo() {
		$info = parent::getInfo();
		$info['description'] = 'Pause block';
		return $info;
	}

	protected function pack()
	{
		$this->bytes = pack($this->pattern2, 
			$this->id, 
			$this->pause).$this->getData();
		return $this->bytes;
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
// Block #21 - Group start
//	0x00	L	BYTE		Length of the group name string
//	0x01	-	CHAR[L]		Group name in ASCII format (please keep it under 30 characters long)
class Block21 extends BlockTSX
{
	protected $pattern = "Cid/Clen";
	protected $pattern2 = "CC";
	protected $headSize = 0x2;
	
	protected $len = 0;

	public function __construct($bytes = NULL)
	{
		$this->id = 0x21;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->headSize + $this->len);
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
		$info = parent::getInfo();
		$info['description'] = 'Group start block';
		$info['label'] = $this->getData();
		return $info;
	}

	public function name($value=NULL) {
		if ($value===NULL) {
			return $this->getData();
		} else {
			$this->bytes = substr($this->bytes, 0, $this->headSize) . substr($value, 0, 0xFF);
			$this->len = strlen($value) & 0xFF;
			$this->pack();
		}
	}
}

// ============================================================================================
// Block #22 - Group end
class Block22 extends BlockTSX
{
	protected $pattern = "Cid";
	protected $pattern2 = "C";
	protected $headSize = 0x1;
	
	public function __construct($bytes = NULL)
	{
		$this->id = 0x22;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->headSize);
		} else {
			$this->pack();
		}
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id) = array_values(unpack($this->pattern, $bytes));
	}

	protected function pack()
	{
		$this->bytes = pack($this->pattern2, 
			$this->id);
		return $this->bytes;
	}

	public function getInfo() {
		$info = parent::getInfo();
		$info['description'] = 'Group end block';
		return $info;
	}
}

// ============================================================================================
// Block #23 - Jump to block
//	0x00	-	WORD		Relative jump value

// ============================================================================================
// Block #24 - Loop start
//	0x00	-	WORD		Number of repetitions (greater than 1)

// ============================================================================================
// Block #25 - Loop end

// ============================================================================================
// Block #26 - Call sequence
//	0x00	N	WORD		Number of calls to be made
//	0x02	-	WORD[N]		Array of call block numbers (relative-signed offsets)

// ============================================================================================
// Block #27 - Return from sequence

// ============================================================================================
// Block #28 - Select block
//	0x00	-	WORD		Length of the whole block (without these two bytes)
//	0x02	N	BYTE		Number of selections
//	0x03	-	SELECT[N]	List of selections
//
//	SELECT structure format
//	0x00	-	WORD		Relative Offset
//	0x02	L	BYTE		Length of description text
//	0x03	-	CHAR[L]		Description text (please use single line and max. 30 chars)

// ============================================================================================
// Block #2A - Stop the tape if in 48K mode
//	0x00	0	DWORD		Length of the block without these four bytes (0)

// ============================================================================================
// Block #30 - Text description
//	0x00	N	BYTE		Length of the text description
//	0x01	-	CHAR[N]		Text description in ASCII format
class Block30 extends BlockTSX
{
	protected $pattern = "Cid/Clen";
	protected $pattern2 = "CC";
	protected $headSize = 0x2;
	
	protected $len = 0;

	public function __construct($bytes = NULL)
	{
		$this->id = 0x30;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->headSize + $this->len);
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
		$info = parent::getInfo();
		$info['description'] = 'Text description block';
		$info['label'] = $this->getData();
		return $info;
	}

	public function text($value=NULL) {
		if ($value===NULL) {
			return $this->getData();
		} else {
			$this->bytes = substr($this->bytes, 0, $this->headSize) . substr($value, 0, 0xFF);
			$this->len = strlen($value) & 0xFF;
			$this->pack();
		}
	}
}

// ============================================================================================
// Block #31 - Message block
//	0x00	-	BYTE		Time (in seconds) for which the message should be displayed
//	0x01	N	BYTE		Length of the text message
//	0x02	-	CHAR[N]		Message that should be displayed in ASCII format
class Block31 extends BlockTSX
{
	protected $pattern = "Cid/Ctime/Clen";
	protected $pattern2 = "CCC";
	protected $headSize = 0x3;
	
	protected $time = 0;
	protected $len = 0;

	public function __construct($bytes = NULL)
	{
		$this->id = 0x31;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->headSize + $this->len);
		} else {
			$this->pack();
		}
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id,
			$this->time, 
			$this->len) = array_values(unpack($this->pattern, $bytes));
	}

	protected function pack()
	{
		$this->bytes = pack($this->pattern2, 
			$this->id, 
			$this->time, 
			$this->len).$this->getData();
		return $this->bytes;
	}

	public function getInfo() {
		$info = parent::getInfo();
		$info['description'] = 'Message block';
		$info['label'] = $this->getData();
		return $info;
	}

	public function time($value=NULL) {
		if ($value===NULL) {
			return $this->time;
		} else {
			$this->time = $value & 0xFF;
			$this->pack();
		}
	}
	public function text($value=NULL) {
		if ($value===NULL) {
			return $this->getData();
		} else {
			$this->bytes = substr($this->bytes, 0, $this->headSize) . substr($value, 0, 0xFF);
			$this->len = strlen($value) & 0xFF;
			$this->pack();
		}
	}
}

// ============================================================================================
// Block #32 - Archive info
//	0x00	-	WORD		Length of the whole block (without these two bytes)
//	0x02	N	BYTE		Number of text strings
//	0x03	-	TEXT[N]		List of text strings
//
//	TEXT structure format
//	0x00	-	BYTE	Text identification byte:
//				00 - Full title
//				01 - Software house/publisher
//				02 - Author(s)
//				03 - Year of publication
//				04 - Language
//				05 - Game/utility type
//				06 - Price
//				07 - Protection scheme/loader
//				08 - Origin
//				FF - Comment(s)
//	0x01	L	BYTE	Length of text string
//	0x02	-	CHAR[L]	Text string in ASCII format
class Block32 extends BlockTSX
{
	protected $pattern = "Cid/vlen/CnumInfo";
	protected $pattern2 = "CvC";
	protected $headSize = 0x4;
	
	protected $len = 1;
	protected $numInfo = 0;

	protected $archive = array();

	public function __construct($bytes = NULL)
	{
		$this->id = 0x32;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->len + 3);

			$tmp = $this->getData();
			while (strlen($tmp)>0) {
				$idx = ord($tmp[0]);
				$len = ord($tmp[1]);
				$this->archive[$idx] = substr($tmp, 2, $len);
				$tmp = substr($tmp, 2 + $len);
			}
		} else {
			$this->pack();
		}
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id,
			$this->len, 
			$this->numInfo) = array_values(unpack($this->pattern, $bytes));
	}

	protected function pack()
	{
		$this->bytes = pack($this->pattern2, 
			$this->id, 
			$this->len, 
			$this->numInfo).$this->getData();
		return $this->bytes;
	}

	public function getInfo() {
		$data = $this->getData();
		$info = parent::getInfo();
		$info['description'] = 'Archive info block';
		$info['archive'] = $this->archive;
		$info['crc32'] = hash("crc32b", $data);
		$info['md5'] = md5($data);
		$info['sha1'] = sha1($data);
		return $info;
	}

	public function len()
	{
		return $this->len;
	}
	public function numInfo()
	{
		return $this->numInfo;
	}
	public function archive($idx, $value=NULL)
	{
		$idx = strval($idx);
		if ($value===NULL) {
			return isset($this->archive[$idx]) ? $this->archive[$idx] : "";
		} else {
			$this->archive[$idx] = substr($value, 0, 0xFF);
			$tmp = "";
			foreach ($this->archive as $key => $data) {
				$tmp .= pack("CCA*", $key, strlen($data), $data);
			}
			$this->bytes = substr($this->bytes, 0, $this->headSize) . $tmp;
			$this->numInfo = count($this->archive) & 0xFF;
			$this->len = (strlen($tmp) + 1) & 0xFFFF;
			$this->pack();
		}
	}
}

// ============================================================================================
// Block #33 - Hardware type
//	0x00	N	BYTE	Number of machines and hardware types for which info is supplied
//	0x01	-	HWINFO[N]	List of machines and hardware
//
//	HWINFO structure format
//	0x00	-	BYTE	Hardware type
//	0x01	-	BYTE	Hardware ID
//	0x02	-	BYTE	Hardware information:
//				00 - The tape RUNS on this machine or with this hardware, but may or may not use the hardware or special features of the machine.
//				01 - The tape USES the hardware or special features of the machine, such as extra memory or a sound chip.
//				02 - The tape RUNS but it DOESN'T use the hardware or special features of the machine.
//				03 - The tape DOESN'T RUN on this machine or with this hardware.

// ============================================================================================
// Block #35 - Custom info block
//	0x00	-	CHAR[16]	Identification string (in ASCII)
//	0x10	L	DWORD		Length of the custom info
//	0x14	-	BYTE[L]		Custom info
class Block35 extends BlockTSX
{
	protected $pattern = "Cid/A16key/V";
	protected $pattern2 = "CA16V";
	protected $headSize = 0x15;
	
	protected $key = "                ";
	protected $len = 0;

	public function __construct($bytes = NULL)
	{
		$this->id = 0x35;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->headSize + $this->len);
		} else {
			$this->pack();
		}
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id,
			$this->key, 
			$this->len) = array_values(unpack($this->pattern, $bytes));
	}

	protected function pack()
	{
		$this->bytes = pack($this->pattern2, 
			$this->id, 
			$this->key, 
			$this->len).$this->getData();
		return $this->bytes;
	}

	public function getInfo() {
		$info = parent::getInfo();
		$info['description'] = 'Custom info block';
		$info['key'] = $this->key;
		$info['value'] = $this->getData();
		return $info;
	}

	public function key($value=NULL)
	{
		if ($value===NULL) {
			return $this->key;
		} else {
			$this->key = substr($value, 0, 16);
			$this->pack();
		}
	}
	public function len()
	{
		return $this->len;
	}
	public function text($value=NULL)
	{
		if ($value===NULL) {
			return $this->getData();
		} else {
			$this->bytes = substr($this->bytes, 0, $this->headSize) . substr($value, 0, 0xFFFF);
			$this->len = strlen($value) & 0xFFFF;
			$this->pack();
		}
	}
}

// ============================================================================================
// Block #4B - Kansas City Standard
//	0x00   N+12	DWORD		Block length without these four bytes (extension rule)
//	0x04	-	WORD		Pause after this block in milliseconds
//	0x06	-	WORD		Duration of a PILOT pulse in T-states {same as ONE pulse}
//	0x08	-	WORD		Number of pulses in the PILOT tone
//	0x0A	-	WORD		Duration of a ZERO pulse in T-states
//	0x0C	-	WORD		Duration of a ONE pulse in T-states
//	0x0E	-	BYTE		Bits 7-4: Number of ZERO pulses in a ZERO bit (0-16) {2}
//			 (bitmapped)	Bits 3-0: Number of ONE pulses in a ONE bit (0-16) {4}
//	0x0F	-	BYTE		Bits 7-6: Numbers of leading bits {1}
//			 (bitmapped)	Bit 5: Value of leading bits {0}
//							Bits 4-3: Number of trailing bits {2}
//							Bit 2: Value of trailing bits {1}
//							Bit 1: Reserved
//							Bit 0: Endianless (0 for LSb first, 1 for MSb first) {0}
//	0x10	-	BYTE[N]		Data stream
class Block4B extends BlockTSX
{
	protected $pattern = "Cid/Vlen/vpause/vpilot/vpulses/vlen0/vlen1/CbitCfg/CbyteCfg";
	protected $pattern2 = "CVvvvvvCC";
	protected $headSize = 0x11;
	
	protected $len = 0;
	protected $pause = 0;
	protected $pilotLen = 729;
	protected $pilotPulses = 30720;
	protected $bit0Len = 1458;
	protected $bit1Len = 729;
	protected $bitCfg = 0x24;
	protected $byteCfg = 0x54;

	public function __construct($bytes = NULL)
	{
		$this->id = 0x4B;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->len + 5);
		} else {
			$this->pack();
		}
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id,
			$this->len, 
			$this->pause, 
			$this->pilotLen, 
			$this->pilotPulses, 
			$this->bit0Len, 
			$this->bit1Len, 
			$this->bitCfg, 
			$this->byteCfg) = array_values(unpack($this->pattern, $bytes));
	}

	protected function pack()
	{
		$this->bytes = pack($this->pattern2, 
			$this->id, 
			$this->len, 
			$this->pause, 
			$this->pilotLen, 
			$this->pilotPulses, 
			$this->bit0Len, 
			$this->bit1Len, 
			$this->bitCfg, 
			$this->byteCfg).$this->getData();
		return $this->bytes;
	}

	public function getInfo() {
		$data = $this->getData();
		$info = parent::getInfo();
		if ($this->bitCfg==0x24 && $this->byteCfg==0x54)
			$info['description'] = 'MSX standard block';
		else
			$info['description'] = 'KCS custom block';
		$info['bytesLength'] = strlen($data);
		if (strlen($data)==16) {
			$first10 = substr($data, 0, 10);
			if ($first10==str_repeat(chr(0xD3), 10)) {
				$info['header'] = 'BASIC';
			} else
			if ($first10==str_repeat(chr(0xEA), 10)) {
				$info['header'] = 'ASCII';
			} else
			if ($first10==str_repeat(chr(0xD0), 10)) {
				$info['header'] = 'Binary';
			}
			if (isset($info['header'])) {
				$info['name'] = substr($data, 10);
			}
		}
		$info['crc32'] = hash("crc32b", $data);
		$info['md5'] = md5($data);
		$info['sha1'] = sha1($data);
		return $info;
	}

	public function len()
	{
		return $this->len;
	}
	public function pause($value=NULL) {
		if ($value===NULL) {
			return $this->pause;
		} else {
			$this->pause = $value & 0xFFFF;
			$this->pack();
		}
	}
	public function pilotLen($value=NULL) {
		if ($value===NULL) {
			return $this->pilotLen;
		} else {
			$this->pilotLen = $value & 0xFFFF;
			$this->pack();
		}
	}
	public function pilotPulses($value=NULL) {
		if ($value===NULL) {
			return $this->pilotPulses;
		} else {
			$this->pilotPulses = $value & 0xFFFF;
			$this->pack();
		}
	}
	public function bit0Len($value=NULL) {
		if ($value===NULL) {
			return $this->bit0Len;
		} else {
			$this->bit0Len = $value & 0xFFFF;
			$this->pack();
		}
	}
	public function bit1Len($value=NULL) {
		if ($value===NULL) {
			return $this->bit1Len;
		} else {
			$this->bit1Len = $value & 0xFFFF;
			$this->pack();
		}
	}
	public function bitCfg($value=NULL) {
		if ($value===NULL) {
			return $this->bitCfg;
		} else {
			$this->bitCfg = $value & 0xFF;
			$this->pack();
		}
	}
	public function byteCfg($value=NULL) {
		if ($value===NULL) {
			return $this->byteCfg;
		} else {
			$this->byteCfg = $value & 0xFF;
			$this->pack();
		}
	}
	public function data($value=NULL) {
		if ($value===NULL) {
			return $this->getData();
		} else {
			$this->bytes = substr($this->bytes, 0, $this->headSize) . $value;
			$this->len = strlen($value) + 12;
			$this->pack();
		}
	}
}

// ============================================================================================
// Block #5A - "Glue" block (90 dec, ASCII Letter 'Z')
//	0x00	-	BYTE[9]		Value: { "XTape!",0x1A,MajR,MinR }
//							Just skip these 9 bytes and you will end up on the next ID.
class Block5A extends BlockTSX
{
	protected $pattern = "Cid/C9magic";
	protected $pattern2 = "CC9";
	protected $headSize = 0x0A;
	
	public function __construct($bytes = NULL)
	{
		$this->id = 0x5A;
		if ($bytes!==NULL) {
			$this->unpack($bytes);
			$this->bytes = substr($bytes, 0, $this->headSize);
		} else {
			$this->pack();
		}
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id) = array_values(unpack($this->pattern, $bytes));
	}

	protected function pack()
	{
		$this->bytes = pack($this->pattern2, 
			$this->id);
		return $this->bytes;
	}

	public function getInfo() {
		$data = $this->getData();
		$info = parent::getInfo();
		$info['description'] = 'Glue block';
		return $info;
	}
}



// ============================================================================================
// Block #18 - CSW recording block

// ============================================================================================
// Block #2B - Set signal level

// ============================================================================================
// Block #16 - C64 ROM type data block

// ============================================================================================
// Block #17 - C64 turbo tape data block

// ============================================================================================
// Block #34 - Emulation info

// ============================================================================================
// Block #40 - Snapshot block



class int_helper
{
	public static function int8($i) {
		return is_int($i) ? pack("c", $i) : unpack("c", $i)[1];
	}

	public static function uInt8($i) {
		return is_int($i) ? pack("C", $i) : unpack("C", $i)[1];
	}

	public static function int16($i) {
		return is_int($i) ? pack("s", $i) : unpack("s", $i)[1];
	}

	public static function uInt16($i, $endianness=false) {
		$f = is_int($i) ? "pack" : "unpack";

		if ($endianness === true) {  // big-endian
			$i = $f("n", $i);
		}
		else if ($endianness === false) {  // little-endian
			$i = $f("v", $i);
		}
		else if ($endianness === null) {  // machine byte order
			$i = $f("S", $i);
		}

		return is_array($i) ? $i[1] : $i;
	}

	public static function int32($i) {
		return is_int($i) ? pack("l", $i) : unpack("l", $i)[1];
	}

	public static function uInt32($i, $endianness=false) {
		$f = is_int($i) ? "pack" : "unpack";

		if ($endianness === true) {  // big-endian
			$i = $f("N", $i);
		}
		else if ($endianness === false) {  // little-endian
			$i = $f("V", $i);
		}
		else if ($endianness === null) {  // machine byte order
			$i = $f("L", $i);
		}

		return is_array($i) ? $i[1] : $i;
	}

	public static function int64($i) {
		return is_int($i) ? pack("q", $i) : unpack("q", $i)[1];
	}

	public static function uInt64($i, $endianness=false) {
		$f = is_int($i) ? "pack" : "unpack";

		if ($endianness === true) {  // big-endian
			$i = $f("J", $i);
		}
		else if ($endianness === false) {  // little-endian
			$i = $f("P", $i);
		}
		else if ($endianness === null) {  // machine byte order
			$i = $f("Q", $i);
		}

		return is_array($i) ? $i[1] : $i;
	}
}
	
?>
