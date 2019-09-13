

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/soundcard.h>

#include "defs.h"
#include "pcm.h"
#include "rc.h"

/* FIXME - all this code is VERY basic, improve it! */


struct pcm pcm;

static int dsp;
static int samplerate = 44100;
static int sound = 1;

rcvar_t pcm_exports[] =
{
	RCV_BOOL("sound", &sound),
	RCV_INT("samplerate", &samplerate),
	RCV_END
};


void pcm_init()
{
	int n;

	if (!sound)
	{
		pcm.hz = 11025;
		pcm.len = 4096;
		pcm.buf = malloc(pcm.len);
		pcm.pos = 0;
		dsp = -1;
		return;
	}
	
	dsp = open("/dev/dsp", O_WRONLY);

	//n = 0x10000a;
	n = 0x80008;
	ioctl(dsp, SNDCTL_DSP_SETFRAGMENT, &n);
	n = AFMT_U8;
	ioctl(dsp, SNDCTL_DSP_SETFMT, &n);
	n = 1; 
	ioctl(dsp, SNDCTL_DSP_STEREO, &n);
	n = samplerate;
	ioctl(dsp, SNDCTL_DSP_SPEED, &n);
	pcm.hz = n;
	pcm.len = n / 60;
	pcm.buf = malloc(pcm.len);
}

void pcm_shutdown()
{
	if (pcm.buf) free(pcm.buf);
	memset(&pcm, 0, sizeof pcm);
	close(dsp);
}

int pcm_submit()
{
	if (dsp < 0)
	{
		pcm.pos = 0;
		return 0;
	}
	if (pcm.buf) write(dsp, pcm.buf, pcm.pos);
	pcm.pos = 0;
	return 1;
}






