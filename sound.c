


#include "defs.h"
#include "pcm.h"
#include "sound.h"
#include "cpu.h"
#include "hw.h"
#include "regs.h"
#include "rc.h"

static byte sqwave[4][8] =
{
	{ 1, 0, 0, 0, 0, 0, 0, 0 },
	{ 1, 1, 0, 0, 0, 0, 0, 0 },
	{ 1, 1, 1, 1, 0, 0, 0, 0 },
	{ 1, 1, 1, 1, 1, 1, 0, 0 }
};

struct snd snd;

#define RATE (snd.rate)
#define WAVE (snd.wave)
#define SW (snd.sw)
#define S1 (snd.ch[0])
#define S2 (snd.ch[1])
#define S3 (snd.ch[2])
#define S4 (snd.ch[3])

rcvar_t sound_exports[] =
{
	RCV_END
};

void sound_reset()
{
	if (pcm.hz) snd.rate = (1<<21) / pcm.hz;
	else snd.rate = 0;
	R_NR10 = 0x80;
	R_NR11 = 0xBF;
	R_NR12 = 0xF3;
	R_NR14 = 0xBF;
	R_NR21 = 0x3F;
	R_NR22 = 0x00;
	R_NR24 = 0xBF;
	R_NR30 = 0x7F;
	R_NR31 = 0xFF;
	R_NR32 = 0x9F;
	R_NR33 = 0xBF;
	R_NR41 = 0xFF;
	R_NR42 = 0x00;
	R_NR43 = 0x00;
	R_NR44 = 0xBF;
	R_NR50 = 0x77;
	R_NR51 = 0xF3;
	R_NR52 = 0xF1;
}


void sound_mix()
{
	int s, l, r, f;

	if (!RATE || cpu.snd < RATE) return;

	for (; cpu.snd >= RATE; cpu.snd -= RATE)
	{
		if (pcm.pos >= pcm.len)
			pcm_submit();

		l = r = 0;

		if (S1.on)
		{
			s = sqwave[S1.pat][(S1.pos>>18)&7] * S1.envol;
			S1.pos += S1.freq;
			if ((R_NR14 & 64) && ((S1.cnt += RATE) >= S1.len))
				S1.on = 0;
			if (S1.enlen && (S1.encnt += RATE) >= S1.enlen)
			{
				S1.encnt -= S1.enlen;
				S1.envol += S1.endir;
				if (S1.envol < 0) S1.envol = 0;
				if (S1.envol > 15) S1.envol = 15;
			}
			if (SW.len && (SW.cnt += RATE) >= SW.len)
			{
				SW.cnt -= SW.len;
				f = ((R_NR14&7)<<8) + R_NR13;
				if (SW.dir) f -= (f>>SW.shift);
				else f += (f>>SW.shift);
				if (f > 2047)
					S1.on = 0;
				else
				{
					S1.freq = 131072 / (2048 - f) * RATE;
					R_NR13 = f;
					R_NR14 = (R_NR14 & 0xFC) | (f>>8);
				}
			}
			s <<= 2;
			if (R_NR51 & 1) l += s;
			if (R_NR51 & 16) r += s;
		}
		
		if (S2.on)
		{
			s = sqwave[S2.pat][(S2.pos>>18)&7] * S2.envol;
			S2.pos += S2.freq;
			if ((R_NR24 & 64) && ((S2.cnt += RATE) >= S2.len))
				S2.on = 0;
			if (S2.enlen && (S2.encnt += RATE) >= S2.enlen)
			{
				S2.encnt -= S2.enlen;
				S2.envol += S2.endir;
				if (S2.envol < 0) S2.envol = 0;
				if (S2.envol > 15) S2.envol = 15;
			}
			s <<= 2;
			if (R_NR51 & 2) l += s;
			if (R_NR51 & 32) r += s;
		}
		
		if (S3.on)
		{
			s = WAVE[(S3.pos>>22) & 15];
			if (S3.pos & (1<<21)) s &= 15;
			else s >>= 4;
			s -= 8;
			/* printf("wave %02X (%08X): %d\n", ((S3.pos >> 21) & 31), S3.pos, s); */
			S3.pos += S3.freq;
			if ((R_NR34 & 64) && ((S3.cnt += RATE) >= S3.len))
				S3.on = 0;
			if (R_NR32 & 96) s <<= (3 - ((R_NR32>>5)&3));
			else s = 0;
			if (R_NR51 & 4) l += s;
			if (R_NR51 & 64) r += s;
		}

		l *= (R_NR50 & 0x07);
		r *= ((R_NR50 & 0x70)>>4);
		l >>= 4;
		r >>= 4;
		
		if (l > 127) l = 127;
		else if (l < -128) l = -128;
		if (r > 127) r = 127;
		else if (r < -128) r = -128;

		pcm.buf[pcm.pos] = l+128;
		pcm.buf[pcm.pos+1] = r+128;
		pcm.pos += 2;
	}
	R_NR52 = (R_NR52&0xf0) | S1.on | (S2.on<<1) | (S3.on<<2) | (S4.on<<3);
	R_NR30 = S3.on << 7;
}



