/**
    msx_const.h
    Purpose: Define MSX constants for BIOS, Address I/O, etc...

    @author Natalia Pujol Cremades
    @version 1.0 11/02/2017 
*/

#ifndef  __MSX_CONST_H__
#define  __MSX_CONST_H__

//=====================
// MSX System Routines
//=====================
// Sources:
//   https://github.com/PeterLemon/MSX/blob/master/LIB/MSX_SYSTEM.INC
//
// BIOS LISTING
// This Section Lists BIOS Entries Available To The User
// There Are 2 Kinds Of BIOS Routines: "MAIN-ROM" & "SUB-ROM"
// Routines In MAIN-ROM Use "CALL" Or "RTS" Instruction As A Subroutine Call
// The Call Sequence Of SUB-ROM Is As Follows:
//
// LD IX,INIPLT ; Set BIOS Entry Address
// CALL EXTROM  ; Returns here
//
// When Contents Of IX Should Not Be Destroyed:
//
// INIPAL:
//   PUSH IX      ; Save IX
//   LD IX,INIPLT ; Set BIOS Entry Address
//   JP SUBROM    ; Return Caller Of INIPAL


//=====================================
// MAIN-ROM
//=====================================

//================================
// RSTs

//  Function: Test RAM & Set RAM Slot For The System
//     Input: NONE
//    Output: NONE
// Registers: ALL
// Available: MSX
#define BIOS_CHKRAM		0x0000	//

//  Function: Test IF Character In [HL] Is Specified
//            IF TRUE GOTO CHRGTR ($0010), ELSE Generate SYNTAX ERROR 
//     Input: [HL] = Test Character
//            Compared Character Is Placed Next To Called RST Instruction
//    Output: HL++, A = [HL], CY-Flag Set IF Tested Character Is Numerical
//            Z-Flag Set IF At End Of Statement ($00 Or $3A)
// Registers: AF, HL
// Available: MSX
#define BIOS_SYNCCHR	0x0008	//

//  Function: Selects Slot Corresponding To Value In A & Read 1 Byte From Slot Memory
//            When Routine Is Called, Interrupt Is Inhibited Even After Execution Ends
//     Input:  A = Slot Number: %F000EEBB - F = BASIC(0) Expansion(1),
//            EE = Expansion Slot Number(0..3), BB = BASIC Slot Number(0..3)
//            HL = Address Of Memory To Be Read
//    Output: Value Of Memory Read From A
// Registers: AF, BC, DE
// Available: MSX
#define BIOS_RDSLT		0x000c	//

//  Function: Get A Character (Or Token) From BASIC Text
//     Input: [HL] = Character To Be Read
//    Output: HL++, A = [HL], CY-Flag Set IF Tested Character Is Numerical
//            Z-Flag Set IF At End Of Statement ($00 Or $3A)
// Registers: AF, HL
// Available: MSX
#define BIOS_CHRGTR		0x0010	//

//  Function: Select Slot Corresponding To Value In A & Write 1 Byte To Slot Memory
//            When Routine Is Called, Interrupt Is Inhibited Even After Execution Ends
//     Input:  A = Slot Number: %F000EEBB - F = BASIC(0) Expansion(1),
//            EE = Expansion Slot Number(0..3), BB = BASIC Slot Number(0..3)
//            HL = Address Of Memory To Be Read
//    Output: NONE
// Registers: AF, BC, D
// Available: MSX
#define BIOS_WRSLT		0x0014	//

//  Function: Send Value To Current Device
//     Input: A = Value To Be Sent
//            Send Output To Printer When PTRFLG ($F416) Is Not Zero
//            Send Output To File Specified By PTRFIL ($F864) IF PTRFIL Not Zero
//    Output: NONE
// Registers: NONE
// Available: MSX (Call SUB-ROM Internally In Screen Modes 5..8)
#define BIOS_OUTDO		0x0018	//

//  Function: Call Routine In Another Slot (Inter-Slot Call)
//     Input: IYH = Slot Number: %F000EEBB - F = BASIC(0) Expansion(1),
//             EE = Expansion Slot Number(0..3), BB = BASIC Slot Number(0..3)
//             IX = Address To Be Called
//    Output: Depends On Called Routine
// Registers: Depends On Called Routine
// Available: MSX
#define BIOS_CALSLT		0x001c

//  Function: Compare Contents Of HL & DE
//     Input: HL, DE
//    Output: Set Z-Flag IF (HL == DE), Set CY-Flag IF (HL < DE)
// Registers: AF
// Available: MSX
#define BIOS_DCOMPR		0x0020	//

//  Function: Select Slot Corresponding To Value In A & Enable Slot To Be Used
//            When Routine Is Called, Interrupt Is Inhibited Even After Execution Ends
//     Input:  A = Slot Number: %F000EEBB - F = BASIC(0) Expansion(1),
//            EE = Expansion Slot Number(0..3), BB = BASIC Slot Number(0..3)
//            HL = Slot Page: 2 High Order Bits
//    Output: NONE
// Registers: ALL
// Available: MSX
#define BIOS_ENASLT		0x0024	//

//  Function: Return Type Of DAC (Decimal Accumulator)
//     Input: NONE
//    Output: S, Z, P/V Flags Change Depending On Type Of DAC:
//            Types Can Be Recognised By Flag Marked By "*"
//            Integer: C = 1,  S = 1*, Z = 0,  P/V = 1
//             String: C = 1,  S = 0,  Z = 1*, P/V = 1
//             Single: C = 1,  S = 0,  Z = 0,  P/V = 0*
//             Double: C = 0*, S = 0,  Z = 0,  P/V = 1
// Registers: AF
// Available: MSX
#define BIOS_GETYPR		0x0028	//

//  Function: Call Routine In Another Slot
//            The Following Is The Call Sequence:
//            RST 30H
//            DB	n	; n Is The Slot Number (Aame As RDSLT)
//            DW	nn	; nn Is The Called Address
//     Input: The Method Described Above
//    Output: Depends On Called Routine
// Registers: AF, Other Registers Depend On Called Routine
// Available: MSX
#define BIOS_CALLF		0x0030	//

//  Function: Executes Timer Interrupt Process Routine
//     Input: NONE
//    Output: NONE
//  Register: NONE
// Available: MSX
#define BIOS_KEYINT		0x0038	//

//================================
// I/O Initialization

//  Function: Initialise I/O Device
//     Input: NONE
//    Output: NONE
// Registers: ALL
// Available: MSX
#define BIOS_INITIO		0x003b	//

//  Function: Initialise Contents Of Function Keys
//     Input: NONE
//    Output: NONE
// Registers: ALL
// Available: MSX
#define BIOS_INIFNK		0x003e	//

//================================
// VDP Access

//  Function: Disable Screen Display
//     Input: NONE
//    Output: NONE
// Registers: AF, BC
// Available: MSX
#define BIOS_DISSCR		0x0041

//  Function: Enable Screen Display
//     Input: NONE
//    Output: NONE
// Registers: ALL
// Available: MSX
#define BIOS_ENASCR		0x0044

//  Function: Write Data To VDP Register
//     Input: C = VDP Register Number (0..27, 32..46), B = Data To Write
//    Output: NONE
// Registers: AF, BC
// Available: MSX (Call SUB-ROM Internally In Screen Modes 5..8)
#define BIOS_WRTVDP		0x0047

//  Function: Read Data From VRAM (14-Bit Address: $0000..$3FFF) (TMS9918)
//            Only Lowest 14-Bits Of VRAM Address Are Valid
//            Call NRDVRM To Use All Bits
//     Input: HL = VRAM Address To Be Read
//    Output:  A = Data Which Was Read
// Registers: AF
// Available: MSX
#define BIOS_RDVRM		0x004a	//Reads the content of VRAM

//  Function: Write Data To VRAM (14-Bit Address: $0000..$3FFF) (TMS9918)
//            Only Lowest 14-Bits Of VRAM Address Are Valid
//            Call NWRVRM To Use All 16-Bits
//     Input: HL = VRAM Address, A = Data To Write
//    Output: NONE
// Registers: AF
// Available: MSX
#define BIOS_WRTVRM		0x004d	//Write content to VRAM

//  Function: Set VRAM (14-Bit Address: $0000..$3FFF) In VDP & Enable It To Be Read (TMS9918)
//            This Is Used To Read Auto-Increment Data From VRAM
//            Enables Faster Reads Than Using RDVRM In A Loop
//            Only Lowest 14-Bits Of VRAM Address Are Valid
//            Call NSETRD To Use All Bits
//     Input: HL = VRAM Address
//    Output: NONE
// Registers: AF
// Available: MSX
#define BIOS_SETRD		0x0050	//

//  Function: Set VRAM (14-Bit Address: $0000..$3FFF) In VDP & Enable It To Be Written (TMS9918)
//            This Is Used To Write Auto-Increment Data To VRAM
//            Enables Faster Writes Than Using WRTVRM In A Loop
//            Only Lowest 14-Bits Of VRAM Address Are Valid
//            Call NSTWRT To Use All Bits
//     Input: HL = VRAM Address
//    Output: NONE
// Registers: AF
// Available: MSX
#define BIOS_SETWRT		0x0053	//

//  Function: Fill Specified VRAM Area (14-Bit Address: $0000..$3FFF) With Repeated Data (TMS9918)
//            Only Lowest 14-Bits Of VRAM Address Are Valid
//            Call BIGFIL To Use All Bits
//     Input: HL = VRAM Destination Address, BC = Data Length, A = Data To Repeat
//    Output: NONE
// Registers: AF, BC
// Available: MSX (Do Not Call SUB-ROM While Screen Modes 4..8 Are Changed)
#define BIOS_FILVRM		0x0056

//  Function: Block Transfer From VRAM To Memory
//     Input: HL = VRAM Source Address, DE = Memory Destination Address, BC = Data Length
//            All Bits Of VRAM Address Are Valid
//    Output: NONE
// Registers: ALL
// Available: MSX (Do Not Call SUB-ROM While Screen Modes 4..8 Are Changed)
#define BIOS_LDIRMV		0x0059

//  Function: Block Transfer From Memory To VRAM
//     Input: HL = Memory Source Address, DE = VRAM Destination Address, BC = Data Length
//            All Bits Of VRAM Address Are Valid
//    Output: NONE
// Registers: ALL
// Available: MSX (Do Not Call SUB-ROM While Screen Modes 4..8 Are Changed)
#define BIOS_LDIRVM		0x005c

//  Function: Change Screen Mode
//            Palette Is Not Initialised
//            To Initialise Palette Call CHGMDP In SUB-ROM After This Call
//     Input: A = Screen Mode (0..8)
//    Output: NONE
// Registers: ALL
// Available: MSX (Always Call SUB-ROM)
#define BIOS_CHGMOD		0x005f

//  Function: Change Screen Color
//     Input: A = Screen Mode
//            FORCLR ($F3E9) Foreground Color
//            BAKCLR ($F3EA) Background Color
//            BDRCLR ($F3EB) Border Color
//    Output: NONE
// Registers: ALL
// Available: MSX
#define BIOS_CHGCLR		0x0062

//  Function: Execute NMI (Non-Maskable Interrupt) Handling Routine
//     Input: NONE
//    Output: NONE
// Registers: NONE
// Available: MSX
#define BIOS_NMI		0x0066	//

//  Function: Initialise All Sprites
//            Sprite Pattern Cleared To Zero, Sprite Number To Sprite Plane Number
//            Sprite Color To Foreground Color
//            Sprite Vertical Location Set To 209 (Mode 0..3) Or 217 (Mode 4..8)
//     Input: SCRMOD ($FCAF) = Screen Mode
//    Output: NONE
// Registers: ALL
// Available: MSX (Always Call SUB-ROM)
#define BIOS_CLRSPR		0x0069	//

//  Function: Initialise Screen To TEXT1 Mode (40x24)
//            Palette Is Not Initialised
//            To Initialise Palette Call INIPLT In SUB-ROM After This Call
//     Input: TXTNAM ($F3B3) = Pattern Name Table
//            TXTCGP ($F3B7) = Pattern Generator Table
//            LINL40 ($F3AE) = Line Length
//    Output: NONE
// Registers: ALL
// Available: MSX (Always Call SUB-ROM)
#define BIOS_INITXT		0x006c	//

//  Function: Initialise Screen To GRAPHIC1 Mode (32x24)
//            Palette Is Not Initialised
//            To Initialise Palette Call INIPLT In SUB-ROM After This Call
//     Input: T32NAM ($F3BD) = Pattern Name Table
//            T32COL ($F3BF) = Color Table
//            T32CGP ($F3C1) = Pattern Generator Table
//            T32ATR ($F3C3) = Sprite Attribute Table
//            T32PAT ($F3C5) = Sprite Generator Table
//    Output: NONE
// Registers: ALL
// Available: MSX (Always Call SUB-ROM)
#define BIOS_INIT32		0x006f	//

