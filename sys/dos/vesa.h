
#pragma pack(1)

typedef struct {
	char    sig[4];
	short   ver;
	char    *oemstr;
	unsigned long   capabilities;
	unsigned short  *vidmodes;
	short   TotalMemory;
	short   oemrev;
	char    *oemvendor;
	char    *oemproduct;
	char    *oemproductrev;
	char    reserved[222];
	char    oemdata[256];
} VBEINFO;

#define vbe8bitdac 0x0001
#define vbenonvga 0x0002
#define vbeblankramdac 0x0004

typedef struct {
	short modeattributes;
	char winaattributes;
	char winbattributes;
	short wingranularity;
	short winsize;
	unsigned short winasegment;
	unsigned short winbsegment;
	void *winfunc;
	short bytesperscanline;
	short xres;
	short yres;
	char xcharsize;
	char ycharsize;
	char numplanes;
	char bpp;
	char numbanks;
	char memmodel;
	char banksize;
	char numpages;
	char res1;
	char redmasksize;
	char redfieldpos;
	char greenmasksize;
	char greenfieldpos;
	char bluemasksize;
	char bluefieldpos;
	char resmasksize;
	char resfieldpos;
	char directcolormodeinfo;
	/* VBE 2.0  */
	unsigned long physbaseptr;
	unsigned long offscreenmemoffset;
	short offscreenmemsize;
	/* VBE 2.1  */
	short linbytesperscanline;
	char bnknumimagepages;
	char linnumimagepages;
	char linredmasksize;
	char linredfieldpos;
	char lingreenmasksize;
	char lingreenfieldpos;
	char linbluemasksize;
	char linbluefieldpos;
	char linresmasksize;
	char linresfieldpos;
	char res2[194];
} VBEMODEINFO;

typedef struct {
	short setwindow;
	short setdisplaydtart;
	short setpalette;
	short ioprivinfo;
    /* VBE 2.1 */
	long extensionsig;
	long setwindowlen;
	long setdisplaystartlen;
	long setpalettelen;
} VBEPMINFO;

#define VBE20_EXT_SIG 0xFBADFBAD

typedef enum {
	vbememTXT = 0,
	vbememCGA = 1,
	vbememHGC = 2,
	vbememPL = 3,
	vbememPK = 4,
	vbememX = 5,
	vbememRGB = 6,
	vbememYUV = 7
} VBEMEMMODELS;

#define vbeDontClear 0x8000
#define vbeLinearBuffer 0x4000

#define vbeMdAvailable  0x0001          /* Video mode is available                      */
#define vbeMdTTYOutput  0x0004          /* TTY BIOS output is supported         */
#define vbeMdColorMode  0x0008          /* Mode is a color video mode           */
#define vbeMdGraphMode  0x0010          /* Mode is a graphics mode                      */
#define vbeMdNonVGA             0x0020          /* Mode is not VGA compatible           */
#define vbeMdNonBanked  0x0040          /* Banked mode is not supported         */
#define vbeMdLinear             0x0080          /* Linear mode supported                        */

#define vbeStHardware   0x0001          /* Save the hardware state                      */
#define vbeStBIOS               0x0002          /* Save the BIOS state                          */
#define vbeStDAC                0x0004          /* Save the DAC state                           */
#define vbeStSVGA               0x0008          /* Save the SuperVGA state                      */
#define vbeStAll                0x000F          /* Save all states                                      */

#pragma pack()



