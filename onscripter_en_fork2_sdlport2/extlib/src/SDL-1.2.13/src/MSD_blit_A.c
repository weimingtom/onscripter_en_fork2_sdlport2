#include "MSD_video.h"

#define RGB_FROM_PIXEL(Pixel, fmt, r, g, b)				\
{									\
	r = (((Pixel&fmt->Rmask)>>fmt->Rshift)<<fmt->Rloss); 		\
	g = (((Pixel&fmt->Gmask)>>fmt->Gshift)<<fmt->Gloss); 		\
	b = (((Pixel&fmt->Bmask)>>fmt->Bshift)<<fmt->Bloss); 		\
}
#define PIXEL_FROM_RGB(Pixel, fmt, r, g, b)				\
{									\
	Pixel = ((r>>fmt->Rloss)<<fmt->Rshift)|				\
		((g>>fmt->Gloss)<<fmt->Gshift)|				\
		((b>>fmt->Bloss)<<fmt->Bshift);				\
}

#define PIXEL_FROM_RGBA(Pixel, fmt, r, g, b, a)				\
{									\
	Pixel = ((r>>fmt->Rloss)<<fmt->Rshift)|				\
		((g>>fmt->Gloss)<<fmt->Gshift)|				\
		((b>>fmt->Bloss)<<fmt->Bshift)|				\
		((a>>fmt->Aloss)<<fmt->Ashift);				\
}


#define ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB)	\
do {						\
	dR = (((sR-dR)*(A))>>8)+dR;		\
	dG = (((sG-dG)*(A))>>8)+dG;		\
	dB = (((sB-dB)*(A))>>8)+dB;		\
} while(0)

#define RGBA_FROM_PIXEL(Pixel, fmt, r, g, b, a)				\
{									\
	r = ((Pixel&fmt->Rmask)>>fmt->Rshift)<<fmt->Rloss; 		\
	g = ((Pixel&fmt->Gmask)>>fmt->Gshift)<<fmt->Gloss; 		\
	b = ((Pixel&fmt->Bmask)>>fmt->Bshift)<<fmt->Bloss; 		\
	a = ((Pixel&fmt->Amask)>>fmt->Ashift)<<fmt->Aloss;	 	\
}



#define DISEMBLE_RGB(buf, bpp, fmt, Pixel, r, g, b)			   \
do {									   \
	switch (bpp) {							   \
		case 2:							   \
			Pixel = *((uint16_t *)(buf));			   \
		break;							   \
									   \
		case 3: {						   \
		        uint8_t *B = (uint8_t *)buf;			   \
			if (MSD_IsLilEndian()) {		   \
			        Pixel = B[0] + (B[1] << 8) + (B[2] << 16); \
			} else {					   \
			        Pixel = (B[0] << 16) + (B[1] << 8) + B[2]; \
			}						   \
		}							   \
		break;							   \
									   \
		case 4:							   \
			Pixel = *((uint32_t *)(buf));			   \
		break;							   \
									   \
	        default:						   \
		        Pixel = 0; \
		break;							   \
	}								   \
	RGB_FROM_PIXEL(Pixel, fmt, r, g, b);				   \
} while(0)

#define ASSEMBLE_RGB(buf, bpp, fmt, r, g, b) 				\
{									\
	switch (bpp) {							\
		case 2: {						\
			uint16_t Pixel;					\
									\
			PIXEL_FROM_RGB(Pixel, fmt, r, g, b);		\
			*((uint16_t *)(buf)) = Pixel;			\
		}							\
		break;							\
									\
		case 3: {						\
                        if (MSD_IsLilEndian()) {		\
			        *((buf)+fmt->Rshift/8) = r;		\
				*((buf)+fmt->Gshift/8) = g;		\
				*((buf)+fmt->Bshift/8) = b;		\
			} else {					\
			        *((buf)+2-fmt->Rshift/8) = r;		\
				*((buf)+2-fmt->Gshift/8) = g;		\
				*((buf)+2-fmt->Bshift/8) = b;		\
			}						\
		}							\
		break;							\
									\
		case 4: {						\
			uint32_t Pixel;					\
									\
			PIXEL_FROM_RGB(Pixel, fmt, r, g, b);		\
			*((uint32_t *)(buf)) = Pixel;			\
		}							\
		break;							\
	}								\
}