//  Function: Initialise Screen To High-Resolution Graphics Mode
//            Palette Is Not Initialised
//            To Initialise Palette Call INIPLT In SUB-ROM After This Call
//     Input: GRPNAM ($F3C7) = Pattern Name Table
//            GRPCOL ($F3C9) = Color Table
//            GRPCGP ($F3CB) = Pattern Generator Table
//            GRPATR ($F3CD) = Sprite Attribute Table
//            GRPPAT ($F3CF) = Sprite Generator Table
//    Output: NONE
// Registers: ALL
// Available: MSX (Always Call SUB-ROM)
#define BIOS_INIGRP		0x0072	//

//  Function: Initialise Screen To MULTI COLOR Mode
//            Palette Is Not Initialised
//            To Initialise Palette Call INIPLT In SUB-ROM After This Call
//     Input: MLTNAM ($F3D1) = Pattern Name Table
//            MLTCOL ($F3D3) = Color Table
//            MLTCGP ($F3D5) = Pattern Generator Table
//            MLTATR ($F3D7) = Sprite Attribute Table
//            MLTPAT ($F3D9) = Sprite Generator Table
//    Output: NONE
// Registers: ALL
// Available: MSX (Always Call SUB-ROM)
#define BIOS_INIMLT		0x0075	//

//  Function: Set Only VDP To TEXT1 Mode (40x24)
//            Palette Is Not Initialised
//            To Initialise Palette Call INIPLT In SUB-ROM After This Call
//     Input: TXTNAM ($F3B3) = Pattern Name Table
//            TXTCGP ($F3B7) = Pattern Generator Table
//            LINL40 ($F3AE) = Line Length
//    Output: NONE
// Registers: ALL
// Available: MSX (Always Call SUB-ROM)
#define BIOS_SETTXT		0x0078	//

//  Function: Set Only VDP To GRAPHIC1 Mode (32x24)
//            Palette Is Not Initialised
//            To Initialise Palette Call INIPLT In SUB-ROM After This Call
//     Input: T32NAM ($F3BD) = Pattern Name Table
//            T32COL ($F3BF) = Color Table
//            T32CGP ($F3C1) = Pattern Generator Table
//            T32ATR ($F3C3) = Sprite Attribute Table
//            T32PAT ($F3C5) = Sprite Generator Table
//    Output: NONE
// Registers: ALL
// Available: MSX (Always Call SUB-ROM)
#define BIOS_SETT32		0x007b	//

//  Function: Set Only VDP To GRAPHIC2 Mode
//            Palette Is Not Initialised
//            To Initialise Palette Call INIPLT In SUB-ROM After This Call
//     Input: GRPNAM ($F3C7) = Pattern Name Table
//            GRPCOL ($F3C9) = Color Table
//            GRPCGP ($F3CB) = Pattern Generator Table
//            GRPATR ($F3CD) = Sprite Attribute Table
//            GRPPAT ($F3CF) = Sprite Generator Table
//    Output: NONE
// Registers: ALL
// Available: MSX (Always Call SUB-ROM)
#define BIOS_SETGRP		0x007e	//

//  Function: Set Only VDP To MULTI COLOR Mode
//            Palette Is Not Initialised
//            To Initialise Palette Call INIPLT In SUB-ROM After This Call
//     Input: MLTNAM ($F3D1) = Pattern Name Table
//            MLTCOL ($F3D3) = Color Table
//            MLTCGP ($F3D5) = Pattern Generator Table
//            MLTATR ($F3D7) = Sprite Attribute Table
//            MLTPAT ($F3D9) = Sprite Generator Table
//    Output: NONE
// Registers: ALL
// Available: MSX (Always Call SUB-ROM)
#define BIOS_SETMLT		0x0081	//

//  Function: Return Address Of Sprite Generator Table
//     Input:  A = Sprite Number
//    Output: HL = Returned Address
// Registers: AF, DE, HL
// Available: MSX
#define BIOS_CALPAT		0x0084	//

//  Function: Return Address Of Sprite Attribute Table
//     Input:  A = Sprite Number
//    Output: HL = Returned Address
// Registers: AF, DE, HL
// Available: MSX
#define BIOS_CALATR		0x0087	//

//  Function: Return Current Sprite Size
//     Input: NONE
//    Output: A = Sprite Size (In Bytes)
//            CY-Flag Set IF (Size == 16x16), ELSE CY-Flag Reset
// Registers: AF
// Available: MSX
#define BIOS_GSPSIZ		0x008a	//

//  Function: Display A Character On The Graphic-Screen
//     Input: A = Character Code To Be Displayed
//            Set Logical Operation Code In LOGOPR ($FB02) IF Screen Mode 0..8
//    Output: NONE
// Registers: NONE
// Available: MSX (Call SUB-ROM Internally In Screen Modes 5..8)
#define BIOS_GRPPRT		0x008d	//

//================================
// PSG Audio

//  Function: Initialise PSG & Set Initial Value For PLAY Statement
//     Input: NONE
//    Output: NONE
// Registers: ALL
// Available: MSX
#define BIOS_GICINI		0x0090	//

//  Function: Write Data To PSG Register
//     Input: A = PSG Register Number, E = Data To Write
//    Output: NONE
// Registers: NONE
// Available: MSX
#define BIOS_WRTPSG		0x0093	//

//  Function: Read PSG Register Data
//     Input: A = PSG Register Number
//    Output: A = Data Which Was Read
// Registers: NONE
// Available: MSX
#define BIOS_RDPSG		0x0096	//

//  Function: Test IF The PLAY Statement Is Being Executed As A Background Task
//            IF Not, Begin To Execute The PLAY Statement
//     Input: NONE
//    Output: NONE
// Registers: ALL
// Available: MSX
#define BIOS_STRTMS		0x0099	//

//================================
// Keyboard, CRT, Printer I/O

//  Function: Test Status Of Keyboard Buffer
//     Input: NONE
//    Output: Z-Flag Set IF Buffer Is Empty, ELSE Z-Flag Reset
// Registers: AF
// Available: MSX
#define BIOS_CHSNS		0x009c	//

//  Function: Get Character Input (Waiting)
//     Input: NONE
//    Output: A = Code Of Input Character
// Registers: AF
// Available: MSX
#define BIOS_CHGET		0x009f

//  Function: Display A Character To The Screen
//     Input: A = Character Code To Be Displayed
//    Output: NONE
// Registers: NONE
// Available: MSX
#define BIOS_CHPUT		0x00a2	//

//  Function: Send A Character To The Printer
//     Input: A = Character Code To Be Sent
//    Output: IF Failed CY-Flag Set, ELSE CY-Flag Reset
// Registers: F
// Available: MSX
#define BIOS_LPTOUT		0x00a5	//

//  Function: Test Printer Status
//     Input: NONE
//    Output: IF (A == 255) && (Z-Flag Reset), Printer Is READY
//            IF (A == 0) && (Z-Flag Set), Printer Is NOT READY
// Registers: AF
// Available: MSX
#define BIOS_LPTSTT		0x00a8	//

//  Function: Test Graphic Header & Transform The Code
//     Input: A = Character Code
//    Output: IF NOT Graphic Header CY-Flag Reset
//            CY-Flag Set, & Z-Flag Set To The Transformed Code Set In A
//            CY-Flag Set, & CY-Flag is Reset To The Utransformed Code Set In A
// Registers: AF
// Available: MSX
#define BIOS_CNVCHR		0x00ab	//

//  Function: Store To Specified Buffer Character Code Input Until RETURN Or STOP Key Pressed
//     Input: NONE
//    Output: HL = Start Address Of Buffer -1
//            CY-Flag Set IF STOP Key Pressed
// Registers: ALL
// Available: MSX
#define BIOS_PINLIN		0x00ae	//

//  Function: Store To Specified Buffer Character Code Input Until RETURN Or STOP Key Pressed
//     Input: NONE
//    Output: HL = Start Address Of Buffer -1
//            CY-Flag Set IF STOP Key Pressed
//            AUTFLG ($F6AA) Set
// Registers: ALL
// Available: MSX
#define BIOS_INLIN		0x00b1	//

//  Function: Store To Specified Buffer Character Code Input Until RETURN Or STOP Key Pressed
//            Displays A Question Mark "?" & A Single Space " "
//     Input: NONE
//    Output: HL = Start Address Of Buffer -1
//            CY-Flag Set IF STOP Key Pressed
//            AUTFLG ($F6AA) Set
// Registers: ALL
// Available: MSX
#define BIOS_QINLIN		0x00b4	//

//  Function: Test CTRL-STOP Key, Interrupts Are Inhibited
//     Input: NONE
//    Output: CY-Flag Set IF CTRL-STOP Key Pressed
// Registers: AF
// Available: MSX
#define BIOS_BREAKX		0x00b7	//

//  Function: Test SHIFT-STOP Key, Interrupts Are Inhibited
//     Input: NONE
//    Output: CY-Flag Set IF SHIFT-STOP Key Pressed
// Registers: AF
// Available: MSX
#define BIOS_ISCNTC		0x00ba	//

//  Function: Test SHIFT-STOP Key, Interrupts Are Inhibited (Same As ISCNTC, Used In BASIC)
//     Input: NONE
//    Output: CY-Flag Set IF SHIFT-STOP Key Pressed
// Registers: AF
// Available: MSX
#define BIOS_CKCNTC		0x00bd	//

//  Function: Generate BEEP
//     Input: NONE
//    Output: NONE
// Registers: ALL
// Available: MSX (Always Call SUB-ROM)
#define BIOS_BEEP		0x00c0	//

//  Function: Clear Screen
//     Input: Set Z-Flag (e.g XOR A)
//    Output: NONE
// Registers: AF, BC, DE
// Available: MSX (Always Call SUB-ROM)
#define BIOS_CLS		0x00c3

//  Function: Move Cursor
//     Input: H = X-Position Of Cursor, L = Y-Position Of Cursor
//    Output: NONE
// Registers: AF
// Available: MSX
#define BIOS_POSIT		0x00c6	//

//  Function: Test Whether Function Key Display Is Active (FNKFLG)
//            IF TRUE, Display Them, ELSE Erase Them
//     Input: FNKFLG ($FBCE)
//    Output: NONE
// Registers: ALL
// Available: MSX
#define BIOS_FNKSB		0x00c9	//

//  Function: Erase Function Key Display
//     Input: NONE
//    Output: NONE
// Registers: ALL
// Available: MSX
#define BIOS_ERAFNK		0x00cc	//Erase function keys display

//  Function: Display Function Keys
//     Input: NONE
//    Output: NONE
// Registers: ALL
// Available: MSX (Call SUB-ROM Internally In Screen Modes 5..8)
#define BIOS_DSPFNK		0x00cf	//

//  Function: Force Screen To Text Mode
//     Input: NONE
//    Output: NONE
// Registers: ALL
// Available: MSX
#define BIOS_TOTEXT		0x00d2	//

//================================
// Game I/O Access

//  Function: Return Joystick Direction Status
//     Input: A = Joystick Number To Test
//                0 = cursors, 1 = port 1, 2 = port 2
//    Output: A = Joystick Direction Status
// Registers: ALL
// Available: MSX
#define BIOS_GTSTCK		0x00d5

//  Function: Return Trigger Button Status
//     Input: A = Trigger Button Number To Test
//                0:spacebar, 1:port1 buttonA, 2:port2 buttonA, 3:port1 buttonB, 4:port2 buttonB
//    Output: IF (A == $00) Trigger Button Is Not Pressed
//            IF (A == $FF) Trigger Button Is Pressed
// Registers: AF
// Available: MSX
#define BIOS_GTTRIG		0x00d8	//

//  Function: Return Touch Pad Status
//     Input: A = Touch Pad Number To Test
//                 [0] Fetch touch pad data from port 1 (#FF if available)
//                 [1] Read X-position
//                 [2] Read Y-position
//                 [3] Read touchpad status from port 1 (#FF if pressed)
//                 [4] Fetch touch pad data from port 2 (#FF if available)
//                 [5] Read X-position
//                 [6] Read Y-position
//                 [7] Read touchpad status from port 2 (#FF if pressed)
//    Output: A = Touch Pad Status
// Registers: ALL
// Available: MSX
#define BIOS_GTPAD		0x00db	//

//  Function: Return Paddle Value
//     Input: A = Paddle Number To Test
//    Output: A = Paddle Value
// Registers: ALL
// Available: MSX (Call SUB-ROM Internally In Screen Modes 5..8)
#define BIOS_GTPDL		0x00de	//

//================================
// Cassette I/O Routines

//  Function: Read Header Block After Turning Cassette Motor ON
//     Input: NONE
//    Output: CY-Flag Set IF Failed
// Registers: ALL
// Available: MSX
#define BIOS_TAPION		0x00e1	//

//  Function: Read Data From Tape
//     Input: NONE
//    Output: A = Tape Data, CY-Flag Set IF Failed
// Registers: ALL
// Available: MSX
#define BIOS_TAPIN		0x00e4	//

//  Function: Stop Reading Data From Tape
//     Input: NONE
//    Output: NONE
// Registers: NONE
// Available: MSX
#define BIOS_TAPIOF		0x00e7	//

//  Function: Write Header Block After Turning Cassette Motor ON
//     Input: IF (A == 0) Short Header, IF (A != 0) Long Header
//    Output: CY-Flag Set IF Failed
// Registers: ALL
// Available: MSX
#define BIOS_TAPOON		0x00ea	//

