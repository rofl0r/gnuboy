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
** tl_sound.c
**
** sound driver
** $Id: tl_sound.c,v 1.3 2001/03/12 06:06:55 matt Exp $
*/

#include "tl_types.h"
#include "tl_sound.h"
#include "tl_sb.h"
#include "tl_log.h"
/* TODO: make GuS driver... */

typedef struct snddriver_s
{
   const char *name;
   int  (*init)(int *sample_rate, int *frag_size, int *format);
   void (*shutdown)(void);
   int  (*start)(audio_callback_t callback);
   void (*stop)(void);
   void (*setrate)(int sample_rate);
   audio_callback_t callback;
} snddriver_t;

static snddriver_t sb =
{
   "Sound Blaster",
   thin_sb_init,
   thin_sb_shutdown,
   thin_sb_start,
   thin_sb_stop,
   thin_sb_setrate,
   NULL
};

static snddriver_t *driver_list[] =
{
   &sb,
   /* TODO: add more drivers here */
   NULL
};

static snddriver_t snddriver = 
{
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
   NULL,
};


int thin_sound_init(thinsound_t *sound_params)
{
   snddriver_t **iter;
   int sample_rate, frag_size, format;

   THIN_ASSERT(sound_params);
   
   sample_rate = sound_params->sample_rate;
   frag_size = sound_params->frag_size;
   format = sound_params->format;

   for (iter = driver_list; *iter != NULL; iter++)
   {
      if (0 == (*iter)->init(&sample_rate, &frag_size, &format))
      {
         snddriver = **iter;

         /* copy the parameters back */
         sound_params->sample_rate = sample_rate;
         sound_params->frag_size = frag_size;
         sound_params->format = format;

         /* and set the callback */
         snddriver.callback = sound_params->callback;

         return 0;
      }
   }

   snddriver.name = NULL;

   thin_printf("thin: could not find any sound drivers.\n");
   return -1;
}

void thin_sound_shutdown(void)
{
   if (NULL == snddriver.name)
      return;

   snddriver.shutdown();
   memset(&snddriver, 0, sizeof(snddriver_t));
}

void thin_sound_start(void)
{
   if (NULL == snddriver.name)
      return;

   THIN_ASSERT(snddriver.callback);
   snddriver.start(snddriver.callback);
}

void thin_sound_stop(void)
{
   if (NULL == snddriver.name)
      return;

   snddriver.stop();
}

void thin_sound_setrate(int sample_rate)
{
   if (snddriver.setrate)
      snddriver.setrate(sample_rate);
}

/*
** $Log: tl_sound.c,v $
** Revision 1.3  2001/03/12 06:06:55  matt
** better keyboard driver, support for bit depths other than 8bpp
**
** Revision 1.2  2001/02/19 02:55:36  matt
** extern this, eh!
**
** Revision 1.1  2000/12/16 17:29:20  matt
** initial revision
**
*/
