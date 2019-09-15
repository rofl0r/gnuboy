
#ifndef __MEM_H__
#define __MEM_H__


#include "defs.h"



#define MBC_NONE 0
#define MBC_MBC1 1
#define MBC_MBC2 2
#define MBC_MBC3 3
#define MBC_MBC5 5
#define MBC_RUMBLE 15

struct mbc
{
	int type;
	int model;
	int rombank;
	int rambank;
	int romsize;
	int ramsize;
	int enableram;
	int batt;
	byte *rmap[0x10], *wmap[0x10];
};


struct rom
{
	byte (*bank)[16384];
};

struct ram
{
	byte ibank[8][4096];
	byte (*sbank)[8192];
	byte stack[128];
	int loaded;
};




extern struct mbc mbc;
extern struct rom rom;
extern struct ram ram;





void mem_updatemap();
void ioreg_write(byte r, byte b);
void mbc_write(addr a, byte b);
void mem_write(addr a, byte b);
byte mem_read(addr a);



#define READB(a) ( mbc.rmap[(a)>>12] \
? mbc.rmap[(a)>>12][(a)&0xFFF] \
: mem_read((a)) )
#define READW(a) ( READB((a)) | ((word)READB((a)+1)<<8) )

#define WRITEB(a, b) ( mbc.wmap[(a)>>12] \
? ( mbc.wmap[(a)>>12][(a)&0xFFF] = (b) ) \
: ( mem_write((a), (b)), (b) ) )
#define WRITEW(a, w) ( WRITEB((a), (w)&0xFF), WRITEB((a)+1, (w)>>8) )

#define CGB_ROM (!forcedmg && rom.bank && (rom.bank[0][0x0143] == 0x80 \
|| rom.bank[0][0x0143] == 0xC0))




#endif



