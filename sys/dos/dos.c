/*
 * dos.c
 *
 * System interface for DOS.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#define US(n) (unsigned long) ( (1000000.0/UCLOCKS_PER_SEC) * (n) )


static struct timeval tv_init;
static int inittimeok;
static char *progname;

static void inittime()
{
	gettimeofday(&tv_init, NULL);
	inittimeok = 1;
}

int sys_msecs()
{
	struct timeval tv;
	int secs, usecs;
	
	if (!inittimeok) inittime();
	gettimeofday(&tv, NULL);
	secs = tv.tv_sec - tv_init.tv_sec;
	usecs = tv.tv_usec - tv_init.tv_usec;
	return secs*1000 + usecs/1000;
}

int sys_realtime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec;
}

void *sys_timer()
{
	uclock_t *cl;
	
	cl = malloc(sizeof *cl);
	*cl = uclock();
	return cl;
}

int sys_elapsed(uclock_t *cl)
{
	uclock_t now;
	int usecs;

	now = uclock();
	usecs = US(now - *cl);
	*cl = now;
	return usecs;
}

void sys_sleep(int us)
{
	uclock_t start;
	if (us <= 0) return;
	start = uclock();
	while(US(uclock()-start) < us);
}

void sys_checkdir(char *path, int wr)
{
}

void sys_initpath(char *exe)
{
	char *buf, *home, *p;

	home = strdup(exe);
	p = strrchr(home, '/');
	if (p) *p = 0;
	else
	{
		rc_setvar("rcpath", ".");
		rc_setvar("savedir", ".");
		return;
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
	int i;
	for (i = 0; s[i]; i++)
		if (s[i] == '\\') s[i] = '/';
}

void sys_shutdown(int err)
{
	cleanup(err);
	vid_close();
	pcm_close();
}

void die(char *fmt, ...)
{
	va_list ap;
	
	sys_shutdown(1);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(1);
}


static void fatalsignal(int s)
{
	char *signame;
	switch (s)
	{
	case SIGINT:
		signame = "SIGINT";
		break;
	case SIGQUIT:
		signame = "SIGQUIT";
		break;
	case SIGTERM:
		signame = "SIGTERM";
		break;
	case SIGSEGV:
		signame = "SIGSEGV";
		break;
	case SIGPIPE:
		signame = "SIGSEGV";
		break;
	case SIGFPE:
		signame = "SIGFPE";
		break;
	default:
		signame = "???";
		break;
	}
	die("fatal signal: %s\n", signame);
}

void sys_init()
{
	signal(SIGINT, fatalsignal);
	signal(SIGQUIT, fatalsignal);
	signal(SIGTERM, fatalsignal);
	signal(SIGSEGV, fatalsignal);
	signal(SIGPIPE, fatalsignal);
	signal(SIGFPE, fatalsignal);
}

void sys_dropperms()
{
}

int main(int argc, char *argv[])
{
	progname = argv[0];
	return real_main(argc, argv);
}