#define DISEMBLE_RGBA(buf, bpp, fmt, Pixel, r, g, b, a)			   \
do {									   \
	switch (bpp) {							   \
		case 2:							   \
			Pixel = *((uint16_t *)(buf));			   \
		break;							   \
									   \
		case 3:	{/* FIXME: broken code (no alpha) */		   \
		        uint8_t *b = (uint8_t *)buf;			   \
			if (MSD_IsLilEndian()) {		   \
			        Pixel = b[0] + (b[1] << 8) + (b[2] << 16); \
			} else {					   \
			        Pixel = (b[0] << 16) + (b[1] << 8) + b[2]; \
			}						   \
		}							   \
		break;							   \
									   \
		case 4:							   \
			Pixel = *((uint32_t *)(buf));			   \
		break;							   \
									   \
		default:						   \
		        Pixel = 0; /* stop gcc complaints */		   \
		break;							   \
	}								   \
	RGBA_FROM_PIXEL(Pixel, fmt, r, g, b, a);			   \
	Pixel &= ~fmt->Amask;						   \
} while(0)


#define ASSEMBLE_RGBA(buf, bpp, fmt, r, g, b, a)			\
{									\
	switch (bpp) {							\
		case 2: {						\
			uint16_t Pixel;					\
									\
			PIXEL_FROM_RGBA(Pixel, fmt, r, g, b, a);	\
			*((uint16_t *)(buf)) = Pixel;			\
		}							\
		break;							\
									\
		case 3: { /* FIXME: broken code (no alpha) */		\
                        if (MSD_IsLilEndian()) {		\
			        *((buf)+fmt->Rshift/8) = r;		\
				*((buf)+fmt->Gshift/8) = g;		\
				*((buf)+fmt->Bshift/8) = b;		\
			} else {					\
			        *((buf)+2-fmt->Rshift/8) = r;		\
				*((buf)+2-fmt->Gshift/8) = g;		\
				*((buf)+2-fmt->Bshift/8) = b;		\
			}						\
		}							\
		break;							\
									\
		case 4: {						\
			uint32_t Pixel;					\
									\
			PIXEL_FROM_RGBA(Pixel, fmt, r, g, b, a);	\
			*((uint32_t *)(buf)) = Pixel;			\
		}							\
		break;							\
	}								\
}

#define RETRIEVE_RGB_PIXEL(buf, bpp, Pixel)				   \
do {									   \
	switch (bpp) {							   \
		case 2:							   \
			Pixel = *((uint16_t *)(buf));			   \
		break;							   \
									   \
		case 3: {						   \
		        uint8_t *B = (uint8_t *)(buf);			   \
			if (MSD_IsLilEndian()) {		   \
			        Pixel = B[0] + (B[1] << 8) + (B[2] << 16); \
			} else {					   \
			        Pixel = (B[0] << 16) + (B[1] << 8) + B[2]; \
			}						   \
		}							   \
		break;							   \
									   \
		case 4:							   \
			Pixel = *((uint32_t *)(buf));			   \
		break;							   \
									   \
		default:						   \
			Pixel = 0;			   \
		break;							   \
	}								   \
} while(0)





#define DUFFS_LOOP8(pixel_copy_increment, width)			\
{ int n = (width+7)/8;							\
	switch (width & 7) {						\
	case 0: do {	pixel_copy_increment;				\
	case 7:		pixel_copy_increment;				\
	case 6:		pixel_copy_increment;				\
	case 5:		pixel_copy_increment;				\
	case 4:		pixel_copy_increment;				\
	case 3:		pixel_copy_increment;				\
	case 2:		pixel_copy_increment;				\
	case 1:		pixel_copy_increment;				\
		} while ( --n > 0 );					\
	}								\
}


#define DUFFS_LOOP4(pixel_copy_increment, width)			\
{ int n = (width+3)/4;							\
	switch (width & 3) {						\
	case 0: do {	pixel_copy_increment;				\
	case 3:		pixel_copy_increment;				\
	case 2:		pixel_copy_increment;				\
	case 1:		pixel_copy_increment;				\
		} while ( --n > 0 );					\
	}								\
}

