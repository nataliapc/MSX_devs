<?php

// ============================================================================================
// PZX class
//
// http://zxds.raxoft.cz/docs/pzx.txt
//
// (2018.06.04) v1.0 First version
// (2018.07.n)
//
class PZX
{
	private $majorVer;
	private $minorVer;
	private $blocks = array();


	public function __construct($filename = NULL)
	{
		$this->clear();
		if ($filename!==NULL) {
			$this->loadFromFile($filename);
		}
	}

	public function clear()
	{
		$this->majorVer = 0;
		$this->minorVer = 0;
		$this->blocks = array();
	}

	public function loadFromFile($filename)
	{
		$this->clear();
		if (($bytes=file_get_contents($filename))!==NULL) {
			$this->createFromString($bytes);
		}
	}

	public function createFromString($bytes = NULL) {
		$this->clear();

		if ($bytes!==NULL && strlen($bytes)>0) {
			//Read Blocks
			while (strlen($bytes)>0) {
				$b = NULL;
				list($tag, $size) = array_values(unpack("A4tag/Vsize", $bytes));
				$bdata = substr($bytes, 0, $size + 8);

				switch ($tag) {
					//****************** Mandatory blocks
					//PZXT - PZX Header block
					case 'PZXT': $b = new BlockPZX_PZXT($bdata); break;
					//PULS - Pulse sequence
					case 'PULS': $b = new BlockPZX_PULS($bdata); break;
					//DATA - Data block
					case 'DATA': $b = new BlockPZX_DATA($bdata); break;
					//PAUS - Pause block
					case 'PAUS': $b = new BlockPZX_PAUS($bdata); break;

					//****************** Optional blocks
					//BRWS - Browse point
					case 'BRWS': $b = new BlockPZX_BRWS($bdata); break;
					//STOP - Stop tape command
					case 'STOP': $b = new BlockPZX_STOP($bdata); break;

					//****************** Extended Data block
					//XDAT - Extended Data Block
					case 'XDAT': $b = new BlockPZX_XDAT($bdata); break;

					//Unknown
					default:   echo "Block '$tag' UNKNOWN!!\n";
							   break;
				}
				if ($b!==NULL) {
					$this->blocks[] = $b;
				}
				$bytes = substr($bytes, $size + 8);
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
		$bytes = '';
		foreach ($this->blocks as $b) {
			$bytes .= $b->getBytes();
		}
		return $bytes;
	}

	public function getNumBlocks()
	{
		return count($this->blocks);
	}

	public function getBlockAt($pos)
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
		if ($index >= count($this->blocks)) {
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

		//File info
		$file = array();
		$tmp = $this->getBytes();
		$file['fileSize'] = strlen($tmp);
		$file['blocks'] = $this->getNumBlocks();
		$file['crc32'] = hash("crc32b", $tmp);
		$file['md5'] = md5($tmp);
		$file['sha1'] = sha1($tmp);

		//Blocks
		$blocks = array();
		$pos = 0;
		foreach ($this->blocks as $b) {
			$tmp = $b->getInfo();
			if ($tmp!="") $blocks[$pos] = $tmp;
			$pos++;
		}

		$info = array();
		$info['file'] = $file;
		$info['blocks'] = $blocks;

		return $info;
	}

	// ========================================================
	// Static constructors
	// ========================================================

	// MSX ====================================================
	// PULSE Pilot:
	//          0x8000+15360,729
	//          0x8000+3840,729
	// Block header:
	//          0  11 2  4   pattern      1458,1458   729,729,729,729
	//   'MSX ' 00 0B 02 04 'Z01234567OO' B2 05 B2 05 D9 02 D9 02 D9 02 D9 02 ... data stream
	//
	public static function createBlock_MSX($bauds=1200) {
		$b = new BlockPZX_XDAT();
		$b->system('MSX ');
		$b->flags(0b00000000);
		$b->pattern('Z01234567OO');
		$b->s0(array(1458, 1458));
		$b->s1(array(729, 729, 729, 729));
		$b->data('');
		return $b;
	}

	//Only 1200/2400 bauds supported
	public static function createBlock_MSX_Pilot($short=false, $bauds=1200) {
		$pilot = 729;
		if ($bauds>=2400) {
			$pilot = 364;
			$count = 31744;
			if ($short) $count = 7936;
		} else {
			$count = 15360;
			if ($short) $count = 3840;
		}
		$b = new BlockPZX_PULS();
		$b->pulses(array(0x8000+$count, $pilot));
		return $b;
	}

	// ZX Spectrum ============================================
	// PULSE Pilot:
	//          0x8000+8063,2168,667,735
	//          0x8000+3223,2168,667,735
	// Block header:
	//          0  8  2  2   pattern   855,855     1710,1710
	//   'ZXSP' 00 08 02 02 '76543210' 57 03 57 03 AE 06 AE 06 ... data stream
	//
	public static function createBlock_Spectrum() {
		$b = new BlockPZX_XDAT();
		$b->system('ZXSP');
		$b->flags(0b00000000);
		$b->pattern('76543210');
		$b->s0(array(855, 855));
		$b->s1(array(1710, 1710));
		$b->data('');
		return $b;
	}

	public static function createBlock_Spectrum_Pilot($short=false) {
		$count = $short ? 3663 : 8063;
		$b = new BlockPZX_PULS();
		$b->pulses(array(0x8000+$count, 2168, 667, 735));
		return $b;
	}

	// ZX-81 ==================================================
	// PULSE Pilot:
	//          0x8000+8063,2168,667,735  ??
	//          0x8000+3223,2168,667,735  ??
	// Block header:
	//          0  8  8  18   pattern   530,520,...    530,520,...
	//   'ZXSP' 00 08 08 12 '76543210' 12 02 08 02... 12 02 08 02 ... data stream
	//
	public static function createBlock_ZX81() {
		$b = new BlockPZX_XDAT();
		$b->system('ZX81');
		$b->flags(0b00000000);
		$b->pattern('76543210');
		$b->s0(array(530, 520, 530, 520, 530, 520, 530, 4689));
		$b->s1(array(530, 520, 530, 520, 530, 520, 530, 520, 530, 520, 530, 520, 530, 520, 530, 520, 530, 4689));
		$b->data('');
		return $b;
	}

	//Not sure if this is true
	public static function createBlock_ZX81_Pilot($short=false) {
		$count = $short ? 3663 : 8063;
		$b = new BlockPZX_PULS();
		$b->pulses(array(0x8000+$count, 2168, 667, 735));
		return $b;
	}

	// Amstrad CPC ============================================
	// PULSE Pilot:
	//          0X8000+4096,1245,673,673
	// Block header:
	//          0  8  2  2   pattern   669,669     1338,1338
	//   'ACPC' 00 08 02 02 '76543210' 9D 02 9D 02 3A 05 3A 05 ... data stream
	//
	public static function createBlock_AmstradCPC() {
		$b = new BlockPZX_XDAT();
		$b->system('ACPC');
		$b->flags(0b00000000);
		$b->pattern('76543210');
		$b->s0(array(669, 669));
		$b->s1(array(1338, 1338));
		$b->data('');
		return $b;
	}

	public static function createBlock_AmstradCPC_Pilot() {
		$b = new BlockPZX_PULS();
		$b->pulses(array(0X8000+4096, 1245, 673, 673));
		return $b;
	}

	// SVI-318/328 ============================================
	// PULSE Pilot:
	//          0X55 sequence
	// Block header:
	//          0  9  2  2   pattern    1458,1458   729,729
	//   'S3x8' 00 09 02 02 '76543210Z' B2 05 B2 05 D9 02 D9 02 ... data stream
	//
	public static function createBlock_SVI3x8() {
		$b = new BlockPZX_XDAT();
		$b->system('S3x8');
		$b->flags(0b00000000);
		$b->pattern = '76543210Z';
		$b->s0(array(1458, 1458));
		$b->s1(array(729, 729));
		$b->data('');
		return $b;
	}

	public static function createBlock_SVI3x8_Pilot() {
		$b = new BlockPZX_XDAT();
		$b->system('S3x8');
		$b->flags(0b00000000);
		$b->pattern = '01234567';
		$b->s0(array(1458, 1458));
		$b->s1(array(729, 729));
		$b->data(str_repeat(chr(0x55), 199).chr(0x7F));
		return $b;
	}

	// BBC Micro/Acorn ========================================
	// PULSE Pilot:
	//          ??
	// Block header:
	//          0  8  2  2   pattern     1458,1458   729,729
	//   'BBCM' 00 08 02 02 'Z01234567O' B2 05 B2 05 D9 02 D9 02 ... data stream
	//
	public static function createBlock_BBC() {
		$b = new BlockPZX_XDAT();
		$b->system('BBCM');
		$b->flags(0b00000000);
		$b->pattern('Z01234567O');
		$b->s0(array(1458, 1458));
		$b->s1(array(729, 729));
		$b->data('');
		return $b;
	}

	// Dragon/Coco ============================================
	// PULSE Pilot:
	//          0x55 sequence
	// Block header:
	//          0  8  2  2   pattern   1458,1458   729,729
	//   'DRAG' 00 08 02 02 '01234567' B2 05 B2 05 D9 02 D9 02 ... data stream
	//
	public static function createBlock_Dragon() {
		$b = new BlockPZX_XDAT();
		$b->system('DRAG');
		$b->flags(0b00000000);
		$b->pattern('01234567');
		$b->s0(array(1458, 1458));
		$b->s1(array(729, 729));
		$b->data('');
		return $b;
	}

	// Atari 400/410/800 ======================================
	// PULSE Pilot:
	//          Not used / Unknown
	// Block header: (600 bauds / 3995Hz[0] 5327Hz[1]) [https://hintermueller.de/dereatari-appendix-c]
	//          0  10 14 18  pattern     438,438,438,438,438,438,438,438,438,438,438,438,438,438  328,328,328,328,328,328,328,328,328,328,328,328,328,328,328,328,328,328
	//   'ATAR' 00 08 0E 12 'Z01234567O' B6 01...B6 01 48 01...48 01 ... data stream
	//
	public static function createBlock_Atari400() {
		$b = new BlockPZX_XDAT();
		$b->system('ATAR');
		$b->flags(0b00000000);
		$b->pattern('Z01234567O');
		$b->s0(array(438, 438, 438, 438, 438, 438, 438, 438, 438, 438, 438, 438, 438, 438));
		$b->s1(array(328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328, 328));
		$b->data('');
		return $b;
	}

	// Oric ===================================================
	// PULSE Pilot:
	//          ??
	// Block header: [http://forum.defence-force.org/viewtopic.php?t=201]
	//          0  13 2  2   pattern        728,1456    728,728
	//   'ORIC' 00 0D 02 02 'Z01234567pOOO' D8 02 B0 05 D8 02 D8 02 ... data stream
	//
	public static function createBlock_Oric($slow = false) {
		$b = new BlockPZX_XDAT();
		$b->system('ORIC');
		$b->flags(0b00000000);
		$b->pattern('Z01234567pOOO');

		if (!$slow) {
			$b->s0(array(728, 1456));
			$b->s1(array(728, 728));
		} else {
			$b->s0(array(1456, 1456, 1456, 1456, 1456, 1456, 1456, 1456));
			$b->s1(array(728, 728, 728, 728, 728, 728, 728, 728, 728, 728, 728, 728, 728, 728, 728, 728));
		}

		$b->data('');
		return $b;
	}

	// TI-99 ==================================================
	// PULSE Pilot:
	//          ??
	// Block header: [http://www.unige.ch/medecine/nouspikel/ti99/cassette.htm]
	//          0  13 1  2   pattern??  2538  1269,1269
	//   'TI99' 00 0D 01 02 '01234567'  EA 09 F5 04 F5 04 ... data stream
	//
	public static function createBlock_Oric() {
		$b = new BlockPZX_XDAT();
		$b->system('TI99');
		$b->flags(0b00000000);
		$b->pattern('01234567'); //Pattern info not found. Assuming plain LSB.
		$b->s0(array(2538));
		$b->s1(array(1269, 1269));
		$b->data('');
		return $b;
	}

}

// ============================================================================================
// Generic Abstract Block class
//
// offset type     name   meaning
// 0      u32      tag    unique identifier for the block type.
// 4      u32      size   size of the block in bytes, excluding the tag and size fields themselves.
// 8      u8[size] data   arbitrary amount of block data.
//
abstract class BlockPZX
{
	protected $id;
	protected $patternIn = "A4tag/Vsize";
	protected $patternout = "A4V";
	protected $headSize = 8;
	protected $bytes;

	public function __construct($bytes = NULL) {
		if ($bytes!==NULL) {
			$this->bytes = $bytes;
			$this->unpack($bytes);
		} else {
			$this->pack();
		}
	}

	//Return the block ID
	public function getId()
	{
		return $this->id;
	}

	//Return the full block size
	public function getSize()
	{
		return strlen($this->bytes);
	}

	//Return the header size of this block
	public function getHeadSize()
	{
		return $this->headSize;
	}

	//Return the full block in bytes
	public function getBytes()
	{
		return $this->bytes;
	}

	//Return the extra bytes that aren't part of header
	public function getData()
	{
		return substr($this->bytes, $this->headSize);
	}

	//Return the block size (excluding tag and size fields)
	public function len() {
		return $this->getSize() - 8;
	}

	//Return verbose info about the block
	public function getInfo()
	{
		return array(
			'tag' => $this->id,
			'name' => 'Block #'.$this->id
		);
	}

	abstract protected function unpack($bytes = NULL);

	abstract protected function pack();
}

// ============================================================================================
// Block PZXT - PZX Header block
//
// offset type     name   meaning
// 0      u8       major  major version number (currently 1).
// 1      u8       minor  minor version number (currently 0).
// 2      u8[?]    info   tape info, see below.
//
class BlockPZX_PZXT extends BlockPZX
{
	protected $patternIn = "A4tag/Vsize/Cmajor/Cminor";
	protected $patternOut = "A4VCC";
	protected $headSize = 4 + 4 + 1 + 1;

	protected $major = 1;
	protected $minor = 0;
	protected $title = '';
	protected $info = array();
	
	public function __construct($bytes = NULL)
	{
		$this->id = 'PZXT';
		parent::__construct($bytes);
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id,
			$len,
			$this->major, 
			$this->minor) = array_values(unpack($this->patternIn, $bytes));

		$tmp = explode("\0", $this->getData());
		if (count($tmp)>0) {
			$this->title = $tmp[0];
			$i = 1;
			$this->info = array();
			while ($i < count($tmp)-1) {
				$this->info[] = array($tmp[$i], $tmp[$i+1]);
				$i+=2;
			}
		}
	}

	protected function pack()
	{
		$aux = '';
		if ($this->title!='' || count($this->info)>0) {
			$aux .= $this->title."\0";
			foreach ($this->info as $value) {
				$aux .= $value[0]."\0".$value[1]."\0";
			}
		}
		$this->bytes = pack($this->patternOut, 
			$this->id, 
			$this->headSize + strlen($aux) - 8,
			$this->major,
			$this->minor).$aux;
		return $this->bytes;
	}

	public function getInfo() {
		$data = $this->getData();
		$info = parent::getInfo();
		$info['description'] = 'Header block';
		$info['title'] = ($this->title);
		$info['info'] = $this->info;
		return $info;
	}

	public function title($value=NULL) {
		if ($value===NULL) {
			return $this->title;
		}
		$this->title = $value;
		$this->pack();
	}

	public function info($value=NULL) {
		if ($value===NULL) {
			return $this->info;
		}
		$this->info = $value;
		$this->pack();
	}

	public function addInfo($key, $value) {
		$this->info[] = array($key, $value);
		$this->pack();
	}

	public function replaceInfo($key, $newValue) {
		foreach ($this->info as $idx => $value) {
			if ($value[0]==$key) {
				$this->info[$idx] = array($key, $newValue);
			}
		}
		$this->pack();
	}

	public function removeInfo($key, $all=FALSE) {
		foreach ($this->info as $idx => $value) {
			if ($value[0]==$key) {
				unset($this->info[$idx]);
				if (!$all) break;
			}
		}
		$this->pack();
	}

	public function clearInfo() {
		$this->info = array();
		$this->pack();
	}
}

// ============================================================================================
// Block PULS - Pulse sequence
//
// offset type   name      meaning
// 0      u16    count     bits 0-14 optional repeat count (see bit 15), always greater than zero
//                         bit 15 repeat count present: 0 not present 1 present
// 2      u16    duration1 bits 0-14 low/high (see bit 15) pulse duration bits
//                         bit 15 duration encoding: 0 duration1 1 ((duration1<<16)+duration2)
// 4      u16    duration2 optional low bits of pulse duration (see bit 15 of duration1) 
// 6      ...    ...       ditto repeated until the end of the block
//
class BlockPZX_PULS extends BlockPZX
{
	protected $patternIn = "A4tag/Vsize";
	protected $patternOut = "A4Vv";
	protected $headSize = 4 + 4;

