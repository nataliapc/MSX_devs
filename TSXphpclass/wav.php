<?php

// ============================================================================================
// Interface WavWriter Class

interface IWavWriter
{
	public function getSampleRate();			// Returns the WAV samplerate
	public function open($filename);			// Open an output stream for writing a new WAV
	public function addBytes($bytes);			// Add bytes in String format to the stream
	public function addSample($value);			// Add a single Sample unsigned char [-128...127]
	public function flush();					// Force the flush of internal buffers to disk
	public function close();					// Close the output file stream
}

interface IWavFromTSX
{
	// Common
	public function setPhase($signal);						// Change the current signal phase
	public function togglePhase();							// Toggle the current signal phase
	public function writeSilence($millis);					// Write a silence in millis
	public function writeSample($tstates, $value);			// Write a single sample
	public function writePulse($tstates);					// Write a single pulse
	// MSX/KCS functions
	public function setSettings4B($block);					// Set the #4B block settings
	public function writePilot4B();							// Write a MSX/KCS pilot
	public function writeBit0();							// Write a MSX/KCS bit 0
	public function writeBit1();							// Write a MSX/KCS bit 1
	public function writeByte4B($byte);						// Write a MSX/KCS byte
	// Spectrum functions
	public function writeTurboPilot($tstates=2168);			// Write a Spectrum pilot
	public function writeTurboSync($syn1=667, $sync2=735);	// Write a Spectrum sync cycle
	public function writeTurboBit($tstates);				// Write a Spectrum bit
	public function writeTurboByte($byte, $bits=8, $zerolen=855, $onelen=1710);	// Write a Spectrum byte
}

// ============================================================================================
// WavWriter Class
//
// (2020.05.28) v1.0 First version
//
class WavWriter implements IWavWriter
{
	const DEFAULT_SAMPLERATE = 48000;	// 48KHz
	const CHANNELS = 1;					// 1 Channel: Mono

	private $filename = NULL;
	private $fd = NULL;
	private $buffer;
	private $sampleRate;
	private $bits = 8;

	public function __construct($filename=NULL)
	{
		if ($filename!==NULL) {
			$this->open($filename);
		}
	}

	//=====================================================
	// IWavWriter functions

	public function getSampleRate()
	{
		return $this->sampleRate;
	}

	public function open($filename)
	{
		$this->filename = $filename;
		$this->fd = fopen($filename, "w+b");
		$this->buffer = "";

		$this->sampleRate = WavWriter::DEFAULT_SAMPLERATE; 	// Default: 48KHz 8bits Mono
		$this->bits = 8;
		$this->addHeader();
	}

	public function addBytes($bytes)
	{
		$this->checkFileHandler();
		$this->buffer .= $bytes;
	}

	public function addSample($value)
	{
		$this->checkFileHandler();
		$this->buffer .= pack("C", $value+0x80);
	}

	public function flush()
	{
		$this->checkFileHandler();
		if ($this->buffer!=="") {
			fwrite($this->fd, $this->buffer);
			fflush($this->fd);
			$this->buffer = "";
		}
	}

	public function close()
	{
		$this->checkFileHandler();
		$this->flush();
		fclose($this->fd);

		$this->updateHeaderSizes();
		$this->fd = NULL;
	}

	//=====================================================
	// Private functions

	private function checkFileHandler()
	{
		if ($this->fd===NULL) {
			throw new UnexpectedValueException("Open first the WAV stream!");
		}
	}

	private function addHeader()
	{
		$sr = $this->sampleRate;
		$b = $this->bits;
		$header = pack('A4VA4A4VvvVVvvA4V', 
			'RIFF',							// [4] riffId: "RIFF"
			0,								// [4] riffSize: Size of chunk
			'WAVE',							// [4] waveId: "WAVE"
			'fmt ',							// [4] fmtId: "fmt "
			16,								// [4] fmtSize: 16 (PCM)
			1,								// [2] wFormatTag: 1 (PCM)
			WavWriter::CHANNELS,			// [2] nChannels: 1(Mono) 2(Stereo)
			$sr,							// [4] nSamplesPerSec: samplerate (8000, 11000, 22050, 44100, 48000, ...)
			$sr*WavWriter::CHANNELS*$b/8,	// [4] nAvgBytesPerSec: SampleRate * NumChannels * BitsPerSample/8
			WavWriter::CHANNELS*$b/8,		// [2] nBlockAlign: NumChannels * BitsPerSample/8
			$b,								// [2] wBitsPerSample: 8:8bits 16:16bits
			'data',							// [4] dataId: "data"
			0								// [4] dataSize: Size of data chunk
		);
		$this->addBytes($header);
	}

	private function updateHeaderSizes()
	{
		$filesize = filesize($this->filename);

		$this->fd = fopen($this->filename, "r+b");

		fseek($this->fd, 4, SEEK_SET);
		fwrite($this->fd, pack("V", $filesize - 8));

		fseek($this->fd, 40, SEEK_SET);
		fwrite($this->fd, pack("V", $filesize - 8 - 36));

		fclose($this->fd);
	}

}

