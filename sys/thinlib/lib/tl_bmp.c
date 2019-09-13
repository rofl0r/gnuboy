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
** tl_bmp.c
**
** Bitmap object manipulation routines
** $Id: tl_bmp.c,v 1.4 2001/03/12 06:06:55 matt Exp $
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tl_types.h"
#include "tl_bmp.h"

void thin_bmp_clear(const bitmap_t *bitmap, uint8 color)
{
   memset(bitmap->data, color, bitmap->pitch * bitmap->height);
}

#define  BPP_PITCH(pitch, bpp)   ((pitch) * (((bpp) + 7) / 8))

static bitmap_t *_make_bitmap(uint8 *data_addr, bool hw, int width, 
                              int height, int bpp, int pitch, int overdraw)
{
   bitmap_t *bitmap;
   int i;

   /* sometimes our data address is zero - in the case
   ** of setting video selectors for VESA mode with far pointers.
   */

   /* Make sure to add in space for line pointers */
   bitmap = malloc(sizeof(bitmap_t) + (sizeof(uint8 *) * height));
   if (NULL == bitmap)
      return NULL;

   bitmap->hardware = hw;
   bitmap->height = height;
   bitmap->width = width;
   bitmap->bpp = bpp;
   bitmap->data = data_addr;
   bitmap->pitch = BPP_PITCH(pitch, bpp);

   /* Set up line pointers */
   bitmap->line[0] = bitmap->data + overdraw;

   for (i = 1; i < height; i++)
      bitmap->line[i] = bitmap->line[i - 1] + bitmap->pitch;

   return bitmap;
}

/* Allocate and initialize a bitmap structure */
bitmap_t *thin_bmp_create(int width, int height, int bpp, int overdraw)
{
   uint8 *addr;
   
   /* left and right overdraw, and dword aligned */
   int pitch = width + (overdraw * 2);
   pitch = (pitch + 3) & ~3;

   addr = malloc(height * BPP_PITCH(pitch, bpp));
   if (NULL == addr)
      return NULL;

   return _make_bitmap(addr, false, width, height, bpp, pitch, overdraw);
}

/* allocate and initialize a hardware bitmap */
bitmap_t *thin_bmp_createhw(uint8 *addr, int width, int height, int bpp, int pitch)
{
   return _make_bitmap(addr, true, width, height, bpp, pitch, 0); /* zero overdraw */
}

/* Deallocate space for a bitmap structure */
void thin_bmp_destroy(bitmap_t **bitmap)
{
   if (*bitmap)
   {
      if ((*bitmap)->data && false == (*bitmap)->hardware)
         free((*bitmap)->data);
      free(*bitmap);
      *bitmap = NULL;
   }
}

/*
** $Log: tl_bmp.c,v $
** Revision 1.4  2001/03/12 06:06:55  matt
** better keyboard driver, support for bit depths other than 8bpp
**
** Revision 1.3  2001/02/19 02:55:01  matt
** our service department is now accepting null pointers.
**
** Revision 1.2  2001/02/01 06:28:26  matt
** thinlib now works under NT/2000
**
** Revision 1.1  2000/11/05 16:32:15  matt
** initial revision
**
*/