	protected $pulses = array();
	
	public function __construct($bytes = NULL)
	{
		$this->id = 'PULS';
		parent::__construct($bytes);
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id,
			 $len) = array_values(unpack($this->patternIn, $bytes));

		$i = 0;
		$this->pulses = array();
		while ($i < $this->len()) {
			list($aux) = array_values(unpack('v', substr($bytes, 8+$i, 2)));
			$this->pulses[] = $aux;
			$i += 2;
		}
	}

	protected function pack()
	{
		$this->bytes = pack($this->patternOut, 
			$this->id, 
			$this->headSize + count($this->pulses)*2 - 8);
		foreach ($this->pulses as $aux) {
			$this->bytes .= pack('v', $aux);
		}
		return $this->bytes;
	}

	public function getInfo() {
		$data = $this->getData();
		$info = parent::getInfo();
		$info['description'] = 'Pulse sequence';
		$info['repeat'] = $this->count & 0x8000 ? 1 : 0;
		
		$aux = array_values($this->pulses);
		for ($i=0; $i < count($aux); $i++) { 
			if ($aux[$i] > 0x8000) {
				$aux[$i] = '(repeat.count)'.($aux[$i]&0x7fff);
			}
		}
		$info['pulses'] = implode(',', $aux);

		return $info;
	}