//  Function: Write Data To Tape
//     Input: A = Data To Write
//    Output: CY-Flag Set IF Failed
// Registers: ALL
// Available: MSX
#define BIOS_TAPOUT		0x00ed	//

//  Function: Stop Writing Data To Tape
//     Input: A = Data To Write
//    Output: CY-Flag Set IF Failed
// Registers: ALL
// Available: MSX
#define BIOS_TAPOOF		0x00f0	//

//  Function: Set Cassette Motor Action
//     Input: IF (A == $00) Stop
//            IF (A == $01) Start
//            IF (A == $FF) Reverse Current Action
//    Output: NONE
// Registers: AF
// Available: MSX
#define BIOS_STMOTR		0x00f3	//

//================================
// Queue routines

//  Function: Gives Number Of Bytes In Queue (Internal Use)
//     Input: NONE
//    Output: A = Length Of Queue In Bytes
// Registers: AF
// Available: MSX
#define BIOS_LFTQ		0x00f6	//

//  Function: Put Byte In Queue (Internal Use)
//     Input: NONE
//    Output: NONE
// Registers: NONE
// Available: MSX
#define BIOS_PUTQ		0x00f9	//

//================================
// Graphic Routines

//  Function: Move Pixel Right
//     Input: CLOC = X-Coordinate, CMASK = Y-Coordinate
//    Output: CLOC = New X-Coordinate, CMASK = New Y-Coordinate
// Registers: AF
// Available: MSX (SCREEN 3 Only)
#define BIOS_RIGHTC		0x00fc	//

//  Function: Move Pixel Left
//     Input: CLOC = X-Coordinate, CMASK = Y-Coordinate
//    Output: CLOC = New X-Coordinate, CMASK = New Y-Coordinate
// Registers: AF
// Available: MSX (SCREEN 3 Only)
#define BIOS_LEFTC		0x00ff	//

//  Function: Move Pixel Up
//     Input: CLOC = X-Coordinate, CMASK = Y-Coordinate
//    Output: CLOC = New X-Coordinate, CMASK = New Y-Coordinate
// Registers: AF
// Available: MSX (SCREEN 3 Only)
#define BIOS_UPC		0x0102	//

//  Function: Test Against Screen Border, IF Inside Screen Border, Execute UPC (Move Pixel Up)
//     Input: CLOC = X-Coordinate, CMASK = Y-Coordinate
//    Output: CLOC = New X-Coordinate, CMASK = New Y-Coordinate
//              CY-Flag Set IF Operation Would End Outside Screen Border
// Registers: AF
// Available: MSX (SCREEN 3 Only)
#define BIOS_TUPC		0x0105	//

//  Function: Move Pixel Down
//     Input: CLOC = X-Coordinate, CMASK = Y-Coordinate
//    Output: CLOC = New X-Coordinate, CMASK = New Y-Coordinate
// Registers: AF
// Available: MSX (SCREEN 3 Only)
#define BIOS_DOWNC		0x0108	//

//  Function: Test Against Screen Border, IF Inside Screen Border, Execute DOWNC (Move Pixel Down)
//     Input: CLOC = X-Coordinate, CMASK = Y-Coordinate
//    Output: CLOC = New X-Coordinate, CMASK = New Y-Coordinate
//              CY-Flag Set IF Operation Would End Outside Screen Border
// Registers: AF
// Available: MSX (SCREEN 3 Only)
#define BIOS_TDOWNC		0x010b	//

//  Function: Clip X,Y Coordinates (Against Screen Border)
//     Input: BC = X-Coordinate, DE = Y-Coordinate
//    Output: BC = X-Coordinate Clipped, DE = Y-Coordinate Clipped
// Registers: AF
// Available: MSX
#define BIOS_SCALXY		0x010e	//

//  Function: Place Cursor At Current Cursor Address
//     Input: NONE
//    Output: NONE
// Registers: NONE
// Available: MSX
#define BIOS_MAPXY		0x0111	//

//  Function: Get Current Cursor Addresses Mask Pattern
//     Input: NONE
//    Output: HL = Cursor Address
//             A = Mask Pattern
// Registers: NONE
// Available: MSX
#define BIOS_FETCHC		0x0114	//

//  Function: Record Current Cursor Addresses Mask Pattern
//     Input: HL = Cursor Address
//             A = Mask Pattern
//    Output: NONE
// Registers: NONE
// Available: MSX
#define BIOS_STOREC		0x0117	//

//  Function: Set Attribute Byte In ATRBYT ($F3F2)
//     Input: A = Attribute
//    Output: CY-Flag Set IF Wrong Attribute
// Registers: F
// Available: MSX
#define BIOS_SETATR		0x011a	//

//  Function: Read Attribute Byte Of Current Screen Pixel
//     Input: CLOC = X-Coordinate, CMASK = Y-Coordinate
//    Output: A = Pixel Attribute
// Registers: AF
// Available: MSX
#define BIOS_READC		0x011d	//

//  Function: Set Attribute Of Pixel
//     Input:   CLOC = X-Coordinate, CMASK = Y-Coordinate
//            ATRBYT = Attribute Byte
//    Output: NONE
// Registers: AF
// Available: MSX
#define BIOS_SETC		0x0120	//

//  Function: Set Horizontal Screen Pixels
//     Input: NONE
//    Output: NONE
// Registers: NONE
// Available: MSX
#define BIOS_NSETCX		0x0123	//

//  Function: Get Screen Relations
//     Input: NONE
//    Output: DE, HL
// Registers: DE, HL
// Available: MSX
#define BIOS_GTASPC		0x0126	//

//  Function: Initalises PAINT Instruction
//     Input: NONE
//    Output: NONE
// Registers: NONE
// Available: MSX
#define BIOS_PNTINI		0x0129	//

//  Function: Scan Screen Pixels Right
//     Input: B = "Suspend" Flag, C = Border-Counting
//    Output: C = "Pixel-Changed" Flag, DE = Border-Counting
// Registers: ALL
// Available: MSX
#define BIOS_SCANR		0x012c	//

//  Function: Scan Screen Pixels Left
//     Input: B = "Suspend" Flag, C = Border-Counting
//    Output: C = "Pixel-Changed" Flag, DE = Border-Counting
// Registers: ALL
// Available: MSX
#define BIOS_SCANL		0x012f	//

//================================
// Miscellaneous

//  Function: Alternate CAP Lamp Status
//     Input: IF (A == 0) Lamp OFF, IF (A != 0) Lamp ON
//    Output: NONE
// Registers: AF
// Available: MSX
#define BIOS_CHGCAP		0x0132	//

//  Function: Alternate 1-Bit Sound Port Status
//     Input: IF (A == 0) Sound Port OFF
//            IF (A != 0) Sound Port ON
//    Output: NONE
// Registers: AF
// Available: MSX
#define BIOS_CHGSND		0x0135	//

//  Function: Read Contents Of Current Output To BASIC Slot Register
//     Input: NONE
//    Output: A = Value Which Was Read
// Registers: A
// Available: MSX
#define BIOS_RSLREG		0x0138	//

//  Function: Write To Primary Slot Register
//     Input: A = Value To Write
//    Output: NONE
// Registers: NONE
// Available: MSX
#define BIOS_WSLREG		0x013b	//

//  Function: Read VDP Status Register
//     Input: NONE
//    Output: A = Value Which Was Read
// Registers: A
// Available: MSX
#define BIOS_RDVDP		0x013e	//

//  Function: Read Data Of Specified Line From Keyboard Matrix
//     Input: A = Specified Line
//    Output: A = Data Which Was Read (Bit Corresponding To Pressed Key Will Be 0)
// Registers: AF, C
// Available: MSX
#define BIOS_SNSMAT		0x0141	//

//  Function: Physical Input/Output For DISK Devices
//     Input:  A = Drive Number (0 = A:, 1 = B:,...)
//             B = Number Of Sector To Be Read From Or Written To
//             C = Media ID
//            DE = First Sector Number To Be Read From Or Written To
//            HL = Starting Address Of RAM Buffer To Be Read From Or Written To Specified Sectors
//            CY-Flag Set For Sector Writing, CY-Flag Reset For Sector Reading
//    Output: CY-Flag Set IF Failed
//             B = Number Of Sectors Actually Read Or Written
//             A = Error Code (Only IF CY-Flag Set):
//                 0 = Write Protected
//                 2 = Not Ready
//                 4 = Data Error
//                 6 = Seek Error
//                 8 = Record Not Found
//                10 = Write Error
//                12 = Bad Parameter
//                14 = Out Of Memory
//                16 = Other Error
// Registers: ALL
// Available: MSX
#define BIOS_PHYDIO		0x0144	//

//  Function: Initialises Mass-Storage Media Like Formatting Of Disks
//     Input: NONE
//    Output: NONE
// Registers: ALL
// Available: MSX (In Minimum Configuration Only A HOOK Is Available)
#define BIOS_FORMAT		0x0147	//

//  Function: Test Whether Device Is Active
//     Input: NONE
//    Output: IF (A == 0) Active
//            IF (A != 0) Inactive
// Registers: AF
// Available: MSX
#define BIOS_ISFLIO		0x014a	//

//  Function: Printer Output, Different From LPTOUT In The Following Points:
//            1. TAB Is Expanded To Spaces
//            2. For Non-MSX Printers, Hiragana Is Transformed To Katakana
//               & Graphic Characters Are Transformed To	1-Byte characters
//            3. Device I/O Error Occurs IF Failed
//     Input: A = Data
//    Output: NONE
// Registers: F
// Available: MSX
#define BIOS_OUTDLP		0x014d	//

//  Function: Return Pointer To Play Queue (Only Used To Play Music In Background)
//     Input: A = Channel Number
//    Output: HL = Pointer
// Registers: AF
// Available: MSX
#define BIOS_GETVCP		0x0150	//

//  Function: Return Pointer To Variable In Queue Number VOICEN (Byte Op $FB38)
//     Input: L = Pointer In Play Buffer
//    Output: HL = Pointer
// Registers: AF
// Available: MSX
#define BIOS_GETVC2		0x0153	//

//  Function: Clear Keyboard Buffer
//     Input: NONE
//    Output: NONE
// Registers: HL
// Available: MSX
#define BIOS_KILBUF		0x0156

//  Function: Executes Inter-Slot Call To Routine In BASIC Interpreter
//     Input: IX = Call Address
//    Output: Depends On Called Routine
// Registers: Depends On Called Routine
// Available: MSX
#define BIOS_CALBAS		0x0159	//

//================================
// Entries appended for MSX2

//  Function: Execute Inter-Slot Call To SUB-ROM
//     Input: IX = Call Address, Pushes IX On Stack
//    Output: Depends On Called Routine
// Registers: Background Registers & IY Are Reserved
// Available: MSX2
#define BIOS_SUBROM		0x015c	//

//  Function: Execute Inter-Slot Call To SUB-ROM
//     Input: IX For The Call Address
//    Output: Depends On Called Routine
// Registers: Background Registers & IY Are Reserved
// Available: MSX2
#define BIOS_EXTROM		0x015f	//

//  Function: Search Slots For SUB-ROM
//     Input: NONE
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_CHKSLZ		0x0162	//

//  Function: Test Screen Mode
//     Input: NONE
//    Output: CY-Flag Set IF Screen Mode = 5, 6, 7 Or 8 
// Registers: AF
// Available: MSX2
#define BIOS_CHKNEW		0x0165	//

//  Function: Delete To End Of Line
//     Input: H = X-Position Of Cursor, L = Y-Position Of Cursor
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_EOL		0x0168	//

//  Function: Same Function As FILVRM, Differences Are As Follows:
//            In FILVRM, It Is Tested Whether Screen Mode Is 0..3
//            IF TRUE, It Treats VDP As Though It Only Has 16KB VRAM
//            (For Compatibility With MSX1)
//            In BIGFIL, The Screen Mode Is Not Tested
//            & Actions Are Carried Out By Given Parameters
//     Input: HL = VRAM Destination Address, BC = Data Length, A = Data To Repeat
//    Output: NONE
// Registers: AF, BC
// Available: MSX2 (Do Not Call SUB-ROM While Screen Modes 4..8 Are Changed)
#define BIOS_BIGFIL		0x016b	//

//  Function: Set VRAM (16-Bit Address: $0000..$FFFF) In VDP & Enable It To Be Read
//            This Is Used To Read Auto-Increment Data From VRAM
//            Enables Faster Reads Than Using NRDVRM In A Loop
//     Input: HL = VRAM Address
//    Output: NONE
// Registers: AF
// Available: MSX2
#define BIOS_NSETRD		0x016e	//

//  Function: Set VRAM (16-Bit Address: $0000..$FFFF) In VDP & Enable It To Be Written
//            This Is Used To Write Auto-Increment Data To VRAM
//            Enables Faster Writes Than Using NWRVRM In A Loop
//     Input: HL = VRAM Address
//    Output: NONE
// Registers: AF
// Available: MSX2
#define BIOS_NSTWRT		0x0171	//

//  Function: Read Data From VRAM (16-Bit Address: $0000..$FFFF)
//     Input: HL = VRAM Address
//    Output: A = Value Which Was Read
// Registers: F
// Available: MSX2
#define BIOS_NRDVRM		0x0174	//

