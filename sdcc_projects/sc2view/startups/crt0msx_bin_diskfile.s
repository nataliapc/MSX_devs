;	BLOADABLE .BIN HEADER (floppy disk)
;
;	Developed by @Emily82
;	https://www.msx.org/forum/msx-talk/development/sdcc-crt0s-for-floppy-bload
.module	crt0dsk
.globl	_main
.globl l__INITIALIZER
.globl s__INITIALIZED
.globl s__INITIALIZER

;; Floppy disk BASIC bload Header
.db 	0xfe
.dw 	init
.dw		end
.dw 	init

init:
	call 	gsinit
	jp		__pre_main
	
;; Ordering of segments for the linker.
.area _HOME
.area _CODE
.area _INITIALIZER
.area _GSINIT
.area _GSFINAL

.area _DATA
_heap_top::
	.dw 0
.area _INITIALIZED
.area _BSEG
.area _BSS
.area _HEAP

.area _CODE
	
__pre_main:
	push de
	ld de,#_HEAP_start
	ld (_heap_top),de
	pop de
	call	_main
	ret

.area   _GSINIT
gsinit::
	ld bc, #l__INITIALIZER
	ld a, b
	or a, c
	jr Z, gsinit_next
	ld de, #s__INITIALIZED
	ld hl, #s__INITIALIZER
	ldir
gsinit_next:
end:

.area   _GSFINAL
ret
	
.area	_HEAP

_HEAP_start::
