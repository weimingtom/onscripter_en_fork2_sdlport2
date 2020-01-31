#include <string.h>
#include <malloc.h>
#include "MSD_video.h"

#define FORMAT_EQUAL(A, B)						\
    ((A)->BitsPerPixel == (B)->BitsPerPixel				\
     && ((A)->Rmask == (B)->Rmask) && ((A)->Amask == (B)->Amask))


#define PIXEL_FROM_RGBA(Pixel, fmt, r, g, b, a)				\
{									\
	Pixel = ((r>>fmt->Rloss)<<fmt->Rshift)|				\
		((g>>fmt->Gloss)<<fmt->Gshift)|				\
		((b>>fmt->Bloss)<<fmt->Bshift)|				\
		((a>>fmt->Aloss)<<fmt->Ashift);				\
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

















MSD_PixelFormat *MSD_AllocFormat(int bpp, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask)
{
	MSD_PixelFormat *format;
	uint32_t mask;

	format = malloc(sizeof(*format));
	if (format == NULL) {
		fprintf(stderr, "Out of memory\n");
		return NULL;
	}
	memset(format, 0, sizeof(*format));
	format->alpha = MSD_ALPHA_OPAQUE;

	format->BitsPerPixel = bpp;
	format->BytesPerPixel = (bpp + 7) / 8;
	if (Rmask || Bmask || Gmask) {
		format->palette = NULL;
		format->Rshift = 0;
		format->Rloss = 8;
		if (Rmask) {
			for (mask = Rmask; !(mask&0x01); mask >>= 1) {
				++format->Rshift;
			}
			for (; (mask&0x01); mask >>= 1) {
				--format->Rloss;
			}
		}
		format->Gshift = 0;
		format->Gloss = 8;
		if (Gmask) {
			for (mask = Gmask; !(mask&0x01); mask >>= 1) {
				++format->Gshift;
			}
			for (; (mask&0x01); mask >>= 1) {
				--format->Gloss;
			}
		}
		format->Bshift = 0;
		format->Bloss = 8;
		if (Bmask) {
			for (mask = Bmask; !(mask&0x01); mask >>= 1) {
				++format->Bshift;
			}
			for (; (mask&0x01); mask >>= 1) {
				--format->Bloss;
			}
		}
		format->Ashift = 0;
		format->Aloss = 8;
		if (Amask) {
			for (mask = Amask; !(mask&0x01); mask >>= 1) {
				++format->Ashift;
			}
			for (; (mask&0x01); mask >>= 1) {
				--format->Aloss;
			}
		}
		format->Rmask = Rmask;
		format->Gmask = Gmask;
		format->Bmask = Bmask;
		format->Amask = Amask;
	} else if (bpp > 8) {
		if (bpp > 24) {
			bpp = 24;
		}
		format->Rloss = 8 - (bpp / 3);
		format->Gloss = 8 - (bpp / 3) - (bpp % 3);
		format->Bloss = 8 - (bpp / 3);
		format->Rshift = ((bpp / 3)+(bpp % 3)) + (bpp / 3);
		format->Gshift = (bpp / 3);
		format->Bshift = 0;
		format->Rmask = ((0xFF >> format->Rloss) << format->Rshift);
		format->Gmask = ((0xFF >> format->Gloss) << format->Gshift);
		format->Bmask = ((0xFF >> format->Bloss) << format->Bshift);
	} else {
		format->Rloss = 8;
		format->Gloss = 8;
		format->Bloss = 8;
		format->Aloss = 8;
		format->Rshift = 0;
		format->Gshift = 0;
		format->Bshift = 0;
		format->Ashift = 0;
		format->Rmask = 0;
		format->Gmask = 0;
		format->Bmask = 0;
		format->Amask = 0;
	}
	if (bpp <= 8) {
		int ncolors = 1<<bpp;
		format->palette = (MSD_Palette *)malloc(sizeof(MSD_Palette));
		if (format->palette == NULL) {
			MSD_FreeFormat(format);
			fprintf(stderr, "Out of memory\n");
			return NULL;
		}
		(format->palette)->ncolors = ncolors;
		(format->palette)->colors = (MSD_Color *)malloc((format->palette)->ncolors * sizeof(MSD_Color));
		if ((format->palette)->colors == NULL) {
			MSD_FreeFormat(format);
			fprintf(stderr, "Out of memory\n");
			return(NULL);
		}
		if (Rmask || Bmask || Gmask) {
			int i;
			int Rm = 0, Gm = 0, Bm = 0;
			int Rw = 0, Gw = 0, Bw = 0;
			if (Rmask) {
				Rw = 8 - format->Rloss;
				for (i = format->Rloss; i > 0; i -= Rw) {
					Rm |= 1 << i;
				}
			}
			if (Gmask)
			{
				Gw = 8 - format->Gloss;
				for (i = format->Gloss; i > 0; i -= Gw) {
					Gm |= 1 << i;
				}
			}
			if (Bmask) {
				Bw = 8 - format->Bloss;
				for (i = format->Bloss; i > 0; i -= Bw) {
					Bm |= 1 << i;
				}
			}
			for (i = 0; i < ncolors; ++i) {
				int r, g, b;
				r = (i & Rmask) >> format->Rshift;
				r = (r << format->Rloss) | ((r * Rm) >> Rw);
				format->palette->colors[i].r = r;

				g = (i & Gmask)>>format->Gshift;
				g = (g << format->Gloss) | ((g * Gm) >> Gw);
				format->palette->colors[i].g = g;

				b = (i & Bmask)>>format->Bshift;
				b = (b << format->Bloss) | ((b * Bm) >> Bw);
				format->palette->colors[i].b = b;

				format->palette->colors[i].unused = 0;
			}
		} else if (ncolors == 2) {
			format->palette->colors[0].r = 0xFF;
			format->palette->colors[0].g = 0xFF;
			format->palette->colors[0].b = 0xFF;
			format->palette->colors[1].r = 0x00;
			format->palette->colors[1].g = 0x00;
			format->palette->colors[1].b = 0x00;
		} else {
			memset((format->palette)->colors, 0, (format->palette)->ncolors * sizeof(MSD_Color));
		}
	}
	return(format);
}

MSD_PixelFormat *MSD_ReallocFormat(MSD_Surface *surface, int bpp, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask)
{
	if (surface->format) {
		MSD_FreeFormat(surface->format);
		MSD_FormatChanged(surface);
	}
	surface->format = MSD_AllocFormat(bpp, Rmask, Gmask, Bmask, Amask);
	return surface->format;
}

void MSD_FormatChanged(MSD_Surface *surface)
{
	static int format_version = 0;
	++format_version;
	if (format_version < 0) {
		format_version = 1;
	}
	surface->format_version = format_version;
	MSD_InvalidateMap(surface->map);
}

void MSD_FreeFormat(MSD_PixelFormat *format)
{
	if (format) {
		if (format->palette) {
			if (format->palette->colors) {
				free(format->palette->colors);
			}
			free(format->palette);
		}
		free(format);
	}
}


void MSD_DitherColors(MSD_Color *colors, int bpp)
{
	int i;
	if (bpp != 8)
		return;

	for (i = 0; i < 256; i++) {
		int r, g, b;
		
		r = i & 0xe0;
		r |= r >> 3 | r >> 6;
		colors[i].r = r;
		
		g = (i << 3) & 0xe0;
		g |= g >> 3 | g >> 6;
		colors[i].g = g;
		
		b = i & 0x3;
		b |= b << 2;
		b |= b << 4;
		colors[i].b = b;
	}
}

uint16_t MSD_CalculatePitch(MSD_Surface *surface)
{
	uint16_t pitch;

	pitch = surface->w * surface->format->BytesPerPixel;
	switch (surface->format->BitsPerPixel) {
	case 1:
		pitch = (pitch + 7) / 8;
		break;

	case 4:
		pitch = (pitch + 1) / 2;
		break;
	
	default:
		break;
	}
	pitch = (pitch + 3) & ~3;
	return pitch;
}

uint8_t MSD_FindColor(MSD_Palette *pal, uint8_t r, uint8_t g, uint8_t b)
{
	unsigned int smallest;
	unsigned int distance;
	int rd, gd, bd;
	int i;
	uint8_t pixel = 0;
		
	smallest = ~0;
	for (i=0; i < pal->ncolors; ++i) {
		rd = pal->colors[i].r - r;
		gd = pal->colors[i].g - g;
		bd = pal->colors[i].b - b;
		distance = (rd * rd) + (gd * gd) + (bd * bd);
		if (distance < smallest) {
			pixel = i;
			if (distance == 0) {
				break;
			}
			smallest = distance;
		}
	}
	return pixel;
}

uint32_t MSD_MapRGB(const MSD_PixelFormat * const format, const uint8_t r, const uint8_t g, const uint8_t b)
{
	if (format->palette == NULL) {
		return (r >> format->Rloss) << format->Rshift
		       | (g >> format->Gloss) << format->Gshift
		       | (b >> format->Bloss) << format->Bshift
		       | format->Amask;
	} else {
		return MSD_FindColor(format->palette, r, g, b);
	}
}

uint32_t MSD_MapRGBA(const MSD_PixelFormat * const format, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a)
{
	if (format->palette == NULL) {
	    return (r >> format->Rloss) << format->Rshift
		    | (g >> format->Gloss) << format->Gshift
		    | (b >> format->Bloss) << format->Bshift
		    | ((a >> format->Aloss) << format->Ashift & format->Amask);
	} else {
		return MSD_FindColor(format->palette, r, g, b);
	}
}

void MSD_GetRGBA(uint32_t pixel, MSD_PixelFormat *fmt, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a)
{
	if (fmt->palette == NULL) {
	    unsigned v;
		v = (pixel & fmt->Rmask) >> fmt->Rshift;
		*r = (v << fmt->Rloss) + (v >> (8 - (fmt->Rloss << 1)));
		v = (pixel & fmt->Gmask) >> fmt->Gshift;
		*g = (v << fmt->Gloss) + (v >> (8 - (fmt->Gloss << 1)));
		v = (pixel & fmt->Bmask) >> fmt->Bshift;
		*b = (v << fmt->Bloss) + (v >> (8 - (fmt->Bloss << 1)));
		if (fmt->Amask) {
			v = (pixel & fmt->Amask) >> fmt->Ashift;
			*a = (v << fmt->Aloss) + (v >> (8 - (fmt->Aloss << 1)));
		} else {
			*a = MSD_ALPHA_OPAQUE;
        }
	} else {
		*r = fmt->palette->colors[pixel].r;
		*g = fmt->palette->colors[pixel].g;
		*b = fmt->palette->colors[pixel].b;
		*a = MSD_ALPHA_OPAQUE;
	}
}

void MSD_GetRGB(uint32_t pixel, MSD_PixelFormat *fmt, uint8_t *r,uint8_t *g,uint8_t *b)
{
	if (fmt->palette == NULL) {
	    unsigned v;
		v = (pixel & fmt->Rmask) >> fmt->Rshift;
		*r = (v << fmt->Rloss) + (v >> (8 - (fmt->Rloss << 1)));
		v = (pixel & fmt->Gmask) >> fmt->Gshift;
		*g = (v << fmt->Gloss) + (v >> (8 - (fmt->Gloss << 1)));
		v = (pixel & fmt->Bmask) >> fmt->Bshift;
		*b = (v << fmt->Bloss) + (v >> (8 - (fmt->Bloss << 1)));
	} else {
		*r = fmt->palette->colors[pixel].r;
		*g = fmt->palette->colors[pixel].g;
		*b = fmt->palette->colors[pixel].b;
	}
}

void MSD_ApplyGamma(uint16_t *gamma, MSD_Color *colors, MSD_Color *output, int ncolors)
{
	int i;
	for (i=0; i<ncolors; ++i) {
		output[i].r = gamma[0*256 + colors[i].r] >> 8;
		output[i].g = gamma[1*256 + colors[i].g] >> 8;
		output[i].b = gamma[2*256 + colors[i].b] >> 8;
	}
}

static uint8_t *Map1to1(MSD_Palette *src, MSD_Palette *dst, int *identical)
{
	uint8_t *map;
	int i;

	if (identical) {
		if (src->ncolors <= dst->ncolors) {
			if (memcmp(src->colors, dst->colors, src->ncolors * sizeof(MSD_Color)) == 0) {
				*identical = 1;
				return NULL;
			}
		}
		*identical = 0;
	}
	map = (uint8_t *)malloc(src->ncolors);
	if (map == NULL) {
		fprintf(stderr, "Out of memory\n");
		return NULL;
	}
	for (i = 0; i < src->ncolors; ++i) {
		map[i] = MSD_FindColor(dst, src->colors[i].r, src->colors[i].g, src->colors[i].b);
	}
	return map;
}

static uint8_t *Map1toN(MSD_PixelFormat *src, MSD_PixelFormat *dst)
{
	uint8_t *map;
	int i;
	int  bpp;
	unsigned alpha;
	MSD_Palette *pal = src->palette;

	bpp = ((dst->BytesPerPixel == 3) ? 4 : dst->BytesPerPixel);
	map = (uint8_t *)malloc(pal->ncolors*bpp);
	if (map == NULL) {
		fprintf(stderr, "Out of memory\n");
		return NULL;
	}

	alpha = dst->Amask ? src->alpha : 0;
	for (i=0; i<pal->ncolors; ++i) {
		ASSEMBLE_RGBA(&map[i*bpp], dst->BytesPerPixel, dst,
			      pal->colors[i].r, pal->colors[i].g,
			      pal->colors[i].b, alpha);
	}
	return map;
}

static uint8_t *MapNto1(MSD_PixelFormat *src, MSD_PixelFormat *dst, int *identical)
{
	MSD_Palette dithered;
	MSD_Color colors[256];
	MSD_Palette *pal = dst->palette;
	
	memset(colors, 0, sizeof(colors));

	dithered.ncolors = 256;
	MSD_DitherColors(colors, 8);
	dithered.colors = colors;
	return Map1to1(&dithered, pal, identical);
}

MSD_BlitMap *MSD_AllocBlitMap(void)
{
	MSD_BlitMap *map;

	map = (MSD_BlitMap *)malloc(sizeof(*map));
	if (map == NULL) {
		fprintf(stderr, "Out of memory\n");
		return(NULL);
	}
	memset(map, 0, sizeof(*map));
	map->sw_data = (struct MSD_private_swaccel *)malloc(sizeof(*map->sw_data));
	if (map->sw_data == NULL) {
		MSD_FreeBlitMap(map);
		fprintf(stderr, "Out of memory\n");
		return NULL;
	}
	memset(map->sw_data, 0, sizeof(*map->sw_data));
	return map;
}

void MSD_InvalidateMap(MSD_BlitMap *map)
{
	if (!map) {
		return;
	}
	map->dst = NULL;
	map->format_version = (unsigned int)-1;
	if (map->table) {
		free(map->table);
		map->table = NULL;
	}
}

int MSD_MapSurface(MSD_Surface *src, MSD_Surface *dst)
{
	MSD_PixelFormat *srcfmt;
	MSD_PixelFormat *dstfmt;
	MSD_BlitMap *map;

	map = src->map;
	MSD_InvalidateMap(map);

	map->identity = 0;
	srcfmt = src->format;
	dstfmt = dst->format;
	switch (srcfmt->BytesPerPixel) {
	case 1:
		switch (dstfmt->BytesPerPixel) {
		case 1:
			if (((src->flags & MSD_HWSURFACE) == MSD_HWSURFACE) &&
				((dst->flags & MSD_HWSURFACE) == MSD_HWSURFACE)) {
				map->identity = 1;
			} else {
				map->table = Map1to1(srcfmt->palette, dstfmt->palette, &map->identity);
			}
			if (!map->identity) {
				if (map->table == NULL) {
					return(-1);
				}
			}
			if (srcfmt->BitsPerPixel != dstfmt->BitsPerPixel) {
				map->identity = 0;
			}
			break;

		default:
			map->table = Map1toN(srcfmt, dstfmt);
			if (map->table == NULL) {
				return -1;
			}
			break;
		}
		break;

	default:
		switch (dstfmt->BytesPerPixel) {
		case 1:
			map->table = MapNto1(srcfmt, dstfmt, &map->identity);
			if (!map->identity) {
				if (map->table == NULL) {
					return(-1);
				}
			}
			map->identity = 0;
			break;
		
		default:
			if (FORMAT_EQUAL(srcfmt, dstfmt)) {
				map->identity = 1;
			}
			break;
		}
		break;
	}

	map->dst = dst;
	map->format_version = dst->format_version;

	return MSD_CalculateBlit(src);
}

void MSD_FreeBlitMap(MSD_BlitMap *map)
{
	if (map) {
		MSD_InvalidateMap(map);
		if (map->sw_data != NULL) {
			free(map->sw_data);
		}
		free(map);
	}
}
