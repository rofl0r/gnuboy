

#ifndef __STATE_H__
#define __STATE_H__

struct state
{
	int p, q, r; /* position, quotient, remainder */
	int s; /* number of sprites on the line */
};

extern struct state state;

#endif


