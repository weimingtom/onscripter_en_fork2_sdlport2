#include <malloc.h>
#include <string.h>
#include "MSD_video.h"

MSD_Surface * MSD_CreateRGBSurface(uint32_t flags,
			int width, int height, int depth,
			uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask)
{
	MSD_Surface *screen;
	MSD_Surface *surface;

	if ( width >= 16384 || height >= 65536 ) {
		fprintf(stderr, "Width or height is too large\n");
		return(NULL);
	}

	screen = NULL;
	flags &= ~MSD_HWSURFACE;
	
	surface = (MSD_Surface *)malloc(sizeof(*surface));
	if (surface == NULL) {
		fprintf(stderr, "Out of memory\n");
		return NULL;
	}
	surface->flags = MSD_SWSURFACE;
	surface->format = MSD_AllocFormat(depth, Rmask, Gmask, Bmask, Amask);
	if (surface->format == NULL) {
		free(surface);
		return NULL;
	}
	if (Amask) {
		surface->flags |= MSD_SRCALPHA;
	}
	surface->w = width;
	surface->h = height;
	surface->pitch = MSD_CalculatePitch(surface);
	surface->pixels = NULL;
	surface->offset = 0;
	surface->locked = 0;
	surface->map = NULL;
	MSD_SetClipRect(surface, NULL);
	MSD_FormatChanged(surface);

	if (((flags & MSD_HWSURFACE) == MSD_SWSURFACE)) {
		if (surface->w && surface->h) {
			surface->pixels = malloc(surface->h*surface->pitch);
			if (surface->pixels == NULL) {
				MSD_FreeSurface(surface);
				fprintf(stderr, "Out of memory\n");
				return NULL;
			}
			memset(surface->pixels, 0, surface->h*surface->pitch);
		}
	}

	surface->map = MSD_AllocBlitMap();
	if (surface->map == NULL) {
		MSD_FreeSurface(surface);
		return NULL;
	}

	surface->refcount = 1;
	return surface;
}

MSD_Surface *MSD_CreateRGBSurfaceFrom(void *pixels,
	int width, int height, int depth, int pitch,
	uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask)
{
	MSD_Surface *surface;

	surface = MSD_CreateRGBSurface(MSD_SWSURFACE, 0, 0, depth,
	                               Rmask, Gmask, Bmask, Amask);
	if (surface != NULL) {
		surface->flags |= MSD_PREALLOC;
		surface->pixels = pixels;
		surface->w = width;
		surface->h = height;
		surface->pitch = pitch;
		MSD_SetClipRect(surface, NULL);
	}
	return surface;
}

int MSD_SetColorKey (MSD_Surface *surface, uint32_t flag, uint32_t key)
{
	if ( flag & MSD_SRCCOLORKEY ) {
		if ( flag & (MSD_RLEACCEL|MSD_RLEACCELOK) ) {
			flag = (MSD_SRCCOLORKEY | MSD_RLEACCELOK);
		} else {
			flag = MSD_SRCCOLORKEY;
		}
	} else {
		flag = 0;
	}

	if ( (flag == (surface->flags & (MSD_SRCCOLORKEY|MSD_RLEACCELOK))) &&
	     (key == surface->format->colorkey) ) {
		return(0);
	}

	if (flag) {
		surface->flags |= MSD_SRCCOLORKEY;
		surface->format->colorkey = key;

		if (flag & MSD_RLEACCELOK) {
			surface->flags |= MSD_RLEACCELOK;
		} else {
			surface->flags &= ~MSD_RLEACCELOK;
		}
	} else {
		surface->flags &= ~(MSD_SRCCOLORKEY|MSD_RLEACCELOK);
		surface->format->colorkey = 0;
	}
	MSD_InvalidateMap(surface->map);
	return 0;
}