	public function pulses($value=NULL) {
		if ($value===NULL) {
			return $this->pulses;
		}
		$this->pulses = $value;
		$this->pack();
	}
}

// ============================================================================================
// Block DATA - Data block
//
// offset      type             name  meaning
// 0           u32              count bits 0-30 number of bits in the data stream
//                                    bit 31 initial pulse level: 0 low 1 high
// 4           u16              tail  duration of extra pulse after last bit of the block
// 6           u8               p0    number of pulses encoding bit equal to 0.
// 7           u8               p1    number of pulses encoding bit equal to 1.
// 8           u16[p0]          s0    sequence of pulse durations encoding bit equal to 0.
// 8+2*p0      u16[p1]          s1    sequence of pulse durations encoding bit equal to 1.
// 8+2*(p0+p1) u8[ceil(bits/8)] data  data stream, see below.
//
class BlockPZX_DATA extends BlockPZX
{
	protected $patternIn = "A4tag/Vsize/Vcount/vtail/Cpz/Cpo";
	protected $patternOut = "A4VVvCC";
	protected $headSize = 4 + 4 + 4 + 2 + 1 + 1;

	protected $count = 0;
	protected $pulseLevel = 0;
	protected $p0 = 2;						//Default for standard spectrum block
	protected $p1 = 2;						//Default for standard spectrum block
	protected $s0 = array(855, 855);		//Default for standard spectrum block
	protected $s1 = array(1710, 1710);		//Default for standard spectrum block
	protected $data = '';
	
