#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>

#include "menu.h"
#include "sys.h"
#include "input.h"
#include "rc.h"
#include "rckeys.h"
#include "emu.h"
#include "loader.h"

#include "ezmenu.h"
#include "font5x7.h"
#include "fb.h"
#include "lcd.h"

#define FONTW 5
#define FONTH 7
#define FONTMAX 127

static char *romdir;
static struct ezmenu ezm;
static enum menu_page currpage;
static unsigned char screen[160*144];
static char statusline[64];


void menu_init(void) {
	ezmenu_init(&ezm, 160, 144, FONTW, FONTH);
	ezm.wraparound = 1;
}

static int allowed_ext(char *fn) {
	static const char *exttab[] = {".gb", ".gbc", ".xz", ".gz", ".zip", 0};
	char *e = strrchr(fn, '.');
	if(!e) return 0;
	int i;
	for(i=0; exttab[i]; ++i)
		if(!strcmp(exttab[i], e)) return 1;
	return 0;
}

static int strlistcmp(const void* a, const void *b) {
	return strcmp(*(char* const*) a, *(char* const*) b);
}

static int strendswith(char *s, char *end) {
	size_t ls = strlen(s), le = strlen(end);
	return ls >= le && !strcmp(s + ls - le, end);
}

/* this is defined this way so a couple 0 bytes can be added that can
   be overwritten with the mapped key name, if desired */
static char* controller_menu_items[] = {
	(char[]){'+', 'a', 0},
	(char[]){'+', 'b', 0},
	(char[]){'+', 's', 'e', 'l', 'e', 'c', 't', 0},
	(char[]){'+', 's', 't', 'a', 'r', 't', 0},
	(char[]){'m', 'e', 'n', 'u', 0},
	(char[]){'b', 'a', 'c', 'k', 0},
};

void menu_initpage(enum menu_page page) {
	static const char* loaderr_menu_items[] = {"back"};
	static const char* main_menu_items[] = {
		"continue",
		"select rom",
		"load state",
		"save state",
		"controller config",
		"reset",
		"quit",
	};
	static const char* state_menu_items[] = {
		"state 0", "state 1", "state 2", "state 3", "state 4",
		"state 5", "state 6", "state 7", "state 8", "state 9",
		"back",
	};
	DIR *dir;
	struct dirent* file;
	char **dirlist = 0;
	unsigned dirlistcnt = 0, i;
	if(currpage == mp_romsel) {
		for(i=0; i<ezm.linecount; ++i)
			free(ezm.lines[i]);
		free(ezm.lines);
	}
	switch(page) {
	case mp_savestate:
	case mp_loadstate:
		ezmenu_setheader(&ezm, page == mp_savestate ? "Save state" : "Load state");
		ezmenu_setlines(&ezm, (void*)state_menu_items, sizeof(state_menu_items)/sizeof(main_menu_items[0]));
		ezmenu_setfooter(&ezm, " ");
		break;
	loaderr:
	case mp_loaderr:
		ezmenu_setheader(&ezm, loader_get_error());
		ezmenu_setlines(&ezm, (void*)loaderr_menu_items, 1);
		break;
	case mp_main:
		ezmenu_setheader(&ezm, "GNUBOY MAIN MENU");
		ezmenu_setlines(&ezm, (void*)main_menu_items, sizeof(main_menu_items)/sizeof(main_menu_items[0]));
		ezmenu_setfooter(&ezm, " ");
		break;
	case mp_controller:
		ezmenu_setheader(&ezm, "Controller config");
		ezmenu_setlines(&ezm, controller_menu_items, sizeof(controller_menu_items)/sizeof(controller_menu_items[0]));
		ezmenu_setfooter(&ezm, " ");
		break;
	case mp_romsel:
		dir = opendir(romdir);
		if(!dir) {
			loader_set_error("failed to open directory");
			if(strendswith(romdir, "/.."))
				romdir[strlen(romdir)-3] = 0;
			page = mp_loaderr;
			goto loaderr;
		}
		ezmenu_setheader(&ezm, "GNUBOY ROM Selection");
		ezmenu_setfooter(&ezm, " ");
		dirlist = malloc(sizeof(char*));
		dirlist[0] = strdup("..");
		dirlistcnt = 1;
		while((file = readdir(dir))) {
			if(file->d_name[0] == '.') continue;
			if (!allowed_ext(file->d_name)) {
				char path[1024];
				struct stat st;
				snprintf(path, sizeof path, "%s/%s", romdir, file->d_name);
				if(!stat(path, &st) && S_ISDIR(st.st_mode));
				else continue;
			}
			dirlist = realloc(dirlist, sizeof(char*)*(++dirlistcnt));
			dirlist[dirlistcnt-1] = strdup(file->d_name);
		}
		closedir(dir);
		qsort(dirlist+1, dirlistcnt-1, sizeof(char*), strlistcmp);
		ezmenu_setlines(&ezm, dirlist, dirlistcnt);
		break;
	}
	currpage = page;
}

