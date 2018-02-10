CC = sdcc
ASM = sdasz80
PLATFORM = -mz80
EMULATOR = openmsx
HEXBIN = makebin
ODIR=out
DIR_GUARD=@mkdir -p $(ODIR)

STARTUPDIR = startups
INCLUDEDIR = -I./includes -I./resources
LIBDIR = libs
SRCDIR = src
FILENAME = sc2view

CRT0 = crt0msx_msxdos_advanced.s
ADDR_CODE = 0x178
ADDR_DATA = 0

#VERBOSE = -V
CCFLAGS = $(VERBOSE) $(PLATFORM) --code-loc $(ADDR_CODE) --data-loc $(ADDR_DATA) \
			--no-std-crt0 --out-fmt-ihx --opt-code-size
LIBS	= $(CRT0) \
			conio.c\
			msxdos.c\
			vdp_tms9918.s\
			heap.c
OBJECTS = $(LIBS)
SOURCES = $(FILENAME).c
OUTFILE = $(FILENAME).com
OUTDISK = OUTPUT.DSK

.PHONY: default all compile clean emulator emulator2 emulatordisk

default: clean compile build

all: default createdisk

compile: $(OBJECTS) $(SOURCES)

$(CRT0):
	@echo "Compiling $(CRT0)"
	$(DIR_GUARD)
	@$(ASM) -o $(ODIR)/$(notdir $(@:.s=.rel)) $(STARTUPDIR)/$(CRT0)

resources.c:
	@echo "Compiling resources.c"
	$(DIR_GUARD)
	@$(CC) $(VERBOSE) $(PLATFORM) $(INCLUDEDIR) -c -o $(ODIR)/resources.rel resources/resources.c

%.s:
	@echo "Compiling $@"
	$(DIR_GUARD)
	@[ -f $(LIBDIR)/$@ ] && $(ASM) -j -l -s -o $(ODIR)/$(notdir $(@:.s=.rel)) $(LIBDIR)/$@ || true
	@[ -f $(SRCDIR)/$@ ] && $(ASM) -j -l -s -o $(ODIR)/$(notdir $(@:.s=.rel)) $(SRCDIR)/$@ || true
	
%.c:
	@echo "Compiling $@"
	$(DIR_GUARD)
	@[ -f $(LIBDIR)/$@ ] && $(CC) $(VERBOSE) $(PLATFORM) $(INCLUDEDIR) -c -o $(ODIR)/$(notdir $(@:.c=.rel)) $(LIBDIR)/$@ || true
	@[ -f $(SRCDIR)/$@ ] && $(CC) $(VERBOSE) $(PLATFORM) $(INCLUDEDIR) -c -o $(ODIR)/$(notdir $(@:.c=.rel)) $(SRCDIR)/$@ || true

$(SOURCES):
	$(DIR_GUARD)
	$(CC) $(INCLUDEDIR) $(CCFLAGS) \
			$(addprefix $(ODIR)/, $(addsuffix .rel, $(basename $(notdir $(OBJECTS))))) \
			$(SRCDIR)/$(SOURCES)

build: $(FILENAME).ihx
	@echo "Building $(OUTFILE)"
	$(DIR_GUARD)
	@$(HEXBIN) -p $(FILENAME).ihx $(OUTFILE).tmp
	@dd skip=256 if=$(OUTFILE).tmp of=$(OUTFILE) bs=1 status=none
	@rm $(OUTFILE).tmp
	@echo "Building Done."

createdisk: $(OUTFILE)
	@echo "Building $(OUTDISK)"
	@cp assets/ALESTE1.SC2 TEST.SC2
	@bin/dsktool a $(OUTDISK) $(OUTFILE) TEST.SC2
	@rm TEST.SC2
	@echo "Disk build Done."

clean:
	@echo "Cleaning..."
	@rm -f $(OUTFILE) $(OUTDISK) *.asm *.ihx *.lk *.lst *.map *.noi *.rel *.sym output.txt $(ODIR)/*

emulator: $(OUTFILE)
	$(EMULATOR) -roma $(OUTFILE) -machine msx1 -ext debugdevice &

emulator2: $(OUTFILE)
	$(EMULATOR) -roma $(OUTFILE) -machine msx2 -ext debugdevice &

emulatordisk: $(OUTFILE)
	$(EMULATOR) -diska $(OUTDISK) -machine msx2 -ext msxdos2 -ext debugdevice &
