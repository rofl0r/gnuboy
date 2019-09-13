/*
 * vga.c
 *
 * dos vga hardware interface
 */


#include <stdlib.h>
#include <stdio.h>

#include <pc.h>
#include <dpmi.h>
#include <go32.h>
#include <sys/movedata.h>
#include "vesa.h"

#include "fb.h"
#include "input.h"
#include "rc.h"



struct fb fb;

static byte static_fb[64000];
static un32 vidaddr;

static int vesa_mode, vesa_dims[3];

static int vw, vh, vb, vp, vs, vl;

rcvar_t vid_exports[] =
{
	RCV_INT("vesa_mode", &vesa_mode),
	RCV_VECTOR("vesa_dims", vesa_dims, 3),
	RCV_END
};


/* keymap - mappings of the form { scancode, localcode } - from keymap.c */
extern int keymap[][2];

static int mapscancode(int scan)
{
	int i;
	for (i = 0; keymap[i][0]; i++)
		if (keymap[i][0] == scan)
			return keymap[i][1];
	return 0;
}

static void kbhandler(int scan, int state)
{
	event_t ev;
	ev.type = state ? EV_PRESS : EV_RELEASE;
	ev.code = mapscancode(scan);
	ev_postevent(&ev);
}



static void settext()
{
	__dpmi_regs regs;

	memset(&regs, 0, sizeof(__dpmi_regs));
	regs.x.ax = 0x0003;
	__dpmi_int(0x10, &regs);
}

static void setvga()
{
	__dpmi_regs r;
	r.x.ax = 0x13;
	__dpmi_int(0x10,&r);
	vp = vw = 320;
	vh = 200;
	vs = 320*28;
	vl = 320*144;
	vidaddr = 0x000A0000;
	fb.pitch = 320;
	fb.w = 320;
	fb.h = 144;
	fb.pelsize = 1;
	fb.indexed = 1;
	fb.cc[0].r = fb.cc[1].r = fb.cc[2].r = 8;
	fb.cc[0].l = fb.cc[1].l = fb.cc[2].l = 0;
	fb.ptr = static_fb;
}

void setvesa(int mode) 
{
	__dpmi_regs r;
	__dpmi_meminfo meminfo;
	VBEINFO vbeinfo;
	VBEMODEINFO modeinfo;

	r.x.ax = 0x4F00;
	r.x.bx = 0;
	r.x.cx = 0;
	r.x.di = __tb & 0x0F;
	r.x.es = (__tb >> 4) & 0xFFFF;
	__dpmi_int(0x10, &r);
	dosmemget(__tb, sizeof(vbeinfo), &vbeinfo);
	if ((vbeinfo.ver & 0xFF00) < 0x0200)
	{
		setvga();
		return;
	}

	r.x.ax = 0x4F01;
	r.x.cx = mode & 0x0FFF;
	r.x.di = __tb & 0x0F;
	r.x.es = (__tb >> 4) & 0xFFFF;
	__dpmi_int(0x10, &r);
	dosmemget(__tb, sizeof(modeinfo), &modeinfo);
	if (!modeinfo.xres || !modeinfo.yres)
	{
		setvga();
		return;
	}
	switch(modeinfo.bpp)
	{
	case 8:
		fb.indexed = 1;
		fb.pelsize = 1;
		vb = 1;
		break;
	case 15:
	case 16:
		fb.indexed = 0;
		fb.pelsize = 2;
		vb = 2;
		break;
	case 24:
		fb.indexed = 0;
		fb.pelsize = 3;
		vb = 3;
		break;
	case 32:
		fb.indexed = 0;
		fb.pelsize = 4;
		vb = 4;
		break;
	default:
		die("unsupported bpp: %d\n", modeinfo.bpp);
		break;
	}
	vw = modeinfo.xres;
	vh = modeinfo.yres;
	vp = modeinfo.bytesperscanline;
	vs = ((vh - 144)>>1)*vp;
	
	fb.pitch = vp;
	fb.w = vw;
	fb.h = 144;
	
	fb.cc[0].r = 8 - modeinfo.redmasksize;
	fb.cc[1].r = 8 - modeinfo.greenmasksize;
	fb.cc[2].r = 8 - modeinfo.bluemasksize;
	fb.cc[0].l = modeinfo.redfieldpos;
	fb.cc[1].l = modeinfo.greenfieldpos;
	fb.cc[2].l = modeinfo.bluefieldpos;

	fb.ptr = static_fb;
	
	meminfo.size = vp * vh;
	meminfo.address = modeinfo.physbaseptr;
	__dpmi_physical_address_mapping(&meminfo);
	vidaddr = (un32)meminfo.address;
	
	r.x.ax = 0x4F02;
	r.x.bx = mode | 0x4000;
	__dpmi_int(0x10, &r);
}

int selectmode()
{
	__dpmi_regs r;
	__dpmi_meminfo meminfo;
	VBEINFO vbeinfo;
	VBEMODEINFO modeinfo;
	int i;
	int x = vesa_dims[0];
	int y = vesa_dims[1];
	int bpp = vesa_dims[2];

	if (!x || !y || !bpp)
	{
		setvga();
		return;
	}

	r.x.ax = 0x4F00;
	r.x.bx = 0;
	r.x.cx = 0;
	r.x.di = __tb & 0x0F;
	r.x.es = (__tb >> 4) & 0xFFFF;
	__dpmi_int(0x10, &r);
	dosmemget(__tb, sizeof(vbeinfo), &vbeinfo);
	if ((vbeinfo.ver & 0xFF00) < 0x0200)
	{
		setvga();
		return;
	}
	for(i = 0x0100; i < 0x0200; i++)
	{
		r.x.ax = 0x4F01;
		r.x.cx = i;
		r.x.di = __tb & 0x0F;
		r.x.es = (__tb >> 4) & 0xFFFF;
		__dpmi_int(0x10, &r);
		dosmemget(__tb, sizeof(modeinfo), &modeinfo);
		if (modeinfo.xres == x && modeinfo.yres == y && modeinfo.bpp == bpp)
		{
			setvesa(i);
			return;
		}
	}
	setvga();
}







void vid_preinit()
{
	/* do nothing; only needed on systems where we must drop perms */
}


void vid_init()
{
	keyboard_init();
	keyboard_chain(0);
	/* selectmode(); */
	setvga();
	fb.enabled = 1;
	fb.dirty = 0;
}


void vid_close()
{
	fb.enabled = 0;
	settext();
	keyboard_close();
}




extern volatile byte keyboard_buffer[0x20];
extern volatile int keyboard_buffer_pos;

static ext_key_table[128] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 101, 0, 102, 103, 104, 0, 105, 0, 106, 0, 107,
	108, 109, 110, 111, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

void ev_poll()
{
	int i;
	int scan, state;
	
	for (i = 0; i < keyboard_buffer_pos; i++)
	{
		scan = keyboard_buffer[i];
		if (scan == 0xE0)
		{
			scan = keyboard_buffer[++i];
			state = (scan & 0x80) ? 0 : 1;
			scan &= 0x7F;
			scan = ext_key_table[scan];
		}
		else
		{
			state = (scan & 0x80) ? 0 : 1;
			scan &= 0x7F;
		}
		kbhandler(scan, state);
	}
	keyboard_buffer_pos = 0;
}





void vid_setpal(int i, int r, int g, int b)
{
	outportb(0x3c8, i);
	outportb(0x3c9, r>>2);
	outportb(0x3c9, g>>2);
	outportb(0x3c9, b>>2);								
}

void vid_begin()
{
}

void vid_end()
{
	_dosmemputl(static_fb, vl>>2, vidaddr + vs);
}












