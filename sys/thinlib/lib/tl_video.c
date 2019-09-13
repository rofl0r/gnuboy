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
** tl_video.h
**
** thinlib video routines
** $Id: tl_video.c,v 1.6 2001/03/12 06:06:56 matt Exp $
*/

#include "tl_types.h"
#include "tl_video.h"
#include "tl_vesa.h"
#include "tl_vga.h"
#include "tl_aa.h"
#include "tl_log.h"

typedef struct viddriver_s
{
   const char *name;
   int  (*init)(int width, int height, int bpp);
   void (*shutdown)(void);
   int  (*setmode)(int width, int height, int bpp);
   void (*setpalette)(rgb_t *palette, int index, int length);
   void (*waitvsync)(void);
   bitmap_t *(*lock)(void);
   void (*free)(int num_dirties, rect_t *dirty_rects);
   void (*blit)(bitmap_t *primary, int num_dirties, rect_t *dirty_rects);
   void (*set_scanlines)(bool scanlines_on);
   int  caps;
   bool scanlines;
} viddriver_t;

static viddriver_t vesa =
{
   "VESA 3.0 LFB",
   thin_vesa_init,
   thin_vesa_shutdown,
   thin_vesa_setmode,
   thin_vga_setpalette,
   thin_vga_waitvsync,
   thin_vesa_lockwrite,
   thin_vesa_freewrite,
   NULL,
   NULL,
   0,
   false
};

static viddriver_t vga =
{
   "VGA",
   thin_vga_init,
   thin_vga_shutdown,
   thin_vga_setmode,
   thin_vga_setpalette,
   thin_vga_waitvsync,
   thin_vga_lockwrite,
   thin_vga_freewrite,
   NULL,
   thin_vga_scanlines,
   THIN_VIDEO_SCANLINES,
   false
};

#ifndef NO_AALIB
static viddriver_t aalib =
{
   "aa-lib",
   thin_aa_init,
   thin_aa_shutdown,
   thin_aa_setmode,
   thin_aa_setpalette,
   NULL,
   thin_aa_lockwrite,
   thin_aa_freewrite,
   thin_aa_blit,
   NULL,
   THIN_VIDEO_CUSTOMBLIT,
   false
};
#endif /* !NO_AALIB */

static viddriver_t *driver_list[] = 
{
   &vesa,
   &vga,
#ifndef NO_AALIB
   &aalib,
#endif /* !NO_AALIB */
   NULL
};

static viddriver_t driver = 
{
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   0,
   false
};

int thin_vid_init(int width, int height, int bpp)
{
   /* cascade driver checks by iterating through all drivers */
   viddriver_t **iter;

   for (iter = driver_list; *iter != NULL; iter++)
   {
      if (0 == (*iter)->init(width, height, bpp))
      {
         driver = **iter;
         return 0;
      }
   }

   driver.name = NULL;
   thin_printf("thin: could not find any matching video modes.\n");
   return -1;
}

void thin_vid_shutdown(void)
{
   if (NULL != driver.name)
   {
      driver.shutdown();
      memset(&driver, 0, sizeof(viddriver_t));
   }
}

int thin_vid_getcaps(void)
{
   return driver.caps;
}

int thin_vid_scanlines(bool scanlines_on)
{
   if (NULL == driver.set_scanlines
       || 0 == (driver.caps & THIN_VIDEO_SCANLINES))
   {
      return -1;
   }

   if (scanlines_on != driver.scanlines)
   {
      driver.scanlines = scanlines_on;
      driver.set_scanlines(scanlines_on);
   }

   return 0;
}

int thin_vid_setmode(int width, int height, int bpp)
{
   if (driver.setmode(width, height, bpp))
   {
      thin_printf("thin: could not set %s video mode %dx%d %dbpp\n",
                  driver.name, width, height, bpp);
      return -1;
   }

   return 0;
}

void thin_vid_setpalette(rgb_t *palette, int index, int length)
{
   driver.setpalette(palette, index, length);
}

bitmap_t *thin_vid_lockwrite(void)
{
   return driver.lock();
}

void thin_vid_freewrite(int num_dirties, rect_t *dirty_rects)
{
   if (NULL != driver.free)
      driver.free(num_dirties, dirty_rects);
}

void thin_vid_customblit(bitmap_t *primary, int num_dirties,
                         rect_t *dirty_rects)
{
   THIN_ASSERT(driver.blit);

   driver.blit(primary, num_dirties, dirty_rects);
}

/*
** $Log: tl_video.c,v $
** Revision 1.6  2001/03/12 06:06:56  matt
** better keyboard driver, support for bit depths other than 8bpp
**
** Revision 1.5  2001/02/01 06:28:26  matt
** thinlib now works under NT/2000
**
** Revision 1.4  2000/12/16 17:36:38  matt
** let's not crash if we shut down video twice...
**
** Revision 1.3  2000/12/14 14:10:14  matt
** cleaner initialization
**
** Revision 1.2  2000/11/25 20:28:34  matt
** moved verboseness into correct places
**
** Revision 1.1  2000/11/06 02:21:29  matt
** initial revision
**
*/
