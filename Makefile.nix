
prefix = /usr/local
bindir = $(prefix)/games

CC = gcc
AS = $(CC)
LD = $(CC)
INSTALL = /bin/install -c

CFLAGS = -O3
LDFLAGS = 
ASFLAGS = 

SYS_DEFS = #-DIS_LITTLE_ENDIAN
ASM_SRCS = 

TARGETS = xgnuboy #sgnuboy

SYS_OBJS = sys/nix/nix.o $(ASM_SRCS)
SYS_INCS = -I/usr/local/include -I/usr/X11R6/include -I./sys/nix

SVGA_OBJS = sys/svga/svgalib.o sys/pc/keymap.o
SVGA_LIBS = -L/usr/local/lib -lvga

X11_OBJS = sys/x11/xlib.o sys/x11/xkeymap.o
X11_LIBS = -L/usr/X11R6/lib -lX11 -lXext

all: $(TARGETS)

include Rules

sgnuboy: $(OBJS) $(SYS_OBJS) $(SVGA_OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(SYS_OBJS) $(SVGA_OBJS) -o $@ $(SVGA_LIBS)

xgnuboy: $(OBJS) $(SYS_OBJS) $(X11_OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(SYS_OBJS) $(X11_OBJS) -o $@ $(X11_LIBS)

install: all
	$(INSTALL) -m 755 $(TARGETS) $(bindir)

clean:
	rm -f ?gnuboy gmon.out *.o sys/*.o sys/*/*.o asm/*/*.o












