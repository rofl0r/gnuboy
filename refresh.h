#ifndef REFRESH_H
#define REFRESH_H
#include "defs.h"

#ifdef USE_ASM
#include "asm.h"
#endif

#ifdef ASM_REFRESH_1
void refresh_1(void *dest, byte *src, void *pal, int cnt);
#endif
#ifdef ASM_REFRESH_2
void refresh_2(void *dest, byte *src, void *pal, int cnt);
#endif
#ifdef ASM_REFRESH_3
void refresh_3(void *dest, byte *src, void *pal, int cnt);
#endif
#ifdef ASM_REFRESH_4
void refresh_4(void *dest, byte *src, void *pal, int cnt);
#endif
#ifdef ASM_REFRESH_1_2X
void refresh_1_2x(void *dest, byte *src, void *pal, int cnt);
#endif
#ifdef ASM_REFRESH_2_2X
void refresh_2_2x(void *dest, byte *src, void *pal, int cnt);
#endif
#ifdef ASM_REFRESH_3_2X
void refresh_3_2x(void *dest, byte *src, void *pal, int cnt);
#endif
#ifdef ASM_REFRESH_4_2X
void refresh_4_2x(void *dest, byte *src, void *pal, int cnt);
#endif
#ifdef ASM_REFRESH_2_3X
void refresh_2_3x(void *dest, byte *src, void *pal, int cnt);
#endif
#ifdef ASM_REFRESH_3_3X
void refresh_3_3x(void *dest, byte *src, void *pal, int cnt);
#endif
#ifdef ASM_REFRESH_4_3X
void refresh_4_3x(void *dest, byte *src, void *pal, int cnt);
#endif
#ifdef ASM_REFRESH_3_4X
void refresh_3_4x(void *dest, byte *src, void *pal, int cnt);
#endif
#ifdef ASM_REFRESH_4_4X
void refresh_4_4x(void *dest, byte *src, void *pal, int cnt);
#endif

#ifdef __GNUC__
#define MAY_ALIAS __attribute__((__may_alias__))
#else
#define MAY_ALIAS
#endif

typedef un16 un16a MAY_ALIAS;
typedef un32 un32a MAY_ALIAS;


#ifndef ASM_REFRESH_1
static void refresh_1(void *dest_, byte *src, void *pal_, int cnt)
{
	byte *dest = dest_, *pal = pal_;
	while(cnt--) *(dest++) = pal[*(src++)];
}
#endif

#ifndef ASM_REFRESH_2
static void refresh_2(void *dest_, byte *src, void *pal_, int cnt)
{
	un16a *dest = dest_, *pal = pal_;
	while (cnt--) *(dest++) = pal[*(src++)];
}
#endif

#ifndef ASM_REFRESH_3
static void refresh_3(void *dest_, byte *src, void *pal_, int cnt)
{
	byte *dest = dest_;
	un32a *pal = pal_;
	un32 c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c>>8;
		*(dest++) = c>>16;
	}
}
#endif

#ifndef ASM_REFRESH_4
static void refresh_4(void *dest_, byte *src, void *pal_, int cnt)
{
	un32a *dest = dest_, *pal = pal_;
	while (cnt--) *(dest++) = pal[*(src++)];
}
#endif


#ifndef ASM_REFRESH_1_2X
static void refresh_1_2x(void *dest_, byte *src, void *pal_, int cnt)
{
	byte *dest = dest_, *pal = pal_;
	byte c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c;
	}
}
#endif

#ifndef ASM_REFRESH_2_2X
static void refresh_2_2x(void *dest_, byte *src, void *pal_, int cnt)
{
	un16a *dest = dest_, *pal = pal_;
	un16 c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c;
	}
}
#endif

#ifndef ASM_REFRESH_3_2X
static void refresh_3_2x(void *dest_, byte *src, void *pal_, int cnt)
{
	byte *dest = dest_;
	un32a *pal = pal;
	un32 c;
	while (cnt--)
	{
		c = pal[*(src++)];
		dest[0] = dest[3] = c;
		dest[1] = dest[4] = c>>8;
		dest[2] = dest[5] = c>>16;
		dest += 6;
	}
}
#endif

#ifndef ASM_REFRESH_4_2X
static void refresh_4_2x(void *dest_, byte *src, void *pal_, int cnt)
{
	un32a *dest = dest_, *pal = pal_;
	un32 c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c;
	}
}
#endif

#ifndef ASM_REFRESH_2_3X
static void refresh_2_3x(void *dest_, byte *src, void *pal_, int cnt)
{
	un16a *dest = dest_, *pal = pal_;
	un16 c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c;
		*(dest++) = c;
	}
}
#endif

#ifndef ASM_REFRESH_3_3X
static void refresh_3_3x(void *dest_, byte *src, void *pal_, int cnt)
{
	byte* dest = dest_;
	un32a *pal = pal_;
	un32 c;
	while (cnt--)
	{
		c = pal[*(src++)];
		dest[0] = dest[3] = dest[6] = c;
		dest[1] = dest[4] = dest[7] = c>>8;
		dest[2] = dest[5] = dest[8] = c>>16;
		dest += 9;
	}
}
#endif

#ifndef ASM_REFRESH_4_3X
static void refresh_4_3x(void *dest_, byte *src, void *pal_, int cnt)
{
	un32a *dest = dest_, *pal = pal_;
	un32 c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c;
		*(dest++) = c;
	}
}
#endif

#ifndef ASM_REFRESH_3_4X
static void refresh_3_4x(void *dest_, byte *src, void *pal_, int cnt)
{
	byte *dest = dest_;
	un32a *pal = pal_;
	un32 c;
	while (cnt--)
	{
		c = pal[*(src++)];
		dest[0] = dest[3] = dest[6] = dest[9] = c;
		dest[1] = dest[4] = dest[7] = dest[10] = c>>8;
		dest[2] = dest[5] = dest[8] = dest[11] = c>>16;
		dest += 12;
	}
}
#endif

#ifndef ASM_REFRESH_4_4X
static void refresh_4_4x(void *dest_, byte *src, void *pal_, int cnt)
{
	un32a *dest = dest_, *pal = pal_;
	un32 c;
	while (cnt--)
	{
		c = pal[*(src++)];
		*(dest++) = c;
		*(dest++) = c;
		*(dest++) = c;
		*(dest++) = c;
	}
}
#endif

#endif