//  Function: Write Data To VRAM (16-Bit Address: $0000..$FFFF)
//     Input: HL = VRAM Address, A = Data To Write
//    Output: NONE
// Registers: AF
// Available: MSX2
#define BIOS_NWRVRM		0x0177	//

//================================
// Entries appended for MSX2+

//  Function: Read Value Of I/O Port $F4
//     Input: NONE
//    Output: A = Value Read
// Registers: AF
// Available: MSX2+
#define BIOS_RDBTST		0x017a	//

//  Function: Write Value To I/O Port $F4
//     Input: A = Value To Write (Bit 7 = Show MSX2+ Startup Screen(0), Skipped(1))
//    Output: NONE
// Registers: NONE
// Available: MSX2+
#define BIOS_WRBTST		0x017d	//

//================================
// Entries appended for MSX Turbo-R

//  Function: Change CPU Mode
//     Input: A = Settings: %L00000MM - L = LED (Indicates IF Turbo LED Is Switched With CPU Mode)
//            M = CPU Mode - 0 = Z80 ROM Mode, 1 = R800 ROM Mode, 2 = R800 DRAM Mode
//    Output: NONE
// Registers: NONE
// Available: MSX Turbo-R
#define BIOS_CHGCPU		0x0180	//

//  Function: Returns Current CPU Mode
//     Input: NONE
//    Output: A = Settings: %L00000MM - L = LED (Indicates IF Turbo LED Is Switched With CPU Mode
//            M = CPU Mode - 0 = Z80 ROM Mode, 1 = R800 ROM Mode, 2 = R800 DRAM Mode
// Registers: AF
// Available: MSX Turbo-R
#define BIOS_GETCPU		0x0183	//

//  Function: Play Specified Memory Area Through PCM Chip
//     Input:  A = Settings: %V00000QQ - V = VRAM Usage Flag, Q = Quality Parameter (Speed: 0 = Fast)
//            HL = Start Address In RAM Or VRAM
//            BC = Length Of Area To Play
//             D = Bit 0 Becomes Bit 17 Of Area Length When Using VRAM
//             E = Bit 0 Becomes Bit 17 Of Start Address When Using VRAM
//    Output: CY-Flag Set IF Aborted With CTRL-STOP
// Registers: ALL
// Available: MSX Turbo-R
#define BIOS_PCMPLY		0x0186	//

//  Function: Record Audio Using PCM Chip To Specified Memory Area
//     Input:  A = Settings: %VTTTTCQQ - V = VRAM Usage Flag, T = Treshold,
//             C = Zero-Data Compression, Q = Quality Parameter (Speed: 0 = Fast)
//            HL = Start Address In RAM Or VRAM
//            BC = Length Of Area To Record
//             D = Bit 0 Becomes Bit 17 Of Area Length When Using VRAM
//             E = Bit 0 Becomes Bit 17 Of Start Address When Using VRAM
//    Output: CY-Flag Set IF Aborted With CTRL-STOP
// Registers: ALL
// Available: MSX Turbo-R
#define BIOS_PCMREC		0x0189	//

//=====================================
// SUB-ROM
//=====================================

//================================
// BASIC Routines

//  Function: Paint Graphic-Screen
//     Input: HL = BASIC Text-Pointer
//    Output: HL = Adapted BASIC Text-Pointer
// Registers: ALL
// Available: MSX2 (BASIC!)
#define BIOS_PAINT		0x0069	//

//  Function: Set Point
//     Input: HL = BASIC Text-Pointer
//    Output: HL = Adapted BASIC Text-Pointer
// Registers: ALL
// Available: MSX2 (BASIC!)
#define BIOS_PSET		0x006d	//

//  Function: Scans Color Attribute
//     Input: HL = BASIC Text-Pointer
//    Output: HL = Adapted BASIC Text-Pointer
// Registers: ALL
// Available: MSX2 (BASIC!)
#define BIOS_ATRSCN		0x0071	//

//  Function: Draw Line
//     Input: HL = BASIC Text-Pointer
//    Output: HL = Adapted BASIC Text-Pointer
// Registers: ALL
// Available: MSX2 (BASIC!)
#define BIOS_GLINE		0x0075	//

//  Function: Draw Filled Box
//     Input: HL = BASIC Text-Pointer
//    Output: HL = Adapted BASIC Text-Pointer
// Registers: ALL
// Available: MSX2 (BASIC!)
#define BIOS_DOBOXF		0x0079	//

//  Function: Draw Line
//     Input: HL = BASIC Text-Pointer
//    Output: HL = Adapted BASIC Text-Pointer
// Registers: ALL
// Available: MSX2 (BASIC!)
#define BIOS_DOLINE		0x007d	//

//  Function: Draw Box
//     Input: HL = BASIC Text-Pointer
//    Output: HL = Adapted BASIC Text-Pointer
// Registers: ALL
// Available: MSX2 (BASIC!)
#define BIOS_BOXLIN		0x0081	//

//================================
// NON-BASIC Routines

//  Function: Draw Line
//     Input: Start Point: BC = X-Coordinate, HL = Y-Coordinate
//              End Point:  GXPOS ($FCB3) = X-Coordinate, GYPOS ($FCB5) = Y-Coordinate
//                  Color: ATRBYT ($F3F3) = Attribute
// Logical Operation Code: LOGOPR ($FB02) = Code
//    Output: NONE
// Registers: AF
// Available: MSX2
#define BIOS_DOGRPH		0x0085	//

//  Function: Display A Character On The Graphic-Screen (Active In Screen Modes 5..8)
//     Input:   Character: A = Character Code To Be Displayed
//                  Color: ATRBYT ($F3F3) = Attribute
// Logical Operation Code: LOGOPR ($FB02) = Code
//    Output: NONE
// Registers: NONE
// Available: MSX2
#define BIOS_GRPRT		0x0089	//

//  Function: Clip X,Y Coordinates (Against Screen Border) (Same As SCALXY In MAIN-ROM)
//     Input: BC = X-Coordinate, DE = Y-Coordinate
//    Output: BC = Clipped X-Coordinate, DE = Clipped Y-Coordinate
// Registers: AF
// Available: MSX2
#define BIOS_SCALXYB	0x008d	//

//  Function: Convert X,Y Coordinates Position To Address: & Mask In CLOC & CMASK
//     Input: BC = X-Coordinate, DE = Y-Coordinate
//    Output: HL = VRAM Address: In SCREEN 3 (Also In CLOC)
//             A = Mask:         In SCREEN 3 (Also In CMASK)
//            HL = X-Coordinate: In SCREEN 5..8 (Also In CLOC)
//             A = Y-Coordinate: In SCREEN 5..8 (Also In CMASK)
// Registers: F
// Available: MSX2
#define BIOS_MAPXYC		0x0091	//

//  Function: Read Attribute Byte Of Current Screen Pixel (Same As READC In MAIN-ROM)
//     Input: CLOC = X-Coordinate, CMASK = Y-Coordinate
//    Output: A = Pixel Attribute
// Registers: AF
// Available: MSX2
#define BIOS_READCB		0x0095	//

//  Function: Set Attribute Byte In ATRBYT ($F3F2) (Same As SETATR In MAIN-ROM)
//     Input: A = Attribute
//    Output: CY-Flag Set IF Wrong Attribute
// Registers: F
// Available: MSX2
#define BIOS_SETATRB	0x0099	//

//  Function: Set Attribute Of Pixel (Same As SETC In MAIN-ROM)
//     Input:   CLOC = X-Coordinate, CMASK = Y-Coordinate
//            ATRBYT = Attribute Byte
//    Output: NONE
// Registers: AF
// Available: MSX2
#define BIOS_SETCB		0x009d	//

//  Function: Test Against Screen Border, IF Inside Screen Border, Execute RIGHTC (Move Pixel Right)
//     Input: CLOC = X-Coordinate, CMASK = Y-Coordinate
//    Output: CLOC = New X-Coordinate, CMASK = New Y-Coordinate
//              CY-Flag Set IF Operation Would End Outside Screen Border
// Registers: AF
// Available: MSX2 (SCREEN 3 Only)
#define BIOS_TRIGHT		0x00a1	//

//  Function: Move Pixel Right (Same As RIGHTC In MAIN-ROM)
//     Input: CLOC = X-Coordinate, CMASK = Y-Coordinate
//    Output: CLOC = New X-Coordinate, CMASK = New Y-Coordinate
// Registers: AF
// Available: MSX2 (SCREEN 3 Only)
#define BIOS_RIGHTCB	0x00a5	//

//  Function: Test Against Screen Border, IF Inside Screen Border, Execute LEFTC (Move Pixel Left)
//     Input: CLOC = X-Coordinate, CMASK = Y-Coordinate
//    Output: CLOC = New X-Coordinate, CMASK = New Y-Coordinate
//              CY-Flag Set IF Operation Would End Outside Screen Border
// Registers: AF
// Available: MSX2 (SCREEN 3 Only)
#define BIOS_TLEFTCB	0x00a9	//

//  Function: Move Pixel Left (Same As LEFTC In MAIN-ROM)
//     Input: CLOC = X-Coordinate, CMASK = Y-Coordinate
//    Output: CLOC = New X-Coordinate, CMASK = New Y-Coordinate
// Registers: AF
// Available: MSX2 (SCREEN 3 Only)
#define BIOS_LEFTCB		0x00ad	//

//  Function: Test Against Screen Border, IF Inside Screen Border, Execute DOWNC (Move Pixel Down) (Same As TDOWNC In MAIN-ROM)
//     Input: CLOC = X-Coordinate, CMASK = Y-Coordinate
//    Output: CLOC = New X-Coordinate, CMASK = New Y-Coordinate
//              CY-Flag Set IF Operation Would End Outside Screen Border
// Registers: AF
// Available: MSX2 (SCREEN 3 Only)
#define BIOS_TDOWNCB	0x00b1	//

//  Function: Move Pixel Down (Same As DOWNC In MAIN-ROM)
//     Input: CLOC = X-Coordinate, CMASK = Y-Coordinate
//    Output: CLOC = New X-Coordinate, CMASK = New Y-Coordinate
// Registers: AF
// Available: MSX2 (SCREEN 3 Only)
#define BIOS_DOWNCB		0x00b5	//

//  Function: Test Against Screen Border, IF Inside Screen Border, Execute UPC (Move Pixel Up) (Same As TUPC In MAIN-ROM)
//     Input: CLOC = X-Coordinate, CMASK = Y-Coordinate
//    Output: CLOC = New X-Coordinate, CMASK = New Y-Coordinate
//              CY-Flag Set IF Operation Would End Outside Screen Border
// Registers: AF
// Available: MSX2 (SCREEN 3 Only)
#define BIOS_TUPCB		0x00b9	//

//  Function: Move Pixel Up (Same As UPC In MAIN-ROM)
//     Input: CLOC = X-Coordinate, CMASK = Y-Coordinate
//    Output: CLOC = New X-Coordinate, CMASK = New Y-Coordinate
// Registers: AF
// Available: MSX2 (SCREEN 3 Only)
#define BIOS_UPCB		0x00bd	//

//  Function: Scan Screen Pixels Right (Same As SCANR In MAIN-ROM)
//     Input: B = "Suspend" Flag, C = Border-Counting
//    Output: C = "Pixel-Changed" Flag, DE = Border-Counting
// Registers: ALL
// Available: MSX2
#define BIOS_SCANRB		0x00c1	//

//  Function: Scan Screen Pixels Left (Same As SCANL In MAIN-ROM)
//     Input: B = "Suspend" Flag, C = Border-Counting
//    Output: C = "Pixel-Changed" Flag, DE = Border-Counting
// Registers: ALL
// Available: MSX2
#define BIOS_SCANLB		0x00c5	//

//  Function: Draw A Box
//     Input: Start Point: BC = X-Coordinate, DE = Y-Coordinate
//              End Point:  GXPOS ($FCB3) = X-Coordinate, GYPOS ($FCB5) = Y-Coordinate
//                  Color: ATRBYT ($F3F3) = Attribute
// Logical Operation Code: LOGOPR ($FB02) = Code
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_NVBXLN		0x00c9	//

//  Function: Draw Filled Box
//     Input: Start Point: BC = X-Coordinate, DE = Y-Coordinate
//              End Point:  GXPOS ($FCB3) = X-Coordinate, GYPOS ($FCB5) = Y-Coordinate
//                  Color: ATRBYT ($F3F3) = Attribute
// Logical Operation Code: LOGOPR ($FB02) = Code
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_NVBXFL		0x00cd	//

//  Function: Change Screen Mode (Same As CHGMOD In MAIN-ROM)
//     Input: A = Screen Mode (0..8)
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_CHGMODB	0x00d1	//

//  Function: Initialise Screen To TEXT1 Mode (40x24) (Same As INITXT In MAIN-ROM)
//     Input: TXTNAM ($F3B3) = Pattern Name Table
//            TXTCGP ($F3B7) = Pattern Generator Table
//            LINL40 ($F3AE) = Length Of Line
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_INITXTB	0x00d5	//

