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
** $Id: tl_video.h,v 1.3 2001/03/12 06:06:56 matt Exp $
*/

#ifndef _TL_VIDEO_H_
#define _TL_VIDEO_H_

#include "tl_types.h"
#include "tl_bmp.h"

/* video driver capabilities */
#define  THIN_VIDEO_CUSTOMBLIT   0x0001
#define  THIN_VIDEO_SCANLINES    0x0002

extern int thin_vid_getcaps(void);

extern int thin_vid_init(int width, int height, int bpp);
extern void thin_vid_shutdown(void);

extern int thin_vid_setmode(int width, int height, int bpp);
extern void thin_vid_setpalette(rgb_t *palette, int index, int length);
extern int thin_vid_scanlines(bool scanlines_on);

extern bitmap_t *thin_vid_lockwrite(void);
extern void thin_vid_freewrite(int num_dirties, rect_t *dirty_rects);

extern void thin_vid_customblit(bitmap_t *primary, int num_dirties,
                                rect_t *dirty_rects);

#endif /* !_TL_VIDEO_H_ */

/*
** $Log: tl_video.h,v $
** Revision 1.3  2001/03/12 06:06:56  matt
** better keyboard driver, support for bit depths other than 8bpp
**
** Revision 1.2  2001/02/01 06:28:26  matt
** thinlib now works under NT/2000
**
** Revision 1.1  2000/11/06 02:21:29  matt
** initial revision
**
*/
