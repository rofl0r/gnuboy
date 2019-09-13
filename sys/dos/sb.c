


#include <stdlib.h>
#include <stdio.h>

#include <dpmi.h>
#include <go32.h>
#include <sys/farptr.h>
#include <sys/movedata.h>

#include "defs.h"
#include "pcm.h"
#include "rc.h"


struct pcm pcm;

static int ver;
static int dmaaddr;

extern volatile int sb_ready;
extern volatile int sb_frag;

static int samplerate = 44100;
static int sound = 1;

rcvar_t pcm_exports[] =
{
	RCV_BOOL("sound", &sound),
	RCV_INT("samplerate", &samplerate),
	RCV_END
};



void pcm_init()
{
	int i;
	char *blaster, *s;
	int addr, irq, dma;
	int dmaseg, dmasel;
	
	ver = -1;
	if ((blaster = getenv("BLASTER")))
	{
		for (s = blaster; *s; s++)
		{
			switch (*s)
			{
			case 'A':
				addr = strtol(s+1, 0, 16);
				break;
			case 'I':
				irq = atoi(s+1);
				break;
			case 'D':
				dma = atoi(s+1);
				break;
			}
		}
		if ((addr & 0xff0) != addr || addr < 0x200 || addr >= 0x300)
			die("invalid sb base address: %03X\n", addr);
		if (irq <= 0 || irq > 15)
			die ("invalid sb irq: %d\n", irq);
		if (dma < 0 || dma > 3)
			die ("invalid sb dma channel: %d\n", dma);
		ver = sb_init(addr, irq, dma);
	}
	if (ver < 0)
	{
		pcm.hz = 11025;
		pcm.len = 11025;
		pcm.buf = malloc(pcm.len);
		pcm.pos = 0;
		return;
	}

	pcm.len = pcm.hz / 60;
	for (i = 1; i < pcm.len; i<<=1);
	pcm.len = i;

	/* Get page-aligned dos memory for dma buffer */
	dmaseg = __dpmi_allocate_dos_memory((pcm.len+7)>>3, &dmasel);
	if (dmaseg < 0)
		die("failed to allocate low memory for sb dma\n");
	dmaaddr = dmaseg << 4;
	if ((dmaaddr & 0xffff) + pcm.len > 0x10000)
		dmaaddr = (dmaaddr & 0x10000) + 0x10000;
}

void pcm_close()
{
}


int pcm_submit()
{
	byte *buf;
	
	if (ver < 0) return 0;

	if (pcm.pos < pcm.len) return 1;
	while (!sb_ready);
	_dosmemputl(pcm.buf, pcm.len >> 2, dmaaddr);
	pcm.pos = 0;
	return 1;
}