#define DUFFS_LOOP_DOUBLE2(pixel_copy_increment,			\
				double_pixel_copy_increment, width)	\
{ int n, w = width;							\
	if( w & 1 ) {							\
	    pixel_copy_increment;					\
	    w--;							\
	}								\
	if ( w > 0 )	{						\
	    n = ( w + 2) / 4;						\
	    switch( w & 2 ) {						\
	    case 0: do {	double_pixel_copy_increment;		\
	    case 2:		double_pixel_copy_increment;		\
		    } while ( --n > 0 );					\
	    }								\
	}								\
}


#define DUFFS_LOOP(pixel_copy_increment, width)				\
	DUFFS_LOOP8(pixel_copy_increment, width)

























static void BlitNto1SurfaceAlpha(MSD_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	uint8_t *src = info->s_pixels;
	int srcskip = info->s_skip;
	uint8_t *dst = info->d_pixels;
	int dstskip = info->d_skip;
	uint8_t *palmap = info->table;
	MSD_PixelFormat *srcfmt = info->src;
	MSD_PixelFormat *dstfmt = info->dst;
	int srcbpp = srcfmt->BytesPerPixel;

	const unsigned A = srcfmt->alpha;

	while (height--) {
	    DUFFS_LOOP4(
	    {
			uint32_t Pixel;
			unsigned sR;
			unsigned sG;
			unsigned sB;
			unsigned dR;
			unsigned dG;
			unsigned dB;
			DISEMBLE_RGB(src, srcbpp, srcfmt, Pixel, sR, sG, sB);
			dR = dstfmt->palette->colors[*dst].r;
			dG = dstfmt->palette->colors[*dst].g;
			dB = dstfmt->palette->colors[*dst].b;
			ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB);
			dR &= 0xff;
			dG &= 0xff;
			dB &= 0xff;
			if (palmap == NULL) {
				*dst = ((dR >> 5) << (3 + 2))|
				  ((dG >> 5) << (2))|
				  ((dB >> 6) << (0));
			} else {
				*dst = palmap[((dR >> 5) << (3 + 2))|
					  ((dG >> 5) << (2)) |
					  ((dB >> 6) << (0))];
			}
			dst++;
			src += srcbpp;
	    },
	    width);
	    src += srcskip;
	    dst += dstskip;
	}
}

static void BlitNto1PixelAlpha(MSD_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	uint8_t *src = info->s_pixels;
	int srcskip = info->s_skip;
	uint8_t *dst = info->d_pixels;
	int dstskip = info->d_skip;
	uint8_t *palmap = info->table;
	MSD_PixelFormat *srcfmt = info->src;
	MSD_PixelFormat *dstfmt = info->dst;
	int srcbpp = srcfmt->BytesPerPixel;

	while (height--) {
	    DUFFS_LOOP4(
	    {
			uint32_t Pixel;
			unsigned sR;
			unsigned sG;
			unsigned sB;
			unsigned sA;
			unsigned dR;
			unsigned dG;
			unsigned dB;
			DISEMBLE_RGBA(src,srcbpp,srcfmt,Pixel,sR,sG,sB,sA);
			dR = dstfmt->palette->colors[*dst].r;
			dG = dstfmt->palette->colors[*dst].g;
			dB = dstfmt->palette->colors[*dst].b;
			ALPHA_BLEND(sR, sG, sB, sA, dR, dG, dB);
			dR &= 0xff;
			dG &= 0xff;
			dB &= 0xff;
			if (palmap == NULL) {
				*dst =((dR >> 5) << (3 + 2))|
				  ((dG >> 5) << (2))|
				  ((dB >> 6) << (0));
			} else {
				*dst = palmap[((dR >> 5)<<(3 + 2)) |
					  ((dG >> 5) << (2)) |
					  ((dB >> 6) << (0))];
			}
			dst++;
			src += srcbpp;
	    },
	    width);
	    src += srcskip;
	    dst += dstskip;
	}
}

