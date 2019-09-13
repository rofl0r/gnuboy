#include <stdlib.h>
#include <string.h>
#include "fb.h"
#include "input.h"
#include "rc.h"
#include "pcm.h"
#include "thinlib.h"

struct pcm pcm;

static byte *backbuf;
static volatile int audio_int;

static int samplerate = 44100;
static int sound = 1;
static int stereo = 0;

struct fb fb;

static bitmap_t *screen = NULL;
static int vid_width = 320;
static int vid_height = 200;
static int vid_bpp = 8;

rcvar_t vid_exports[] = 
{
	RCV_INT("vid_width", &vid_width),
	RCV_INT("vid_height", &vid_height),
	RCV_INT("vid_bpp", &vid_bpp),
	RCV_END
};

rcvar_t pcm_exports[] =
{
	RCV_BOOL("sound", &sound),
	RCV_INT("samplerate", &samplerate),
	RCV_INT("stereo", &stereo),
	RCV_END
};

/* hardware audio buffer fill */
static void _audio_callback(void *buf, int len)
{
	memcpy(buf, backbuf, len);
	audio_int = 1;
}

void pcm_init()
{
	thinsound_t params;

	if (!sound)
	{
		pcm.hz = 11025;
		pcm.len = 4096;
		pcm.buf = (byte *) malloc(pcm.len);
		pcm.pos = 0;
		pcm.stereo = stereo;
		return;
	}

	params.sample_rate = samplerate;
	params.frag_size = samplerate / 60;
	params.format = THIN_SOUND_8BIT;
	if (stereo)
		params.format |= THIN_SOUND_STEREO;
	else
		params.format |= THIN_SOUND_MONO;
	params.callback = _audio_callback;

	if (thin_sound_init(&params))
	{
		sound = 0;
		return;
	}

	pcm.hz = params.sample_rate;
	pcm.len = params.frag_size;
	pcm.stereo = (params.format & THIN_SOUND_STEREO) ? 1 : 0;
	pcm.buf = (byte *) malloc(pcm.len);
	if (!pcm.buf)
		die("failed to allocate sound buffer\n");

	memset(pcm.buf, 0, pcm.len);
	pcm.pos = 0;

	backbuf = (byte *) malloc(pcm.len);
	if (!backbuf)
		die("failed to allocate sound backbuffer\n");

	memset(backbuf, 0, pcm.len);

	thin_sound_start();
}

void pcm_close()
{
	if (sound)
	{
		thin_sound_stop();
		thin_shutdown();
	}

	if (pcm.buf)
		free(pcm.buf);
	if (backbuf)
		free(backbuf);
	backbuf = 0;

	memset(&pcm, 0, sizeof pcm);
}

int pcm_submit()
{
	if (!sound)
	{
		pcm.pos = 0;
		return 0;
	}

	if (pcm.pos < pcm.len)
		return 1;

	while (!audio_int)
		; /* spin */

	memcpy(backbuf, pcm.buf, pcm.len);
	audio_int = 0;
	pcm.pos = 0;

	return 1;
}


/* keyboard stuff... */

/* keymap - mappings of the form { scancode, localcode } - from keymap.c */
extern int keymap[][2];

/* TODO: this is terrible. */
static int mapscancode(int scan)
{
	int i;
	for (i = 0; keymap[i][0]; i++)
		if (keymap[i][0] == scan)
			return keymap[i][1];
	return 0;
}


void ev_poll()
{
	int i;
	keydata_t *key;
	event_t ev;
	
	key = thin_key_dequeue();
	while (key)
	{
		ev.type = key->signal ? EV_PRESS : EV_RELEASE;
		ev.code = mapscancode(key->key);
		ev_postevent(&ev);

		key = thin_key_dequeue();
	}
}

void vid_preinit()
{
}

void vid_init()
{
	int red_length, green_length, blue_length;
	int red_offset, green_offset, blue_offset;

	int gotmask = thin_init(THIN_VIDEO | THIN_SOUND | THIN_KEY);
	if ((THIN_VIDEO | THIN_KEY) != (gotmask & (THIN_VIDEO | THIN_KEY)))
		die("thinlib initialization failed.");
	thin_key_set_repeat(false);

	/* don't spam the graphics screen if we don't have soundcard */
	thin_setlogfunc(NULL);

	if (thin_vid_init(vid_width, vid_height, vid_bpp))
		die("could not set video mode");

	screen = thin_vid_lockwrite();
	if (NULL == screen)
		die("could not get ahold of video surface");

	fb.w = screen->width;
	fb.h = screen->height;
	fb.pitch = screen->pitch;
	fb.ptr = screen->data;

	fb.pelsize = (screen->bpp + 7) / 8;
	fb.indexed = (screen->bpp == 8) ? 1 : 0;

	switch (screen->bpp)
	{
	case 8:
		red_length = 0;
		green_length = 0;
		blue_length = 0;
		red_offset = 0;
		green_offset = 0;
		blue_offset = 0;
		break;

	case 16:
		red_length = 5;
		green_length = 6;
		blue_length = 5;
		red_offset = 11;
		green_offset = 5;
		blue_offset = 0;
		break;

	case 32:
		red_length = 8;
		green_length = 8;
		blue_length = 8;
		red_offset = 16;
		green_offset = 8;
		blue_offset = 0;
		break;

	case 15:
	case 24:
	default:
		die("i don't know what to do with %dbpp mode", screen->bpp);
		break;
	}

	fb.cc[0].r = 8 - red_length;
	fb.cc[1].r = 8 - green_length;
	fb.cc[2].r = 8 - blue_length;
	fb.cc[0].l = red_offset;
	fb.cc[1].l = green_offset;
	fb.cc[2].l = blue_offset;

	fb.enabled = 1;
	fb.dirty = 0;
}

void vid_close()
{
	fb.enabled = 0;
	thin_shutdown();
}

void vid_settitle(char *title)
{
}

void vid_setpal(int i, int r, int g, int b)
{
	rgb_t color;

	color.r = r;
	color.g = g;
	color.b = b;
	thin_vid_setpalette(&color, i, 1);
}

void vid_begin()
{
	screen = thin_vid_lockwrite();

	fb.ptr = screen->data;
	fb.pitch = screen->pitch;
	fb.w = screen->width;
	fb.h = screen->height;
}

void vid_end()
{
	thin_vid_freewrite(-1, NULL);
}