static void font_blit(unsigned char* screen, int dx, int dy, int ch, int hl) {
	unsigned char* dest = screen + dy*160 + dx;
	unsigned const char* font = font5x7 + (ch>FONTMAX?0:ch)*FONTH*FONTW;
	int x,y;
	for(y = 0; y < FONTH; ++y, dest += 160)
		for(x = 0; x < FONTW; ++x)
			dest[x] = hl&&!font[y*FONTW+x]?2:font[y*FONTW+x];
}

struct palbkup {
	char pal1[3];
	short pal2[3];
	int pal4[3];
};

static void bkup_pal(struct palbkup *bk) {
	bk->pal1[0] = scan.pal1[0];
	bk->pal1[1] = scan.pal1[1];
	bk->pal1[2] = scan.pal1[2];
	bk->pal2[0] = scan.pal2[0];
	bk->pal2[1] = scan.pal2[1];
	bk->pal2[2] = scan.pal2[2];
	bk->pal4[0] = scan.pal4[0];
	bk->pal4[1] = scan.pal4[1];
	bk->pal4[2] = scan.pal4[2];
}

static void restore_pal(struct palbkup *bk) {
	scan.pal1[0] = bk->pal1[0];
	scan.pal1[1] = bk->pal1[1];
	scan.pal1[2] = bk->pal1[2];
	scan.pal2[0] = bk->pal2[0];
	scan.pal2[1] = bk->pal2[1];
	scan.pal2[2] = bk->pal2[2];
	scan.pal4[0] = bk->pal4[0];
	scan.pal4[1] = bk->pal4[1];
	scan.pal4[2] = bk->pal4[2];
}

static void menu_paint(void) {
	struct palbkup bk;
	/* since we use gb's lcd routines to draw to vram, we have to backup
	   previous palette entries */
	bkup_pal(&bk);
	scan.pal1[0] = 0;
	scan.pal1[1] = 0xff;
	scan.pal1[2] = 0x66;
	scan.pal2[0] = 0;
	scan.pal2[1] = 0xffff;
	scan.pal2[2] = 0x6666;
	scan.pal4[0] = 0;
	scan.pal4[1] = 0xffffffff; // alpha left or right ?
	scan.pal4[2] = 0x66666666;

	int x,y,l;
	for(y = 0; y < ezm.h; ++y) {
		l=strlen(ezm.vislines[y]);
		for(x = 0; x < ezm.w; ++x)
			font_blit(screen, x*FONTW, y*FONTH, x>=l?' ':ezm.vislines[y][x], y==ezm.vissel);
	}

	vid_begin();
	lcd_begin();

	for(y = 0; y < 144; ++y) {
		memcpy(scan.buf, screen+160*y, 160);
		lcd_linetovram();
	}

	vid_end();
	restore_pal(&bk);
}

static int menu_getevent(int *st) {
	event_t ev;
	int polled = 0;
	while (1) {
		if(!ev_getevent(&ev)) {
			if(polled) break;
			ev_poll(1);
			polled = 1;
			continue;
		}

		if (ev.type != EV_PRESS && ev.type != EV_RELEASE)
			continue;
		*st = (ev.type != EV_RELEASE);
		return ev.code;
	}
	return 0;
}


enum menu_key {
	mk_ignore,
	mk_down,
	mk_up,
	mk_ok,
	mk_cancel,
};

