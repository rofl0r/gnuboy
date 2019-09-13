





#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdarg.h>
#include <signal.h>

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
	"bind joyup +up",
	"bind joydown +down",
	"bind joyleft +left",
	"bind joyright +right",
	"bind joy0 +b",
	"bind joy1 +a",
	"bind joy2 +select",
	"bind joy3 +start",
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
	fprintf(stderr,"Copyright (C) 2000-2001 Laguna and Gilgamesh
Portions contributed by other authors; see CREDITS for details.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

");
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
	fprintf(stderr, "
      --source FILE             read rc commands from FILE
      --bind KEY COMMAND        bind KEY to perform COMMAND
      --VAR=VALUE               set rc variable VAR to VALUE
      --VAR                     set VAR to 1 (turn on boolean options)
      --no-VAR                  set VAR to 0 (turn off boolean options)
      --showvars                list all available rc variables
      --help                    display this help and exit
      --version                 output version information and exit
      --copying                 show copying permissions

");
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

	//ev_refresh();
	ev_poll();
	while (ev_getevent(&ev))
	{
		if (ev.type != EV_PRESS && ev.type != EV_RELEASE)
			continue;
		st = (ev.type != EV_RELEASE);
		rc_dokey(ev.code, st);
	}
}




static void shutdown()
{
	vid_close();
	pcm_close();
}

void die(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(1);
}

static int bad_signals[] =
{
	/* These are all standard, so no need to #ifdef them... */
	SIGINT, SIGSEGV, SIGTERM, SIGFPE, SIGABRT, SIGILL,
#ifdef SIGQUIT
	SIGQUIT,
#endif
#ifdef SIGPIPE
	SIGPIPE,
#endif
	0
};

static void fatalsignal(int s)
{
	die("Signal %d\n", s);
}

static void catch_signals()
{
	int i;
	for (i = 0; bad_signals[i]; i++)
		signal(bad_signals[i], fatalsignal);
}



static char *base(char *s)
{
	char *p;
	p = strrchr(s, '/');
	if (p) return p+1;
	return s;
}


int main(int argc, char *argv[])
{
	int i;
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
			show_exports();
			exit(0);
		}
		else if (argv[i][0] == '-' && argv[i][1] == '-');
		else if (argv[i][0] == '-' && argv[i][1]);
		else rom = argv[i];
	}
	
	if (!rom) usage(base(argv[0]));

	/* If we have special perms, drop them ASAP! */
	vid_preinit();

	init_exports();

	s = strdup(argv[0]);
	sys_sanitize(s);
	sys_initpath(s);

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
			while ((s = strchr(opt, '-'))) *s = '_';
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
			while ((s = strchr(opt, '-'))) *s = '_';
			while ((s = strchr(arg, ','))) *s = ' ';
			
			cmd = malloc(strlen(opt) + strlen(arg) + 6);
			sprintf(cmd, "set %s %s", opt, arg);
			
			rc_command(cmd);
			free(cmd);
			free(opt);
		}
		/* short options not yet implemented */
		else if (argv[i][0] == '-' && argv[i][1]);
	}

	/* FIXME - make interface modules responsible for atexit() */
	atexit(shutdown);
	catch_signals();
	vid_init();
	pcm_init();

	rom = strdup(rom);
	sys_sanitize(rom);
	
	loader_init(rom);
	
	emu_reset();
	emu_run();

	/* never reached */
	return 0;
}