static void BlitNto1SurfaceAlphaKey(MSD_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	uint8_t *src = info->s_pixels;
	int srcskip = info->s_skip;
	uint8_t *dst = info->d_pixels;
	int dstskip = info->d_skip;
	uint8_t *palmap = info->table;
	MSD_PixelFormat *srcfmt = info->src;
	MSD_PixelFormat *dstfmt = info->dst;
	int srcbpp = srcfmt->BytesPerPixel;
	uint32_t ckey = srcfmt->colorkey;

	const int A = srcfmt->alpha;

	while (height--) {
	    DUFFS_LOOP(
	    {
			uint32_t Pixel;
			unsigned sR;
			unsigned sG;
			unsigned sB;
			unsigned dR;
			unsigned dG;
			unsigned dB;
			DISEMBLE_RGB(src, srcbpp, srcfmt, Pixel, sR, sG, sB);
			if (Pixel != ckey) {
				dR = dstfmt->palette->colors[*dst].r;
				dG = dstfmt->palette->colors[*dst].g;
				dB = dstfmt->palette->colors[*dst].b;
				ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB);
				dR &= 0xff;
				dG &= 0xff;
				dB &= 0xff;
				if (palmap == NULL) {
					*dst = ((dR >> 5) << (3 + 2)) |
					  ((dG >> 5) << (2)) |
					  ((dB >> 6) << (0));
				} else {
					*dst = palmap[((dR >> 5) << (3 + 2))|
						  ((dG >> 5) << (2)) |
						  ((dB >> 6) << (0))];
				}
			}
			dst++;
			src += srcbpp;
	    },
	    width);
	    src += srcskip;
	    dst += dstskip;
	}
}

static void BlitRGBtoRGBSurfaceAlpha128(MSD_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	uint32_t *srcp = (uint32_t *)info->s_pixels;
	int srcskip = info->s_skip >> 2;
	uint32_t *dstp = (uint32_t *)info->d_pixels;
	int dstskip = info->d_skip >> 2;

	while (height--) {
	    DUFFS_LOOP4({
		    uint32_t s = *srcp++;
		    uint32_t d = *dstp;
		    *dstp++ = ((((s & 0x00fefefe) + (d & 0x00fefefe)) >> 1)
			       + (s & d & 0x00010101)) | 0xff000000;
	    }, width);
	    srcp += srcskip;
	    dstp += dstskip;
	}
}

static void BlitRGBtoRGBSurfaceAlpha(MSD_BlitInfo *info)
{
	unsigned alpha = info->src->alpha;
	if (alpha == 128) {
		BlitRGBtoRGBSurfaceAlpha128(info);
	} else {
		int width = info->d_width;
		int height = info->d_height;
		uint32_t *srcp = (uint32_t *)info->s_pixels;
		int srcskip = info->s_skip >> 2;
		uint32_t *dstp = (uint32_t *)info->d_pixels;
		int dstskip = info->d_skip >> 2;
		uint32_t s;
		uint32_t d;
		uint32_t s1;
		uint32_t d1;

		while (height--) {
			DUFFS_LOOP_DOUBLE2({
				s = *srcp;
				d = *dstp;
				s1 = s & 0xff00ff;
				d1 = d & 0xff00ff;
				d1 = (d1 + ((s1 - d1) * alpha >> 8))
				     & 0xff00ff;
				s &= 0xff00;
				d &= 0xff00;
				d = (d + ((s - d) * alpha >> 8)) & 0xff00;
				*dstp = d1 | d | 0xff000000;
				++srcp;
				++dstp;
			},{
				s = *srcp;
				d = *dstp;
				s1 = s & 0xff00ff;
				d1 = d & 0xff00ff;
				d1 += (s1 - d1) * alpha >> 8;
				d1 &= 0xff00ff;
				     
				s = ((s & 0xff00) >> 8) | 
					((srcp[1] & 0xff00) << 8);
				d = ((d & 0xff00) >> 8) |
					((dstp[1] & 0xff00) << 8);
				d += (s - d) * alpha >> 8;
				d &= 0x00ff00ff;
				
				*dstp++ = d1 | ((d << 8) & 0xff00) | 0xff000000;
				++srcp;
				
				s1 = *srcp;
				d1 = *dstp;
				s1 &= 0xff00ff;
				d1 &= 0xff00ff;
				d1 += (s1 - d1) * alpha >> 8;
				d1 &= 0xff00ff;
				
				*dstp = d1 | ((d >> 8) & 0xff00) | 0xff000000;
				++srcp;
				++dstp;
			}, width);
			srcp += srcskip;
			dstp += dstskip;
		}
	}
}

