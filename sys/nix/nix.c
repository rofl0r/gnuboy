/*
 * nix.c
 *
 * System interface for *nix systems.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#define DOTDIR ".gnuboy"

#ifndef HAVE_USLEEP
static void usleep(unsigned long us)
{
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = us;
	select(0, NULL, NULL, NULL, &tv);
}
#endif

void *sys_timer()
{
	struct timeval *tv;
	
	tv = malloc(sizeof(struct timeval));
	gettimeofday(tv, NULL);
	return tv;
}

int sys_elapsed(struct timeval *prev)
{
	struct timeval tv;
	int secs, usecs;
	
	gettimeofday(&tv, NULL);
	secs = tv.tv_sec - prev->tv_sec;
	usecs = tv.tv_usec - prev->tv_usec;
	*prev = tv;
	if (!secs) return usecs;
	return 1000000 + usecs;
}

static void hardsleep(int us)
{
	struct timeval tv, start;
	int secs, usecs;
	
	gettimeofday(&tv, NULL);
	start = tv;
	usecs = 0;
	while(usecs < us)
	{
		gettimeofday(&tv, NULL);
		secs = tv.tv_sec - start.tv_sec;
		usecs = tv.tv_usec - start.tv_usec;
		if (secs) usecs += 1000000;
	}
}

void sys_sleep(int us)
{
	if (us <= 0) return;
	usleep(us);
	/* hardsleep(us); */
}

void sys_checkdir(char *path, int wr)
{
	char *p;
	if (access(path, X_OK | (write ? W_OK : 0)))
	{
		if (!access(path, F_OK))
			die("cannot access %s: %s\n", path, strerror(errno));
		p = strrchr(path, '/');
		if (!p) die("descended to root trying to create dirs\n");
		*p = 0;
		sys_checkdir(path, wr);
		*p = '/';
		if (mkdir(path, 0777))
			die("cannot create %s: %s\n", path, strerror(errno));
	}
}

void sys_initpath()
{
	char *buf, *home = getenv("HOME");
	if (!home)
	{
		rc_setvar("rcpath", ".");
		rc_setvar("savedir", ".");
		return;
	}
	buf = malloc(strlen(home) + strlen(DOTDIR) + 8);
	sprintf(buf, "%s/" DOTDIR ":.", home);
	rc_setvar("rcpath", 1, &buf);
	sprintf(buf, "%s/" DOTDIR "/saves" , home);
	rc_setvar("savedir", 1, &buf);
	free(buf);
}

void sys_sanitize(char *s)
{
}




