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
** tl_dpp.h
**
** DOS DirectPad Pro scanning code prototypes
** $Id: tl_dpp.h,v 1.2 2000/11/05 16:32:36 matt Exp $
*/

#ifndef _TL_DPP_H_
#define _TL_DPP_H_

#include "tl_types.h"

typedef struct dpp_s
{
   uint16 port; /* LPT port */
   int pad_num;
   int down, up, left, right;
   int b, a, select, start;
} dpp_t;

extern int thin_dpp_add(dpp_t *pad, uint16 port, int pad_num);
extern int thin_dpp_init(void);
extern void thin_dpp_shutdown(void);
extern void thin_dpp_read(dpp_t *pad);

#endif /* !_TL_DPP_H_ */

/*
** $Log: tl_dpp.h,v $
** Revision 1.2  2000/11/05 16:32:36  matt
** thinlib round 2
**
** Revision 1.1  2000/11/05 06:29:03  matt
** initial revision
**
*/
