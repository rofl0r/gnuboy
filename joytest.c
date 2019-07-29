#include "sys.h"
#include "events.c"
#include "keytable.c"
#include <stdio.h>
#include <unistd.h>

int main() {
	joy_init();
	event_t e;
	while(1) {
		joy_poll();
		if(ev_getevent(&e)) {
			static const char* evt_str[] = {
				[EV_NONE] = "none",
				[EV_PRESS] = "press",
				[EV_RELEASE]  = "release",
				[EV_REPEAT] = "repeat",
				[EV_MOUSE] = "mouse",
			};
			printf("%s: %s\n", evt_str[e.type], k_keyname(e.code));
		} else {
			usleep(300);
		}
	}
}