int MSD_SetAlpha (MSD_Surface *surface, uint32_t flag, uint8_t value)
{
	uint32_t oldflags = surface->flags;
	uint32_t oldalpha = surface->format->alpha;

	if (flag & MSD_SRCALPHA) {
		if (flag & (MSD_RLEACCEL|MSD_RLEACCELOK)) {
			flag = (MSD_SRCALPHA | MSD_RLEACCELOK);
		} else {
			flag = MSD_SRCALPHA;
		}
	} else {
		flag = 0;
	}

	if ((flag == (surface->flags & (MSD_SRCALPHA|MSD_RLEACCELOK))) &&
	    (!flag || value == oldalpha)) {
		return(0);
	}

	if (flag) {
		surface->flags |= MSD_SRCALPHA;
		surface->format->alpha = value;
		if ( flag & MSD_RLEACCELOK ) {
		        surface->flags |= MSD_RLEACCELOK;
		} else {
		        surface->flags &= ~MSD_RLEACCELOK;
		}
	} else {
		surface->flags &= ~MSD_SRCALPHA;
		surface->format->alpha = MSD_ALPHA_OPAQUE;
	}
	if ((surface->flags & MSD_HWACCEL) == MSD_HWACCEL
	     || oldflags != surface->flags
	     || (((oldalpha + 1) ^ (value + 1)) & 0x100)) {
		MSD_InvalidateMap(surface->map);
	}
	return 0;
}

int MSD_SetAlphaChannel(MSD_Surface *surface, uint8_t value)
{
	int row, col;
	int offset;
	uint8_t *buf;

	if ((surface->format->Amask != 0xFF000000) &&
	    (surface->format->Amask != 0x000000FF)) {
		fprintf(stderr, "Unsupported surface alpha mask format\n");
		return -1;
	}

	if (MSD_IsLilEndian()) {
		if (surface->format->Amask == 0xFF000000) {
			offset = 3;
		} else {
			offset = 0;
		}
	} else {
		if (surface->format->Amask == 0xFF000000) {
			offset = 0;
		} else {
			offset = 3;
		}
	}

	if (MSD_MUSTLOCK(surface)) {
		if (MSD_LockSurface(surface) < 0) {
			return -1;
		}
	}
	row = surface->h;
	while (row--) {
		col = surface->w;
		buf = (uint8_t *)surface->pixels + row * surface->pitch + offset;
		while(col--) {
			*buf = value;
			buf += 4;
		}
	}
	if ( MSD_MUSTLOCK(surface) ) {
		MSD_UnlockSurface(surface);
	}
	return 0;
}

static MSD_bool MSD_IntersectRect(const MSD_Rect *A, const MSD_Rect *B, MSD_Rect *intersection)
{
	int Amin, Amax, Bmin, Bmax;

	/* Horizontal intersection */
	Amin = A->x;
	Amax = Amin + A->w;
	Bmin = B->x;
	Bmax = Bmin + B->w;
	if(Bmin > Amin)
	        Amin = Bmin;
	intersection->x = Amin;
	if(Bmax < Amax)
	        Amax = Bmax;
	intersection->w = Amax - Amin > 0 ? Amax - Amin : 0;

	/* Vertical intersection */
	Amin = A->y;
	Amax = Amin + A->h;
	Bmin = B->y;
	Bmax = Bmin + B->h;
	if(Bmin > Amin)
	        Amin = Bmin;
	intersection->y = Amin;
	if(Bmax < Amax)
	        Amax = Bmax;
	intersection->h = Amax - Amin > 0 ? Amax - Amin : 0;

	return (intersection->w && intersection->h);
}

MSD_bool MSD_SetClipRect(MSD_Surface *surface, const MSD_Rect *rect)
{
	MSD_Rect full_rect;

	/* Don't do anything if there's no surface to act on */
	if ( ! surface ) {
		return MSD_FALSE;
	}

	/* Set up the full surface rectangle */
	full_rect.x = 0;
	full_rect.y = 0;
	full_rect.w = surface->w;
	full_rect.h = surface->h;

	/* Set the clipping rectangle */
	if ( ! rect ) {
		surface->clip_rect = full_rect;
		return 1;
	}
	return MSD_IntersectRect(rect, &full_rect, &surface->clip_rect);
}
void MSD_GetClipRect(MSD_Surface *surface, MSD_Rect *rect)
{
	if ( surface && rect ) {
		*rect = surface->clip_rect;
	}
}