static void BlitRGBtoRGBPixelAlpha(MSD_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	uint32_t *srcp = (uint32_t *)info->s_pixels;
	int srcskip = info->s_skip >> 2;
	uint32_t *dstp = (uint32_t *)info->d_pixels;
	int dstskip = info->d_skip >> 2;

	while(height--) {
	    DUFFS_LOOP4({
		uint32_t dalpha;
		uint32_t d;
		uint32_t s1;
		uint32_t d1;
		uint32_t s = *srcp;
		uint32_t alpha = s >> 24;

		if (alpha) {   
			if (alpha == MSD_ALPHA_OPAQUE) {
				*dstp = (s & 0x00ffffff) | (*dstp & 0xff000000);
			} else {
				d = *dstp;
				dalpha = d & 0xff000000;
				s1 = s & 0xff00ff;
				d1 = d & 0xff00ff;
				d1 = (d1 + ((s1 - d1) * alpha >> 8)) & 0xff00ff;
				s &= 0xff00;
				d &= 0xff00;
				d = (d + ((s - d) * alpha >> 8)) & 0xff00;
				*dstp = d1 | d | dalpha;
			}
		}
		++srcp;
		++dstp;
	    }, width);
	    srcp += srcskip;
	    dstp += dstskip;
	}
}

#define BLEND16_50(d, s, mask)						\
	((((s & mask) + (d & mask)) >> 1) + (s & d & (~mask & 0xffff)))

#define BLEND2x16_50(d, s, mask)					     \
	(((s & (mask | mask << 16)) >> 1) + ((d & (mask | mask << 16)) >> 1) \
	 + (s & d & (~(mask | mask << 16))))

static void Blit16to16SurfaceAlpha128(MSD_BlitInfo *info, uint16_t mask)
{
	int width = info->d_width;
	int height = info->d_height;
	uint16_t *srcp = (uint16_t *)info->s_pixels;
	int srcskip = info->s_skip >> 1;
	uint16_t *dstp = (uint16_t *)info->d_pixels;
	int dstskip = info->d_skip >> 1;

	while (height--) {
		if(((uintptr_t)srcp ^ (uintptr_t)dstp) & 2) {
			uint32_t prev_sw;
			int w = width;

			if ((uintptr_t)dstp & 2) {
				uint16_t d = *dstp, s = *srcp;
				*dstp = BLEND16_50(d, s, mask);
				dstp++;
				srcp++;
				w--;
			}
			srcp++;

			prev_sw = ((uint32_t *)srcp)[-1];

			while (w > 1) {
				uint32_t sw, dw, s;
				sw = *(uint32_t *)srcp;
				dw = *(uint32_t *)dstp;
				if (!MSD_IsLilEndian()) {
					s = (prev_sw << 16) + (sw >> 16);
				} else {
					s = (prev_sw >> 16) + (sw << 16);
				}
				prev_sw = sw;
				*(uint32_t *)dstp = BLEND2x16_50(dw, s, mask);
				dstp += 2;
				srcp += 2;
				w -= 2;
			}

			if (w) {
				uint16_t d = *dstp, s;
				if (!MSD_IsLilEndian()) {
					s = (uint16_t)prev_sw;
				} else {
					s = (uint16_t)(prev_sw >> 16);
				}
				*dstp = BLEND16_50(d, s, mask);
				srcp++;
				dstp++;
			}
			srcp += srcskip - 1;
			dstp += dstskip;
		} else {
			int w = width;

			if ((uintptr_t)srcp & 2) {
				uint16_t d = *dstp, s = *srcp;
				*dstp = BLEND16_50(d, s, mask);
				srcp++;
				dstp++;
				w--;
			}

			while (w > 1) {
				uint32_t sw = *(uint32_t *)srcp;
				uint32_t dw = *(uint32_t *)dstp;
				*(uint32_t *)dstp = BLEND2x16_50(dw, sw, mask);
				srcp += 2;
				dstp += 2;
				w -= 2;
			}

			if (w) {
				uint16_t d = *dstp, s = *srcp;
				*dstp = BLEND16_50(d, s, mask);
				srcp++;
				dstp++;
			}
			srcp += srcskip;
			dstp += dstskip;
		}
	}
}

