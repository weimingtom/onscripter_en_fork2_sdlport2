#include "MSD_video.h"

static void copy_row1(uint8_t *src, int src_w, uint8_t *dst, int dst_w)
{
	int i;
	int pos, inc;
	uint8_t pixel = 0;

	pos = 0x10000;
	inc = (src_w << 16) / dst_w;
	for ( i=dst_w; i>0; --i ) {
		while ( pos >= 0x10000L ) {
			pixel = *src++;
			pos -= 0x10000L;
		}
		*dst++ = pixel;
		pos += inc;
	}
}

static void copy_row2(uint16_t *src, int src_w, uint16_t *dst, int dst_w)
{
	int i;
	int pos, inc;
	uint16_t pixel = 0;

	pos = 0x10000;
	inc = (src_w << 16) / dst_w;
	for (i = dst_w; i > 0; --i) {
		while (pos >= 0x10000L) {
			pixel = *src++;
			pos -= 0x10000L;
		}
		*dst++ = pixel;
		pos += inc;
	}
}

static void copy_row4(uint32_t *src, int src_w, uint32_t *dst, int dst_w)
{
	int i;
	int pos, inc;
	uint32_t pixel = 0;

	pos = 0x10000;
	inc = (src_w << 16) / dst_w;
	for (i = dst_w; i > 0; --i) {
		while (pos >= 0x10000L) {
			pixel = *src++;
			pos -= 0x10000L;
		}
		*dst++ = pixel;
		pos += inc;
	}
}

static void copy_row3(uint8_t *src, int src_w, uint8_t *dst, int dst_w)
{
	int i;
	int pos, inc;
	uint8_t pixel[3] = { 0, 0, 0 };

	pos = 0x10000;
	inc = (src_w << 16) / dst_w;
	for (i = dst_w; i > 0; --i) {
		while (pos >= 0x10000L) {
			pixel[0] = *src++;
			pixel[1] = *src++;
			pixel[2] = *src++;
			pos -= 0x10000L;
		}
		*dst++ = pixel[0];
		*dst++ = pixel[1];
		*dst++ = pixel[2];
		pos += inc;
	}
}

int MSD_SoftStretch(MSD_Surface *src, MSD_Rect *srcrect, MSD_Surface *dst, MSD_Rect *dstrect)
{
	int src_locked;
	int dst_locked;
	int pos, inc;
	int dst_width;
	int dst_maxrow;
	int src_row, dst_row;
	uint8_t *srcp = NULL;
	uint8_t *dstp;
	MSD_Rect full_src;
	MSD_Rect full_dst;
	const int bpp = dst->format->BytesPerPixel;

	if (src->format->BitsPerPixel != dst->format->BitsPerPixel) {
		fprintf(stderr, "Only works with same format surfaces\n");
		return(-1);
	}

	if (srcrect) {
		if ((srcrect->x < 0) || (srcrect->y < 0) ||
		    ((srcrect->x+srcrect->w) > src->w) ||
		    ((srcrect->y+srcrect->h) > src->h)) {
			fprintf(stderr, "Invalid source blit rectangle\n");
			return -1;
		}
	} else {
		full_src.x = 0;
		full_src.y = 0;
		full_src.w = src->w;
		full_src.h = src->h;
		srcrect = &full_src;
	}
	if (dstrect) {
		if ((dstrect->x < 0) || (dstrect->y < 0) ||
		    ((dstrect->x+dstrect->w) > dst->w) ||
		    ((dstrect->y+dstrect->h) > dst->h)) {
			fprintf(stderr, "Invalid destination blit rectangle\n");
			return -1;
		}
	} else {
		full_dst.x = 0;
		full_dst.y = 0;
		full_dst.w = dst->w;
		full_dst.h = dst->h;
		dstrect = &full_dst;
	}

	dst_locked = 0;
	if (MSD_MUSTLOCK(dst)) {
		if (MSD_LockSurface(dst) < 0) {
			fprintf(stderr, "Unable to lock destination surface\n");
			return -1;
		}
		dst_locked = 1;
	}
	
	src_locked = 0;
	if (MSD_MUSTLOCK(src)) {
		if (MSD_LockSurface(src) < 0) {
			if (dst_locked) {
				MSD_UnlockSurface(dst);
			}
			fprintf(stderr, "Unable to lock source surface\n");
			return -1;
		}
		src_locked = 1;
	}

	pos = 0x10000;
	inc = (srcrect->h << 16) / dstrect->h;
	src_row = srcrect->y;
	dst_row = dstrect->y;
	dst_width = dstrect->w * bpp;

	for (dst_maxrow = dst_row + dstrect->h; dst_row < dst_maxrow; ++dst_row) {
		dstp = (uint8_t *)dst->pixels + (dst_row * dst->pitch) + (dstrect->x * bpp);
		while (pos >= 0x10000L) {
			srcp = (uint8_t *)src->pixels + (src_row * src->pitch) + (srcrect->x * bpp);
			++src_row;
			pos -= 0x10000L;
		}
		switch (bpp) {
		case 1:
			copy_row1(srcp, srcrect->w, dstp, dstrect->w);
			break;
		
		case 2:
			copy_row2((uint16_t *)srcp, srcrect->w, (uint16_t *)dstp, dstrect->w);
			break;
		
		case 3:
			copy_row3(srcp, srcrect->w, dstp, dstrect->w);
			break;

		case 4:
			copy_row4((uint32_t *)srcp, srcrect->w, (uint32_t *)dstp, dstrect->w);
			break;
		}
		pos += inc;
	}
	if (dst_locked) {
		MSD_UnlockSurface(dst);
	}
	if (src_locked) {
		MSD_UnlockSurface(src);
	}
	return 0;
}