byte sound_read(byte r)
{
	sound_mix();
	/* printf("read %02X: %02X\n", r, REG(r)); */
	return REG(r);
}

void s1_init()
{
	SW.cnt = 0;
	S1.envol = R_NR12 >> 4;
	S1.endir = (R_NR12>>3) & 1;
	S1.endir |= S1.endir - 1;
	S1.enlen = (R_NR12 & 3) << 15;
	S1.freq = 131072 / (2048 - (((R_NR14&7)<<8)+R_NR13)) * RATE;
	S1.on = 1;
	S1.pos = 0;
	S1.cnt = 0;
	S1.encnt = 0;
}

void s2_init()
{
	S2.envol = R_NR22 >> 4;
	S2.endir = (R_NR22>>3) & 1;
	S2.endir |= S2.endir - 1;
	S2.enlen = (R_NR22 & 3) << 15;
	S2.freq = 131072 / (2048 - (((R_NR24&7)<<8)+R_NR23)) * RATE;
	S2.on = 1;
	S2.pos = 0;
	S2.cnt = 0;
	S2.encnt = 0;
}

void s3_init()
{
	S3.len = (256-R_NR31) << 20;
	S3.pos = 0;
	S3.cnt = 0;
	S3.on = 1;
}


void sound_write(byte r, byte b)
{
	if (!(R_NR52 & 128) && r != RI_NR52) return;
	/* printf("write %02X: %02X\n", r, b); */
	sound_mix();
	if ((r & 0xF0) == 0x30)
	{
		if (!S3.on) WAVE[r - 0x30] = b;
		return;
	}
	switch (r)
	{
	case RI_NR10:
		R_NR10 = b;
		SW.len = ((R_NR10>>4) & 7) << 14;
		SW.dir = (R_NR10>>3) & 1;
		SW.shift = R_NR10 & 3;
		break;
	case RI_NR11:
		R_NR11 = b;
		S1.pat = R_NR11 >> 6;
		S1.len = (64-(R_NR11&63)) << 13;
		break;
	case RI_NR12:
		R_NR12 = b;
		S1.envol = R_NR12 >> 4;
		S1.endir = (R_NR12>>3) & 1;
		S1.endir |= S1.endir - 1;
		S1.enlen = (R_NR12 & 3) << 15;
		break;
	case RI_NR13:
		R_NR13 = b;
		break;
	case RI_NR14:
		R_NR14 = b;
		if (b & 128) s1_init();
		break;
	case RI_NR21:
		R_NR21 = b;
		S2.pat = R_NR21 >> 6;
		S2.len = (64-(R_NR21&63)) << 13;
		break;
	case RI_NR22:
		R_NR22 = b;
		S2.envol = R_NR22 >> 4;
		S2.endir = (R_NR22>>3) & 1;
		S2.endir |= S2.endir - 1;
		S2.enlen = (R_NR22 & 3) << 15;
		break;
	case RI_NR23:
		R_NR23 = b;
		break;
	case RI_NR24:
		R_NR24 = b;
		if (b & 128) s2_init();
		break;
	case RI_NR31:
	case RI_NR32:
	case RI_NR50:
		REG(r) = b;
		break;
	case RI_NR33:
		R_NR33 = b;
		S3.freq = 32 * 65536 / (2048 - (((R_NR34&7)<<8)+R_NR33)) * RATE;
		break;
	case RI_NR34:
		R_NR34 = b;
		S3.freq = 32 * 65536 / (2048 - (((R_NR34&7)<<8)+R_NR33)) * RATE;
		if (b & 128) s3_init();
		break;
	case RI_NR30:
		if (!(b & 128)) S3.on = 0;
		break;
	case RI_NR51:
		REG(r) = b;
		break;
	case RI_NR52:
		R_NR52 = b & 128;
		if (!(R_NR52 & 128))
			S1.on = S2.on = S3.on = S4.on = 0;
		break;
	default:
		return;
	}
	/* printf("write to %02X: %02X\n", r, b); */
}




