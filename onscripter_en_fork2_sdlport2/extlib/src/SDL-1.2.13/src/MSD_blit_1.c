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
                        if(MSD_IsLilEndian()) {		\
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



#define DUFFS_LOOP(pixel_copy_increment, width)				\
	DUFFS_LOOP8(pixel_copy_increment, width)




















static void Blit1to1(MSD_BlitInfo *info)
{
	int width, height;
	uint8_t *src, *map, *dst;
	int srcskip, dstskip;

	/* Set up some basic variables */
	width = info->d_width;
	height = info->d_height;
	src = info->s_pixels;
	srcskip = info->s_skip;
	dst = info->d_pixels;
	dstskip = info->d_skip;
	map = info->table;

	while (height--) {
		DUFFS_LOOP(
			{
			  *dst = map[*src];
			}
			dst++;
			src++;
		, width);
		src += srcskip;
		dst += dstskip;
	}
}

#define HI	(MSD_IsLilEndian() ? 1 : 0) 
#define LO	(MSD_IsLilEndian() ? 0 : 1)

static void Blit1to2(MSD_BlitInfo *info)
{
	int width, height;
	uint8_t *src, *dst;
	uint16_t *map;
	int srcskip, dstskip;

	width = info->d_width;
	height = info->d_height;
	src = info->s_pixels;
	srcskip = info->s_skip;
	dst = info->d_pixels;
	dstskip = info->d_skip;
	map = (uint16_t *)info->table;

	while ( height-- ) {
		DUFFS_LOOP(
		{
			*(uint16_t *)dst = map[*src++];
			dst += 2;
		},
		width);
		src += srcskip;
		dst += dstskip;
	}
}

static void Blit1to3(MSD_BlitInfo *info)
{
	int o;
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

	while (height--) {
		DUFFS_LOOP(
			{
				o = *src * 4;
				dst[0] = map[o++];
				dst[1] = map[o++];
				dst[2] = map[o++];
			}
			src++;
			dst += 3;
		, width);
		src += srcskip;
		dst += dstskip;
	}
}

static void Blit1to4(MSD_BlitInfo *info)
{
	int width, height;
	uint8_t *src;
	uint32_t *map, *dst;
	int srcskip, dstskip;

	width = info->d_width;
	height = info->d_height;
	src = info->s_pixels;
	srcskip = info->s_skip;
	dst = (uint32_t *)info->d_pixels;
	dstskip = info->d_skip / 4;
	map = (uint32_t *)info->table;

	while (height--) {
		DUFFS_LOOP(
			*dst++ = map[*src++];
		, width);
		src += srcskip;
		dst += dstskip;
	}
}

static void Blit1to1Key(MSD_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	uint8_t *src = info->s_pixels;
	int srcskip = info->s_skip;
	uint8_t *dst = info->d_pixels;
	int dstskip = info->d_skip;
	uint8_t *palmap = info->table;
	uint32_t ckey = info->src->colorkey;
        
	if (palmap) {
		while (height--) {
			DUFFS_LOOP(
			{
				if (*src != ckey) {
				  *dst = palmap[*src];
				}
				dst++;
				src++;
			},
			width);
			src += srcskip;
			dst += dstskip;
		}
	} else {
		while (height--) {
			DUFFS_LOOP(
			{
				if (*src != ckey) {
				  *dst = *src;
				}
				dst++;
				src++;
			},
			width);
			src += srcskip;
			dst += dstskip;
		}
	}
}

static void Blit1to2Key(MSD_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	uint8_t *src = info->s_pixels;
	int srcskip = info->s_skip;
	uint16_t *dstp = (uint16_t *)info->d_pixels;
	int dstskip = info->d_skip;
	uint16_t *palmap = (uint16_t *)info->table;
	uint32_t ckey = info->src->colorkey;

	dstskip /= 2;

	while (height--) {
		DUFFS_LOOP(
		{
			if (*src != ckey) {
				*dstp=palmap[*src];
			}
			src++;
			dstp++;
		},
		width);
		src += srcskip;
		dstp += dstskip;
	}
}

static void Blit1to3Key(MSD_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	uint8_t *src = info->s_pixels;
	int srcskip = info->s_skip;
	uint8_t *dst = info->d_pixels;
	int dstskip = info->d_skip;
	uint8_t *palmap = info->table;
	uint32_t ckey = info->src->colorkey;
	int o;

	while (height--) {
		DUFFS_LOOP(
		{
			if (*src != ckey) {
				o = *src * 4;
				dst[0] = palmap[o++];
				dst[1] = palmap[o++];
				dst[2] = palmap[o++];
			}
			src++;
			dst += 3;
		},
		width);
		src += srcskip;
		dst += dstskip;
	}
}

static void Blit1to4Key(MSD_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	uint8_t *src = info->s_pixels;
	int srcskip = info->s_skip;
	uint32_t *dstp = (uint32_t *)info->d_pixels;
	int dstskip = info->d_skip;
	uint32_t *palmap = (uint32_t *)info->table;
	uint32_t ckey = info->src->colorkey;

	dstskip /= 4;

	while (height--) {
		DUFFS_LOOP(
		{
			if (*src != ckey) {
				*dstp = palmap[*src];
			}
			src++;
			dstp++;
		},
		width);
		src += srcskip;
		dstp += dstskip;
	}
}

static void Blit1toNAlpha(MSD_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	uint8_t *src = info->s_pixels;
	int srcskip = info->s_skip;
	uint8_t *dst = info->d_pixels;
	int dstskip = info->d_skip;
	MSD_PixelFormat *dstfmt = info->dst;
	const MSD_Color *srcpal	= info->src->palette->colors;
	int dstbpp;
	const int A = info->src->alpha;

	dstbpp = dstfmt->BytesPerPixel;

	while (height--) {
	    int sR, sG, sB;
		int dR, dG, dB;
	    DUFFS_LOOP4(
		{
			uint32_t pixel;
			sR = srcpal[*src].r;
			sG = srcpal[*src].g;
			sB = srcpal[*src].b;
			DISEMBLE_RGB(dst, dstbpp, dstfmt, pixel, dR, dG, dB);
			ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB);
			ASSEMBLE_RGB(dst, dstbpp, dstfmt, dR, dG, dB);
			src++;
			dst += dstbpp;
		},
		width);
		src += srcskip;
		dst += dstskip;
	}
}

