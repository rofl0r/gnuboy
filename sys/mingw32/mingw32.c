/*
 * MinGW32 system file
 * based on nix.c and dos.c
 * req's SDL
 * -Dave Kiddell
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SDL/SDL.h>

int sys_msecs()
{
	return SDL_GetTicks();
}

int sys_realtime()
{
	return time(NULL);
}

void *sys_timer()
{
	Uint32 *tv;
	
	tv = malloc(sizeof *tv);
	*tv = SDL_GetTicks() * 1000;
	return tv;
}

int sys_elapsed(Uint32 *cl)
{
	Uint32 now;
	Uint32 usecs;

	now = SDL_GetTicks();
	usecs = now - *cl;
	*cl = now;
	return usecs;
}

void sys_sleep(int us)
{
	/* dbk: for some reason 2000 works..
	   maybe its just compensation for the time it takes for SDL_Delay to
	   execute, or maybe sys_timer is too slow */
    SDL_Delay(us/2000);
}

void sys_initpath(char *exe)
{
	char *buf, *home, *p;

	home = strdup(exe);
	p = strrchr(home, '/');
	if (p) *p = 0;
	else
	{
		/* FIXME dbk: this isnt working */
		/*rc_setvar("rcpath",1, ".");
		rc_setvar("savedir",1, ".");
		return;*/
	}
	buf = malloc(strlen(home) + 8);
	sprintf(buf, ".:%s/", home);
	rc_setvar("rcpath", 1, &buf);
	sprintf(buf, ".", home);
	rc_setvar("savedir", 1, &buf);
	free(buf);
}

void sys_sanitize(char *s)
{
}

static void cleanup()
{
	sram_save();
	rtc_save();
	/* IDEA - if error, write emergency savestate..? */
}

void shutdown(int err)
{
	cleanup();
	vid_close();
	pcm_close();
}

void sys_checkdir(char *path, int wr)
{
}
