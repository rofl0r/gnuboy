/*
 * svgalib.c
 *
 * svgalib interface.
 */


#include <stdlib.h>
#include <stdio.h>

#include <vga.h>
#include <vgakeyboard.h>
#include <vgamouse.h>

#include "screen.h"
#include "input.h"
#include "rc.h"



screen_t screen;



static byte *fb;

static int svga_mode, svga_dims[3];

rcvar_t vid_exports[] =
{
	RCV_INT("svga_mode", &svga_mode),
	RCV_VECTOR("svga_dims", svga_dims, 3),
	RCV_END
};



/* pckeymap - mappings of the form { scancode, localcode } - from pckeymap.c */
extern int pckeymap[][2];

static int mapscancode(int scan)
{
	int i;
	for (i = 0; pckeymap[i][0]; i++)
		if (pckeymap[i][0] == scan)
			return pckeymap[i][1];
	return 0;
}

static void kbhandler(int scan, int state)
{
	event_t ev;
	ev.type = state ? EV_PRESS : EV_RELEASE;
	ev.code = mapscancode(scan);
	ev_postevent(&ev);
}

int *rc_getvec();

static int selectmode()
{
	int i;
	int stop;
	vga_modeinfo *mi;
	int best = -1;
	int besterr = 1<<24;
	int err;
	int *vd, vddummy[] = { 320, 200, 8 };

	vd = rc_getvec("svga_dims");
	if (!vd) vd = vddummy;
	
	stop = vga_lastmodenumber();
	for (i = 0; i <= stop; i++)
	{
		if (!vga_hasmode(i)) continue;
		mi = vga_getmodeinfo(i);
		
		/* modex is too crappy to deal with */
		if (!mi->bytesperpixel) continue;

		/* so are banked modes */
		if (mi->width * mi->height * mi->bytesperpixel > 65536)
			if (!(mi->flags & (IS_LINEAR))) continue;

		/* we can't use modes that are too small */
		if (mi->colors < 256) continue;
		if (mi->width < vd[0]) continue;
		if (mi->height < vd[1]) continue;

		/* perfect matches always win */
		if (mi->width == vd[0] && mi->height == vd[1]
			&& (mi->bytesperpixel<<3) == vd[2])
		{
			best = i;
			break;
		}

		/* compare error */
		err = mi->width * mi->height - vd[0] * vd[1]
			+ abs((mi->bytesperpixel<<3)-vd[2]);
		if (err < besterr)
		{
			best = i;
			besterr = err;
		}
	}
	if (best < 0)
		die("no suitable modes available\n");

	return best;
}



void vid_preinit()
{
	vga_init();
}

void vid_init()
{
	//int m = G320x200x256;
	//int m = G512x384x256;
	//int m = G1024x768x256;
	//int m = G320x240x256;
	//int m = G320x240x64K;
	//int m = G320x240x16M;
	//int m = 242;
	//int m = 243;
	int m;
	vga_modeinfo *mi;

	m = rc_getint("svga_mode");
	if (!m) m = selectmode();
	
	keyboard_init();
	keyboard_seteventhandler(kbhandler);

	if (!vga_hasmode(m))
		die("no such video mode: %d\n", m);

	vga_setmode(m);
	mi = vga_getmodeinfo(m);
	screen.w = mi->width;
	screen.h = mi->height;
	screen.pelsize = mi->bytesperpixel;
	screen.pitch = mi->linewidth;
	screen.fb = fb = vga_getgraphmem();

	switch (mi->colors)
	{
	case 256:
		screen.indexed = 1;
		screen.cc[0].r = screen.cc[1].r = screen.cc[2].r = 8;
		screen.cc[0].l = screen.cc[1].l = screen.cc[2].l = 0;
		break;
	case 32768:
		screen.indexed = 0;
		screen.cc[0].r = screen.cc[1].r = screen.cc[2].r = 3;
		screen.cc[0].l = 10;
		screen.cc[1].l = 5;
		screen.cc[2].l = 0;
		break;
	case 65536:
		screen.indexed = 0;
		screen.cc[0].r = screen.cc[2].r = 3;
		screen.cc[1].r = 2;
		screen.cc[0].l = 11;
		screen.cc[1].l = 5;
		screen.cc[2].l = 0;
		break;
	case 16384*1024:
		screen.indexed = 0;
		screen.cc[0].r = screen.cc[1].r = screen.cc[2].r = 0;
		screen.cc[0].l = 16;
		screen.cc[1].l = 8;
		screen.cc[2].l = 0;
		break;
	}
}


void vid_disable()
{
	if (!fb) return;
	memset(screen.fb, 0, sizeof screen);
	keyboard_close();
	vga_setmode(TEXT);
}





void ev_refresh()
{
	keyboard_update();
}

void ev_wait()
{
	int done;
	while (!done)
	{
		done = keyboard_update();
		usleep(10000);
	}
}





void vid_setpal(int i, int r, int g, int b)
{
	vga_setpalette(i, r>>2, g>>2, b>>2);
}

void vid_begin()
{
	vga_waitretrace();
}

void vid_end()
{
}