static void Blit1toNAlphaKey(MSD_BlitInfo *info)
{
	int width = info->d_width;
	int height = info->d_height;
	uint8_t *src = info->s_pixels;
	int srcskip = info->s_skip;
	uint8_t *dst = info->d_pixels;
	int dstskip = info->d_skip;
	MSD_PixelFormat *srcfmt = info->src;
	MSD_PixelFormat *dstfmt = info->dst;
	const MSD_Color *srcpal	= info->src->palette->colors;
	uint32_t ckey = srcfmt->colorkey;
	int dstbpp;
	const int A = srcfmt->alpha;

	dstbpp = dstfmt->BytesPerPixel;

	while (height--) {
	    int sR, sG, sB;
		int dR, dG, dB;
		DUFFS_LOOP(
		{
			if (*src != ckey) {
			    uint32_t pixel;
				sR = srcpal[*src].r;
				sG = srcpal[*src].g;
				sB = srcpal[*src].b;
				DISEMBLE_RGB(dst, dstbpp, dstfmt, pixel, dR, dG, dB);
				ALPHA_BLEND(sR, sG, sB, A, dR, dG, dB);
			  	ASSEMBLE_RGB(dst, dstbpp, dstfmt, dR, dG, dB);
			}
			src++;
			dst += dstbpp;
		},
		width);
		src += srcskip;
		dst += dstskip;
	}
}

static MSD_loblit one_blit[] = {
	NULL, Blit1to1, Blit1to2, Blit1to3, Blit1to4
};

static MSD_loblit one_blitkey[] = {
	NULL, Blit1to1Key, Blit1to2Key, Blit1to3Key, Blit1to4Key
};

MSD_loblit MSD_CalculateBlit1(MSD_Surface *surface, int blit_index)
{
	int which;
	MSD_PixelFormat *dstfmt;

	dstfmt = surface->map->dst->format;
	if (dstfmt->BitsPerPixel < 8) {
		which = 0;
	} else {
		which = dstfmt->BytesPerPixel;
	}
	switch(blit_index) {
	case 0:
	    return one_blit[which];

	case 1:
	    return one_blitkey[which];

	case 2:	
	    return which >= 2 ? (MSD_loblit)Blit1toNAlpha : NULL;

	case 3:	
	    return which >= 2 ? (MSD_loblit)Blit1toNAlphaKey : NULL;

	}
	return NULL;
}