//  Function: Initialise Screen To GRAPHIC1 Mode (32x24) (Same As INIT32 In MAIN-ROM)
//     Input: T32NAM ($F3BD) = Pattern Name Table
//            T32COL ($F3BF) = Color Table
//            T32CGP ($F3C1) = Pattern Generator Table
//            T32ATR ($F3C3) = Sprite Attribute Table
//            T32PAT ($F3C5) = Sprite Generator Table
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_INIT32B	0x00d9	//

//  Function: Initialise Screen To High-Resolution Graphics Mode (Same As INIGRP In MAIN-ROM)
//     Input: GRPNAM ($F3C7) = Pattern Name Table
//            GRPCOL ($F3C9) = Color Table
//            GRPCGP ($F3CB) = Pattern Generator Table
//            GRPATR ($F3CD) = Sprite Attribute Table
//            GRPPAT ($F3CF) = Sprite Generator Table
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_INIGRPB	0x00dd	//

//  Function: Initialise Screen To MULTI COLOR Mode (Same As INIMLT In MAIN-ROM)
//     Input: MLTNAM ($F3D1) = Pattern Name Table
//            MLTCOL ($F3D3) = Color Table
//            MLTCGP ($F3D5) = Pattern Generator Table
//            MLTATR ($F3D7) = Sprite Attribute Table
//            MLTPAT ($F3D9) = Sprite Generator Table
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_INIMLTB	0x00e1	//

//  Function: Set VDP To Text Mode (40x24) (Same As SETTXT In MAIN-ROM)
//     Input: TXTNAM ($F3B3) = Pattern Name Table
//            TXTCGP ($F3B7) = Pattern Generator Table
//            LINL40 ($F3AE) = Length Of Line
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_SETTXTB	0x00e5	//

//  Function: Set VDP To Text Mode (32x24) (Same As SETT32 In MAIN-ROM)
//     Input: T32NAM ($F3BD) = Pattern Name Table
//            T32COL ($F3BF) = Color Table
//            T32CGP ($F3C1) = Pattern Generator Table
//            T32ATR ($F3C3) = Sprite Attribute Table
//            T32PAT ($F3C5) = Sprite Generator Table
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_SETT32B	0x00e9	//

//  Function: Set VDP To High-Resolution Mode (Same As SETGRP In MAIN-ROM)
//     Input: GRPNAM ($F3C7) = Pattern Name Table
//            GRPCOL ($F3C9) = Color Table
//            GRPCGP ($F3CB) = Pattern Generator Table
//            GRPATR ($F3CD) = Sprite Attribute Table
//            GRPPAT ($F3CF) = Sprite Generator Table
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_SETGRPB	0x00ed	//

//  Function: Set VDP To MULTI COLOR Mode (Same As SETMLT In MAIN-ROM)
//     Input: MLTNAM ($F3D1) = Pattern Name Table
//            MLTCOL ($F3D3) = Color Table
//            MLTCGP ($F3D5) = Pattern Generator Table
//            MLTATR ($F3D7) = Sprite Attribute Table
//            MLTPAT ($F3D9) = Sprite Generator Table
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_SETMLTB	0x00f1	//

//  Function: Initialise All Sprites (Same As CLRSPR In MAIN-ROM)
//            Sprite Pattern Cleared To Zero, Sprite Number To Sprite Plane Number
//            Sprite Color To Foreground Color
//            Sprite Vertical Location Set To 209 (Mode 0..3) Or 217 (Mode 4..8)
//     Input: SCRMOD ($FCAF) = Screen Mode
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_CLRSPRB	0x00f5	//

//  Function: Return Address Of Sprite Generator Table (Same As CALPAT In MAIN-ROM)
//     Input:  A = Sprite Number
//    Output: HL = Returned Address
// Registers: AF, DE, HL
// Available: MSX2
#define BIOS_CALPATB	0x00f9	//

//  Function: Return Address Of Sprite Attribute Table (Same As CALATR In MAIN-ROM)
//     Input:  A = Sprite Number
//    Output: HL = Returned Address
// Registers: AF, DE, HL
// Available: MSX2
#define BIOS_CALATRB	0x00fd	//

//  Function: Return Current Sprite Size (Same As GSPSIZ In MAIN-ROM)
//     Input: NONE
//    Output: A = Sprite Size (In Bytes)
//            CY-Flag Set IF (Size == 16x16), ELSE CY-Flag Reset
// Registers: AF
// Available: MSX2
#define BIOS_GSPSIZB	0x0101	//

//================================
// MSX2 Specific Routines

//  Function: Return Current Character Pattern
//     Input: A = Character Code
//    Output: PATWRK ($FC40 Onwards) = Character Pattern
// Registers: ALL
// Available: MSX2 (Same As Non-Callable Routine In MSX1-BIOS)
#define BIOS_GETPAT		0x0105	//

//  Function: Write Data To VRAM (16-Bit Address: $0000..$FFFF) (Same As NWRVRM In MAIN-ROM)
//     Input: HL = VRAM Address, A = Data To Write
//    Output: NONE
// Registers: AF
// Available: MSX2
#define BIOS_WRTVRMB	0x0109	//

//  Function: Read Data From VRAM (16-Bit Address: $0000..$FFFF) (Same As NRDVRM In MAIN-ROM)
//     Input: HL = VRAM Address
//    Output: A = Data Which Was Read
// Registers: AF
// Available: MSX2
#define BIOS_RDVRMB		0x010d	//

//  Function: Change Screen Color (Same As CHGCLR In MAIN-ROM)
//     Input: A = Screen Mode
//            FORCLR ($F3E9) = Foreground Color
//            BAKCLR ($F3EA) = Background Color
//            BDRCLR ($F3EB) = Border Color
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_CHGCLRB	0x0111	//

//  Function: Clear Screen
//     Input: NONE
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_CLSSUB		0x0115	//

//  Function: Clear Text-Screen
//     Input: NONE
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_CLRTXT		0x0119	//

//  Function: Display Function Keys (Same As DSPFNK In MAIN-ROM)
//     Input: NONE
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_DSPFNKB	0x011d	//

//  Function: Remove Line In Text-Screen
//     Input: L = Line Number
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_DELLNO		0x0121	//

//  Function: Add Line To Text-Screen
//     Input: L = Line Number
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_INSLNO		0x0125	//

//  Function: Put Character On Text-Screen
//     Input: L = X-Position, H = Y-Position
//    Output: NONE
// Registers: AF
// Available: MSX2
#define BIOS_PUTVRM		0x0129	//

//  Function: Write Data To VDP Register (Same As WRTVDP In MAIN-ROM)
//     Input: C = VDP Register Number (0..27, 32..46), B = Data To Write
//    Output: NONE
// Registers: AF, BC
// Available: MSX2
#define BIOS_WRTVDPB	0x012d	//

//  Function: Read Data From VDP Register
//     Input: A = Register Number (0..9)
//    Output: A = Data That Has Been Read
// Registers: F
// Available: MSX2
#define BIOS_VDPSTA		0x0131	//

//  Function: Control KANA-Key & KANA-Lamp (Japan)
//     Input: NONE
//    Output: NONE
// Registers: AF
// Available: MSX2
#define BIOS_KIKLOK		0x0135	//

//  Function: Get Key-Code From Keyboard, Convert To KANA & Place In Buffer (Japan)
//     Input: Z-Flag = Conversion Mode (Set IF Not In Conversion Mode)
//    Output: NONE
// Registers: AF
// Available: MSX2
#define BIOS_PUTCHR		0x0139	//

//  Function: Switche Page
//     Input: DPPAGE ($FAF5) = Display Page Number
//            ACPAGE ($FAF6) =  Active Page Number
//    Output: NONE
// Registers: AF
// Available: MSX2
#define BIOS_SETPAG		0x013d	//

//================================
// Palette Routines

//  Function: Initialise Palette (Current Palette Is Saved To VRAM)
//     Input: NONE
//    Output: NONE
// Registers: AF, BC, DE
// Available: MSX2
#define BIOS_INIPLT		0x0141	//

//  Function: Restore Palette From VRAM
//     Input: NONE
//    Output: NONE
// Registers: AF, BC, DE
// Available: MSX2
#define BIOS_RSTPLT		0x0145	//

//  Function: Get Colour Code From Palette
//     Input: D = Palette Number (0..15)
//    Output: B = Red & Blue: %RRRRBBBB - R = Red Code, B = Blue Code
//            C = Green: %0000GGGG - G = Green Code
// Registers: AF, DE
// Available: MSX2
#define BIOS_GETPLT		0x0149	//

//  Function: Set Colour Code To Palette
//     Input: D = Palette Number (0..15)
//            A = Red & Blue: %RRRRBBBB - R = Red Code, B = Blue Code
//            E = Green: %0000GGGG - G = Green Code
//    Output: NONE
// Registers: AF
// Available: MSX2
#define BIOS_SETPLT		0x014d	//

//================================
// BASIC Routines

//  Function: Set Sprites
//     Input: HL = BASIC Text-Pointer
//    Output: HL = Adapted BASIC Text-Pointer
// Registers: ALL
// Available: MSX2 (BASIC!)
#define BIOS_PUTSPRT	0x0151	//

//  Function: Change Screen, Sprite, Or Palette Color Value
//     Input: HL = BASIC Text-Pointer
//    Output: HL = Adapted BASIC Text-Pointer
// Registers: ALL
// Available: MSX2 (BASIC!)
#define BIOS_COLOR		0x0155	//

//  Function: Change Screen Mode
//     Input: HL = BASIC Text-Pointer
//    Output: HL = Adapted BASIC Text-Pointer
// Registers: ALL
// Available: MSX2 (BASIC!)
#define BIOS_SCREEN		0x0159	//

//  Function: Change Text-Screen Width
//     Input: HL = BASIC Text-Pointer
//    Output: HL = Adapted BASIC Text-Pointer
// Registers: ALL
// Available: MSX2 (BASIC!)
#define BIOS_WIDTHS		0x015d	//

//  Function: Write Data To VDP Register
//     Input: HL = BASIC Text-Pointer
//    Output: HL = Adapted BASIC Text-Pointer
// Registers: ALL
// Available: MSX2 (BASIC!)
#define BIOS_VDP		0x0161	//

//  Function: Read Data From VDP Register
//     Input: HL = BASIC Text-Pointer
//    Output: HL = Adapted BASIC Text-Pointer
// Registers: ALL
// Available: MSX2 (BASIC!)
#define BIOS_VDPF		0x0165	//

//  Function: Write VDP Base Register
//     Input: HL = BASIC Text-Pointer
//    Output: HL = Adapted BASIC Text-Pointer
// Registers: ALL
// Available: MSX2 (BASIC!)
#define BIOS_BASE		0x0169	//

//  Function: Read VDP Base Register
//     Input: HL = BASIC Text-Pointer
//    Output: HL = Adapted BASIC Text-Pointer
// Registers: ALL
// Available: MSX2 (BASIC!)
#define BIOS_BASEF		0x016d	//

//  Function: Write Byte To VRAM
//     Input: HL = BASIC Text-Pointer
//    Output: HL = Adapted BASIC Text-Pointer
// Registers: ALL
// Available: MSX2 (BASIC!)
#define BIOS_VPOKE		0x0171	//

//  Function: Read Byte From VRAM
//     Input: HL = BASIC Text-Pointer
//    Output: HL = Adapted BASIC Text-Pointer
// Registers: ALL
// Available: MSX2 (BASIC!)
#define BIOS_VPEEK		0x0175	//

//  Function: Set BEEP, ADJUST, TIME & DATE
//     Input: HL = BASIC Text-Pointer
//    Output: HL = Adapted BASIC Text-Pointer
// Registers: ALL
// Available: MSX2 (BASIC!)
#define BIOS_SETS		0x0179	//

//================================
// NON-BASIC Routines

//  Function: Generate BEEP (Same As BEEP In MAIN-ROM)
//     Input: NONE
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_BEEPB		0x017d	//

//  Function: Display Prompt (Default: Ok)
//     Input: NONE
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_PROMPT		0x0181	//

//  Function: Recover Screen-Parameter Of Clock-Chip
//  When CY-Flag Set, Function-Key Text Will Display
//     Input: CY-Flag Reset After MSX-DOS Call
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_SDFSCR		0x0185	//

//  Function: Recover Screen-Parameter Of Clock-Chip, & Print Welcome Message
//  When CY-Flag Set, Function-Key Text Will Display
//     Input: NONE
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_SETSCR		0x0189	//

//  Function: Copy VRAM, Array & DISK-FILE
//     Input: HL = BASIC Text-Pointer
//    Output: HL = Adapted BASIC Text-Pointer
// Registers: ALL
// Available: MSX2 (BASIC!)
#define BIOS_SCOPY		0x018d	//

//================================
// BIT-BLIT Routines

// Mostly Executed by the VDP, Parameters Must Be Passed Through System RAM ($F562)
// See MSX2 System Variables For More Detail, HL Must Be $F562 When Calling
//  Function: Copy VRAM To VRAM
//     Input: SX, SY, DX, DY, NX, NY, ARG, L_OP
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_BLTVV		0x0191	//

