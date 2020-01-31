#include <string.h>
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
/* Blend the RGB values of two Pixels based on a source alpha value */
#define ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB)	\
do {						\
	dR = (((sR-dR)*(A))>>8)+dR;		\
	dG = (((sG-dG)*(A))>>8)+dG;		\
	dB = (((sB-dB)*(A))>>8)+dB;		\
} while(0)



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





















static void BlitBto1(MSD_BlitInfo *info)
{
	int c;
	int width, height;
	uint8_t *src, *map, *dst;
	int srcskip, dstskip;

	width = info->d_width;
	height = info->d_height;
	src = info->s_pixels;
	srcskip = info->s_skip;
	dst = info->d_pixels;
	dstskip = info->d_skip;
	map = info->table;
	srcskip += width - (width + 7) / 8;

	if (map) {
		while (height--) {
		    uint8_t byte = 0, bit;
	    	for (c = 0; c < width; ++c ) {
				if ((c & 7) == 0) {
					byte = *src++;
				}
				bit = (byte & 0x80)>>7;
				if (1) {
					*dst = map[bit];
				}
				dst++;
				byte <<= 1;
			}
			src += srcskip;
			dst += dstskip;
		}
	} else {
		while (height--) {
		    uint8_t byte = 0, bit;
	    	for (c = 0; c < width; ++c) {
				if ((c & 7) == 0) {
					byte = *src++;
				}
				bit = (byte & 0x80) >> 7;
				if (1) {
				  *dst = bit;
				}
				dst++;
				byte <<= 1;
			}
			src += srcskip;
			dst += dstskip;
		}
	}
}

static void BlitBto2(MSD_BlitInfo *info)
{
	int c;
	int width, height;
	uint8_t *src;
	uint16_t *map, *dst;
	int srcskip, dstskip;

	width = info->d_width;
	height = info->d_height;
	src = info->s_pixels;
	srcskip = info->s_skip;
	dst = (uint16_t *)info->d_pixels;
	dstskip = info->d_skip / 2;
	map = (uint16_t *)info->table;
	srcskip += width - (width + 7) / 8;

	while (height--) {
	    uint8_t byte = 0, bit;
	    for (c = 0; c < width; ++c) {
			if ((c & 7) == 0) {
				byte = *src++;
			}
			bit = (byte & 0x80) >> 7;
			if (1) {
				*dst = map[bit];
			}
			byte <<= 1;
			dst++;
		}
		src += srcskip;
		dst += dstskip;
	}
}

static void BlitBto3(MSD_BlitInfo *info)
{
	int c, o;
	int width, height;
	uint8_t *src, *map, *dst;
	int srcskip, dstskip;

	width = info->d_width;
	height = info->d_height;
	src = info->s_pixels;
	srcskip = info->s_skip;
	dst = info->d_pixels;
	dstskip = info->d_skip;
	map = info->table;
	srcskip += width - (width + 7) / 8;

	while (height--) {
	    uint8_t byte = 0, bit;
	    for (c = 0; c < width; ++c) {
			if ( (c & 7) == 0 ) {
				byte = *src++;
			}
			bit = (byte & 0x80) >> 7;
			if (1) {
				o = bit * 4;
				dst[0] = map[o++];
				dst[1] = map[o++];
				dst[2] = map[o++];
			}
			byte <<= 1;
			dst += 3;
		}
		src += srcskip;
		dst += dstskip;
	}
}

static void BlitBto4(MSD_BlitInfo *info)
{
	int width, height;
	uint8_t *src;
	uint32_t *map, *dst;
	int srcskip, dstskip;
	int c;

	width = info->d_width;
	height = info->d_height;
	src = info->s_pixels;
	srcskip = info->s_skip;
	dst = (uint32_t *)info->d_pixels;
	dstskip = info->d_skip / 4;
	map = (uint32_t *)info->table;
	srcskip += width - (width + 7) / 8;

	while (height--) {
	    uint8_t byte = 0, bit;
	    for (c = 0; c < width; ++c) {
			if ((c & 7) == 0) {
				byte = *src++;
			}
			bit = (byte & 0x80) >> 7;
			if (1) {
				*dst = map[bit];
			}
			byte <<= 1;
			dst++;
		}
		src += srcskip;
		dst += dstskip;
	}
}