	public function __construct($bytes = NULL)
	{
		$this->id = 'DATA';
		parent::__construct($bytes);
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id,
			$len,
			$this->count,
			$this->tail,
			$this->p0,
			$this->p1) = array_values(unpack($this->patternIn, $bytes));

		$this->pulseLevel = $this->count & 0x80000000;
		$this->count &= 0x7fffffff;

		$i = 0;
		$this->s0 = array();
		while ($i < $this->p0) {
			list($aux) = array_values(unpack('v', substr($bytes, 16+$i*2, 2)));
			$this->s0[] = $aux;
			$i++;
		}

		$i = 0;
		$this->s1 = array();
		while ($i < $this->p1) {
			list($aux) = array_values(unpack('v', substr($bytes, 16+$this->p0*2+$i*2, 2)));
			$this->s1[] = $aux;
			$i++;
		}

		$this->data	= substr($bytes, $this->headSize + ($this->p0+$this->p1)*2);
	}

	protected function pack()
	{
		$this->bytes = pack($this->patternOut, 
			$this->id, 
			$this->headSize + ($this->p0 + $this->p1)*2 + strlen($this->data) - 8,
			$this->count | $this->pulseLevel,
			$this->tail,
			$this->p0,
			$this->p1);
		$aux0 = $aux1 = '';
		for ($i=0; $i<count($this->s0); $i++) {
			$aux0 = pack('v', $this->s0[$i]);
		}
		for ($i=0; $i<count($this->s1); $i++) {
			$aux1 = pack('v', $this->s1[$i]);
		}
		$this->bytes .= $aux0.$aux1.$this->data;
		return $this->bytes;
	}

	public function getInfo() {
		$data = $this->getData();
		$info = parent::getInfo();
		$info['description'] = 'Data block';
		$info['bits.number'] = $this->count;
		$info['initial.pulse.level'] = ($this->pulseLevel>0 ? '(1) high' : '(0) low');
		$info['pulses.0'] = "(".$this->p0.") ".implode(',', $this->s0);
		$info['pulses.1'] = "(".$this->p1.") ".implode(',', $this->s1);
		$info['data.length'] = strlen($data);
		return $info;
	}

	public function pulses0($pulses = NULL) {
		if ($pulses==NULL) {
			return $this->s0;
		}
		if (is_array($pulses)) {
			$this->p0 = size($pulses);
			$this->s0 = $pulses;
		}
	}

	public function pulses1($pulses = NULL) {
		if ($pulses==NULL) {
			return $this->s1;
		}
		if (is_array($pulses)) {
			$this->p1 = size($pulses);
			$this->s1 = $pulses;
		}
	}

	public function data($value=NULL) {
		if ($value===NULL) {
			return $this->data;
		}
		$this->data = $value;
		$this->count = strlen($value) * 8;
		$this->pack();
	}
}