//  Function: Copy MAIN-RAM To VRAM
//     Input: SX = Address Of Screen Data In RAM, DX, DY, ARG, L_OP,
//            NX & NY Must Be In Screen Data
//    Output: CY-Flag Set IF Data Failure In RAM
// Registers: ALL
// Available: MSX2
#define BIOS_BLTVM		0x0195	//

//  Function: Copy VRAM To MAIN-RAM
//     Input: DX = Address Of Screen Data In RAM, SX, SY, ARG, L_OP,
//            NX & NY Must Be In Screen Data
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_BLTMV		0x0199	//

//  Function: Copy DISK-FILE To VRAM
//     Input: SX = Address Of Disk Filename, DX, DY, ARG, L_OP,
//            NX & NY Must Be In Screen Data
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_BLTVD		0x019d	//

//  Function: Copy VRAM To DISK-FILE
//     Input: DX = Address Of Disk Filename, SX, SY, NX, NY, ARG
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_BLTDV		0x01a1	//

//  Function: Copy DISK-FILE To MAIN-RAM
//     Input: SX = Address Of Disk Filename
//            DX = Start Address In RAM
//            DY = End Address In RAM
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_BLTMD		0x01a5	//

//  Function: Copy MAIN-RAM To DISK-FILE
//     Input: DX = Address Of Disk Filename
//            SX = Start Address In RAM
//            SY = End Address In RAM
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_BLTDM		0x01a9	//

//================================
// Miscellaneus

//  Function: Read Status Of Mouse Or Light-Pen
//     Input: Call Setting Following Data In A:
//            Descriptions In Parenthesis Are Return Values
//               8 ....... Light-Pen Check ($FF = Available / Touching Screen)
//               9 ....... Read X-Coordinate
//              10 ....... Read Y-Coordinate
//              11 ....... Read Light-Pen Switch Status ($FF = Pressed)
//              12 ....... Whether Mouse Is Connected To Port 1 ($FF = Available)
//              13 ....... Read Offset In X-Direction
//              14 ....... Read Offset In Y-Direction
//              15 ....... No Function (Always 0)
//              16 ....... Whether Mouse Is Connected To Port 2 ($FF = Available)
//              17 ....... Read Offset In X-Direction
//              18 ....... Read Offset In Y-Direction
//              19 ....... No Function (Always 0)
//              21 ....... 2nd Light-Pen Check ($FF = Available / Touching Screen)
//              22 ....... Read X-Coordinate
//              23 ....... Read Y-Coordinate
//              24 ....... Read 2nd Light-Pen Switch Status ($FF = Pressed)
//    Output: A
// Registers: ALL
// Available: MSX2 (Access Via GTPAD In MAIN-ROM, Numbers 8 & Up Will Be Forwarded To This Call)
#define BIOS_NEWPAD		0x01ad	//

//  Function: GET TIME, GET DATE & PUT KANJI
//     Input: HL = BASIC Text-Pointer
//    Output: HL = Adapted BASIC Text-Pointer
// Registers: ALL
// Available: MSX2 (BASIC!)
#define BIOS_GETPUT		0x01b1	//

//  Function: Change Screen Mode, Palette Is Initialised
//     Input: A = Screen Mode (0..8)
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_CHGMDP		0x01b5	//

//  Function: Not Used (Reserved Entry)
//     Input: NONE
//    Output: NONE
// Registers: ALL
// Available: MSX2
#define BIOS_REVI		0x01b9	//

//  Function: Send A Kanji Character To The Graphic-Screen (Mode 5..8)
//     Input: BC = JIS Kanji-Character Code, A = Display Mode
//            The Display Mode Is Similar To PUT KANJI BASIC Command:
//              0 ........ Display In 16x16 Dots (FULL)
//              1 ........ Display Even Dots (EVEN)
//              2 ........ Display Odd Dots (ODD)
// Available: MSX2
#define BIOS_NKJPRT		0x01bd	//

//  Function: Read CLOCK-RAM Data
//     Input: C = CLOCK-RAM Address: %00BBAAAA - A = Address (0..15), B = Block Number (0..3)
//    Output: A = Data Which Was Read (Lower 4-Bits)
// Registers: F
// Available: MSX2
#define BIOS_REDCLK		0x01f5	//

//  Function: Write CLOCK-RAM Data
//     Input: C = CLOCK-RAM Address: %00BBAAAA - A = Address (0..15), B = Block Number (0..3)
//            A = Data To Write
//    Output: NONE
// Registers: F
// Available: MSX2
#define BIOS_WRTCLK		0x01f9	//


//=====================================
// MSX System Variables
//=====================================
// Sources:
//   http://map.grauw.nl/resources/msxbios.php
//   https://github.com/PeterLemon/MSX/blob/master/LIB/MSX.INC
//   https://www.msx.org/wiki/System_variables,_code_%26_hooks_in_RAM_after_boot

//MSX System Variables located in Main ROM
#define ADDR_CGTABL		0x0004	//Base Address Of MSX Character Set In ROM (2B/R)
#define ADDR_VDP_DR		0x0006	//Base Port Address For VDP Data Read (1B/R)
#define ADDR_VDP_DW		0x0007	//Base Port Address For VDP Data Write (1B/R)
#define ADDR_SYSVER		0x002b	/*BASIC System Version (1B/R)
  7 6-5-4 3-2-1-0
  |   |      +-------- Character Set - 0:Japanese 1:International 2:Korean
  |   +--------------- Date Format - 0:Y-M-D 1:M-D-Y 2:D-M-Y
  +------------------- Default Interrupt Frequency - 0:60Hz 1:50Hz */
#define ADDR_ROMVER		0x002c	/*BASIC ROM Version (1B/R)
  7-6-5-4 3-2-1-0
     |       +-------- Keyboard Type - 0:Japanese 1:Inter(QUERTY) 2:French(AZERTY)
     |                                 3:UK 4:German(DIN) 5:Russian 6:Spanish
     +---------------- BASIC Version - 0:Japanese 1:International */
#define ADDR_MSXVER		0x002d	//0:MSX1 1:MSX2 2:MSX2+ 3:TR 4:OCM (1B/R)

//MSX-DOS (DiskROM) System Variables located in RAM
//Only initialized when a DiskROM is present
#define ADDR_RAMAD0		0xf341	//Slot address of RAM in page 0 -DOS- (1B/RW)
#define ADDR_RAMAD1		0xf342	//Slot address of RAM in page 1 -DOS- (1B/RW)
#define ADDR_RAMAD2		0xf343	//Slot address of RAM in page 2 -DOS/BASIC- (1B/RW)
#define ADDR_RAMAD3		0xf344	//Slot address of RAM in page 3 -DOS/BASIC- (1B/RW)
#define ADDR_DISKROM	0xf348	//Main DiskROM slot address (1B/RW)

//MSX System Variables located in RAM
#define ADDR_RDPRIM		0xf380	//Read from basic slot, used by RDSLT (0Ch) BIOS routine (5B/RW)
#define ADDR_WRPRIM		0xf385	//Write to basic slot, used by WRSLT (14h) BIOS routine (7B/RW)
#define ADDR_CLPRIM		0xf38c	//Call basic slot, used by CALSLT (1Ch) BIOS routine (14B/RW)
#define ADDR_USRTB0		0xf39a	//USR function starting USR(0) (2B/RW)
#define ADDR_USRTB1		0xf39c	//USR function starting USR(1) (2B/RW)
#define ADDR_USRTB2		0xf39e	//USR function starting USR(2) (2B/RW)
#define ADDR_USRTB3		0xf3a0	//USR function starting USR(3) (2B/RW)
#define ADDR_USRTB4		0xf3a2	//USR function starting USR(4) (2B/RW)
#define ADDR_USRTB5		0xf3a4	//USR function starting USR(5) (2B/RW)
#define ADDR_USRTB6		0xf3a6 	//USR function starting USR(6) (2B/RW)
#define ADDR_USRTB7		0xf3a8	//USR function starting USR(7) (2B/RW)
#define ADDR_USRTB8		0xf3aa	//USR function starting USR(8) (2B/RW)
#define ADDR_USRTB9		0xf3ac	//USR function starting USR(9) (2B/RW)

#define ADDR_LINL40		0xf3ae	//Screen width per line in SCREEN 0 (Default 37) (1B/RW)
#define ADDR_LINL32		0xf3af	//Screen width per line in SCREEN 1 (Default 29) (1B/RW)
#define ADDR_LINLEN		0xf3b0	//Current screen width per line (1B/RW)
#define ADDR_CRTCNT		0xf3b1	//Number of lines of current screen (default 24) (1B/RW)
#define ADDR_CLMLST		0xf3b2	//Horizontal location in the case that items are divided by commas in PRINT statement (1B/RW)
#define ADDR_TXTNAM		0xf3b3	//BASE(0) Text SCREEN 0 Name Table (2B/RW)
#define ADDR_TXTCOL		0xf3b5	//BASE(1) Text SCREEN 0 Color Table (2B/RW)
#define ADDR_TXTCGP		0xf3b7	//BASE(2) Text SCREEN 0 Character Pattern Table (2B/RW)
#define ADDR_TXTATR		0xf3b9	//BASE(3) Text SCREEN 0 Sprite Attribute Table (2B/RW)
#define ADDR_TXTPAT		0xf3bb	//BASE(4) Text SCREEN 0 Sprite Pattern Table (2B/RW)
#define ADDR_T32NAM		0xf3bd	//BASE(5) Text SCREEN 1 Name Table (2B/RW)
#define ADDR_T32COL		0xf3bf	//BASE(6) Text SCREEN 1 Color Table (2B/RW)
#define ADDR_T32CGP		0xf3c1	//BASE(7) Text SCREEN 1 Character Pattern Table (2B/RW)
#define ADDR_T32ATR		0xf3c3	//BASE(8) Text SCREEN 1 Sprite Attribute Table (2B/RW)
#define ADDR_T32PAT		0xf3c5	//BASE(9) Text SCREEN 1 Sprite Pattern Table (2B/RW)
#define ADDR_GRPNAM		0xf3c7	//BASE(10) Graphics SCREEN 2 Name Table (2B/RW)
#define ADDR_GRPCOL		0xf3c9	//BASE(11) Graphics SCREEN 2 Color Table (2B/RW)
#define ADDR_GRPCGP		0xf3cb	//BASE(12) Graphics SCREEN 2 Character Pattern Table (2B/RW)
#define ADDR_GRPATR		0xf3cd	//BASE(13) Graphics SCREEN 2 Sprite Attribute Table (2B/RW)
#define ADDR_GRPPAT		0xf3cf	//BASE(14) Graphics SCREEN 2 Sprite Pattern Table (2B/RW)
#define ADDR_MLTNAM		0xf3d1	//BASE(15) MultiColor SCREEN 3 Name Table (2B/RW)
#define ADDR_MLTCOL		0xf3d3	//BASE(16) MultiColor SCREEN 3 Color Table (2B/RW)
#define ADDR_MLTCGP		0xf3d5	//BASE(17) MultiColor SCREEN 3 Character Pattern Table (2B/RW)
#define ADDR_MLTATR		0xf3d7	//BASE(18) MultiColor SCREEN 3 Sprite Attribute Table (2B/RW)
#define ADDR_MLTPAT		0xf3d9	//BASE(19) MultiColor SCREEN 3 Sprite Pattern Table (2B/RW)
#define ADDR_CLIKSW		0xf3db	//Key Press Click Switch 0:Off 1:On (1B/RW)
#define ADDR_CSRY		0xf3dc	//Current Cursor Y-Position (Row) (1B/RW)
#define ADDR_CSRX		0xf3dd	//Current Cursor X-Position (Column) (1B/RW)
#define ADDR_CNSDFG		0xf3de	//Function Keys Display 0:Disabled 1:Enabled (1B/RW)

#define ADDR_RG0SAV		0xf3df	//Content of VDP(0) register (R#0)
#define ADDR_RG1SAV		0xf3e0	//Content of VDP(1) register (R#1)
#define ADDR_RG2SAV		0xf3e1	//Content of VDP(2) register (R#2)
#define ADDR_RG3SAV		0xf3e2	//Content of VDP(3) register (R#3)
#define ADDR_RG4SAV		0xf3e3	//Content of VDP(4) register (R#4)
#define ADDR_RG5SAV		0xf3e4	//Content of VDP(5) register (R#5)
#define ADDR_RG6SAV		0xf3e5	//Content of VDP(6) register (R#6)
#define ADDR_RG7SAV		0xf3e6	//Content of VDP(7) register (R#7)
#define ADDR_STATFL		0xf3e7	//Content of VDP(8) register (MSX2- VDP status register 0  S#0)
#define ADDR_TRGFLG		0xf3e8	/*Stores trigger button status of joystick
  Information about trigger buttons and space bar state 
    7 6 5 4 3 2 1 0
    | | | |       +-- Space bar, trig(0) (0 = pressed)
    | | | +---------- Joystick 1, Trigger 1 (0 = pressed)
    | | +------------ Joystick 1, Trigger 2 (0 = pressed)
    | +-------------- Joystick 2, Trigger 1 (0 = pressed)
    +---------------- Joystick 2, Trigger 2 (0 = pressed)*/
