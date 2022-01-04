
AC_INIT(cpu.c)

CFLAGS="$CFLAGS"
# if user sets custom CFLAGS, don't enforce our optimization flags
test -n "$CFLAGS" && enable_optimize=no


AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL

old_cflags="$CFLAGS"
CFLAGS="$CFLAGS -Wno-unused-function -Wno-deprecated-declarations -Wno-overlength-strings"
AC_TRY_COMPILE([], [typedef int foo;], [], [CFLAGS="$old_cflags"])

test "$cross_compiling" = "yes" || AC_C_BIGENDIAN
test "$ac_cv_c_bigendian" = "no" && ENDIAN="-DIS_LITTLE_ENDIAN"
test "$cross_compiling" = "yes" || LIBS="$LIBS -L/usr/X11R6/lib"
test x"$host" = x && host=`$CC -dumpmachine`


AC_ARG_WITH(fb,      [  --with-fb                       build framebuffer device interface], [], [with_fb=yes])
AC_ARG_WITH(svgalib, [  --with-svgalib                  build Linux svgalib interface], [], [with_svgalib=yes])
AC_ARG_WITH(sdl,     [  --with-sdl                      build SDL interface], [], [with_sdl=auto])
AC_ARG_WITH(sdl2,    [  --with-sdl2                     build SDL2 interface], [], [with_sdl2=yes])
AC_ARG_WITH(sound,   [  --with-sound=no,oss,sdl,ao      select sound interface], [], [with_sound=yes])



SOUND=""
JOY=""

if test "$with_sound" != "no" ; then
if test "$with_sound" = "yes" || test "$with_sound" = "ao" ; then
AC_CHECK_HEADERS(ao/ao.h, [SOUND=sys/ao/ao.o ; LIBS="$LIBS -Wl,--as-needed -lao -Wl,--no-as-needed"])
fi
if test -z "$SOUND" && ( test "$with_sound" = "yes" || test "$with_sound" = "sdl" ) ; then
AC_CHECK_HEADERS(SDL/SDL.h, [SOUND=sys/sdl/sdl-audio.o ; LIBS="$LIBS -lSDL"])
fi
if test -z "$SOUND" && ( test "$with_sound" = "yes" || test "$with_sound" = "oss" ) ; then
AC_CHECK_HEADERS(sys/soundcard.h, [SOUND=sys/oss/oss.o ; CPPFLAGS="$CPPFLAGS -DHAVE_SYS_SOUNDCARD_H"], [
 AC_CHECK_HEADERS(machine/soundcard.h, [SOUND=sys/oss/oss.o ; CPPFLAGS="$CPPFLAGS -DHAVE_MACHINE_SOUNDCARD_H"], [
  AC_CHECK_HEADERS(soundcard.h, [SOUND=sys/oss/oss.o ; CPPFLAGS="$CPPFLAGS -DHAVE_SOUNDCARD_H"], [
   test "$with_sound" = "oss" && AC_MSG_WARN(oss selected but required headers not found)
  ])
 ])
])
fi
fi

AC_CHECK_HEADERS(linux/joystick.h, [JOY=sys/linux/joy.o])
test "$with_fb" = "no" || AC_CHECK_HEADERS(linux/fb.h, [with_fb=linux])

test "$SOUND" || SOUND=sys/dummy/nosound.o
test "$JOY" || JOY=sys/dummy/nojoy.o






case "$with_fb" in
linux) FB_OBJS="sys/linux/fbdev.o sys/linux/kb.o sys/pc/keymap.o" ;;
*) FB_OBJS="" ; with_fb=no ;;
esac

if test "$with_svgalib" != "no" ; then
AC_CHECK_LIB(vga, vga_init, [
AC_CHECK_HEADERS(vga.h vgakeyboard.h, ,[
AC_MSG_WARN(svgalib found but headers are missing!!)
with_svgalib=no
])], [with_svgalib=no])
fi

if test "$with_sdl2" != no ; then
SDL2_LIBS=-lSDL2
AC_CHECK_LIB(SDL2, SDL_Init, [
 AC_CHECK_HEADERS(SDL2/SDL.h, [
   with_sdl2=yes
  ], [
   AC_MSG_WARN(SDL2 found but headers are missing!!)
   with_sdl2=no
 ])
], [with_sdl2=no], $SDL2_LIBS)
fi

if test "$with_sdl2" = "no" && test "$with_sdl" != "no" ; then
 SDL_LIBS=-lSDL
 AC_CHECK_LIB(SDL, SDL_Init, [
  AC_CHECK_HEADERS(SDL/SDL.h, [
    with_sdl=yes
   ], [
    AC_MSG_WARN(SDL found but headers are missing!!)
    with_sdl=no
  ])
 ], [with_sdl=no], $SDL2_LIBS)
else
 with_sdl=no
fi

if test "$with_sdl" != yes && test "$with_sdl2" != yes ; then
AC_CHECK_FUNCS(usleep, ,[
AC_CHECK_FUNCS(select, ,[
AC_MSG_ERROR(your system must support either usleep or select)
])])
fi

