; ==============================================================================                                                                            
;   VDP_TMS9918_MSX.s                                                          
;   v1.0 (14 February 2014)                                                                     
;   Description:                                                              
;     *Opensource library for acces to VDP TMS9918
;     *Not use the BIOS (except the start screen mode, as it is used the 
;                        function CALSLT for inter-slot call).
;     *compatible with V9938 and V9958 (except MSX1 models: Yamaha CX5MII/128 & 
;        Spectravideo 738 because the detection system is based on the constant 
;        MSXVER (0x002D) from MSX BIOS)
;     *Directly using the ports 0x98 and 0x99 from MSX computers.
;     *Designed for develop MSX-DOS applications.                                                                           
; ============================================================================== 
	.area _DATA


  .area _CODE
  
; Ports  
VDPVRAM   = 0x98 ;VRAM Data (Read/Write)
VDPSTATUS = 0x99 ;VDP Status Registers 


; BIOS calls
CALSLT = 0x001C ;Executes inter-slot call
CHGMOD = 0x005F ;Switches to given screenmode


;system var
MSXVER = 0x002D
LINL40 = 0xF3AE ;Screen width per line in SCREEN 0
RG0SAV = 0xF3DF ;#F3DF - #F3E6: vdp registers 0-7
FORCLR = 0xF3E9 ;Foreground colour
BAKCLR = 0xF3EA ;Background colour
BDRCLR = 0xF3EB ;Border colour
EXPTBL = 0xFCC1 



