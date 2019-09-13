



#include "defs.h"
#include "pcm.h"
#include "rc.h"


struct pcm pcm;

static buf[4096];


rcvar_t pcm_exports[] =
{
	RCV_END
};


void pcm_init()
{
	pcm.hz = 11025;
	pcm.buf = buf;
	pcm.len = sizeof buf;
	pcm.pos = 0;
}

void pcm_shutdown()
{
}

int pcm_submit()
{
	pcm.pos = 0;
	return 0;
}