static void Blit565to565SurfaceAlpha(MSD_BlitInfo *info)
{
	unsigned alpha = info->src->alpha;
	if (alpha == 128) {
		Blit16to16SurfaceAlpha128(info, 0xf7de);
	} else {
		int width = info->d_width;
		int height = info->d_height;
		uint16_t *srcp = (uint16_t *)info->s_pixels;
		int srcskip = info->s_skip >> 1;
		uint16_t *dstp = (uint16_t *)info->d_pixels;
		int dstskip = info->d_skip >> 1;
		alpha >>= 3;

		while (height--) {
			DUFFS_LOOP4({
				uint32_t s = *srcp++;
				uint32_t d = *dstp;

				s = (s | s << 16) & 0x07e0f81f;
				d = (d | d << 16) & 0x07e0f81f;
				d += (s - d) * alpha >> 5;
				d &= 0x07e0f81f;
				*dstp++ = (uint16_t)(d | d >> 16);
			}, width);
			srcp += srcskip;
			dstp += dstskip;
		}
	}
}

static void Blit555to555SurfaceAlpha(MSD_BlitInfo *info)
{
	unsigned alpha = info->src->alpha;
	if (alpha == 128) {
		Blit16to16SurfaceAlpha128(info, 0xfbde);
	} else {
		int width = info->d_width;
		int height = info->d_height;
		uint16_t *srcp = (uint16_t *)info->s_pixels;
		int srcskip = info->s_skip >> 1;
		uint16_t *dstp = (uint16_t *)info->d_pixels;
		int dstskip = info->d_skip >> 1;
		alpha >>= 3;

		while (height--) {
			DUFFS_LOOP4({
				uint32_t s = *srcp++;
				uint32_t d = *dstp;

				s = (s | s << 16) & 0x03e07c1f;
				d = (d | d << 16) & 0x03e07c1f;
				d += (s - d) * alpha >> 5;
				d &= 0x03e07c1f;
				*dstp++ = (uint16_t)(d | d >> 16);
			}, width);
			srcp += srcskip;
			dstp += dstskip;
		}
	}
}

static void BlitARGBto565PixelAlpha(MSD_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	uint32_t *srcp = (uint32_t *)info->s_pixels;
	int srcskip = info->s_skip >> 2;
	uint16_t *dstp = (uint16_t *)info->d_pixels;
	int dstskip = info->d_skip >> 1;

	while (height--) {
	    DUFFS_LOOP4({
		uint32_t s = *srcp;
		unsigned alpha = s >> 27; 

		if (alpha) {   
		  if (alpha == (MSD_ALPHA_OPAQUE >> 3)) {
		    *dstp = (uint16_t)((s >> 8 & 0xf800) + (s >> 5 & 0x7e0) + (s >> 3  & 0x1f));
		  } else {
		    uint32_t d = *dstp;

		    s = ((s & 0xfc00) << 11) + (s >> 8 & 0xf800)
		      + (s >> 3 & 0x1f);
		    d = (d | d << 16) & 0x07e0f81f;
		    d += (s - d) * alpha >> 5;
		    d &= 0x07e0f81f;
		    *dstp = (uint16_t)(d | d >> 16);
		  }
		}
		srcp++;
		dstp++;
	    }, width);
	    srcp += srcskip;
	    dstp += dstskip;
	}
}