#define ADDR_FORCLR		0xf3e9	//Foreground colour
#define ADDR_BAKCLR		0xf3ea	//Background colour
#define ADDR_BDRCLR		0xf3eb	//Border colour
#define ADDR_MAXUPD		0xf3ec	//JMP 0000 (used by CIRCLE statement)
#define ADDR_MINUPD		0xf3ef	//JMP 0000 (used by CIRCLE statement)
#define ADDR_ATRBYT		0xf3f2	//Color code in csing graphic (1B/RW)

#define ADDR_QUEUES		0xf3f3	//Queue Table Address (QUETAB Initial Value) (2B/RW)
#define ADDR_FRCNEW		0xf3f5	//CLOAD Flag: 0 = When CLOAD, 255 = When NOT CLOAD (1B/RW)
#define ADDR_SCNCNT		0xf3f6	//Key Scan Counter: 0 = Ready To Scan Pressed Keys (1B/RW)
#define ADDR_REPCNT		0xf3f7	//Key Repeat Delay Counter: 0 = Key Will Repeat (1B/RW)
#define ADDR_PUTPNT		0xf3f8	//Keyboard Buffer Address To Write Character (2B/RW)
#define ADDR_GETPNT		0xf3fa	//Keyboard Buffer Address To Read Character (2B/RW)
#define ADDR_CS120		0xf3fc	//Cassette I/O Parameters For 1200 Baud (5B/RW)
#define ADDR_CS240		0xf401	//Cassette I/O Parameters For 2400 Baud (5B/RW)
#define ADDR_LOW		0xf406	//Signal Delay When Writing 0 To Tape (2B/RW)
#define ADDR_HIGH		0xf408	//Signal Delay When Writing 1 To Tape (2B/RW)
#define ADDR_HEADER		0xf40a	//Delay Of Tape Header Sync Block (1B/RW)
#define ADDR_ASPCT1		0xf40b	//Horizontal / Vertical Aspect Ratio For CIRCLE Command (2B/RW)
#define ADDR_ASPCT2		0xf40d	//Horizontal / Vertical Aspect Ratio For CIRCLE Command (2B/RW)
#define ADDR_ENDPRG		0xf40f	//Pointer For RESUME NEXT Command (5B/RW)
#define ADDR_ERRFLG		0xf414	//BASIC Error Flag Code (1B/RW)

#define ADDR_LPTPOS		0xf415	//Printer Head Position: (1B/RW)
								//Read By BASIC Function LPOS & Used By LPRINT BASIC Command
#define ADDR_PRTFLG		0xf416	/*Printer Output Flag Read By OUTDO: (1B/RW)
									0 = Print To Screen, 1 = Print To Printer*/
#define ADDR_NTMSXP		0xf417	/*Printer Type Read By OUTDO, SCREEN n Writes To Address: (1B/RW)
									0 = MSX Printer, 1 = Non-MSX Printer*/
#define ADDR_RAWPRT		0xf418	/*Raw Printer Output Read By OUTDO: (1B/RW)
									0 = Set Tab & Unknown Character To Space & Remove Graphics Headers
									1 = Send Data Just Like It Gets It (RAW Data)*/

#define ADDR_VLZADR		0xf419	//Address Of Data Replaced By O In BASIC Function VAL("") (2B/RW)
#define ADDR_VLZDAT		0xf41b	//Original Value From Address Pointed To With VLZADR (1B/RW)
#define ADDR_CURLIN		0xf41c	//Current Line Number BASIC Interpreter Is Working On (1B/RW)
								//In Direct Mode It Will Be Filled With #FFFF
#define ADDR_CHRSID		0xf91f	//Character Set SlotID (1B/RW)
#define ADDR_CHRSAD		0xf920	//Character Set Address (2B/RW)
#define ADDR_QUETAB		0xf959	/*QUETAB - Queue-Table Containing Queue Information For 4 Queues (24B/RW)
									3 PLAY Queues & 1 RS232 Queue, Using 6-Byte Information Blocks:
									Byte 0 = Queue Head Offset (For Writing)
									Byte 1 = Queue Tail Offset (For Reading)
									Byte 2 = Flag Indicating Whether A Byte Has Been Returned
									Byte 3 = Queue Size
									Byte 4 & 5 = Queue Start Address*/
#define ADDR_CHSLOTID	0xf91f	//Character set SlotID (1B/RW)
#define ADDR_CHSETDDR	0xf920	//Character set address (2B/RW)

#define ADDR_EXBRSA		0xfaf8	//SUBROM Slot Address (EXTENDED BIOS-ROM Slot Address) (1B/RW)

#define ADDR_XSAVE		0xfafe	//Light Pen X Coordinate Read From Device (Internal Use) (1B/RW)
#define ADDR_XOFFS		0xfaff	/*Light Pen X Calibration Offset & Interrupt %IXXXXXXX: (1B/RW)
									X = X Calibration Offset (7-Bit)
									I = Light Pen Interrupt (Used Internally By MSX-BIOS, Do Not Set)*/
#define ADDR_YSAVE		0xfb00	//Light Pen Y Coordinate Read From Device (Internal Use) (1B/RW)
#define ADDR_YOFFS		0xfb01	/*Light Pen Y Calibration Offset %RYYYYYYY: (1B/RW)
									Y = Y Calibration Offset (7-Bit)
									R = Reserved For Future Use (Mask From Reads & Do Not Set)*/

#define ADDR_DRVINF		0xfb21	//Drive Info: (8B/RW)
#define ADDR_DRCNT1		0xfb21	//Number Of Drives Connected To Disk Interface 1 (1B/RW)
#define ADDR_DRSAD1		0xfb22	//Slot Address Of Disk Interface 1 (1B/RW)
#define ADDR_DRCNT2		0xfb23	//Number Of Drives Connected To Disk Interface 2 (1B/RW)
#define ADDR_DRSAD2		0xfb24	//Slot Address Of Disk Interface 2 (1B/RW)
#define ADDR_DRCNT3		0xfb25	//Number Of Drives Connected To Disk Interface 3 (1B/RW)
#define ADDR_DRSAD3		0xfb26	//Slot Address Of Disk Interface 3 (1B/RW)
#define ADDR_DRCNT4		0xfb27	//Number Of Drives Connected To Disk Interface 4 (1B/RW)
#define ADDR_DRSAD4		0xfb28	//Slot Address Of Disk Interface 4 (1B/RW)

//Example of European Keyboard Layout
//FBE5 0  => 7       6         5         4         3         2         1         0
//FBE6 1  => ;       ]         [         \         =         -         9         8
//FBE7 2  => B       A         ACCENT    /         .         ,         `         '
//FBE8 3  => J       I         H         G         F         E         D         C
//FBE9 4  => R       Q         P         O         N         M         L         K
//FBEA 5  => Z       Y         X         W         V         U         T         S
//FBEB 6  => F3      F2        F1        CODE      CAPS      GRPH      CTRL      SHIFT
//FBEC 7  => RET     SEL       BS        STOP      TAB       ESC       F5        F4
#define ADDR_GTKEY8		0xfbed	/*Special keys (1B/R)
                                [7]RIGHT [6]DOWN [5]UP [4]LEFT [3]DEL [2]INS [1]HOME [0]SPACE */
//FBEE 9  => 4       3         2         1         0          /         +         *
//FBEF 10 => .         ,         -         9         8         7         6         5

#define ADDR_EXPTBL		0xfcc1	//Expansion Slot 0..3 Table: (4B/RW)
#define ADDR_EXSAD0		0xfcc1	//Slot 0: $80 = Expanded, 0 = Not Expanded (Main Slot) (1B/RW)
#define ADDR_EXSAD1		0xfcc2	//Slot 1: $80 = Expanded, 0 = Not Expanded (1B/RW)
#define ADDR_EXSAD2		0xfcc3	//Slot 2: $80 = Expanded, 0 = Not Expanded (1B/RW)
#define ADDR_EXSAD3		0xfcc4	//Slot 3: $80 = Expanded, 0 = Not Expanded (1B/RW)

#define ADDR_SLTTBL		0xfcc5	//Mirror Of Slot 0..3 Secondary Slot Selection Registers: (4B/RW)
#define ADDR_SLMIR0		0xfcc5	//Slot 0: Mirror Of Secondary Slot Selection Register (1B/RW)
#define ADDR_SLMIR1		0xfcc6	//Slot 1: Mirror Of Secondary Slot Selection Register (1B/RW)
#define ADDR_SLMIR2		0xfcc7	//Slot 2: Mirror Of Secondary Slot Selection Register (1B/RW)
#define ADDR_SLMIR3		0xfcc8	//Slot 3: Mirror Of Secondary Slot Selection Register (1B/RW)

#define ADDR_RG8SAV		0xffe7	//Mirror Of VDP Register  8 (Basic: VDP(9))  (1B/RW)
#define ADDR_RG9SAV		0xffe8	//Mirror Of VDP Register  9 (Basic: VDP(10)) (1B/RW)
#define ADDR_RG10SA		0xffe9	//Mirror Of VDP Register 10 (Basic: VDP(11)) (1B/RW)
#define ADDR_RG11SA		0xffea	//Mirror Of VDP Register 11 (Basic: VDP(12)) (1B/RW)
#define ADDR_RG12SA		0xffeb	//Mirror Of VDP Register 12 (Basic: VDP(13)) (1B/RW)
#define ADDR_RG13SA		0xffec	//Mirror Of VDP Register 13 (Basic: VDP(14)) (1B/RW)
#define ADDR_RG14SA		0xffed	//Mirror Of VDP Register 14 (Basic: VDP(15)) (1B/RW)
#define ADDR_RG15SA		0xffee	//Mirror Of VDP Register 15 (Basic: VDP(16)) (1B/RW)
#define ADDR_RG16SA		0xffef	//Mirror Of VDP Register 16 (Basic: VDP(17)) (1B/RW)
#define ADDR_RG17SA		0xfff0	//Mirror Of VDP Register 17 (Basic: VDP(18)) (1B/RW)
#define ADDR_RG18SA		0xfff1	//Mirror Of VDP Register 18 (Basic: VDP(19)) (1B/RW)
#define ADDR_RG19SA		0xfff2	//Mirror Of VDP Register 19 (Basic: VDP(20)) (1B/RW)
#define ADDR_RG20SA		0xfff3	//Mirror Of VDP Register 20 (Basic: VDP(21)) (1B/RW)
#define ADDR_RG21SA		0xfff4	//Mirror Of VDP Register 21 (Basic: VDP(22)) (1B/RW)
#define ADDR_RG22SA		0xfff5	//Mirror Of VDP Register 22 (Basic: VDP(23)) (1B/RW)
#define ADDR_RG23SA		0xfff6	//Mirror Of VDP Register 23 (Basic: VDP(24)) (1B/RW)
#define ADDR_ROMSLT		0xfff7	//Main BIOS Slot ID (MSX2 Upwards) (1B/RW)
#define ADDR_RG25SA		0xfffa	//Mirror Of VDP Register 25 (Basic: VDP(26)) (1B/RW)
#define ADDR_RG26SA		0xfffb	//Mirror Of VDP Register 26 (Basic: VDP(27)) (1B/RW)
#define ADDR_RG27SA		0xfffc	//Mirror Of VDP Register 27 (Basic: VDP(28)) (1B/RW)

#define ADDR_2NDSEL		0xffff	/*Secondary Slot Select (IF Slot Is Expanded) %DDCCBBAA: (1B/RW)
									A = Page 0: Secondary Slot ($0000..$3FFF) (2-Bit)
									B = Page 1: Secondary Slot ($4000..$7FFF) (2-Bit)
									C = Page 2: Secondary Slot ($8000..$BFFF) (2-Bit)
									D = Page 3: Secondary Slot ($C000..$FFFF) (2-Bit)
								  Reading Returns Inverse Of Current Subslot Selection
								  Values Are Mirrored In SLTTBL For Convenience
								  Note Each Expanded Primary Slot Has Its Own Register
								  To Access A Specific Primary Slot Register Select It In Page 3 */

// Access Pointer
#define ADDR_POINTER(X)		(*((byte*)X))
// #0:MSX1 #1:MSX2 #2:MSX2+ #3:TurboR #4:¿OneChipMSX?
#define getMSXVER()		(ADDR_POINTER(ADDR_MSXVER))
// #0:60Hz #128:50Hz
#define getINTHZ()		(ADDR_POINTER(ADDR_SYSVER) & 0b10000000)
// #0:off #1:on
#define setCLIKSW(v)	ADDR_POINTER(ADDR_CLIKSW) = v
//
#define getGTKEY8()		ADDR_POINTER(ADDR_GTKEY8)