AC_PATH_X

AH_TEMPLATE(HAVE_LIBXEXT)
if test "$no_x" != "yes" ; then
with_x=yes
AC_CHECK_LIB(Xext, XShmCreateImage, [AC_DEFINE(HAVE_LIBXEXT)])
AC_CHECK_HEADERS(sys/ipc.h sys/shm.h X11/Xlib.h X11/Xutil.h X11/keysym.h X11/extensions/XShm.h, [], [], [[
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#define _SVID_SOURCE
#define _XOPEN_SOURCE
]])
test "$x_includes" && XINCS="-I$x_includes"
test "$x_libraries" && XLIBS="-L$x_libraries"
else
with_x=no
fi



test "$with_x" = "no" || TARGETS="$TARGETS xgnuboy"
test "$with_fb" = "no" || TARGETS="$TARGETS fbgnuboy"
test "$with_svgalib" = "no" || TARGETS="$TARGETS sgnuboy"
test "$with_sdl" = "no" || TARGETS="$TARGETS sdlgnuboy"
test "$with_sdl2" = "no" || TARGETS="$TARGETS sdl2gnuboy"










AC_ARG_ENABLE(warnings, [  --enable-warnings       enable selected compiler warnings], [], [enable_warnings=yes])
AC_ARG_ENABLE(debug,    [  --enable-debug          include debugging symbols], [])
AC_ARG_ENABLE(profile,  [  --enable-profile        enable performance profiling], [])
AC_ARG_ENABLE(arch,     [  --enable-arch           compile for specific host cpu architecture], [], [enable_arch=no])
AC_ARG_ENABLE(optimize, [  --enable-optimize=LEVEL select optimization level (full,low,none)], [], [enable_optimize=yes])
AC_ARG_ENABLE(asm,      [  --enable-asm            use hand-optimized asm cores], [], [enable_asm=no])


if test "$enable_warnings" = yes ; then
case "$CC" in *gcc*)
AC_MSG_RESULT(enabling selected compiler warnings)
CFLAGS="$CFLAGS -ansi -pedantic -Wall" ;;
*)
AC_MSG_RESULT(disabling warnings for non-gcc compiler) ;;
esac
fi

if test "$enable_debug" = yes ; then
AC_MSG_RESULT(including debugging symbols)
CFLAGS="$CFLAGS -g"
fi

if test "$enable_profile" = yes ; then
AC_MSG_RESULT(enabling performance profiling)
CFLAGS="$CFLAGS -pg"
fi

if test "$enable_arch" = yes ; then
CFLAGS="$CFLAGS -march=native"
fi

case "$enable_optimize" in
yes|full)
AC_MSG_RESULT(producing heavily optimized code)

CFLAGS="$CFLAGS -O3"

case "$host" in
x86_64*|i?86*) CFLAGS="$CFLAGS -DALLOW_UNALIGNED_IO" ;;
esac

case "$CC" in
*gcc*) CFLAGS="$CFLAGS -fstrength-reduce -fthread-jumps \
 -fcse-follow-jumps -fcse-skip-blocks -frerun-cse-after-loop \
 -fexpensive-optimizations -fforce-addr" ;;
esac

if test "$enable_debug" != yes -a "$enable_profile" != yes ; then
CFLAGS="$CFLAGS -fomit-frame-pointer"
LDFLAGS="$LDFLAGS -s"
fi ;;

low)

AC_MSG_RESULT(using minimal optimizations)
CFLAGS="$CFLAGS -O3" ;;

esac

if test "$enable_asm" = yes ; then
case "$host" in
i?86*)
AC_MSG_RESULT(using optimized i386 cores)
ASM="-DUSE_ASM -I./asm/i386" ; ASM_OBJS="asm/i386/cpu.o asm/i386/lcd.o asm/i386/refresh.s" ;;
*)
AC_MSG_RESULT(no optimized asm core available for $host) ;;
esac
fi

case "$host" in
    *-*-mingw* | *-*-cygwin* | *-*-windows* ) SYS_OBJS=sys/windows/windows.o ;;
    *-*-dos* ) SYS_OBJS=sys/dos/dos.o ;;
    *) SYS_OBJS=sys/nix/nix.o ;;
esac


AC_SUBST(SYS_DEFS)
AC_SUBST(ENDIAN)
AC_SUBST(SOUND)
AC_SUBST(JOY)
AC_SUBST(ASM)
AC_SUBST(ASM_OBJS)
AC_SUBST(SYS_OBJS)
AC_SUBST(FB_OBJS)
AC_SUBST(SDL_CFLAGS)
AC_SUBST(SDL_LIBS)
AC_SUBST(SDL2_LIBS)
AC_SUBST(TARGETS)
AC_SUBST(XINCS)
AC_SUBST(XLIBS)

AC_CONFIG_HEADER(sys/nix/config.h)
AC_OUTPUT(Makefile)



