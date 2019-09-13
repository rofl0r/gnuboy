

#ifndef __SOUND_H__
#define __SOUND_H__


struct sndchan
{
	int on, pos, cnt, encnt, swcnt;
	int len, enlen, swlen;
	int freq;
	int envol, endir;
};


struct snd
{
	int rate;
	struct sndchan ch[4];
};


extern struct snd snd;






#endif


