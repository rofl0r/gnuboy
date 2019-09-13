/*
** thinlib (c) 2000 Matthew Conte (matt@conte.com)
**
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of version 2 of the GNU Library General 
** Public License as published by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful, 
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
** Library General Public License for more details.  To obtain a 
** copy of the GNU Library General Public License, write to the Free 
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
**
**
** tl_bmp.h
**
** Bitmap object defines / prototypes
** $Id: tl_bmp.h,v 1.2 2000/12/13 13:58:20 matt Exp $
*/

#ifndef _TL_BMP_H_
#define _TL_BMP_H_

#include "tl_types.h"

/* a bitmap rectangle */
typedef struct rect_s
{
   int16 x, y;
   uint16 w, h;
} rect_t;

typedef struct rgb_s
{
   int r, g, b;
} rgb_t;

typedef struct bitmap_s
{
   int width, height, pitch;
   bool hardware;             /* is data a hardware region? */
   uint8 *data;               /* protected */
   uint8 *line[0];            /* will hold line pointers */
} bitmap_t;

extern void thin_bmp_clear(const bitmap_t *bitmap, uint8 color);
extern bitmap_t *thin_bmp_create(int width, int height, int overdraw);
extern bitmap_t *thin_bmp_createhw(uint8 *addr, int width, int height, int pitch);
extern void thin_bmp_destroy(bitmap_t **bitmap);

#endif /* !_TL_BMP_H_ */

/*
** $Log: tl_bmp.h,v $
** Revision 1.2  2000/12/13 13:58:20  matt
** cosmetic fixes
**
** Revision 1.1  2000/11/05 16:32:15  matt
** initial revision
**
*/
