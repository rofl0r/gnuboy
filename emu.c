


#include "defs.h"
#include "regs.h"
#include "hw.h"
#include "cpu.h"
#include "mem.h"
#include "lcd.h"
#include "rc.h"


static int framelen = 16743;
static int framecount;

rcvar_t emu_exports[] =
{
	RCV_INT("framelen", &framelen),
	RCV_INT("framecount", &framecount),
	RCV_END
};







void emu_init()
{
	
}


/*
 * emu_reset is called to initialize the state of the emulated
 * system. It should set cpu registers, hardware registers, etc. to
 * their appropriate values at powerup time.
 */

void emu_reset()
{
	lcd_reset();
	lcdc_reset();
	cpu_reset();
	hw_reset();
	mbc_reset();
}








/* This mess needs to be moved to another module; it's just here to
 * make things work in the mean time. */

void *sys_timer();

void emu_run()
{
	static void *timer;
	int delay;

	lcd_begin();
	for (;;)
	{
		while (R_LY < 144) // && ((R_LCDC & 0x80) || (R_STAT & 3)))
			lcdc_step();
		
		vid_end();
		if (!timer) timer = sys_timer();
		delay = framelen - sys_elapsed(timer);
		/* printf("%d\n", delay); */
		sys_sleep(delay);
		vid_begin();
		doevents();
		sys_elapsed(timer);
		if (framecount) { if (!--framecount) die("finished\n"); }
		
		while (R_LY > 0) /* wait for next frame */
			lcdc_step();
	}
}