static void BlitARGBto555PixelAlpha(MSD_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	uint32_t *srcp = (uint32_t *)info->s_pixels;
	int srcskip = info->s_skip >> 2;
	uint16_t *dstp = (uint16_t *)info->d_pixels;
	int dstskip = info->d_skip >> 1;

	while (height--) {
	    DUFFS_LOOP4({
			unsigned alpha;
			uint32_t s = *srcp;
			alpha = s >> 27;
			if (alpha) {   
				if (alpha == (MSD_ALPHA_OPAQUE >> 3)) {
					*dstp = (uint16_t)((s >> 9 & 0x7c00) + (s >> 6 & 0x3e0) + (s >> 3  & 0x1f));
				} else {
					uint32_t d = *dstp;
					
					s = ((s & 0xf800) << 10) + (s >> 9 & 0x7c00)
					  + (s >> 3 & 0x1f);
					d = (d | d << 16) & 0x03e07c1f;
					d += (s - d) * alpha >> 5;
					d &= 0x03e07c1f;
					*dstp = (uint16_t)(d | d >> 16);
				}
			}
			srcp++;
			dstp++;
	    }, width);
	    srcp += srcskip;
	    dstp += dstskip;
	}
}

static void BlitNtoNSurfaceAlpha(MSD_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	uint8_t *src = info->s_pixels;
	int srcskip = info->s_skip;
	uint8_t *dst = info->d_pixels;
	int dstskip = info->d_skip;
	MSD_PixelFormat *srcfmt = info->src;
	MSD_PixelFormat *dstfmt = info->dst;
	int srcbpp = srcfmt->BytesPerPixel;
	int dstbpp = dstfmt->BytesPerPixel;
	unsigned sA = srcfmt->alpha;
	unsigned dA = dstfmt->Amask ? MSD_ALPHA_OPAQUE : 0;

	if(sA) {
	  while (height--) {
	    DUFFS_LOOP4(
	    {
			uint32_t Pixel;
			unsigned sR;
			unsigned sG;
			unsigned sB;
			unsigned dR;
			unsigned dG;
			unsigned dB;
			DISEMBLE_RGB(src, srcbpp, srcfmt, Pixel, sR, sG, sB);
			DISEMBLE_RGB(dst, dstbpp, dstfmt, Pixel, dR, dG, dB);
			ALPHA_BLEND(sR, sG, sB, sA, dR, dG, dB);
			ASSEMBLE_RGBA(dst, dstbpp, dstfmt, dR, dG, dB, dA);
			src += srcbpp;
			dst += dstbpp;
			},
			width);
			src += srcskip;
			dst += dstskip;
		}
	}
}

static void BlitNtoNSurfaceAlphaKey(MSD_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	uint8_t *src = info->s_pixels;
	int srcskip = info->s_skip;
	uint8_t *dst = info->d_pixels;
	int dstskip = info->d_skip;
	MSD_PixelFormat *srcfmt = info->src;
	MSD_PixelFormat *dstfmt = info->dst;
	uint32_t ckey = srcfmt->colorkey;
	int srcbpp = srcfmt->BytesPerPixel;
	int dstbpp = dstfmt->BytesPerPixel;
	unsigned sA = srcfmt->alpha;
	unsigned dA = dstfmt->Amask ? MSD_ALPHA_OPAQUE : 0;

	while (height--) {
	    DUFFS_LOOP4(
	    {
		uint32_t Pixel;
		unsigned sR;
		unsigned sG;
		unsigned sB;
		unsigned dR;
		unsigned dG;
		unsigned dB;
		RETRIEVE_RGB_PIXEL(src, srcbpp, Pixel);
		if(sA && Pixel != ckey) {
		    RGB_FROM_PIXEL(Pixel, srcfmt, sR, sG, sB);
		    DISEMBLE_RGB(dst, dstbpp, dstfmt, Pixel, dR, dG, dB);
		    ALPHA_BLEND(sR, sG, sB, sA, dR, dG, dB);
		    ASSEMBLE_RGBA(dst, dstbpp, dstfmt, dR, dG, dB, dA);
		}
		src += srcbpp;
		dst += dstbpp;
	    },
	    width);
	    src += srcskip;
	    dst += dstskip;
	}
}

