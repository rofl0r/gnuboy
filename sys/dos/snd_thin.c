#include "defs.h"
#include "pcm.h"
#include "rc.h"
#include "thinlib.h"

struct pcm pcm;

static byte *backbuf;
static volatile int audio_int;

static int samplerate = 44100;
static int sound = 1;
static int stereo = 0;

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

	if (thin_init(THIN_SOUND))
	{
		sound = 0;
		return;
	}

	/* don't spam the graphics screen if we don't have soundcard */
	thin_setlogfunc(NULL);
	
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
