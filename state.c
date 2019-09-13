

#include <stdlib.h>

#include "defs.h"
#include "hw.h"
#include "regs.h"
#include "state.h"


/* The global state structure - just a few numbers really... */

struct state state;


/* Some macros for accessing the state concisely and legibly. */

#define P (state.p)
#define Q (state.q)
#define R (state.r)
#define S (state.s)


/* The following data makes up the state/transition table, used to
 * synchronize the flow of time for the emulated system with its
 * primary piece of timing hardware, the LCD controller. Using a table
 * like this allows for easy handling of irregularities in particular
 * parts of the LCDC sequence, reduces the amount of state information
 * that needs to be read/written when working with saved image files,
 * and makes the code for advancing states quite elegant. */

enum trans
{
	tr_null,
	tr_begin,
	tr_next,
	tr_refresh,
	tr_oam,
	tr_display,
	tr_hblank,
	tr_vblank,
	tr_end
};

struct statetable
{
	const int *len;
	enum trans trans;
};

#define CYC_N(n) { (n), (n), (n), (n), (n), (n), (n), (n), (n), (n), (n) }

/* FIXME - rewrite cycle count tables to be influenced by number of
 * sprites visible on the line */

static const int cyc_zero[] = CYC_N(0);
static const int cyc_oam[] = { 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40 };
static const int cyc_display[] = { 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86 };
static const int cyc_hblank[] = { 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102 };
static const int cyc_vblank[] = { 228, 228, 228, 228, 228, 228, 228, 228, 228, 228, 228 };
static const int cyc_28[] = CYC_N(28);
static const int cyc_200[] = CYC_N(200);

#define BEGIN { cyc_zero, tr_begin }
#define NEXT { cyc_zero, tr_next }
#define REFRESH { cyc_zero, tr_refresh }
#define OAM { cyc_oam, tr_oam }
#define DISPLAY { cyc_display, tr_display }
#define HBLANK { cyc_hblank, tr_hblank }
#define VBLANK { cyc_vblank, tr_vblank }
#define END { cyc_zero, tr_end }

/* Things probaby aren't properly ordered here -- the refresh might
 * need to happen between the OAM period and the DISPLAY period, since
 * VRAM is still writable during the OAM period. However, for the
 * change to be made, sprite enumeration would have to take place
 * before lcd_refreshline is called, which would require some changes
 * to lcd.c. Of course, this bears no consequence on emulation, only
 * on display output, so unless it looks like it's causing visual
 * glitches, the issue will most likely be ignored indefinitely. */

#define LINE REFRESH,OAM,DISPLAY,HBLANK,NEXT
#define BLANK VBLANK,NEXT
#define LASTBLANK { cyc_28, tr_vblank },NEXT,{ cyc_200, tr_vblank }

/* The actual table is fairly uniform right now, but eventually it may
 * need a good many tweaks for precise emulation. */

const static struct statetable states[] =
{
	BEGIN,
	
	LINE, LINE, LINE, LINE, LINE, LINE, LINE, LINE,
	LINE, LINE, LINE, LINE, LINE, LINE, LINE, LINE,
	LINE, LINE, LINE, LINE, LINE, LINE, LINE, LINE,
	LINE, LINE, LINE, LINE, LINE, LINE, LINE, LINE,
	
	LINE, LINE, LINE, LINE, LINE, LINE, LINE, LINE,
	LINE, LINE, LINE, LINE, LINE, LINE, LINE, LINE,
	LINE, LINE, LINE, LINE, LINE, LINE, LINE, LINE,
	LINE, LINE, LINE, LINE, LINE, LINE, LINE, LINE,
	
	LINE, LINE, LINE, LINE, LINE, LINE, LINE, LINE,
	LINE, LINE, LINE, LINE, LINE, LINE, LINE, LINE,
	LINE, LINE, LINE, LINE, LINE, LINE, LINE, LINE,
	LINE, LINE, LINE, LINE, LINE, LINE, LINE, LINE,

	LINE, LINE, LINE, LINE, LINE, LINE, LINE, LINE,
	LINE, LINE, LINE, LINE, LINE, LINE, LINE, LINE,
	LINE, LINE, LINE, LINE, LINE, LINE, LINE, LINE,
	LINE, LINE, LINE, LINE, LINE, LINE, LINE, LINE,

	LINE, LINE, LINE, LINE, LINE, LINE, LINE, LINE,
	LINE, LINE, LINE, LINE, LINE, LINE, LINE, LINE,

	BLANK, BLANK, BLANK, BLANK, BLANK,
	BLANK, BLANK, BLANK, BLANK, LASTBLANK,

	END
};