int MSD_LowerBlit(MSD_Surface *src, MSD_Rect *srcrect, MSD_Surface *dst, MSD_Rect *dstrect)
{
	MSD_blit do_blit;

	if ((src->map->dst != dst) || (src->map->dst->format_version != src->map->format_version)) {
		if (MSD_MapSurface(src, dst) < 0) {
			return(-1);
		}
	}
	do_blit = src->map->sw_blit;
	return do_blit(src, srcrect, dst, dstrect);
}

int MSD_UpperBlit (MSD_Surface *src, MSD_Rect *srcrect, MSD_Surface *dst, MSD_Rect *dstrect)
{
    MSD_Rect fulldst;
	int srcx, srcy, w, h;

	if (!src || !dst) {
		fprintf(stderr, "MSD_UpperBlit: passed a NULL surface\n");
		return -1;
	}
	if (src->locked || dst->locked) {
		fprintf(stderr, "Surfaces must not be locked during blit\n");
		return -1;
	}

	/* If the destination rectangle is NULL, use the entire dest surface */
	if (dstrect == NULL) {
	    fulldst.x = fulldst.y = 0;
		dstrect = &fulldst;
	}

	if (srcrect) {
		int maxw, maxh;
	
		srcx = srcrect->x;
		w = srcrect->w;
		if (srcx < 0) {
		    w += srcx;
			dstrect->x -= srcx;
			srcx = 0;
		}
		maxw = src->w - srcx;
		if (maxw < w) {
			w = maxw;
		}

		srcy = srcrect->y;
		h = srcrect->h;
		if (srcy < 0) {
			h += srcy;
			dstrect->y -= srcy;
			srcy = 0;
		}
		maxh = src->h - srcy;
		if (maxh < h) {
			h = maxh;
		}
	} else {
	    srcx = srcy = 0;
		w = src->w;
		h = src->h;
	}

	{
		MSD_Rect *clip = &dst->clip_rect;
		int dx, dy;

		dx = clip->x - dstrect->x;
		if (dx > 0) {
			w -= dx;
			dstrect->x += dx;
			srcx += dx;
		}
		dx = dstrect->x + w - clip->x - clip->w;
		if (dx > 0) {
			w -= dx;
		}

		dy = clip->y - dstrect->y;
		if (dy > 0) {
			h -= dy;
			dstrect->y += dy;
			srcy += dy;
		}
		dy = dstrect->y + h - clip->y - clip->h;
		if (dy > 0) {
			h -= dy;
		}
	}

	if (w > 0 && h > 0) {
	    MSD_Rect sr;
	    sr.x = srcx;
		sr.y = srcy;
		sr.w = dstrect->w = w;
		sr.h = dstrect->h = h;
		return MSD_LowerBlit(src, &sr, dst, dstrect);
	}
	dstrect->w = dstrect->h = 0;
	return 0;
}

static int MSD_FillRect1(MSD_Surface *dst, MSD_Rect *dstrect, uint32_t color)
{
	fprintf(stderr, "1-bpp rect fill not yet implemented\n");
	return -1;
}

static int MSD_FillRect4(MSD_Surface *dst, MSD_Rect *dstrect, uint32_t color)
{
	fprintf(stderr, "4-bpp rect fill not yet implemented\n");
	return -1;
}

#define MSD_memset4(dst, val, len)		\
do {						\
	unsigned _count = (len);		\
	unsigned _n = (_count + 3) / 4;		\
	uint32_t *_p = (uint32_t *)(dst);		\
	uint32_t _val = (val);			\
        switch (_count % 4) {			\
        case 0: do {    *_p++ = _val;		\
        case 3:         *_p++ = _val;		\
        case 2:         *_p++ = _val;		\
        case 1:         *_p++ = _val;		\
		} while ( --_n );		\
	}					\
} while(0)

