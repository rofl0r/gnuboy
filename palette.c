

#include "defs.h"


static byte palmap[32768];
static byte pallock[256];
static int palrev[256];

/* Course color mapping, for when palette is exhausted. */
static byte crsmap[4][32768];
static int crsrev[4][256];
static const int crsmask[4] = { 0x7BDE, 0x739C, 0x6318, 0x4210 };

enum plstatus
{
	pl_unused = 0,
	pl_linger,
	pl_active,
	pl_locked
};


static byte bestmatch(int c)
{
	byte n, best;
	int r, g, b;
	int r2, g2, b2, c2;
	int err, besterr;

	r = (c & 0x001F) << 3;
	g = (c & 0x03E0) >> 2;
	b = (c & 0x7C00) >> 7;

	best = 0;
	besterr = 1024;
	for (n = 1; n; n++)
	{
		c2 = palrev[n];
		r2 = (c2 & 0x001F) << 3;
		g2 = (c2 & 0x03E0) >> 2;
		b2 = (c2 & 0x7C00) >> 7;
		err = abs(r-r2) + abs(b-b2) + abs(g-g2);
		if (err < besterr)
		{
			besterr = err;
			best = n;
		}
	}
	return best;
}

static void makecourse(int c, byte n)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		c &= crsmask[i];
		crsmap[i][c] = n;
		crsrev[i][n] = c;
	}
}

static byte findcourse(int c)
{
	int i;
	byte n;
	for (i = 0; i < 4; i++)
	{
		c &= crsmask[i];
		n = crsmap[i][c];
		if (crsrev[i][n] == c)
			return n;
	}
	return 0;
}


void pal_lock(byte n)
{
	if (!n) return;
	if (pallock[n] >= pl_locked)
		pallock[n]++;
	else pallock[n] = pl_locked;
}

byte pal_getcolor(int c, int r, int g, int b)
{
	byte n;
	static byte l;
	n = palmap[c];
	if (n && pallock[n] && palrev[n] == c)
	{
		pal_lock(n);
		return n;
	}
	for (n = l+1; n != l; n++)
	{
		if (!n || pallock[n] || n < 16) continue;
		pal_lock(n);
		palmap[c] = n;
		palrev[n] = c;
		makecourse(c, n);
		vid_setpal(n, r, g, b);
		return (l = n);
	}
	n = findcourse(c);
	pal_lock(n);
	return n;
}

void pal_release(byte n)
{
	if (pallock[n] >= pl_locked)
		pallock[n]--;
}


void pal_expire()
{
	int i;
	for (i = 0; i < 256; i++)
		if (pallock[i] && pallock[i] < pl_locked)
			pallock[i]--;
}