// ============================================================================================
// Block PAUS - Pause block
//
// offset type   name      meaning
// 0      u32    duration  bits 0-30 duration of the pause
//                         bit 31 initial pulse level: 0 low 1 high
//
class BlockPZX_PAUS extends BlockPZX
{
	protected $patternIn = "A4tag/Vsize/Vduration";
	protected $patternOut = "A4VV";
	protected $headSize = 4 + 4 + 4;

	protected $duration = 0;
	protected $pulseLevel = 0;
	
	public function __construct($bytes = NULL)
	{
		$this->id = 'PAUS';
		parent::__construct($bytes);
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id,
			$len,
			$this->duration) = array_values(unpack($this->patternIn, $bytes));
		$this->pulseLevel = $this->duration & 0x80000000;
		$this->duration &= 0x7fffffff;
	}

	protected function pack()
	{
		$duration = $this->duration | $this->pulseLevel;
		$this->bytes = pack($this->patternOut, 
			$this->id, 
			$this->headSize - 8,
			$duration);
		return $this->bytes;
	}

	public function getInfo() {
		$data = $this->getData();
		$info = parent::getInfo();
		$info['description'] = 'Pause block';
		$info['duration'] = $this->duration;
		$info['initial.pulse.level'] = ($this->pulseLevel>0 ? '(1) high' : '(0) low');
		return $info;
	}

	public function duration($value=NULL) {
		if ($value===NULL) {
			return $this->duration;
		}
		$this->duration = $value;
		$this->pack();
	}

	public function pulseLevel($valur=NULL) {
		if ($value===NULL) {
			return $this->pulseLevel;
		}
		$this->pulseLevel = ($value >= 0 ? 0x80000000 : 0);
		$this->pack();
	}
}