int MSD_FillRect(MSD_Surface *dst, MSD_Rect *dstrect, uint32_t color)
{
	int x, y;
	uint8_t *row;

	if (dst->format->BitsPerPixel < 8) {
		switch (dst->format->BitsPerPixel) {
		case 1:
			return MSD_FillRect1(dst, dstrect, color);
			break;

		case 4:
			return MSD_FillRect4(dst, dstrect, color);
			break;
		
		default:
			fprintf(stderr, "Fill rect on unsupported surface format\n");
			return -1;
		}
	}

	if (dstrect) {
		if (!MSD_IntersectRect(dstrect, &dst->clip_rect, dstrect)) {
			return(0);
		}
	} else {
		dstrect = &dst->clip_rect;
	}

	if ( MSD_LockSurface(dst) != 0 ) {
		return(-1);
	}
	row = (uint8_t *)dst->pixels+dstrect->y*dst->pitch+
			dstrect->x*dst->format->BytesPerPixel;
	if ( dst->format->palette || (color == 0) ) {
		x = dstrect->w*dst->format->BytesPerPixel;
		if ( !color && !((uintptr_t)row&3) && !(x&3) && !(dst->pitch&3) ) {
			int n = x >> 2;
			for ( y=dstrect->h; y; --y ) {
				MSD_memset4(row, 0, n);
				row += dst->pitch;
			}
		} else {
			for(y = dstrect->h; y; y--) {
				memset(row, color, x);
				row += dst->pitch;
			}
		}
	} else {
		switch (dst->format->BytesPerPixel) {
		case 2:
			for ( y=dstrect->h; y; --y ) {
				uint16_t *pixels = (uint16_t *)row;
				uint16_t c = (uint16_t)color;
				uint32_t cc = (uint32_t)c << 16 | c;
				int n = dstrect->w;
				if((uintptr_t)pixels & 3) {
					*pixels++ = c;
					n--;
				}
				if(n >> 1)
					MSD_memset4(pixels, cc, n >> 1);
				if(n & 1)
					pixels[n - 1] = c;
				row += dst->pitch;
			}
			break;

		case 3:
			if (!MSD_IsLilEndian()) {
				color <<= 8;
			}
			for ( y=dstrect->h; y; --y ) {
				uint8_t *pixels = row;
				for ( x=dstrect->w; x; --x ) {
					memcpy(pixels, &color, 3);
					pixels += 3;
				}
				row += dst->pitch;
			}
			break;

		case 4:
			for(y = dstrect->h; y; --y) {
				MSD_memset4(row, color, dstrect->w);
				row += dst->pitch;
			}
			break;
		}
	}
	MSD_UnlockSurface(dst);
	return 0;
}

int MSD_LockSurface (MSD_Surface *surface)
{
	if (!surface->locked) {
		surface->pixels = (uint8_t *)surface->pixels + surface->offset;
	}
	++surface->locked;
	return 0;
}

void MSD_UnlockSurface (MSD_Surface *surface)
{
	if (!surface->locked || (--surface->locked > 0)) {
		return;
	}
	surface->pixels = (uint8_t *)surface->pixels - surface->offset;
}