;===============================================================================
; screen
; Sets the screen mode.
;
; void screen(char mode)
; ==============================================================================
_screen::
  push IX
  ld   IX,#0
  add  IX,SP

  ld   A,4(IX)
  or   A
  jr   NZ,setSCR
  
  ;screen 0 > 40 columns mode
  ld   A,#39  ;default value
  ld   (#LINL40),A 
  
  xor  A
  
setSCR:  
  ld   IX,#CHGMOD
  ld   IY,(EXPTBL-1)
  call CALSLT ;acces to MSX BIOS
  ei
  
  pop ix
  ret
  


;===============================================================================
; color
; Function : Specifies the foreground color, background and area of the edges. 
; Input    : (char) - ink color
;            (char) - background color.        <<<< Not used. BIOS version only.
;            (char) - border color
;
; void color(char ink, char background, char border)
;===============================================================================
;(info by Portar Doc)
;Register 7: colour register.
;  Bit  Name  Expl.
;  0-3  TC0-3 Background colour in SCREEN 0 (also border colour in SCREEN 1-3)
;  4-7  BD0-3 Foreground colour in SCREEN 0      
;===============================================================================
_color::
  push IX
  ld   IX,#0
  add  IX,SP

  ld   A,5(IX)
  ld   (BAKCLR),A ;save in system vars
  
  ld   A,6(IX)
  ld   (BDRCLR),A
  ld   B,A
  
  ld   A,4(IX)
  ld   (FORCLR),A ;save in system vars  

  sla  A
  sla  A
  sla  A
  sla  A
  add  A,B
  
  ld   C,#0x07 ;VDP reg 7
  ld   B,A  
  call WRTVDP
   
  pop  IX
  ret



;===============================================================================
; vpoke
; Function : Writes a byte to the video RAM. 
; Input    : (uint)  - VRAM address
;            (byte)  - value 
;
;void vpoke(uint address, byte value)
;===============================================================================
_vpoke::
  push IX
  ld   IX,#0
  add  IX,SP
  
  ld   L,4(IX)
  ld   H,5(IX)
   
  ld   A,6(IX)
  
  call WRTVRM
  
  pop  IX
  ret



;===============================================================================
; vpeek
; Function : Reads data from the video RAM. 
; Input    : (unsigned int) - memory address
; Output   : (byte) value
;
; unsigned char vpeek(uint address)
;=============================================================================== 
_vpeek::
  push IX
  ld   IX,#0
  add  IX,SP
  
  ld   L,4(IX)
  ld   H,5(IX) 
   
  call RDVRM
  
  ld   L,A
  
  pop  IX
  ret



;===============================================================================
; fillVRAM                                
; Function : Fill a large area of the VRAM of the same byte.
; Input    : (unsigned int) - VRAM address
;            (unsigned int) - length
;            (byte) - value
;
; void fillVRAM (uint vaddress, uint size, byte value)
;===============================================================================
_fillVRAM::
  push IX
  ld   IX,#0
  add  IX,SP
      
  ld   L,4(IX) ; vaddress
  ld   H,5(IX)
    
  ld   E,6(IX) ;length
  ld   D,7(IX)
    
  ld   A,8(IX) ;value
  
  call FILVRM
    
  pop  IX
  ret



;===============================================================================
; copyToVRAM
; Function : Block transfer from memory to VRAM
; Input    : (unsigned int) - source RAM address 
;            (unsigned int) - target VRAM address
;            (unsigned int) - length
;
; void copyToVRAM(uint address, uint vaddress, uint length)
;===============================================================================
_copyToVRAM::

  push IX
  ld   IX,#0
  add  IX,SP  

  ld   L,4(IX) ;ram address 
  ld   H,5(IX)
      
  ld   E,6(IX) ;vaddress
  ld   D,7(IX)
  
  ld   C,8(IX) ;length
  ld   B,9(IX)
  
  call LDIRVM
  
  pop  IX
  ret



;===============================================================================
; copyFromVRAM
; Function : Block transfer from VRAM to memory.
; Input    : (unsigned int) - source VRAM address
;            (unsigned int) - target RAM address
;            (unsigned int) - length
;
;void copyFromVRAM(uint vaddress, uint address, uint length)
;===============================================================================
_copyFromVRAM::

  push IX
  ld   IX,#0
  add  IX,SP
    
  ld   L,4(IX) ;vaddress
  ld   H,5(IX)
  
  ld   E,6(IX) ;ram address 
  ld   D,7(IX)     
  
  ld   C,8(IX) ;length
  ld   B,9(IX)
    
  call LDIRMV
    
  pop  IX

  ret
  


;===============================================================================
; _setVDP
; Function : writes a value in VDP reg.
; Input    : (byte) - VDP reg
;            (byte) - value
;
;void setVDP(byte, byte)
;===============================================================================
_setVDP::
  push IX
  ld   IX,#0
  add  IX,SP
      
  ld   C,4(IX) ;reg
  ld   B,5(IX) ;value
  
  call WRTVDP  
    
  pop  IX
  ret

  
  
;===============================================================================
; WRTVDP
; write data in the VDP-register  
; Input    : B  - data to write
;            C  - number of the register
;===============================================================================
WRTVDP::

  ld   HL,#RG0SAV
  ld   E,C
  ld   D,#0
  add  HL,DE
  ld   (HL),B ;save copy of vdp value in system var
  
  ld   A,B
  di
	out	 (#VDPSTATUS),A
	ld   A,C
  or   #0x80            ;add  A,#0x80 ;add 128 to VDP reg value
  out	 (#VDPSTATUS),A
  ei
  ret


  
;===============================================================================
; WRTVRM                                
; Function : 
; Input    : A  - value
;            HL - VRAM address
;===============================================================================
WRTVRM::
  
  push   AF
  
  call SET2WRITE  

  ;ex (SP),HL
  ;ex (SP),HL
  
  pop   AF
  out  (VDPVRAM),A
  
  ret


SET2WRITE:
  di
  ld   A,(#MSXVER) ;if MSX2?
  or   A
  jr   Z,WRITETMS
  xor  A
  out  (VDPSTATUS),A
  ld   A,#0x8E
  out  (VDPSTATUS),A
WRITETMS:
  ld   A,L
  out  (VDPSTATUS),A
  ld   A,H        
  and  #0x3F
  or   #0x40
  out  (VDPSTATUS),A
  ei
  
  ret



;===============================================================================
; RDVRM                                
; Function : 
; Input    : HL - VRAM address
; Output   : A  - value
;===============================================================================
RDVRM::
  call SET2READ
  ;ex (SP),HL
  ;ex (SP),HL
    
  in   A,(VDPVRAM)

  ret
  
  
SET2READ:
  di
  ld A,(#MSXVER) ;if MSX2?
  or A
  jr Z,READTMS
  xor A
  out  (VDPSTATUS),A
  ld   A,#0x8E
  out  (VDPSTATUS),A
READTMS:
  ld   A,L
  out  (VDPSTATUS),A
  ld   A,H
  and  #0x3F
  out  (VDPSTATUS),A 
  ei

  ret



;===============================================================================
; FILVRM                                
; Function : Fill a large area of the VRAM of the same byte.
; Input    : A  - value
;            DE - Size
;            HL - VRAM address
;===============================================================================
FILVRM::
  
  ld   B,A
  ld   C,#VDPVRAM
   
  call SET2WRITE  
    
VFILL_loop:
  out  (C),B 
  
  dec  DE
  ld   A,D
  or   E
  jr   nz,VFILL_loop 
  
  ret



;===============================================================================
; LDIRVM
; Function : 
;    Block transfer from memory to VRAM 
; Input    : BC - blocklength
;            HL - source RAM address
;            DE - target VRAM address
;===============================================================================
LDIRVM::

  ex   DE,HL
  
  call SET2WRITE
    
  ex   DE,HL
  
  ld   D,B
  ld   E,C
        
  ld   C,#VDPVRAM
    
VWRITE_loop:
  outi
  
  dec  DE
  ld   A,D
  or   E
  jr   nz,VWRITE_loop    
  
  ret   
    

        
;===============================================================================
; LDIRMV
; Function : 
;    Block transfer from VRAM to memory.  
; Input    : BC - blocklength
;            HL - source VRAM address                     
;            DE - target RAM address            
;===============================================================================    
LDIRMV::
  call SET2READ
;  ex (SP),HL
;  ex (SP),HL
  
  ex   DE,HL
  
  ld   D,B
  ld   E,C
  
  ld   C,#VDPVRAM
    
VREAD_loop:
  ini
  
  dec  DE
  ld   A,D
  or   E
  jr   NZ,VREAD_loop    
   
  ret
  
