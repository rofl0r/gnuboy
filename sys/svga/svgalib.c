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

#include "fb.h"
#include "input.h"
#include "rc.h"



struct fb fb;




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
	int m;
	vga_modeinfo *mi;

	m = svga_mode;
	if (!m) m = selectmode();
	
	if (!vga_hasmode(m))
		die("no such video mode: %d\n", m);

	vga_setmode(m);
	mi = vga_getmodeinfo(m);
	fb.w = mi->width;
	fb.h = mi->height;
	fb.pelsize = mi->bytesperpixel;
	fb.pitch = mi->linewidth;
	fb.ptr = vga_getgraphmem();

	switch (mi->colors)
	{
	case 256:
		fb.indexed = 1;
		fb.cc[0].r = fb.cc[1].r = fb.cc[2].r = 8;
		fb.cc[0].l = fb.cc[1].l = fb.cc[2].l = 0;
		break;
	case 32768:
		fb.indexed = 0;
		fb.cc[0].r = fb.cc[1].r = fb.cc[2].r = 3;
		fb.cc[0].l = 10;
		fb.cc[1].l = 5;
		fb.cc[2].l = 0;
		break;
	case 65536:
		fb.indexed = 0;
		fb.cc[0].r = fb.cc[2].r = 3;
		fb.cc[1].r = 2;
		fb.cc[0].l = 11;
		fb.cc[1].l = 5;
		fb.cc[2].l = 0;
		break;
	case 16384*1024:
		fb.indexed = 0;
		fb.cc[0].r = fb.cc[1].r = fb.cc[2].r = 0;
		fb.cc[0].l = 16;
		fb.cc[1].l = 8;
		fb.cc[2].l = 0;
		break;
	}
	
	keyboard_init();
	keyboard_seteventhandler(kbhandler);
}


void vid_disable()
{
	if (!fb.ptr) return;
	memset(&fb, 0, sizeof fb);
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













