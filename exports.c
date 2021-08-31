#include <stdio.h>
#include <stdlib.h>

#include "rc.h"

extern rcvar_t rcfile_exports[], emu_exports[], loader_exports[],
	lcd_exports[], rtc_exports[], debug_exports[], sound_exports[],
	vid_exports[], joy_exports[], pcm_exports[];


rcvar_t *sources[] =
{
	rcfile_exports,
	emu_exports,
	loader_exports,
	lcd_exports,
	rtc_exports,
	debug_exports,
	sound_exports,
	vid_exports,
	joy_exports,
	pcm_exports,
	NULL
};


void init_exports()
{
	rcvar_t **s = sources;

	while (*s)
		rc_exportvars(*(s++));
}


void show_exports()
{
	int i, j, n, *vec;
	char value[256], *v;

	for (i = 0; sources[i]; i++)
		for (j = 0; sources[i][j].name; j++) {
			v = value;
			switch (sources[i][j].type) {
			case rcv_bool:
			case rcv_int:
				sprintf(value, "%d", rc_getint(sources[i][j].name));
				break;
			case rcv_string:
				v = rc_getstr(sources[i][j].name);
				v = v ? v : "";
				break;
			case rcv_vector:
				vec = rc_getvec(sources[i][j].name);
				if (vec)
					for (n = 0; n < sources[i][j].len; ++n)
						v += sprintf(v, "%d ", *(vec++));
				else value[0] = 0;
				v = value;
				break;
			}
			printf("%-16s %-8s %s\n",
				sources[i][j].name,
				rc_type_to_string(sources[i][j].type),
				v);
		}
}
