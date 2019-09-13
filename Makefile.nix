
prefix = /usr/local
bindir = /bin

CC = gcc
AS = $(CC)
LD = $(CC)
INSTALL = /bin/install -c

CFLAGS = -O3
LDFLAGS = 
ASFLAGS = 

SYS_DEFS = #-DIS_LITTLE_ENDIAN
ASM_OBJS = 
SND_OBJS = sys/oss/oss.o
#SND_OBJS = sys/dummy/nosound.o

TARGETS = xgnuboy #fbgnuboy #sgnuboy #sdlgnuboy

SYS_OBJS = sys/nix/nix.o $(ASM_OBJS) $(SND_OBJS)
SYS_INCS = -I/usr/local/include -I/usr/X11R6/include -I./sys/nix

FB_OBJS = sys/linux/fbdev.o sys/linux/kb.o sys/pc/keymap.o
FB_LIBS = 

SVGA_OBJS = sys/svga/svgalib.o sys/pc/keymap.o
SVGA_LIBS = -L/usr/local/lib -lvga

SDL_OBJS = sys/sdl/sdl.o sys/sdl/keymap.o
SDL_LIBS = -L/usr/local/lib -lSDL -lpthread

X11_OBJS = sys/x11/xlib.o sys/x11/keymap.o
X11_LIBS = -L/usr/X11R6/lib -lX11 -lXext

all: $(TARGETS)

include Rules

fbgnuboy: $(OBJS) $(SYS_OBJS) $(FB_OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(SYS_OBJS) $(FB_OBJS) -o $@ $(FB_LIBS)

sgnuboy: $(OBJS) $(SYS_OBJS) $(SVGA_OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(SYS_OBJS) $(SVGA_OBJS) -o $@ $(SVGA_LIBS)

sdlgnuboy: $(OBJS) $(SYS_OBJS) $(SDL_OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(SYS_OBJS) $(SDL_OBJS) -o $@ $(SDL_LIBS)

xgnuboy: $(OBJS) $(SYS_OBJS) $(X11_OBJS)
	$(LD) $(LDFLAGS) $(OBJS) $(SYS_OBJS) $(X11_OBJS) -o $@ $(X11_LIBS)

install: all
	$(INSTALL) -m 755 $(TARGETS) $(prefix)$(bindir)

clean:
	rm -f *gnuboy gmon.out *.o sys/*.o sys/*/*.o asm/*/*.o












