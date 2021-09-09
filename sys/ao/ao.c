#undef _GNU_SOURCE
#define _GNU_SOURCE
#include <string.h>

#include <unistd.h>
#include <sys/ioctl.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <ao/ao.h>

#include "defs.h"
#include "pcm.h"
#include "rc.h"

struct pcm pcm;

static int stereo = 1;
static int samplerate = 44100;
static int sound = 1;
static ao_device *device;
static ao_sample_format format;
static int aodriver;


rcvar_t pcm_exports[] =
{
	RCV_BOOL("sound", &sound, "enable sound"),
	RCV_INT("stereo", &stereo, "enable stereo"),
	RCV_INT("samplerate", &samplerate, "samplerate, recommended: 32768"),
	RCV_END
};


static void no_sound(void) {
	pcm.hz = 11025;
	pcm.len = 4096;
	pcm.buf = malloc(pcm.len);
	pcm.pos = 0;
}

void pcm_init()
{
	if (!sound)
	{
		no_sound();
		return;
	}

	ao_initialize();
	format.bits = 8;
	format.channels = 2;
	format.rate = samplerate;
	format.byte_format = AO_FMT_LITTLE;
	aodriver = ao_default_driver_id();
	device = ao_open_live(aodriver, &format, NULL);

	if(!device) {
		no_sound();
		return;
	}

	pcm.stereo = 1;
	pcm.hz = samplerate;
	pcm.len = samplerate / 60;
	pcm.buf = malloc(pcm.len);
}

void pcm_close()
{
	if (pcm.buf) free(pcm.buf);
	memset(&pcm, 0, sizeof pcm);
	if(device) ao_close(device);
}

int pcm_submit()
{
	if (!device)
	{
		pcm.pos = 0;
		return 0;
	}
	if (pcm.buf) ao_play(device, (void*)pcm.buf, pcm.pos);
	pcm.pos = 0;
	return 1;
}