static void BlitNtoNPixelAlpha(MSD_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	uint8_t *src = info->s_pixels;
	int srcskip = info->s_skip;
	uint8_t *dst = info->d_pixels;
	int dstskip = info->d_skip;
	MSD_PixelFormat *srcfmt = info->src;
	MSD_PixelFormat *dstfmt = info->dst;

	int  srcbpp;
	int  dstbpp;

	srcbpp = srcfmt->BytesPerPixel;
	dstbpp = dstfmt->BytesPerPixel;

	while (height--) {
	    DUFFS_LOOP4(
	    {
			uint32_t Pixel;
			unsigned sR;
			unsigned sG;
			unsigned sB;
			unsigned dR;
			unsigned dG;
			unsigned dB;
			unsigned sA;
			unsigned dA;
			DISEMBLE_RGBA(src, srcbpp, srcfmt, Pixel, sR, sG, sB, sA);
			if (sA) {
				DISEMBLE_RGBA(dst, dstbpp, dstfmt, Pixel, dR, dG, dB, dA);
				ALPHA_BLEND(sR, sG, sB, sA, dR, dG, dB);
				ASSEMBLE_RGBA(dst, dstbpp, dstfmt, dR, dG, dB, dA);
			}
			src += srcbpp;
			dst += dstbpp;
	    },
	    width);
	    src += srcskip;
	    dst += dstskip;
	}
}


MSD_loblit MSD_CalculateAlphaBlit(MSD_Surface *surface, int blit_index)
{
    MSD_PixelFormat *sf = surface->format;
    MSD_PixelFormat *df = surface->map->dst->format;

    if (sf->Amask == 0) {
		if ((surface->flags & MSD_SRCCOLORKEY) == MSD_SRCCOLORKEY) {
			if (df->BytesPerPixel == 1) {
				return (MSD_loblit)BlitNto1SurfaceAlphaKey;
			} else {
				return (MSD_loblit)BlitNtoNSurfaceAlphaKey;
			}
		} else {
			switch (df->BytesPerPixel) {
			case 1:
				return (MSD_loblit)BlitNto1SurfaceAlpha;

			case 2:
				if (surface->map->identity) {
					if (df->Gmask == 0x7e0) {
						return (MSD_loblit)Blit565to565SurfaceAlpha;
					} else if (df->Gmask == 0x3e0) {
						return (MSD_loblit)Blit555to555SurfaceAlpha;
					}
				}
				return (MSD_loblit)BlitNtoNSurfaceAlpha;

			case 4:
				if (sf->Rmask == df->Rmask
				   && sf->Gmask == df->Gmask
				   && sf->Bmask == df->Bmask
				   && sf->BytesPerPixel == 4)
				{
					if ((sf->Rmask | sf->Gmask | sf->Bmask) == 0xffffff)
					{
						return (MSD_loblit)BlitRGBtoRGBSurfaceAlpha;
					}
				}
				return (MSD_loblit)BlitNtoNSurfaceAlpha;

			case 3:
			default:
				return (MSD_loblit)BlitNtoNSurfaceAlpha;
			}
		}
    } else {
		switch (df->BytesPerPixel) {
		case 1:
			return (MSD_loblit)BlitNto1PixelAlpha;

		case 2:
			if (sf->BytesPerPixel == 4 && sf->Amask == 0xff000000
			   && sf->Gmask == 0xff00
			   && ((sf->Rmask == 0xff && df->Rmask == 0x1f)
			   || (sf->Bmask == 0xff && df->Bmask == 0x1f))) {
				if (df->Gmask == 0x7e0) {
					return (MSD_loblit)BlitARGBto565PixelAlpha;
				} else if (df->Gmask == 0x3e0) {
					return (MSD_loblit)BlitARGBto555PixelAlpha;
				}
			}
			return (MSD_loblit)BlitNtoNPixelAlpha;

		case 4:
			if (sf->Rmask == df->Rmask
			   && sf->Gmask == df->Gmask
			   && sf->Bmask == df->Bmask
			   && sf->BytesPerPixel == 4) {
				if(sf->Amask == 0xff000000) {
					return (MSD_loblit)BlitRGBtoRGBPixelAlpha;
				}
			}
			return (MSD_loblit)BlitNtoNPixelAlpha;

		case 3:
		default:
			return (MSD_loblit)BlitNtoNPixelAlpha;
		}
    }
}

