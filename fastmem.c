

#include "fastmem.h"


#define D 0 /* direct */
#define R 1 /* io register */
#define S 2 /* sound register */
#define W 3 /* wave pattern */

#define F 0xFF /* fail */

const byte himask[256];

const byte hi_rmap[256] =
{
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S,
	S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S,
	S, S, S, S, S, S, S, S, S, S, S, S, S, S, S, S,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};


void sound_write();
static void no_write()
{
}

