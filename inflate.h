#ifndef INFLATE_H
#define INFLATE_H

int unzip (const unsigned char *data, long *p, int (* callback) (unsigned char d));
int inflate (const unsigned char *data, long *p,int (* callback) (unsigned char d));

#endif