MSD_Surface * MSD_ConvertSurface (MSD_Surface *surface,
					MSD_PixelFormat *format, uint32_t flags)
{
	MSD_Surface *convert;
	uint32_t colorkey = 0;
	uint8_t alpha = 0;
	uint32_t surface_flags;
	MSD_Rect bounds;

	if (format->palette != NULL) {
		int i;
		for ( i=0; i<format->palette->ncolors; ++i ) {
			if ( (format->palette->colors[i].r != 0) ||
			     (format->palette->colors[i].g != 0) ||
			     (format->palette->colors[i].b != 0) )
				break;
		}
		if ( i == format->palette->ncolors ) {
			fprintf(stderr, "Empty destination palette\n");
			return(NULL);
		}
	}

	/* Only create hw surfaces with alpha channel if hw alpha blits
	   are supported */
	if (format->Amask != 0 && (flags & MSD_HWSURFACE)) {
		flags &= ~MSD_HWSURFACE;
	}

	/* Create a new surface with the desired format */
	convert = MSD_CreateRGBSurface(flags,
				surface->w, surface->h, format->BitsPerPixel,
		format->Rmask, format->Gmask, format->Bmask, format->Amask);
	if ( convert == NULL ) {
		return(NULL);
	}

	/* Copy the palette if any */
	if ( format->palette && convert->format->palette ) {
		memcpy(convert->format->palette->colors,
				format->palette->colors,
				format->palette->ncolors*sizeof(MSD_Color));
		convert->format->palette->ncolors = format->palette->ncolors;
	}

	/* Save the original surface color key and alpha */
	surface_flags = surface->flags;
	if ( (surface_flags & MSD_SRCCOLORKEY) == MSD_SRCCOLORKEY ) {
		/* Convert colourkeyed surfaces to RGBA if requested */
		if((flags & MSD_SRCCOLORKEY) != MSD_SRCCOLORKEY
		   && format->Amask) {
			surface_flags &= ~MSD_SRCCOLORKEY;
		} else {
			colorkey = surface->format->colorkey;
			MSD_SetColorKey(surface, 0, 0);
		}
	}
	if ( (surface_flags & MSD_SRCALPHA) == MSD_SRCALPHA ) {
		/* Copy over the alpha channel to RGBA if requested */
		if ( format->Amask ) {
			surface->flags &= ~MSD_SRCALPHA;
		} else {
			alpha = surface->format->alpha;
			MSD_SetAlpha(surface, 0, 0);
		}
	}

	/* Copy over the image data */
	bounds.x = 0;
	bounds.y = 0;
	bounds.w = surface->w;
	bounds.h = surface->h;
	MSD_LowerBlit(surface, &bounds, convert, &bounds);

	/* Clean up the original surface, and update converted surface */
	if ( convert != NULL ) {
		MSD_SetClipRect(convert, &surface->clip_rect);
	}
	if ( (surface_flags & MSD_SRCCOLORKEY) == MSD_SRCCOLORKEY ) {
		uint32_t cflags = surface_flags&(MSD_SRCCOLORKEY|MSD_RLEACCELOK);
		if ( convert != NULL ) {
			uint8_t keyR, keyG, keyB;

			MSD_GetRGB(colorkey,surface->format,&keyR,&keyG,&keyB);
			MSD_SetColorKey(convert, cflags|(flags&MSD_RLEACCELOK),
				MSD_MapRGB(convert->format, keyR, keyG, keyB));
		}
		MSD_SetColorKey(surface, cflags, colorkey);
	}
	if ( (surface_flags & MSD_SRCALPHA) == MSD_SRCALPHA ) {
		uint32_t aflags = surface_flags&(MSD_SRCALPHA|MSD_RLEACCELOK);
		if ( convert != NULL ) {
		        MSD_SetAlpha(convert, aflags|(flags&MSD_RLEACCELOK),
				alpha);
		}
		if ( format->Amask ) {
			surface->flags |= MSD_SRCALPHA;
		} else {
			MSD_SetAlpha(surface, aflags, alpha);
		}
	}

	return convert;
}

void MSD_FreeSurface(MSD_Surface *surface)
{
	if (surface == NULL) {
		return;
	}
	if (--surface->refcount > 0) {
		return;
	}
	while (surface->locked > 0) {
		MSD_UnlockSurface(surface);
	}
	if (surface->format) {
		MSD_FreeFormat(surface->format);
		surface->format = NULL;
	}
	if (surface->map != NULL) {
		MSD_FreeBlitMap(surface->map);
		surface->map = NULL;
	}
	if (surface->pixels && ((surface->flags & MSD_PREALLOC) != MSD_PREALLOC)) {
		free(surface->pixels);
	}
	free(surface);
}
