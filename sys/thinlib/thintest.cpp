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
** thintest.cpp
**
** thinlib test
** $Id: thintest.cpp,v 1.2 2001/02/01 06:28:26 matt Exp $
*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "thinlib.h"

#define SAMPLE_RATE 44100
#define FRAGSIZE 1024
#define VID_WIDTH 320
#define VID_HEIGHT 240

class test
{
private:

public:
   test();
   ~test();

   int testSound();
   int testVideo();
   int testTimer();
};

test::test()
{
   int ret = thin_init(THIN_KEY | THIN_MOUSE | THIN_TIMER
                       | THIN_VIDEO | THIN_SOUND);
   ASSERT(-1 != ret);
}

test::~test()
{
   thin_shutdown();
}

static void fillbuf(void *buf, int size)
{
   static int pos = 0;
   int i;

   for (i = 0; i < size; i++)
   {
      *((uint8 *) buf)++ = 127 + (uint8)(127.0 * sin(2 * PI * pos / 128));
      pos = (pos + 1) & 1023;
   }
}

int test::testSound()
{
   thinsound_t params;

   params.sample_rate = SAMPLE_RATE;
   params.frag_size = FRAGSIZE;
   params.format = THIN_SOUND_MONO | THIN_SOUND_8BIT;
   params.callback = fillbuf;

   if (thin_sound_init(&params))
      return -1;

   thin_sound_stop();

   return 0;
}

int test::testVideo()
{
   int i, x, y;
   bitmap_t *screen;
   bitmap_t *buffer;

   /* set up video */
   if (thin_vid_init(VID_WIDTH, VID_HEIGHT))
      return -1;

   buffer = thin_bmp_create(VID_WIDTH, VID_HEIGHT, 0);
   if (NULL == buffer)
      return -1;

   /* fill it up with something interesting */
   for (y = 0; y < buffer->height; y++)
      for (x = 0; x < buffer->width; x++)
         buffer->line[y][x] = x ^ y;

   /* blit it out 1000 times */
   for (i = 0; i < 1000; i++)
   {
      screen = thin_vid_lockwrite();
      if (NULL == screen)
         return -1;

      for (y = 0; y < screen->height; y++)
         memcpy(screen->line[y], buffer->line[y], screen->width);
         
      thin_vid_freewrite(-1, NULL);
   }
   
   thin_vid_shutdown();

   if (1000 != i)
      return -1;

   return 0;
}

static volatile int timer_ticks = 0;
static void timer_handler(void)
{
   timer_ticks++;
}
THIN_LOCKED_STATIC_FUNC(timer_handler)

int test::testTimer()
{
   int last_ticks;

   THIN_LOCK_FUNC(timer_handler);
   THIN_LOCK_VAR(timer_ticks);

   /* one second intervals... */
   if (thin_timer_init(60, timer_handler))
      return -1;

   timer_ticks = last_ticks = 0;
   while (timer_ticks <= 60)
   {
      if (last_ticks != timer_ticks)
      {
         last_ticks = timer_ticks;
         thin_printf("%d 60 hz tick\n", last_ticks);
      }
   }

   thin_timer_setrate(1);
   last_ticks = 0;
   timer_ticks = 0;
   while (timer_ticks <= 3)
   {
      if (last_ticks != timer_ticks)
      {
         last_ticks = timer_ticks;
         thin_printf("%d seconds\n", last_ticks);
      }
   }

   thin_timer_shutdown();

   return 0;
}

int main(int argc, char *argv[])
{
   test *t = new test;

   if (t->testSound())
      return -1;

   if (t->testVideo())
      return -1;

   if (t->testTimer())
      return -1;

   delete t;

   return 0;
}

/*
** $Log: thintest.cpp,v $
** Revision 1.2  2001/02/01 06:28:26  matt
** thinlib now works under NT/2000
**
** Revision 1.1  2001/01/15 05:27:43  matt
** initial revision
**
*/
