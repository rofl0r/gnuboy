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
** tl_aa.c
**
** aalib thinlib driver
** $Id: tl_aa.c,v 1.4 2001/02/01 06:28:26 matt Exp $
*/

#include <stdio.h>
#include <string.h>

#include "tl_types.h"
#include "tl_bmp.h"
#include "tl_aa.h"

#ifndef NO_AALIB

#include <aalib.h>
static aa_context *aacontext;
static aa_palette aapalette;
static aa_renderparams *aaparams;

static bitmap_t *aa_screen = NULL;

bitmap_t *thin_aa_lockwrite(void)
{
   return aa_screen;
}

void thin_aa_freewrite(int num_dirties, rect_t *dirty_rects)
{
   UNUSED(num_dirties);
   UNUSED(dirty_rects);
}

/* TODO: generalize dirty rectangle code to be used on this */
void thin_aa_blit(bitmap_t *primary, int num_dirties, rect_t *dirty_rects)
{
   int line, pos;
   int xsiz, ysiz, xscale, yscale;
   int xratio, yratio;
   bitmap_t *screen;

   UNUSED(num_dirties);
   UNUSED(dirty_rects);

   screen = aa_screen;

   xsiz = screen->width;
   ysiz = screen->height;

   if (xsiz > primary->width)
      xsiz = primary->width;
   if (ysiz > primary->height)
      ysiz = primary->height;
   
   xratio = (primary->width << 16) / xsiz;
   yratio = (primary->height << 16) / ysiz;

   for (yscale = 0, line = 0; line < ysiz; line++, yscale += yratio)
   {
      for (xscale = 0, pos = 0; pos < xsiz; pos++, xscale += xratio)
         screen->line[line][pos] = primary->line[yscale >> 16][xscale >> 16];
   }

   aa_renderpalette(aacontext, aapalette, aaparams, 0, 0, 
                    aa_scrwidth(aacontext), aa_scrheight(aacontext));
   aa_flush(aacontext);
}

void thin_aa_shutdown(void)
{
   aa_close(aacontext);

   thin_bmp_destroy(&aa_screen);
}

int thin_aa_init(int width, int height)
{
   UNUSED(width);
   UNUSED(height);

   aacontext = aa_autoinit(&aa_defparams);

   if (NULL == aacontext)
      return -1;

   aaparams = aa_getrenderparams();

   aaparams->dither = AA_NONE;
   aaparams->bright = 75;
   aaparams->contrast = 64;
   aaparams->gamma = 2.5;

   aa_hidecursor(aacontext);

   memset(aa_image(aacontext), 0, aa_imgwidth(aacontext) * aa_imgheight(aacontext));

   if (NULL != aa_screen)
      thin_bmp_destroy(&aa_screen);

   aa_screen = thin_bmp_createhw(aa_image(aacontext), aa_imgwidth(aacontext), 
                                 aa_imgheight(aacontext), aa_imgwidth(aacontext));
   if (NULL == aa_screen)
      return -1;

   return 0;
}

void thin_aa_setpalette(rgb_t *palette)
{
   int i;

   for (i = 0; i < 256; i++)
      aa_setpalette(aapalette, i, palette[i].r, palette[i].g, palette[i].b);
}

int thin_aa_setmode(int width, int height)
{
   UNUSED(width);
   UNUSED(height);

   return 0;
}

#endif /* !NO_AALIB */

/*
** $Log: tl_aa.c,v $
** Revision 1.4  2001/02/01 06:28:26  matt
** thinlib now works under NT/2000
**
** Revision 1.3  2000/12/13 13:58:20  matt
** cosmetic fixes
**
** Revision 1.2  2000/11/05 16:32:36  matt
** thinlib round 2
**
** Revision 1.1  2000/11/05 06:29:03  matt
** initial revision
**
*/