//=====================================
// VDP Ports  
//=====================================
//Sources: 
//	https://www.msx.org/wiki/VDP_Registers
//	https://www.msx.org/wiki/VDP_Mode_Registers
//	http://problemkaputt.de/portar.htm
//
// MSX1
#define VDPVRAM   0x98  //VRAM Data (Read/Write)
#define VDPSTATUS 0x99  //VDP Status Registers
/*
	MODE REGISTERS

	Control Register 0: (R/W) All MSX [VDP(0) see ADDR_RG0SAV]
     7 6 5 4 3 2 1 0
     | | | | | | | +--- EV  External VDP input: #1 when enabled. Always #0 on MSX2/2+
     | | | | | | +----- M3  Screen mode flag. #1 using SCREEN 2,5,7,8,10,12
     | | | | | +------- M4  Screen mode flag. #1 using SCREEN 4,5,8,10,12 & TXT80. Always #0 on MSX1
     | | | | +--------- M5  Screen mode flag. #1 using SCREEN 6,8,10,12. Always #0 on MSX1
     | | | +----------- IE1 Horizontal Retrace interrupt. #1 enabled. Always #0 on MSX1
     | | +------------- IE2 Light Pen interrupt. #1 enabled. Always #0 on MSX1/2+
     | +--------------- DG  Digitized mode. Sets the color bus to the input (#1) or output mode (#0). Always #0 on MSX1
     +----------------- Always #0

	Control Register 1: (R/W) All MSX [VDP(1) see ADDR_RG1SAV]
     7 6 5 4 3 2 1 0
     | | | | | | | +--- MAG Sprite enlarging. #1 enables enlarged sprites. #0 disabled
     | | | | | | +----- SI  Sprite size. #0:8x8. #1:16x16
     | | | | | +------- Always #0
     | | | | +--------- M2  Screen mode flag. #1 using SCREEN 3
     | | | +----------- M1  Screen mode flag. #1 using SCREEN TXT40 or TXT80
     | | +------------- IE0 Vertical Retrace interrupt. #1:enabled #0:disabled
     | +--------------- BL  Blank screen. #1:screen display is enabled #0:disabled (no VRAM read operations are performed)
     +----------------- 4/16K VRAM mode selection. #1:4kB VRAM mode #0:16kB. Always #0 on MSX2/2+

	Control Register 8: (R/W) MSX2/2+ [VDP(9)]
     7 6 5 4 3 2 1 0
     | | | | | | | +--- BW  Black/White output. #1:32 tones grayscale #0:color output
     | | | | | | +----- SPD Sprite Disable. #1 disable sprites and related VRAM reads. Default #1 for SCREEN 9 and KANJI screens.
     | | | | | +------- Always #0
     | | | | +--------- VR  VRAM type. #1:VRAM 64K x 1bit or 64K x 4bits
     | | | |                           #0:VRAM 16K x 1bit or 16K x 4bits
     | | | +----------- CB  Color Bus. #1:color bus in input mode #0:output mode
     | | +------------- TP  Transparent/Palette. For machines w/video input. #1:color 0 from color palette #0:color 0 transparent.
     | +--------------- LP  Light Pen. #1:Light Pen enabled. Always #0 on MSX2+
     +----------------- MS  Mouse. #1:color bus in input mode & mouse enabled. #0:color bus in output mode & mouse disabled. Always #0 on MSX2+

	Control Register 9: (R/W) MSX2/2+ [VDP(10)]
     7 6 5-4 3 2 1 0
     | |  |  | | | +--- DC  Dot Clock. For machines w/video input. #1:*DLCLK in input mode #0:*DLCLK in output mode 
     | |  |  | | +----- NT 	PAL/NTSC. Only for RGB outputs. #1:PAL (313 lines 50Hz) #0:NTSC (262 lines 60Hz)
     | |  |  | +------- EO  Even/Odd screen. #1 enable two graphic screens interchangeably by even/odd. #0 display same graphic for even/odd
     | |  |  +--------- IL  Interlace. #1:interlace mode enabled #0:disabled
     | |  +------------ S1-S0 Simultaneous mode. Bits that allow select simultaneous mode
     | |                                         #00:normal #01:ext.synch #10:ext.video
     | +--------------- Always #0
     +----------------- LN  Line. #1:vertical dot count set to 212 (26,5 lines on SCREEN TXT80 width 41-80)
                                  #0:vertical dot count is set to 192
*/


//=====================================
// screen modes
//=====================================
#define TXT40  0x00  // text mode       (sc0)
#define TXT32  0x01  // graphics 1 mode (sc1)
#define GRAPH1 0x02  // graphics 2 mode (sc2)
#define GRAPH2 0x03  // multicolor mode (sc3)


//=====================================
// MSX Colors
//=====================================
#define COL_TRANSPARENT		0
#define COL_BLACK			1
#define COL_GREEN			2
#define COL_LIGHT_GREEN		3
#define COL_DARK_BLUE		4
#define COL_BLUE			5
#define COL_DARK_RED		6
#define COL_CYAN			7
#define COL_RED				8
#define COL_LIGHT_RED		9
#define COL_DARK_YELLOW		10
#define COL_LIGHT_YELLOW	11
#define COL_DARK_GREEN		12
#define COL_MAGENTA			13
#define	COL_GRAY			14
#define COL_WHITE			15


//=====================================
// MSX SCREEN BASE Values
//=====================================

#define BASE0			ADDR_TXTNAM	//BASE(0) Text SCREEN 0 Name Table (2B/RW)
#define BASE1			ADDR_TXTCOL	//BASE(1) Text SCREEN 0 Color Table (2B/RW)
#define BASE2			ADDR_TXTCGP	//BASE(2) Text SCREEN 0 Character Pattern Table (2B/RW)
#define BASE3			ADDR_TXTATR	//BASE(3) Text SCREEN 0 Sprite Attribute Table (2B/RW)
#define BASE4			ADDR_TXTPAT	//BASE(4) Text SCREEN 0 Sprite Pattern Table (2B/RW)
#define BASE5			ADDR_T32NAM	//BASE(5) Text SCREEN 1 Name Table (2B/RW)
#define BASE6			ADDR_T32COL	//BASE(6) Text SCREEN 1 Color Table (2B/RW)
#define BASE7			ADDR_T32CGP	//BASE(7) Text SCREEN 1 Character Pattern Table (2B/RW)
#define BASE8			ADDR_T32ATR	//BASE(8) Text SCREEN 1 Sprite Attribute Table (2B/RW)
#define BASE9			ADDR_T32PAT	//BASE(9) Text SCREEN 1 Sprite Pattern Table (2B/RW)
#define BASE10			ADDR_GRPNAM	//BASE(10) Graphics SCREEN 2 Name Table (2B/RW)
#define BASE11			ADDR_GRPCOL	//BASE(11) Graphics SCREEN 2 Color Table (2B/RW)
#define BASE12			ADDR_GRPCGP	//BASE(12) Graphics SCREEN 2 Character Pattern Table (2B/RW)
#define BASE13			ADDR_GRPATR	//BASE(13) Graphics SCREEN 2 Sprite Attribute Table (2B/RW)
#define BASE14			ADDR_GRPPAT	//BASE(14) Graphics SCREEN 2 Sprite Pattern Table (2B/RW)
#define BASE15			ADDR_MLTNAM	//BASE(15) MultiColor SCREEN 3 Name Table (2B/RW)
#define BASE16			ADDR_MLTCOL	//BASE(16) MultiColor SCREEN 3 Color Table (2B/RW)
#define BASE17			ADDR_MLTCGP	//BASE(17) MultiColor SCREEN 3 Character Pattern Table (2B/RW)
#define BASE18			ADDR_MLTATR	//BASE(18) MultiColor SCREEN 3 Sprite Attribute Table (2B/RW)
#define BASE19			ADDR_MLTPAT	//BASE(19) MultiColor SCREEN 3 Sprite Pattern Table (2B/RW)

/*
SCREEN 0 (Text mode, 40 column):
Name table (char positions)     0000-03BF
Character patterns (font)       0800-0FFF
*/
#define SCR0_CHARPOS			0x0000	//BASE(0)
#define SCR0_CHARPATTERN		0x0800	//BASE(2)

/*
SCREEN 1 (coloured text mode, 32 column):
Character patterns (font)       0000-07FF
Name table (char positions)     1800-1AFF
Sprite attribute table          1B00-1B7F
Characters colour table (8/byte)2000-201F
Sprite character patterns       3800-3FFF
*/
#define SCR1_CHARPATTERN		0x0000	//BASE(7)
#define SCR1_CHARPOS			0x1800	//BASE(5)
#define SCR1_SPRATTRIB			0x1b00	//BASE(8)
#define SCR1_CHARCOLOR			0x2000	//BASE(6)
#define SCR1_SPRPATTERN			0x3800	//BASE(9)

/*
SCREEN 2 (256*192 Graphics mode):
Character patterns              0000-17FF
Name table (char positions)     1800-1AFF
Sprite attribute table          1B00-1B7F
PixelByte colour table          2000-37FF
Sprite character patterns       3800-3FFF
*/
#define SCR2_CHARPATTERN		0x0000	//BASE(12)
#define SCR2_CHARPOS			0x1800	//BASE(10)
#define SCR2_SPRATTRIB			0x1b00	//BASE(13)
#define SCR2_PIXELCOLOR			0x2000	//BASE(11)
#define SCR2_SPRPATTERN			0x3800	//BASE(14)

/*
SCREEN 3 (4x4 pixel blocks/Multicolour mode):
Character colour patterns       0000-05FF
Name table (char positions)     0800-0AFF
Sprite attribute table          1B00-1B7F
Sprite character patterns       3800-3FFF
*/
#define SCR3_CHARCOLOR			0x0000	//BASE(16)
#define SCR3_CHARPOS			0x0800	//BASE(15)
#define SCR3_SPRATTRIB			0x1b00	//BASE(18)
#define SCR3_SPRPATTERN			0x3800	//BASE(19)

/*
SCREEN 4 (256*192 Graphics mode with multicolour sprites):
Character patterns              0000-17FF
Name table (char positions)     1800-1AFF
Sprite colours                  1C00-1DFF
Sprite attribute table          1E00-1E7F
Palette                         1E80-1E9F
PixelByte colour table          2000-37FF
Sprite character patterns       3800-3FFF
*/
#define SCR4_CHARPATTERN		0x0000
#define SCR4_CHARPOS			0x1800
#define SCR4_SPRCOLOR			0x1c00
#define SCR4_SPRATTRIB			0x1e00
#define SCR4_PALETTE			0x1e80
#define SCR4_PIXELCOLORTABLE	0x2000
#define SCR4_SPRPATTERN			0x3800

/*
SCREEN 5 (256*212 Graphic mode, 16 colours):
Matrix                          0000-69FF
Sprite colours                  7400-75FF
Sprite attribute table          7600-767F
Palette                         7680-769F
Sprite character patterns       7800-7FFF
*/
#define SCR5_MATRIX				0x0000
#define SCR5_SPRCOLOR			0x7400
#define SCR5_SPRATTRIB			0x7600
#define SCR5_PALETTE			0x7680
#define SCR5_SPRPATTERN			0x7800

/*
SCREEN 6 (512*212 Graphic mode, 4 colours):
Matrix                          0000-69FF
Sprite colours                  7400-75FF
Sprite attribute table          7600-767F
Palette                         7680-769F
Sprite character patterns       7800-7FFF
*/
#define SCR6_MATRIX				0x0000
#define SCR6_SPRCOLOR			0x7400
#define SCR6_SPRATTRIB			0x7600
#define SCR6_PALETTE			0x7680
#define SCR6_SPRPATTERN			0x7800

/*
SCREEN 7 (512*212 Graphic mode, 16 colours):
Matrix                          0000-D3FF
Sprite character patterns       F000-F7FF
Sprite colours                  F800-F9FF
Sprite attribute table          FA00-FA7F
Palette                         FA80-FA9F
*/
#define SCR7_MATRIX				0x0000
#define SCR7_SPRPATTERN			0xf000
#define SCR7_SPRCOLOR			0xf800
#define SCR7_SPRATTRIB			0xfa00
#define SCR7_PALETTE			0xfa80

/*
SCREEN 8 (256*212 Graphic mode, 256 colours):
RGB Matrix                      0000-D3FF
Sprite character patterns       F000-F7FF
Sprite colours                  F800-F9FF
Sprite attribute table          FA00-FA7F
Palette                         FA80-FA9F
*/
#define SCR8_MATRIX				0x0000
#define SCR8_SPRPATTERN			0xf000
#define SCR8_SPRCOLOR			0xf800
#define SCR8_SPRATTRIB			0xfa00
#define SCR8_PALETTE			0xfa80

/*
SCREEN 0 (Text mode, 80 column):
Name table (char positions)     0000-077F (086F)        (See note A)
Character attribute (Blink)     0800-08EF (090D)        (See note G)
Character patterns (font)       1000-17FF               (See note B)
*/
#define SCR0B_CHARPOS			0x0000
#define SCR0B_CHARATTRIB		0x0800
#define SCR0B_CHARPATTERN		0x1000


//=====================================
// ASM Useful macros
//=====================================

#define START_FUNCTION \
	push ix\
	ld ix,#0\
	add ix,sp

#define END_FUNCTION \
	pop ix\
	ret

#define PUSH_ALL_REGS \
	push af\
	push bc\
	push de\
	push hl\
	push iy

#define POP_ALL_REGS \
	pop iy\
	pop hl\
	pop de\
	pop bc\
	pop af

#ifndef  __DI_EI__
#define __DI_EI__
#define EI __asm ei __endasm;
#define DI __asm di __endasm;
#endif


#endif  // __MSX_CONST_H__
