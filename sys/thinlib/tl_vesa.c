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
** tl_vesa.c
**
** VESA code.
** $Id: tl_vesa.c,v 1.7 2001/02/01 06:28:26 matt Exp $
*/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <go32.h>
#include <dos.h>
#include <dpmi.h>

#include "tl_types.h"
#include "tl_log.h"
#include "tl_bmp.h"

#include "tl_djgpp.h"

#include "tl_vga.h"
#include "tl_vesa.h"

#define  __PACKED__  __attribute__ ((packed))

/* VESA information block structure */
typedef struct vesainfo_s
{
   char   VESASignature[4]    __PACKED__;
   uint16 VESAVersion         __PACKED__;
   uint32 OEMStringPtr        __PACKED__;
   char   Capabilities[4]     __PACKED__;
   uint32 VideoModePtr        __PACKED__; 
   uint16 TotalMemory         __PACKED__; 
   uint16 OemSoftwareRev      __PACKED__; 
   uint32 OemVendorNamePtr    __PACKED__; 
   uint32 OemProductNamePtr   __PACKED__; 
   uint32 OemProductRevPtr    __PACKED__; 
   uint8  Reserved[222]       __PACKED__; 
   char   OemData[256]        __PACKED__; 
} vesainfo_t;

/* SuperVGA mode information block */
typedef struct modeinfo_s
{
   uint16 ModeAttributes      __PACKED__; 
   uint8  WinAAttributes      __PACKED__; 
   uint8  WinBAttributes      __PACKED__; 
   uint16 WinGranularity      __PACKED__; 
   uint16 WinSize             __PACKED__; 
   uint16 WinASegment         __PACKED__; 
   uint16 WinBSegment         __PACKED__; 
   uint32 WinFuncPtr          __PACKED__; 
   uint16 BytesPerScanLine    __PACKED__; 
   uint16 XResolution         __PACKED__; 
   uint16 YResolution         __PACKED__; 
   uint8  XCharSize           __PACKED__; 
   uint8  YCharSize           __PACKED__; 
   uint8  NumberOfPlanes      __PACKED__; 
   uint8  BitsPerPixel        __PACKED__; 
   uint8  NumberOfBanks       __PACKED__; 
   uint8  MemoryModel         __PACKED__; 
   uint8  BankSize            __PACKED__; 
   uint8  NumberOfImagePages  __PACKED__;
   uint8  Reserved_page       __PACKED__; 
   uint8  RedMaskSize         __PACKED__; 
   uint8  RedMaskPos          __PACKED__; 
   uint8  GreenMaskSize       __PACKED__; 
   uint8  GreenMaskPos        __PACKED__;
   uint8  BlueMaskSize        __PACKED__; 
   uint8  BlueMaskPos         __PACKED__; 
   uint8  ReservedMaskSize    __PACKED__; 
   uint8  ReservedMaskPos     __PACKED__; 
   uint8  DirectColorModeInfo __PACKED__;

   /* VBE 2.0 extensions */
   uint32 PhysBasePtr         __PACKED__; 
   uint32 OffScreenMemOffset  __PACKED__; 
   uint16 OffScreenMemSize    __PACKED__; 

   /* VBE 3.0 extensions */
   uint16 LinBytesPerScanLine __PACKED__;
   uint8  BnkNumberOfPages    __PACKED__;
   uint8  LinNumberOfPages    __PACKED__;
   uint8  LinRedMaskSize      __PACKED__;
   uint8  LinRedFieldPos      __PACKED__;
   uint8  LinGreenMaskSize    __PACKED__;
   uint8  LinGreenFieldPos    __PACKED__;
   uint8  LinBlueMaskSize     __PACKED__;
   uint8  LinBlueFieldPos     __PACKED__;
   uint8  LinRsvdMaskSize     __PACKED__;
   uint8  LinRsvdFieldPos     __PACKED__;
   uint32 MaxPixelClock       __PACKED__;

   uint8  Reserved[190]       __PACKED__; 
} modeinfo_t;


#define  MASK_LINEAR(addr)    (addr & 0x000FFFFF)
#define  RM_TO_LINEAR(addr)   (((addr & 0xFFFF0000) >> 12) + (addr & 0xFFFF))
#define  RM_OFFSET(addr)      (addr & 0xF)
#define  RM_SEGMENT(addr)     ((addr >> 4) & 0xFFFF)

