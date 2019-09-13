





#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "input.h"
#include "rc.h"


#include "Version"


static char *defaultconfig[] =
{
	"bind esc quit",
	"bind up +up",
	"bind down +down",
	"bind left +left",
	"bind right +right",
	"bind alt +a",
	"bind ctrl +b",
	"bind enter +start",
	"bind space +select",
	"bind 1 \"set saveslot 1\"",
	"bind 2 \"set saveslot 2\"",
	"bind 3 \"set saveslot 3\"",
	"bind 4 \"set saveslot 4\"",
	"bind 5 \"set saveslot 5\"",
	"bind 6 \"set saveslot 6\"",
	"bind 7 \"set saveslot 7\"",
	"bind 8 \"set saveslot 8\"",
	"bind 9 \"set saveslot 9\"",
	"bind 0 \"set saveslot 0\"",
	"bind ins savestate",
	"bind del loadstate",
	"source gnuboy.rc",
	NULL
};


static void banner()
{
	fprintf(stderr, "\ngnuboy " VERSION "\n");
}

static void copyright()
{
	banner();
	fprintf(stderr,
"Copyright (C) 2000-2001 Laguna and Gilgamesh\n"
"\n"
"This program is free software; you can redistribute it and/or modify\n"
"it under the terms of the GNU General Public License as published by\n"
"the Free Software Foundation; either version 2 of the License, or\n"
"(at your option) any later version.\n"
"\n"
"This program is distributed in the hope that it will be useful,\n"
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"GNU General Public License for more details.\n"
"\n"
"You should have received a copy of the GNU General Public License\n"
"along with this program; if not, write to the Free Software\n"
"Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n"
"\n");
}

static void usage(char *name)
{
	copyright();
	fprintf(stderr, "Type %s --help for detailed help.\n\n", name);
	exit(1);
}

static void copying()
{
	copyright();
	exit(0);
}

static void help(char *name)
{
	banner();
	fprintf(stderr, "Usage: %s [options] romfile\n", name);
	fprintf(stderr, "\n"
"      --source FILE             read rc commands from FILE\n"
"      --bind KEY COMMAND        bind KEY to perform COMMAND\n"
"      --VAR=VALUE               set rc variable VAR to VALUE\n"
"      --VAR                     set VAR to 1 (turn on boolean options)\n"
"      --no-VAR                  set VAR to 0 (turn off boolean options)\n"
"      --showvars                list all available rc variables\n"
"      --help                    display this help and exit\n"
"      --version                 output version information and exit\n"
"      --copying                 show copying permissions\n"
"");
	exit(0);
}

static void version(char *name)
{
	fprintf(stderr, "%s-" VERSION "\n", name);
	exit(0);
}


void doevents()
{
	event_t ev;
	int st;

	ev_refresh();
	while (ev_getevent(&ev))
	{
		if (ev.type != EV_PRESS && ev.type != EV_RELEASE)
			continue;
		st = (ev.type != EV_RELEASE);
		rc_dokey(ev.code, st);
	}
}


static char *base(char *s)
{
	char *p;
	p = strrchr(s, '/');
	if (p) return p+1;
	return s;
}


int real_main(int argc, char *argv[])
{
	int i, j, c;
	char *opt, *arg, *cmd, *s, *rom = 0;

	/* Avoid initializing video if we don't have to */
	for (i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "--help"))
			help(base(argv[0]));
		else if (!strcmp(argv[i], "--version"))
			version(base(argv[0]));
		else if (!strcmp(argv[i], "--copying"))
			copying();
		else if (!strcmp(argv[i], "--bind")) i += 2;
		else if (!strcmp(argv[i], "--source")) i++;
		else if (!strcmp(argv[i], "--showvars"))
		{
			sys_dropperms();
			show_exports();
			exit(0);
		}
		else if (argv[i][0] == '-' && argv[i][1] == '-');
		else if (argv[i][0] == '-');
		else rom = argv[i];
	}
	
	if (!rom) usage(base(argv[0]));

	/* If we have special perms, drop them ASAP! */
	vid_preinit();

	init_exports();

	sys_initpath(argv[0]);

	for (i = 0; defaultconfig[i]; i++)
		rc_command(defaultconfig[i]);

	for (i = 1; i < argc; i++)
	{
		if (!strcmp(argv[i], "--bind"))
		{
			if (i + 2 >= argc) die("missing arguments to bind\n");
			cmd = malloc(strlen(argv[i+1]) + strlen(argv[i+2]) + 9);
			sprintf(cmd, "bind %s \"%s\"", argv[i+1], argv[i+2]);
			rc_command(cmd);
			free(cmd);
			i += 2;
		}
		else if (!strcmp(argv[i], "--source"))
		{
			if (i + 1 >= argc) die("missing argument to source\n");
			cmd = malloc(strlen(argv[i+1]) + 6);
			sprintf(cmd, "source %s", argv[++i]);
			rc_command(cmd);
			free(cmd);
		}
		else if (!strncmp(argv[i], "--no-", 5))
		{
			opt = strdup(argv[i]+5);
			cmd = malloc(strlen(opt) + 7);
			sprintf(cmd, "set %s 0", opt);
			rc_command(cmd);
			free(cmd);
			free(opt);
		}
		else if (argv[i][0] == '-' && argv[i][1] == '-')
		{
			opt = strdup(argv[i]+2);
			if ((s = strchr(opt, '=')))
			{
				*s = 0;
				arg = s+1;
			}
			else arg = "1";
			
			cmd = malloc(strlen(opt) + strlen(arg) + 6);
			sprintf(cmd, "set %s %s", opt, arg);
			
			rc_command(cmd);
			free(cmd);
			free(opt);
		}
		else if (argv[i][0] == '-');  /* short options not yet implemented */
	}

	vid_init();
	sys_init();
	pcm_init();

	rom = strdup(rom);
	sys_sanitize(rom);
	
	loader_init(rom);
	
	emu_reset();
	emu_run();

	/* never reached */
	return 0;
}











