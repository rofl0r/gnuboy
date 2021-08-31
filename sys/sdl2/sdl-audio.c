/*
 * sdl-audio.c
 * sdl2 audio interface
 *
 * (C) 2001 Laguna
 * (C) 2021 rofl0r
 *
 * Licensed under the GPLv2, or later.
 */

#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "rc.h"
#include "pcm.h"


struct pcm pcm;

static int sound = 1;
static int samplerate = 44100;
static int stereo = 1;
static SDL_AudioDeviceID device;

rcvar_t pcm_exports[] =
{
	RCV_BOOL("sound", &sound, "enable sound"),
	RCV_INT("stereo", &stereo, "enable stereo"),
	RCV_INT("samplerate", &samplerate, "samplerate, recommended: 32768"),
	RCV_END
};

void pcm_init()
{
	int i;
	SDL_AudioSpec as = {0}, ob;

	if (!sound) return;

	SDL_InitSubSystem(SDL_INIT_AUDIO);
	as.freq = samplerate;
	as.format = AUDIO_U8;
	as.channels = 1 + stereo;
	as.samples = samplerate / 60;
	as.userdata = 0;
	for (i = 1; i < as.samples; i<<=1);
	as.samples = i;
	as.callback = NULL;
	device = SDL_OpenAudioDevice(NULL, 0, &as, &ob, SDL_AUDIO_ALLOW_CHANNELS_CHANGE|SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
	if (!device) {
		sound = 0;
		return;
	}
	pcm.hz = ob.freq;
	pcm.stereo = ob.channels - 1;
	pcm.len = ob.size;
	pcm.buf = malloc(pcm.len);
	pcm.pos = 0;
	memset(pcm.buf, 0, pcm.len);
	SDL_PauseAudioDevice(device, 0);
}

int pcm_submit()
{
	int res, min = pcm.len*2;
	if (!sound || !pcm.buf) return 0;
	res = SDL_QueueAudio(device, pcm.buf, pcm.pos) == 0;
	pcm.pos = 0;
	while (res && SDL_GetQueuedAudioSize(device) > min)
		SDL_Delay(1);
	return res;
}

void pcm_close()
{
	if (sound) SDL_CloseAudioDevice(device);
}

