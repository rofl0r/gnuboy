
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char *strdup();
#include <linux/joystick.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "input.h"
#include "rc.h"

static int usejoy = 1;
static char *joydev;
static int joyfd = -1;
static int pos[2], max[2], min[2];
static const int axis[2][2] =
{
	{ K_JOYLEFT, K_JOYRIGHT },
	{ K_JOYUP, K_JOYDOWN }
};

rcvar_t joy_exports[] =
{
	RCV_BOOL("joy", &usejoy),
	RCV_STRING("joy_device", &joydev),
	RCV_END
};




void joy_init()
{
	if (!usejoy) return;
	if (!joydev) joydev = strdup("/dev/js0");
	joyfd = open(joydev, O_RDONLY|O_NONBLOCK);
}

void joy_close()
{
	close(joyfd);
}

void joy_poll()
{
	struct js_event js;
	event_t ev;
	int n;
	
	if (joyfd < 0) return;

	while (read(joyfd,&js,sizeof(struct js_event)) == sizeof(struct js_event))
	{
		switch(js.type)
		{
		case JS_EVENT_BUTTON:
			ev.type = js.value ? EV_PRESS : EV_RELEASE;
			ev.code = K_JOY0 + js.number;
			ev_postevent(&ev);
			break;
		case JS_EVENT_AXIS:
			n = js.number & 1;
			if (js.value < min[n]) min[n] = js.value;
			else if(js.value > max[n]) max[n] = js.value;
			ev.code = axis[n][0];
			if(js.value < (min[n]>>2) && js.value < pos[n])
			{
				ev.type = EV_PRESS;
				ev_postevent(&ev);
			}
			else if (js.value > pos[n])
			{
				ev.type = EV_RELEASE;
				ev_postevent(&ev);
			}
			ev.code = axis[n][1];
			if(js.value > (max[n]>>2) && js.value > pos[n])
			{
				ev.type = EV_PRESS;
				ev_postevent(&ev);
			}
			else if (js.value < pos[n])
			{
				ev.type = EV_RELEASE;
				ev_postevent(&ev);
			}
			pos[n] = js.value;
		}
	}
}
