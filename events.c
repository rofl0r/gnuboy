/*
 * events.c
 *
 * Event queue.
 */


#include "input.h"


char keystates[MAX_KEYS];
int nkeysdown;

static int reptimers[MAX_KEYS];
static int repdelay = 200, reprate = 30;

#define MAX_EVENTS 32

static event_t eventqueue[MAX_EVENTS];
static int eventhead, eventpos;


int ev_postevent(event_t *ev)
{
	int nextevent;
	nextevent = (eventhead+1)%MAX_EVENTS;
	if (nextevent == eventpos)
		return 0;
	eventqueue[eventhead] = *ev;
	eventhead = nextevent;
	if (ev->type == EV_PRESS)
		reptimers[ev->code] = sys_msecs()+repdelay;
	return 1;
}

int ev_getevent(event_t *ev)
{
	if (eventpos == eventhead)
	{
		ev->type = EV_NONE;
		return 0;
	}
	*ev = eventqueue[eventpos];
	eventpos = (eventpos+1)%MAX_EVENTS;
	if (ev->type == EV_PRESS)
	{
		keystates[ev->code] = 1;
		nkeysdown++;
	}
	if (ev->type == EV_RELEASE)
	{
		keystates[ev->code] = 0;
		nkeysdown--;
		if (nkeysdown < 0) nkeysdown = 0;
	}
	return 1;
}

void ev_repeatkeys(int cnt)
{
	int i;
	int t;
	event_t ev;

	t = sys_msecs();
	ev.type = EV_REPEAT;
	for (i = 0; i < MAX_KEYS; i++)
	{
		if (t < reptimers[i] || !keystates[i])
			continue;
		ev.code = i;
		ev_postevent(&ev);
		reptimers[i] = t + reprate;
	}
}