static void BlitBto1Key(MSD_BlitInfo *info)
{
    int width = info->d_width;
	int height = info->d_height;
	uint8_t *src = info->s_pixels;
	uint8_t *dst = info->d_pixels;
	int srcskip = info->s_skip;
	int dstskip = info->d_skip;
	uint32_t ckey = info->src->colorkey;
	uint8_t *palmap = info->table;
	int c;

	srcskip += width - (width + 7) / 8;

	if (palmap) {
		while (height--) {
		    uint8_t byte = 0, bit;
	    	for (c = 0; c < width; ++c) {
				if ((c & 7) == 0) {
					byte = *src++;
				}
				bit = (byte & 0x80) >> 7;
				if (bit != ckey) {
					*dst = palmap[bit];
				}
				dst++;
				byte <<= 1;
			}
			src += srcskip;
			dst += dstskip;
		}
	} else {
		while (height--) {
		    uint8_t  byte = 0, bit;
	    	for (c = 0; c < width; ++c) {
				if ((c & 7) == 0) {
					byte = *src++;
				}
				bit = (byte & 0x80) >> 7;
				if (bit != ckey) {
					*dst = bit;
				}
				dst++;
				byte <<= 1;
			}
			src += srcskip;
			dst += dstskip;
		}
	}
}

static void BlitBto2Key(MSD_BlitInfo *info)
{
    int width = info->d_width;
	int height = info->d_height;
	uint8_t *src = info->s_pixels;
	uint16_t *dstp = (uint16_t *)info->d_pixels;
	int srcskip = info->s_skip;
	int dstskip = info->d_skip;
	uint32_t ckey = info->src->colorkey;
	uint8_t *palmap = info->table;
	int c;

	srcskip += width - (width + 7) / 8;
	dstskip /= 2;

	while (height--) {
	    uint8_t byte = 0, bit;
	    for (c = 0; c < width; ++c) {
			if ((c & 7) == 0) {
				byte = *src++;
			}
			bit = (byte & 0x80) >> 7;
			if (bit != ckey) {
				*dstp = ((uint16_t *)palmap)[bit];
			}
			byte <<= 1;
			dstp++;
		}
		src += srcskip;
		dstp += dstskip;
	}
}

static void BlitBto3Key(MSD_BlitInfo *info)
{
    int width = info->d_width;
	int height = info->d_height;
	uint8_t *src = info->s_pixels;
	uint8_t *dst = info->d_pixels;
	int srcskip = info->s_skip;
	int dstskip = info->d_skip;
	uint32_t ckey = info->src->colorkey;
	uint8_t *palmap = info->table;
	int c;

	srcskip += width - (width + 7) / 8;

	while ( height-- ) {
	    uint8_t  byte = 0, bit;
	    for (c = 0; c < width; ++c) {
			if ((c & 7) == 0) {
				byte = *src++;
			}
			bit = (byte & 0x80) >> 7;
			if (bit != ckey) {
				memcpy(dst, &palmap[bit * 4], 3);
			}
			byte <<= 1;
			dst += 3;
		}
		src += srcskip;
		dst += dstskip;
	}
}

static void BlitBto4Key(MSD_BlitInfo *info)
{
    int width = info->d_width;
	int height = info->d_height;
	uint8_t *src = info->s_pixels;
	uint32_t *dstp = (uint32_t *)info->d_pixels;
	int srcskip = info->s_skip;
	int dstskip = info->d_skip;
	uint32_t ckey = info->src->colorkey;
	uint8_t *palmap = info->table;
	int c;

	srcskip += width - (width + 7) / 8;
	dstskip /= 4;

	while (height--) {
	    uint8_t byte = 0, bit;
	    for (c = 0; c < width; ++c) {
			if ((c & 7) == 0) {
				byte = *src++;
			}
			bit = (byte & 0x80) >> 7;
			if (bit != ckey) {
				*dstp = ((uint32_t *)palmap)[bit];
			}
			byte <<= 1;
			dstp++;
		}
		src += srcskip;
		dstp += dstskip;
	}
}