// ============================================================================================
// Block BRWS - Browse point
//
// offset type   name   meaning
// 0      u8[?]  text   text describing this browse point
//
class BlockPZX_BRWS extends BlockPZX
{
	protected $patternIn = "A4tag/Vsize";
	protected $patternOut = "A4V";
	protected $headSize = 4 + 4;

	protected $text = '';
	
	public function __construct($bytes = NULL)
	{
		$this->id = 'BRWS';
		parent::__construct($bytes);
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id,
			 $len) = array_values(unpack($this->patternIn, $bytes));
		$this->text = substr($bytes, 8);
	}

	protected function pack()
	{
		$this->bytes = pack($this->patternOut, 
			$this->id, 
			$this->headSize + strlen($this->text) - 8).$this->text;
		return $this->bytes;
	}

	public function getInfo() {
		$data = $this->getData();
		$info = parent::getInfo();
		$info['description'] = 'Browse point';
		$info['label'] = $this->text;
		return $info;
	}

	public function text($value=NULL) {
		if ($value===NULL) {
			return $this->text;
		}
		$this->text = $value;
		$this->pack();
	}
}

// ============================================================================================
// Block STOP - Stop tape command
//
// offset type   name   meaning
// 0      u16    flags  when exactly to stop the tape (1 48k only, other always).
//
class BlockPZX_STOP extends BlockPZX
{
	protected $patternIn = "A4tag/Vsize/vflags";
	protected $patternOut = "A4Vv";
	protected $headSize = 4 + 4 + 2;