#define  VBE_LINEAR_ADDR      0x4000
#define  VBE_LINEAR_AVAIL     0x0080

#define  VBE_INT              0x10
#define  VBE_SUCCESS          0x004F
#define  VBE_FUNC_DETECT      0x4F00
#define  VBE_FUNC_GETMODEINFO 0x4F01
#define  VBE_FUNC_SETMODE     0x4F02
#define  VBE_FUNC_GETMODE     0x4F03
#define  VBE_FUNC_FLIPPAGE    0x4F07

#define  VBE_MAX_NUM_MODES    1024  /* liberal */

/* this is NOT the right way to set the damn palette! */
#define  VGA_PAL_READ         0x3C7 /* Palette read address                */
#define  VGA_PAL_WRITE        0x3C8 /* Palette write address               */
#define  VGA_PAL_DATA         0x3C9 /* Palette data register               */


static struct
{
   uint32 address;
   int width, height;
} cur_mode;


static vesainfo_t vesa_info;
static bitmap_t *screen = NULL;
static bitmap_t *hardware = NULL;

/* look for vesa */
static int vesa_detect(vesainfo_t *vesainfo)
{
   __dpmi_regs regs;

   /* Use DOS transfer buffer to hold VBE info */
   ASSERT(sizeof(*vesainfo) < _go32_info_block.size_of_transfer_buffer);
   memset(&regs, 0, sizeof(regs));

   strncpy(vesainfo->VESASignature, "VBE2", 4);
   dosmemput(vesainfo, sizeof(*vesainfo), MASK_LINEAR(__tb));

   regs.x.ax = VBE_FUNC_DETECT;
   regs.x.di = RM_OFFSET(__tb);
   regs.x.es = RM_SEGMENT(__tb);

   __dpmi_int(VBE_INT, &regs);
   if (VBE_SUCCESS != regs.x.ax)
      return -1;

   dosmemget(MASK_LINEAR(__tb), sizeof(*vesainfo), vesainfo);
   if (strncmp(vesainfo->VESASignature, "VESA", 4) != 0)
      return -1;

   return 0;
}

static int vesa_getmodeinfo(uint16 mode, modeinfo_t *modeinfo)
{
   __dpmi_regs regs;

   ASSERT(sizeof(*modeinfo) < _go32_info_block.size_of_transfer_buffer);

   memset(&regs, 0, sizeof(regs));
   regs.x.ax = VBE_FUNC_GETMODEINFO; 
   regs.x.cx = mode;
   regs.x.di = RM_OFFSET(__tb);
   regs.x.es = RM_SEGMENT(__tb);
  
   __dpmi_int(VBE_INT, &regs);
   if (VBE_SUCCESS != regs.x.ax)
      return -1;

   dosmemget(MASK_LINEAR(__tb), sizeof(*modeinfo), modeinfo);
   return 0;
}

static uint16 vesa_findmode(vesainfo_t *vesainfo, int width, int height, int bpp)
{
   modeinfo_t mode_info;
   uint16 modes[VBE_MAX_NUM_MODES], mode;
   int i, mode_total = 0;
   long list_ptr;
   
   list_ptr = RM_TO_LINEAR(vesainfo->VideoModePtr);

   while (mode_total < VBE_MAX_NUM_MODES)
   {
      dosmemget(list_ptr, 2, &mode);

      if (0xFFFF == mode)
         break;

      modes[mode_total++] = mode;
      list_ptr += 2;
   }

   /* VESA card on steroids? */
   ASSERT(mode_total < VBE_MAX_NUM_MODES);

   for (i = 0; i < mode_total; i++)
   {
      mode = modes[i];

      if (vesa_getmodeinfo(mode, &mode_info))
         return -1; /* we are definitely screwed */
      
      if (mode_info.XResolution == width 
          && mode_info.YResolution == height 
          && mode_info.BitsPerPixel == bpp)
         return mode;
   }

   return 0;
}

short int vid_selector = -1;

