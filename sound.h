

#ifndef __SOUND_H__
#define __SOUND_H__


struct sndchan
{
	int on;
	int pos, freq, pat;
	int len, cnt;
	int envol, endir, enlen, encnt;
};

struct sweep
{
	int len, cnt, dir, shift;
};


struct snd
{
	byte wave[16];
	int rate;
	struct sndchan ch[4];
	struct sweep sw;
};


extern struct snd snd;






#endif


