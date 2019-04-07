#include <string.h>
#include "MSD_video.h"

static int MSD_SoftBlit(MSD_Surface *src, MSD_Rect *srcrect, MSD_Surface *dst, MSD_Rect *dstrect)
{
	int okay;
	int src_locked;
	int dst_locked;

	okay = 1;

	dst_locked = 0;
	if (MSD_MUSTLOCK(dst)) {
		if (MSD_LockSurface(dst) < 0) {
			okay = 0;
		} else {
			dst_locked = 1;
		}
	}
	src_locked = 0;
	if (MSD_MUSTLOCK(src)) {
		if (MSD_LockSurface(src) < 0) {
			okay = 0;
		} else {
			src_locked = 1;
		}
	}

	if (okay && srcrect->w && srcrect->h) {
		MSD_BlitInfo info;
		MSD_loblit RunBlit;

		info.s_pixels = (uint8_t *)src->pixels +
				(uint16_t)srcrect->y*src->pitch +
				(uint16_t)srcrect->x*src->format->BytesPerPixel;
		info.s_width = srcrect->w;
		info.s_height = srcrect->h;
		info.s_skip=src->pitch-info.s_width*src->format->BytesPerPixel;
		info.d_pixels = (uint8_t *)dst->pixels +
				(uint16_t)dstrect->y*dst->pitch +
				(uint16_t)dstrect->x*dst->format->BytesPerPixel;
		info.d_width = dstrect->w;
		info.d_height = dstrect->h;
		info.d_skip=dst->pitch-info.d_width*dst->format->BytesPerPixel;
		info.aux_data = src->map->sw_data->aux_data;
		info.src = src->format;
		info.table = src->map->table;
		info.dst = dst->format;
		RunBlit = src->map->sw_data->blit;

		RunBlit(&info);
	}

	if (dst_locked) {
		MSD_UnlockSurface(dst);
	}
	if (src_locked) {
		MSD_UnlockSurface(src);
	}
	return okay ? 0 : -1;
}

static void MSD_BlitCopy(MSD_BlitInfo *info)
{
	uint8_t *src, *dst;
	int w, h;
	int srcskip, dstskip;

	w = info->d_width*info->dst->BytesPerPixel;
	h = info->d_height;
	src = info->s_pixels;
	dst = info->d_pixels;
	srcskip = w+info->s_skip;
	dstskip = w+info->d_skip;

	while (h--) {
		memcpy(dst, src, w);
		src += srcskip;
		dst += dstskip;
	}
}

static void *MSD_revcpy_(void *dst, const void *src, size_t len)
{
    char *srcp = (char *)src;
    char *dstp = (char *)dst;
    srcp += len-1;
    dstp += len-1;
    while (len--) {
        *dstp-- = *srcp--;
    }
    return dst;
}

static void MSD_BlitCopyOverlap(MSD_BlitInfo *info)
{
	uint8_t *src, *dst;
	int w, h;
	int srcskip, dstskip;

	w = info->d_width*info->dst->BytesPerPixel;
	h = info->d_height;
	src = info->s_pixels;
	dst = info->d_pixels;
	srcskip = w+info->s_skip;
	dstskip = w+info->d_skip;
	if (dst < src) {
		while (h--) {
			memcpy(dst, src, w);
			src += srcskip;
			dst += dstskip;
		}
	} else {
		src += ((h - 1) * srcskip);
		dst += ((h - 1) * dstskip);
		while (h--) {
			MSD_revcpy_(dst, src, w);
			src -= srcskip;
			dst -= dstskip;
		}
	}
}

int MSD_CalculateBlit(MSD_Surface *surface)
{
	int blit_index;

	surface->map->sw_blit = NULL;

	surface->flags &= ~MSD_HWACCEL;

	blit_index = 0;
	blit_index |= (!!(surface->flags & MSD_SRCCOLORKEY)) << 0;
	if (surface->flags & MSD_SRCALPHA
	    && (surface->format->alpha != MSD_ALPHA_OPAQUE
		|| surface->format->Amask)) {
		blit_index |= 2;
	}

	if (surface->map->identity && blit_index == 0) {
	    surface->map->sw_data->blit = MSD_BlitCopy;

		if (surface == surface->map->dst) {
			surface->map->sw_data->blit = MSD_BlitCopyOverlap;
		}
	} else {
		if (surface->format->BitsPerPixel < 8) {
			surface->map->sw_data->blit = MSD_CalculateBlit0(surface, blit_index);
		} else {
			switch ( surface->format->BytesPerPixel ) {
			case 1:
				surface->map->sw_data->blit = MSD_CalculateBlit1(surface, blit_index);
				break;

			case 2:
			case 3:
			case 4:
				surface->map->sw_data->blit = MSD_CalculateBlitN(surface, blit_index);
				break;
			
			default:
				surface->map->sw_data->blit = NULL;
				break;
			}
		}
	}
	if (surface->map->sw_data->blit == NULL) {
		MSD_InvalidateMap(surface->map);
		fprintf(stderr, "Blit combination not supported\n");
		return -1;
	}

	if (surface->map->sw_blit == NULL) {
		surface->map->sw_blit = MSD_SoftBlit;
	}
	return 0;
}