static void BlitBtoNAlpha(MSD_BlitInfo *info)
{
    int width = info->d_width;
	int height = info->d_height;
	uint8_t *src = info->s_pixels;
	uint8_t *dst = info->d_pixels;
	int srcskip = info->s_skip;
	int dstskip = info->d_skip;
	const MSD_Color *srcpal	= info->src->palette->colors;
	MSD_PixelFormat *dstfmt = info->dst;
	int  dstbpp;
	int c;
	const int A = info->src->alpha;

	dstbpp = dstfmt->BytesPerPixel;
	srcskip += width - (width + 7) / 8;

	while (height--) {
	    uint8_t byte = 0, bit;
	    for (c = 0; c < width; ++c) {
			if ((c & 7) == 0) {
				byte = *src++;
			}
			bit = (byte & 0x80) >> 7;
			if (1) {
				uint32_t pixel;
				unsigned sR, sG, sB;
				unsigned dR, dG, dB;
				sR = srcpal[bit].r;
				sG = srcpal[bit].g;
				sB = srcpal[bit].b;
				DISEMBLE_RGB(dst, dstbpp, dstfmt, pixel, dR, dG, dB);
				ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB);
				ASSEMBLE_RGB(dst, dstbpp, dstfmt, dR, dG, dB);
			}
			byte <<= 1;
			dst += dstbpp;
		}
		src += srcskip;
		dst += dstskip;
	}
}

static void BlitBtoNAlphaKey(MSD_BlitInfo *info)
{
    int width = info->d_width;
	int height = info->d_height;
	uint8_t *src = info->s_pixels;
	uint8_t *dst = info->d_pixels;
	int srcskip = info->s_skip;
	int dstskip = info->d_skip;
	MSD_PixelFormat *srcfmt = info->src;
	MSD_PixelFormat *dstfmt = info->dst;
	const MSD_Color *srcpal	= srcfmt->palette->colors;
	int dstbpp;
	int c;
	const int A = srcfmt->alpha;
	uint32_t ckey = srcfmt->colorkey;

	dstbpp = dstfmt->BytesPerPixel;
	srcskip += width - (width + 7) / 8;

	while (height--) {
	    uint8_t  byte = 0, bit;
	    for (c = 0; c < width; ++c) {
			if ((c & 7) == 0 ) {
				byte = *src++;
			}
			bit = (byte & 0x80) >> 7;
			if (bit != ckey) {
				int sR, sG, sB;
				int dR, dG, dB;
				uint32_t pixel;
				sR = srcpal[bit].r;
				sG = srcpal[bit].g;
				sB = srcpal[bit].b;
				DISEMBLE_RGB(dst, dstbpp, dstfmt, pixel, dR, dG, dB);
				ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB);
				ASSEMBLE_RGB(dst, dstbpp, dstfmt, dR, dG, dB);
			}
			byte <<= 1;
			dst += dstbpp;
		}
		src += srcskip;
		dst += dstskip;
	}
}

static MSD_loblit bitmap_blit[] = {
	NULL, BlitBto1, BlitBto2, BlitBto3, BlitBto4
};

static MSD_loblit colorkey_blit[] = {
    NULL, BlitBto1Key, BlitBto2Key, BlitBto3Key, BlitBto4Key
};

MSD_loblit MSD_CalculateBlit0(MSD_Surface *surface, int blit_index)
{
	int which;

	if (surface->format->BitsPerPixel != 1) {
		return NULL;
	}
	if (surface->map->dst->format->BitsPerPixel < 8) {
		which = 0;
	} else {
		which = surface->map->dst->format->BytesPerPixel;
	}
	switch(blit_index) {
	case 0:
	    return bitmap_blit[which];

	case 1:
	    return colorkey_blit[which];

	case 2:
	    return which >= 2 ? (MSD_loblit)BlitBtoNAlpha : NULL;

	case 4:
	    return which >= 2 ? (MSD_loblit)BlitBtoNAlphaKey : NULL;
	}
	return NULL;
}

