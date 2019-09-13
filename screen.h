

#ifndef __SCREEN_H__
#define __SCREEN_H__


#include "defs.h"



typedef struct screen_s
{
	byte *fb;
	int w, h;
	int pelsize;
	int pitch;
	int indexed;
	struct
	{
		int l, r;
	} cc[3];
} screen_t;


extern screen_t screen;


#endif




