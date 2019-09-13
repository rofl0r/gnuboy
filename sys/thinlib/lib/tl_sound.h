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
** tl_sound.h
**
** thinlib sound routines
** $Id: tl_sound.h,v 1.1 2000/12/16 17:29:20 matt Exp $
*/

#ifndef _TL_SOUND_H_
#define _TL_SOUND_H_

#include "tl_types.h"

#define  THIN_SOUND_8BIT   0x00
#define  THIN_SOUND_16BIT  0x01
#define  THIN_SOUND_MONO   0x00
#define  THIN_SOUND_STEREO 0x02

typedef void (*audio_callback_t)(void *buffer, int samples);

typedef struct thinsound_s
{
   int sample_rate;
   int frag_size;
   int format;
   audio_callback_t callback;   
} thinsound_t;

extern int thin_sound_init(thinsound_t *sound_params);
extern void thin_sound_shutdown(void);

extern void thin_sound_start(void);
extern void thin_sound_stop(void);
extern void thin_sound_setrate(int sample_rate);

#endif /* !_TL_SOUND_H_ */

/*
** $Log: tl_sound.h,v $
** Revision 1.1  2000/12/16 17:29:20  matt
** initial revision
**
*/
