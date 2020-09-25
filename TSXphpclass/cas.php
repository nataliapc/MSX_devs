<?php

// ============================================================================================
// Interface CAS Class

interface ICAS {
	public function clear();					// Initialize the CAS object
	public function loadFromFile($filename);	// Initialize and load a CAS from file
	public function saveToFile($filename);		// Save the current CAS to a file
	public function getBytes();					// Obtain a raw binary string of the CAS
	public function getNumBlocks();				// Get the blocks count
	public function getBlock($index);			// Get the Block at a position
	public function getLastBlock();				// Get the last Block
	public function addBlock($newBlock);		// Add a block at the end of the TSX
	public function insertBlock($index, $newBlock);	// Insert a block in a position of the TSX
	public function deleteBlock($index);		// Deletes the block at a position
	public function getInfo();					// Get an object with a full TSX/Blocks info
}

// ============================================================================================
// Interface Generic Block

interface IBlockCAS {
	public function getSize();				// Get the full block length (header included)
	public function getData();				// Get the raw data even in no-data blocks
	public function getInfo();				// Get an object with info about the block
	public function data($value=NULL);		// Get/Set raw data
	public function isASCIIHeader();		// Return if this block is a ASCII header
	public function isBasicHeader();		// Return if this block is a BASIC header
	public function isBinaryHeader();		// Return if this block is a BINARY header
}

// ============================================================================================
// CAS class
//
// (2017.10.31) v1.0 First version
// (2020.09.07) v1.1 Revision. Interfaces added
//

class CAS implements ICAS
{
	const HEADER = "\x1F\xA6\xDE\xBA\xCC\x13\x7D\x74";

	private $blocks = array();


	public function __construct($filename = NULL)
	{
		$this->clear();
		if ($filename!==NULL && !$this->loadFromFile($filename)) {
			clear();
		}
	}

	public function clear()
	{
		$this->blocks = array();
	}

	public function loadFromFile($filename)
	{
		if (($bytes=file_get_contents($filename))!==NULL) {
			$this->clear();

			//Search for block headers
			while (strlen($bytes)>0 && substr($bytes, 0, 8)===self::HEADER) {
				$bytes = substr($bytes, 8);
				$type = substr($bytes, 0, 10);

				$b = new BlockCAS();
				$tmp = "";
				while (strlen($bytes)>0 && substr($bytes, 0, 8)!==self::HEADER) {
					$tmp .= substr($bytes, 0, 8);
					$bytes = substr($bytes, 8);
				}
				$b->data($tmp);
				$this->blocks[] = $b;
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
		$bytes = "";
		foreach ($this->blocks as $b) {
			$bytes .= self::HEADER . $b->data();
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

class BlockCAS implements IBlockCAS
{
	const MSX_ASCII_HEADER  = "\xEA\xEA\xEA\xEA\xEA\xEA\xEA\xEA\xEA\xEA";
	const MSX_BIN_HEADER    = "\xD0\xD0\xD0\xD0\xD0\xD0\xD0\xD0\xD0\xD0";
	const MSX_BASIC_HEADER  = "\xD3\xD3\xD3\xD3\xD3\xD3\xD3\xD3\xD3\xD3";

	const INFO_LENGTH	= "bytesLength";
	const INFO_CRC32	= "crc32";


	private $data = "";

	public function getSize() {
		return strlen($this->data);
	}

	public function getData() {
		return $this->data;
	}

	public function getInfo() {
		$info = array();
		$info[BlockCAS::INFO_LENGTH] = strlen($this->data);
		$info[BlockCAS::INFO_CRC32] = hash("crc32b", $this->data);
		$info['md5'] = md5($this->data);
		$info['sha1'] = sha1($this->data);
		return $info;
	}

	public function data($value=NULL)
	{
		if ($value===NULL) {
			return $this->data;
		} else {
			$this->data = $value;
		}
	}

	public function isASCIIHeader()
	{
		return strlen($this->data)==16 && substr_compare($this->data, self::MSX_ASCII_HEADER, 10)===0;
	}

	public function isBasicHeader()
	{
		return strlen($this->data)==16 && substr_compare($this->data, self::MSX_BASIC_HEADER, 10)===0;
	}

	public function isBinaryHeader()
	{
		return strlen($this->data)==16 && substr_compare($this->data, self::MSX_BIN_HEADER, 10)===0;
	}

}
	
?>