	protected $flags = 0;
	
	public function __construct($bytes = NULL)
	{
		$this->id = 'STOP';
		parent::__construct($bytes);
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id,
			$len,
			$this->flags) = array_values(unpack($this->patternIn, $bytes));
	}

	protected function pack()
	{
		$this->bytes = pack($this->patternOut, 
			$this->id, 
			$this->headSize - 8,
			$this->flags);
		return $this->bytes;
	}

	public function getInfo() {
		$data = $this->getData();
		$info = parent::getInfo();
		$info['description'] = 'Browse point';
		$info['flags'] = $this->flags;
		return $info;
	}

	public function flags($value=NULL) {
		if ($value===NULL) {
			return $this->flags;
		}
		$this->flags = $value;
		$this->pack();
	}
}

// ============================================================================================
// Block XDAT - Extended Data block (not official block definition)
//
// offset type      name      meaning
// 0                u8[4]     system     Optional label defining system ('MSX ', 'ZXSP', 'ZX81', 'ACPC', S3x8', 'DRAG', 'A410')
// 4                u8        flags      bit 0: initial pulse level: 0 high | 1 low
//                                       bits 1-7: unused/reserved (default: 0)
// 5                u8        psiz       Pattern size in bytes
// 6                u8        p0         Number of pulses encoding bit equal to 0
// 7                u8        p1         Number of pulses encoding bit equal to 1
// 8                u8[psiz]  pattern    Pattern char array describing the byte layout:
//                                         Z|O    Literal (Z)ero or (O)ne value
//                                         0…7    Bit at position n (0: Least Significant Bit, 7: Most)
//                                         P|p    Byte parity (P: normal, p: inversed)
// 8+psiz           u16[p0]   s0         Sequence of pulse durations encoding bit equal to 0.
// 8+psiz+2*p0      u16[p1]   s1         Sequence of pulse durations encoding bit equal to 1.
// 8+psiz+2*(p0+p1) u8[?]     data       Data stream
//
class BlockPZX_XDAT extends BlockPZX
{
	protected $patternIn = "A4tag/Vsize/A4sys/Cflags/Cpsiz/Cp0/Cp1";
	protected $patternOut = "A4VA4CCCC";
	protected $headSize = 4 + 4 + 4 + 1 + 1 + 1 + 1;	//16 + variable sizes

