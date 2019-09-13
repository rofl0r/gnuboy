


#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>

#include "defs.h"
#include "fb.h"
#include "rc.h"

struct fb fb;



#define FBSET_CMD "fbset"
static char *fb_mode;
static int fb_depth;

#define FB_DEVICE "/dev/fb0"
static char *fb_device;

static int fbfd = -1;
static byte *fbmap;
static int maplen;

static struct fb_fix_screeninfo fi;
static struct fb_var_screeninfo vi, initial_vi;

rcvar_t vid_exports[] =
{
	RCV_STRING("fb_device", &fb_device),
	RCV_STRING("fb_mode", &fb_mode),
	RCV_INT("fb_depth", &fb_depth),
	RCV_END
};


void vid_preinit()
{
}

void vid_init()
{
	char cmd[256];

	kb_init();
	joy_init();

	if (!fb_device) fb_device = strdup(FB_DEVICE);
	fbfd = open(fb_device, O_RDWR);
	if (fbfd < 0) die("cannot open %s\n", fb_device);
	
	ioctl(fbfd, FBIOGET_VSCREENINFO, &initial_vi);
	initial_vi.xoffset = initial_vi.yoffset = 0;

	if (fb_mode)
	{
		sprintf(cmd, FBSET_CMD " %.80s", fb_mode);
		system(cmd);
	}
	
	ioctl(fbfd, FBIOGET_VSCREENINFO, &vi);
	if (fb_depth) vi.bits_per_pixel = fb_depth;
	vi.xoffset = vi.yoffset = 0;
	vi.activate = FB_ACTIVATE_NOW;
	ioctl(fbfd, FBIOPUT_VSCREENINFO, &vi);
	ioctl(fbfd, FBIOGET_VSCREENINFO, &vi);
	ioctl(fbfd, FBIOGET_FSCREENINFO, &fi);

	fb.w = vi.xres;
	fb.h = vi.yres;
	fb.pelsize = (vi.bits_per_pixel+7)>>3;
	fb.pitch = vi.xres_virtual * fb.pelsize;
	fb.indexed = fi.visual == FB_VISUAL_PSEUDOCOLOR;

	fb.cc[0].r = 8 - vi.red.length;
	fb.cc[1].r = 8 - vi.green.length;
	fb.cc[2].r = 8 - vi.blue.length;
	fb.cc[0].l = vi.red.offset;
	fb.cc[1].l = vi.green.offset;
	fb.cc[2].l = vi.blue.offset;

	maplen = fb.pitch * fb.h;
	
	fbmap = mmap(0, maplen, PROT_READ|PROT_WRITE, MAP_SHARED, fbfd, 0);
	if (!fbmap) die("cannot mmap %s (%d bytes)\n", fb_device, maplen);

	fb.ptr = fbmap;
	memset(fbmap, 0, maplen);
	fb.dirty = 0;
	fb.enabled = 1;
}

void vid_close()
{
	fb.enabled = 0;
	joy_close();
	kb_close();
	ioctl(fbfd, FBIOPUT_VSCREENINFO, &initial_vi);
	memset(fbmap, 0, maplen);
}

void vid_settitle(char *title)
{
}

void vid_setpal(int i, int r, int g, int b)
{
	unsigned short rr = r<<8, gg = g<<8, bb = b<<8;
	struct fb_cmap cmap = { i, 1, &rr, &gg, &bb };
	ioctl(fbfd, FBIOPUTCMAP, &cmap);
}

void vid_begin()
{
}

void vid_end()
{
}

void ev_poll()
{
	kb_poll();
	joy_poll();
}



