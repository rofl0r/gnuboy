

#include <stdlib.h>

#include "rc.h"

extern rcvar_t rcfile_exports[], emu_exports[], loader_exports[],
	lcd_exports[], debug_exports[], vid_exports[];


rcvar_t *sources[] =
{
	rcfile_exports,
	emu_exports,
	loader_exports,
	lcd_exports,
	debug_exports,
	vid_exports,
	NULL
};


void init_exports()
{
	rcvar_t **s = sources;
	
	while (*s)
		rc_exportvars(*(s++));
}