	protected $system;
	protected $flags;
	protected $psiz;
	protected $p0;
	protected $p1;
	protected $pattern;
	protected $s0 = array();
	protected $s1 = array();
	protected $data;

	public function __construct($bytes = NULL)
	{
		$this->id = 'XDAT';
		parent::__construct($bytes);
	}

	protected function unpack($bytes = NULL)
	{
		if ($bytes===NULL) $bytes = $this->bytes;
		list($this->id,
			$len,
			$this->system,
			$this->flags,
			$this->psiz,
			$this->p0,
			$this->p1) = array_values(unpack($this->patternIn, $bytes));

		$this->headSize = 16 + psiz + 2 * (p0 + p1);

		$this->pattern = substr($bytes, 16, $this->psiz);

		$this->s0 = array();
		for ($i=0; $i<$this->p0; $i++) {
			$this->s0[] = unpack('v', substr($bytes, 16 + $this->psiz + $i*2, 2));
		}
		$this->s1 = array();
		for ($i=0; $i<$this->p1; $i++) {
			$this->s1[] = unpack('v', substr($bytes, 16 + $this->psiz + $this->p0 + $i*2, 2));
		}

		$this->data	= substr($bytes, $this->headSize);
	}

	protected function pack()
	{
		$this->bytes = pack($this->patternOut, 
			$this->id, 
			$this->headSize + strlen($this->data) + $this->headSize,
			$this->system,
			$this->flags,
			$this->psiz,
			$this->p0,
			$this->p1);
		$this->bytes .= $this->pattern;
		for ($i=0; $i<count($this->s0); $i++) {
			$this->bytes .= pack('v', $this->s0[$i]);
		}
		for ($i=0; $i<count($this->s1); $i++) {
			$this->bytes .= pack('v', $this->s1[$i]);
		}
		$this->bytes .= $this->data;
		return $this->bytes;
	}

	public function getInfo() {
		$info = parent::getInfo();
		$info['description'] = 'eXtended DATA block';
		$info['system'] = $this->system;
		$info['data.length'] = strlen($this->getData());
		$info['initial.pulse.level'] = (($this->flags & 1)>0 ? '(1) low' : '(0) high');
		$info['flags'] = $this->flags;
		return $info;
	}

	// ========================================================
	// Getters/Setters
	// ========================================================

	public function system($value=NULL) {
		if ($value===NULL) {
			return $this->system;
		}
		$this->system = substr($value.'    ', 0, 4);
		$this->pack();
	}

	public function flags($value=NULL) {
		if ($value===NULL) {
			return $this->flags;
		}
		$this->flags = $value;
		$this->pack();
	}

	public function pattern($value=NULL) {
		if ($value===NULL) {
			return $this->pattern;
		}
		$this->pattern = $value;
		$this->psiz = strlen($value);
		$this->pack();
	}

	public function s0($value=NULL) {
		if ($value===NULL) {
			return $this->s0;
		}
		$this->s0 = $value;
		$this->p0 = count($value);
		$this->pack();
	}

	public function s1($value=NULL) {
		if ($value===NULL) {
			return $this->s1;
		}
		$this->s1 = $value;
		$this->p1 = count($value);
		$this->pack();
	}

	public function data($value=NULL) {
		if ($value===NULL) {
			return $this->data;
		}
		$this->data = $value;
		$this->pack();
	}

}



$pzx = new PZX('superscramble.pzx');

$pzx->addBlock(PZX::createBlock_MSX_Pilot());

$b = PZX::createBlock_MSX();
$b->data("Prueba de Browse...");
$pzx->addBlock($b);

$b = $pzx->getBlockAt(0);
$b->replaceInfo('Price','nose');

print_r($pzx->getInfo());

?>
