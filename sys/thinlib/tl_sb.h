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
** tl_sb.h
**
** DOS Sound Blaster header file
** $Id: tl_sb.h,v 1.2 2000/11/05 16:32:36 matt Exp $
*/

#ifndef _TL_SB_H_
#define _TL_SB_H_

typedef void (*sbmix_t)(void *buffer, int size);

/* Sample format bitfields */
#define  SB_FORMAT_8BIT       0x00
#define  SB_FORMAT_16BIT      0x01
#define  SB_FORMAT_MONO       0x00
#define  SB_FORMAT_STEREO     0x02

extern int  thin_sb_init(int *sample_rate, int *buf_size, int *format);
extern void thin_sb_shutdown(void);
extern int  thin_sb_start(sbmix_t fillbuf);
extern void thin_sb_stop(void);
extern void thin_sb_setrate(int rate);

#endif /* !_TL_SB_H_ */

/*
** $Log: tl_sb.h,v $
** Revision 1.2  2000/11/05 16:32:36  matt
** thinlib round 2
**
** Revision 1.1  2000/11/05 06:29:03  matt
** initial revision
**
*/
