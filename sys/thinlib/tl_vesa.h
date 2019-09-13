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
** tl_vesa.h
**
** VESA code header
** $Id: tl_vesa.h,v 1.3 2001/02/01 06:28:26 matt Exp $
*/

#ifndef _TL_VESA_H_
#define _TL_VESA_H_

#include "tl_bmp.h"

extern int thin_vesa_init(int width, int height);
extern void thin_vesa_shutdown(void);

extern int thin_vesa_setmode(int width, int height);
extern void thin_vesa_setpalette(rgb_t *palette);

extern bitmap_t *thin_vesa_lockwrite(void);
extern void thin_vesa_freewrite(int num_dirties, rect_t *dirty_rects);

#endif /* !_TL_VESA_H_ */

/*
** $Log: tl_vesa.h,v $
** Revision 1.3  2001/02/01 06:28:26  matt
** thinlib now works under NT/2000
**
** Revision 1.2  2000/11/05 16:32:36  matt
** thinlib round 2
**
** Revision 1.1  2000/11/05 06:29:03  matt
** initial revision
**
*/