/*
 * stat_trigger updates the STAT interrupt line to reflect whether any
 * of the conditions set to be tested (by bits 3-6 of R_STAT) are met.
 * This function should be called whenever any of the following occur:
 * 1) LY or LYC changes.
 * 2) A state transition affects the low 2 bits of R_STAT (see below).
 * 3) The program writes to the upper bits of R_STAT.
 * stat_trigger also updates bit 2 of R_STAT to reflect whether LY=LYC.
 */

void stat_trigger()
{
	static const int condbits[4] = { 0x08, 0x30, 0x20, 0x00 };
	int flag = 0;

	if (!(R_LCDC & 0x80)) return;
	
	if (R_LY == R_LYC)
	{
		R_STAT |= 0x04;
		if (R_STAT & 0x40) flag = IF_STAT;
	}
	else R_STAT &= ~0x04;

	if (R_STAT & condbits[R_STAT&3]) flag = IF_STAT;
	
	hw_interrupt(flag, IF_STAT);
}


/*
 * stat_change is called when a transition in the state table results
 * in a change to the LCD STAT condition (the low 2 bits of R_STAT).
 * It raises or lowers the VBLANK interrupt line appropriately and
 * calls stat_trigger to update the STAT interrupt line.
 */

static void stat_change(int stat)
{
	int iraise;
	
	stat &= 3;
	R_STAT = (R_STAT & 0x7C) | stat;

	if (!(R_LCDC & 0x80)) return;

	hw_interrupt((stat == 1) ? IF_VBLANK : 0, IF_VBLANK);
	stat_trigger();
}




/*
 * stat_trans is to be called whenever advancing the cycle position
 * results in crossing over the next state transition boundary. It
 * simply takes the transition code from the state table (tr) and
 * performs the appropriate action, e.g. advancing R_LY, changing
 * R_STAT, etc.
 */

void stat_trans(int tr)
{
	switch(tr)
	{
	case tr_begin:
		lcd_begin();
		break;
	case tr_next:
		if (++R_LY > 0x99) R_LY = 0;
		break;
	case tr_refresh:
		lcd_refreshline();
		break;
	case tr_oam:
		stat_change(2);
		break;
	case tr_display:
		stat_change(3);
		break;
	case tr_hblank:
		stat_change(0);
		if (hw.hdma) hw_hdma();
		break;
	case tr_vblank:
		stat_change(1);
		break;
	}
}


/*
 * The state position is made up of three numbers: the absolute
 * position in cycles (p), the quotient as an index into the state
 * table (q), and a remainder (r), the number of cycles performed so
 * far within the present state. stat_divide performs this "division",
 * resulting in a remainder 0 <= r < length of state q, optionally
 * calling stat_trans each time q changes (if act != 0). This, we can
 * use stat_divide for resyncing the state after loading from a saved
 * image, or for actually advancing the state while the emulator is
 * running.
 */

static void state_divide(int act)
{
	while (R >= states[Q].len[S])
	{
		R -= states[Q++].len[S];
		if (act) stat_trans(states[Q].trans);
		if (states[Q].trans == tr_end)
		{
			Q = 0;
			P = R;
			if (act) stat_trans(states[Q].trans);
		}
	}
}

void state_reset()
{
	P = Q = R = S = 0;
}

void state_resync()
{
	Q = 0;
	R = P;
	state_divide(0);
}

void state_advance(int cycles)
{
	P += cycles;
	R += cycles;
	state_divide(1);
}

int state_cyclesleft()
{
	return states[Q].len[S] - R;
}


/*
 * state_step advances through the remainder of the present state,
 * then returns.
 */

void state_step()
{
	state_advance(cpu_emulate(state_cyclesleft()));
}







