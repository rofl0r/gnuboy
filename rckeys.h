#ifndef RCKEYS_H
#define RCKEYS_H

int rc_dokey(int key, int st);
int rc_bindkey(char *keyname, char *cmd);
int rc_unbindkey(char *keyname);
void rc_unbindall();
char *rc_getkeybind(int key);

#endif