static int vesa_setmodenum(uint16 mode)
{
   __dpmi_regs regs;
   __dpmi_meminfo mi;
   modeinfo_t mode_info;

   if (vesa_getmodeinfo(mode, &mode_info))
      return -1;

   mi.size = mode_info.BytesPerScanLine * mode_info.YResolution;
   mi.address = mode_info.PhysBasePtr;
   if (-1 == __dpmi_physical_address_mapping(&mi))
      return -1;

   if (0 == thinlib_nearptr)
   {
      vid_selector = __dpmi_allocate_ldt_descriptors(1);
      if (-1 == vid_selector)
         return -1;

      if (-1 == __dpmi_set_descriptor_access_rights(vid_selector, 0x40f3))
         return -1;

      if (-1 == __dpmi_set_segment_base_address(vid_selector, mi.address))
         return -1;

      if (-1 == __dpmi_set_segment_limit(vid_selector, vesa_info.TotalMemory << 16 | 0xfff))
         return -1;
   }

   memset(&regs, 0, sizeof(regs));
   regs.x.ax = VBE_FUNC_SETMODE;
   regs.x.bx = mode | VBE_LINEAR_ADDR;
 
   __dpmi_int(VBE_INT, &regs);
   if (VBE_SUCCESS != regs.x.ax)
      return -1;

   if (0 == thinlib_nearptr)
      cur_mode.address = 0;
   else
      cur_mode.address = mi.address;

   cur_mode.width = mode_info.XResolution;
   cur_mode.height = mode_info.YResolution;

   if (NULL != screen)
      thin_bmp_destroy(&screen);

   if (thinlib_nearptr)
   {
      screen = thin_bmp_createhw((uint8 *) THIN_PHYS_ADDR(cur_mode.address),
                                 cur_mode.width, cur_mode.height, cur_mode.width);
      if (NULL == screen)
         return -1;
   }
   else
   {
      if (NULL != hardware)
         thin_bmp_destroy(&hardware);

      hardware = thin_bmp_createhw((uint8 *) cur_mode.address, 
                                   cur_mode.width, cur_mode.height, cur_mode.width);
      if (NULL == hardware)
         return -1;

      screen = thin_bmp_create(cur_mode.width, cur_mode.height, 0);
      if (NULL == screen)
         return -1;
   }

   return 0;
}

void thin_vesa_shutdown(void)
{
   __dpmi_regs regs;

   memset(&regs, 0, sizeof(regs));
   regs.x.ax = 0x0003;
   __dpmi_int(VBE_INT, &regs);

   thin_bmp_destroy(&screen);

   if (0 == thinlib_nearptr)
      thin_bmp_destroy(&hardware);
}


int thin_vesa_setmode(int width, int height)
{
   uint16 mode;

   /* assume we always want 8bpp */
   mode = vesa_findmode(&vesa_info, width, height, 8);
   if (0 == mode)
      return -1;

   return vesa_setmodenum(mode);
}

int thin_vesa_init(int width, int height)
{
   memset(&vesa_info, 0, sizeof(vesa_info));
   screen = NULL;
   hardware = NULL;
   
   if (vesa_detect(&vesa_info))
      return -1;

   /* check to see if linear framebuffer is available */
   if ((vesa_info.VESAVersion >> 8) < 2)
   {
      thin_printf("thin@vesa: no linear framebuffer available\n");
      return -1;
   }

   return thin_vesa_setmode(width, height);
}

bitmap_t *thin_vesa_lockwrite(void)
{
   return screen;
}

void thin_vesa_freewrite(int num_dirties, rect_t *dirty_rects)
{
   UNUSED(num_dirties);
   UNUSED(dirty_rects);

   if (0 == thinlib_nearptr)
   {
      int line;

      for (line = 0; line < hardware->height; line++)
      {         
         movedata(_my_ds(), (unsigned) screen->line[line],
                  vid_selector, (unsigned) hardware->line[line],
                  hardware->pitch);
      }
   }
}

/*
** $Log: tl_vesa.c,v $
** Revision 1.7  2001/02/01 06:28:26  matt
** thinlib now works under NT/2000
**
** Revision 1.6  2001/01/15 05:25:52  matt
** i hate near pointers
**
** Revision 1.5  2000/12/14 14:10:32  matt
** cleaner initialization
**
** Revision 1.4  2000/11/25 20:28:34  matt
** moved verboseness into correct places
**
** Revision 1.3  2000/11/06 02:22:33  matt
** generalized video driver (tl_video.c)
**
** Revision 1.2  2000/11/05 16:32:36  matt
** thinlib round 2
**
** Revision 1.1  2000/11/05 06:29:03  matt
** initial revision
**
*/
