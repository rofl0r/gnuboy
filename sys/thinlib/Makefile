#
# Makefile
#
# thinlib library makefile
#
# Copyright (C) 2000 Matthew Conte (matt@conte.com)
#
# $Id: Makefile,v 1.10 2001/02/01 06:28:26 matt Exp $

################################
# Configuration

# compilers
CC = gcc
CXX = gxx
CFLAGS =

DBGCFLAGS = -W -Wall -Werror -ggdb -DTHINLIB_DEBUG
OPTCFLAGS = -O3 -fomit-frame-pointer -ffast-math

# Assembler
ASM = nasm
ASMFLAGS = -f coff

DBGASMFLAGS = -g

# must keep this undefined because of bug in djgpp make
# Linker options
#LDFLAGS =

################################

# WANT_DEBUG = TRUE
# WANT_AALIB = TRUE

################################

ifeq "$(WANT_DEBUG)" "TRUE"
	CFLAGS += $(DBGCFLAGS)
	ASMFLAGS += $(DBGASMFLAGS)
else
	CFLAGS += $(OPTCFLAGS)
endif

ifneq "$(WANT_AALIB)" "TRUE"
	CFLAGS += -DNO_AALIB
endif

################################

CFILES = thinlib tl_log tl_timer tl_key tl_mouse tl_joy \
	tl_dpp tl_bmp tl_aa tl_vesa tl_vga tl_video tl_sb tl_sound

CSRCS = $(addsuffix .c, $(CFILES))
OBJS = $(addsuffix .o, $(CFILES))

################################

.PHONY = all dep clean

all: libthin.a thintest.exe

clean:
	rm -f libthin.a thintest.exe $(OBJS) _dep

thintest.exe: thintest.cpp libthin.a
	$(CXX) -o $@ thintest.cpp -L. -lthin

libthin.a: $(OBJS)
	rm -f $@
	ar scru $@ $(OBJS)

dep: rmdep _dep

################################

rmdep:
	@rm -f _dep
	@echo "# dep file" > _dep
ifneq "$(CSRCS)" ""
	@$(foreach .a, $(CSRCS), $(CC) $(CFLAGS) -MM $(.a) >> _dep;)
endif
ifneq "$(ASMSRCS)" ""
	@$(foreach .a, $(ASMSRCS), $(ASM) $(ASMFLAGS) -M $(.a) >> _dep;)
endif

_dep:
# this is done so that we don't get all the no such file warnings
	@echo "# dep file" > _dep
ifneq "$(CSRCS)" ""
	@$(foreach .a, $(CSRCS), $(CC) $(CFLAGS) -MM $(.a) >> _dep;)
endif
ifneq "$(ASMSRCS)" ""
	@$(foreach .a, $(ASMSRCS), $(ASM) $(ASMFLAGS) -M $(.a) >> _dep;)
endif

include _dep

################################

%.o: %.cpp
	$(CXX) $(CFLAGS) -o $@ -c $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.o: %.asm
	$(ASM) $(ASMFLAGS) -o $@ $<

################################

# $Log: Makefile,v $
# Revision 1.10  2001/02/01 06:28:26  matt
# thinlib now works under NT/2000
#
# Revision 1.9  2001/01/15 05:25:52  matt
# i hate near pointers
#
# Revision 1.8  2000/12/16 17:38:25  matt
# tl_sound
#
# Revision 1.7  2000/12/14 14:13:47  matt
# test suite
#
# Revision 1.6  2000/12/13 14:14:27  matt
# DJGPP_USE_NEARPTR -> THINLIB_NEARPTR
#
# Revision 1.5  2000/12/13 13:57:32  matt
# self-sufficient makefile
#