// ============================================================================================
// WavFromTSX Class
//
// (2020.05.28) v1.0 First version
//
class WavFromTSX extends WavWriter implements IWavFromTSX
{
	const TZX_Z80_FREQ = 3500000;

	private $pilotLen = 729;
	private $pilotPulses = 5000;
	private $pulseOne = 729;
	private $pulseZero = 1458;
	private $numOnePulses = 2;
	private $numZeroPulses = 4;
	private $startBits = 1;
	private $startBitValue = 0;
	private $stopBits = 2;
	private $stopBitValue = 1;
	private $msb = 0;

	private $currentValue = 127;

	//=====================================================
	// IWavFromTSX functions

	// ========== Common functions

	public function tstates2bytes($tstates)
	{
		return 	($tstates * $this->getSampleRate() / WavFromTSX::TZX_Z80_FREQ);
	}

	public function setPhase($signal)
	{
		$this->currentValue = $signal <= 0 ? -127 : 127;
	}

	public function togglePhase()
	{
		$this->currentValue = -$this->currentValue;
	}

	public function writeSilence($millis)
	{
		$this->addBytes(str_repeat(chr(0x80), 48000/1000*$millis));
	}

	public function writeSample($tstates, $value)
	{
		static $acumBytes = 0;

		$acumBytes += $this->tstates2bytes($tstates);
		$this->addBytes(str_repeat(chr($value+128), intval($acumBytes)));
		$acumBytes -= intval($acumBytes);
	}

	public function writePulse($tstates)
	{
		$this->writeSample($tstates, $this->currentValue);
		$this->togglePhase();
	}

	// ========== MSX/KCS functions

	public function setSettings4B($b) {
		$this->pilotLen = $b->pilotLen();
		$this->pilotPulses = $b->pilotPulses();
		$this->pulseOne = $b->bit1Len();
		$this->pulseZero = $b->bit0Len();
		$this->numOnePulses = ($b->bitCfg() & 0b00001111);
		$this->numZeroPulses = ($b->bitCfg() & 0b11110000) >> 4;
		if ($this->numOnePulses==0) {
			$this->numOnePulses = 16;
		}
		if ($this->numZeroPulses==0) {
			$this->numZeroPulses = 16;
		}
		$this->startBits = ($b->byteCfg() & 0b11000000) >> 6;
		$this->startBitValue = ($b->byteCfg() & 0b00100000) >> 5;
		$this->stopBits = ($b->byteCfg() & 0b00011000) >> 3;
		$this->stopBitValue = ($b->byteCfg() & 0b00000100) >> 2;
		$this->msb = ($b->byteCfg() & 0b00000001);
	}

	public function writePilot4B()
	{
		for ($i=0; $i<$this->pilotPulses; $i++) {
			$this->writePulse($this->pilotLen);
		}
	}

	public function writeBit0()
	{
		for ($i=0; $i<$this->numZeroPulses; $i++) {
			$this->writePulse($this->pulseZero);
		}
	}

	public function writeBit1()
	{
		for ($i=0; $i<$this->numOnePulses; $i++) {
			$this->writePulse($this->pulseOne);
		}
	}

	public function writeByte4B($byte)
	{
		// start bits
		for ($t=0; $t<$this->startBits; $t++) {
			$this->startBitValue ? $this->writeBit1() : $this->writeBit0();
		}
		// eight data bits
		for ($i=0; $i<8; $i++) {
			if ($this->msb) {
				$this->writeBitMSB($byte, $i);
			} else {
				$this->writeBitLSB($byte, $i);
			}
		}
		// stop bits
		for ($t=0; $t<$this->stopBits; $t++) {
			$this->stopBitValue ? $this->writeBit1() : $this->writeBit0();
		}
	}

	// ========== Spectrum functions
	
	public function writeTurboPilot($tstates=2168)
	{
		$this->writePulse($tstates);
	}

	public function writeTurboSync($sync1=667, $sync2=735)
	{
		$this->writePulse($sync1);
		$this->writePulse($sync2);
	}

	public function writeTurboBit($tstates)
	{
		$this->writePulse($tstates);
		$this->writePulse($tstates);
	}

	public function writeTurboByte($byte, $bits=8, $zerolen=855, $onelen=1710)
	{
		// eight data bits
		for ($i=0; $i<$bits; $i++) {
			$this->writeTurboBit($byte & (128 >> $i) ? $onelen : $zerolen);
		}
	}

	//=====================================================
	// Private functions

	private function writeBitLSB($byte, $i)
	{
		$byte & (1 << $i) ? $this->writeBit1() : $this->writeBit0();
	}

	private function writeBitMSB($byte, $i)
	{
		$byte & (1 << (7-$i)) ? $this->writeBit1() : $this->writeBit0();
	}

}

?>