static enum menu_key menu_translate_key(int k) {
	char *bind;
	switch(k) {
	case K_UP:
	case K_JOYUP:
		return mk_up;
	case K_DOWN:
	case K_JOYDOWN:
		return mk_down;
	case K_ENTER:
		return mk_ok;
	case 0:
		return mk_ignore;
	default:
		bind = rc_getkeybind(k);
		if(!bind || (bind[0] != '+' && bind[0] != '-'))
			return mk_ignore;
		++bind;
		if(!strcmp(bind, "a")) return mk_ok;
		if(!strcmp(bind, "b")) return mk_cancel;
		if(!strcmp(bind, "start")) return mk_ok;
		if(!strcmp(bind, "up")) return mk_up;
		if(!strcmp(bind, "down")) return mk_down;
		return mk_ignore;
	}
}

void menu_enter(void) {
entry:;
	ezmenu_update(&ezm);
	menu_paint();
	while(1) {
		int st, k = menu_getevent(&st);
		if (!k || !st) goto next;
		switch(menu_translate_key(k)) {
		case mk_up:
			ezmenu_userinput(&ezm, EZM_UP);
			menu_paint();
			break;
		case mk_down:
			ezmenu_userinput(&ezm, EZM_DOWN);
			menu_paint();
			break;
		case mk_cancel:
			if(currpage == mp_main) break;
			menu_initpage(mp_main);
			goto entry;
		case mk_ok:
			if(currpage == mp_romsel) {
				char rd[1024];
				struct stat st;
				snprintf(rd, sizeof rd, "%s/%s", romdir, ezm.vislines[ezm.vissel]);
				if(!stat(rd, &st) && S_ISDIR(st.st_mode)) {
					free(romdir);
					romdir = strdup(rd);
					menu_initpage(mp_romsel);
					goto entry;
				} else {
					loader_unload();
					if(load_rom_and_rc(rd)) {
						menu_initpage(mp_loaderr);
						goto entry;
					}
					goto out;
				}
			} else if (currpage == mp_main) {
				if(!strcmp(ezm.vislines[ezm.vissel], "continue")) {
					if(emu_paused()) goto out;
				}
				else if(!strcmp(ezm.vislines[ezm.vissel], "reset")) {
					if(emu_paused()) {
						emu_reset();
						goto out;
					}
				}
				else if(!strcmp(ezm.vislines[ezm.vissel], "select rom")) {
					menu_initpage(mp_romsel);
					goto entry;
				}
				else if(!strcmp(ezm.vislines[ezm.vissel], "controller config")) {
					menu_initpage(mp_controller);
					goto entry;
				}
				else if(!strcmp(ezm.vislines[ezm.vissel], "load state")) {
					menu_initpage(mp_loadstate);
					goto entry;
				}
				else if(!strcmp(ezm.vislines[ezm.vissel], "save state")) {
					menu_initpage(mp_savestate);
					goto entry;
				}
				else if(!strcmp(ezm.vislines[ezm.vissel], "quit")) {
					loader_unload();
					exit(0);
				}
			} else if (currpage == mp_loaderr) {
				menu_initpage(mp_romsel);
				goto entry;
			} else if (currpage == mp_controller) {
				if(!strcmp(ezm.vislines[ezm.vissel], "back")) {
					menu_initpage(mp_main);
					goto entry;
				}
				ezmenu_setfooter(&ezm, "press a button to override");
				ezmenu_update(&ezm);
				menu_paint();
				while((k = menu_getevent(&st)) == 0 || !st);
				rc_unbindkey(k_keyname(k));
				rc_bindkey(k_keyname(k), ezm.vislines[ezm.vissel]);
				snprintf(statusline, sizeof statusline, "key assigned: %s", k_keyname(k));
				ezmenu_setfooter(&ezm, statusline);
				ezmenu_update(&ezm);
				menu_paint();
			} else if (currpage == mp_savestate || currpage == mp_loadstate) {
				if(!strcmp(ezm.vislines[ezm.vissel], "back")) {
					menu_initpage(mp_main);
					goto entry;
				}
				if(!emu_paused()) break;
				int n = atoi(ezm.vislines[ezm.vissel]+6);
				if(currpage == mp_savestate) state_save(n);
				else state_load(n);
				goto out;
			}
			break;
		default:
			next:;
			sys_sleep(300);
		}
	}
	out:; return;
}

rcvar_t menu_exports[] =
{
	RCV_STRING("romdir", &romdir, "rom directory"),
	RCV_END
};

