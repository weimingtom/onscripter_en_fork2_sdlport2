#pragma once

#include <stdio.h>

#if defined(_MSC_VER)
typedef signed __int8 int8_t;
typedef unsigned __int8 uint8_t;
typedef signed __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef signed __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef signed __int64 int64_t;
typedef unsigned __int64 uint64_t;
typedef unsigned int   uintptr_t;
#elif 1
#include <stdint.h>
#else
typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef signed short int16_t;
typedef unsigned short uint16_t;
typedef signed int int32_t;
typedef unsigned int uint32_t;
typedef signed long long int64_t;
typedef unsigned long long uint64_t;
typedef unsigned int uintptr_t;
#endif

typedef enum MSD_bool {
	MSD_FALSE = 0,
	MSD_TRUE = 1
} MSD_bool;

extern uint16_t MSD_Swap16(uint16_t x);
extern uint32_t MSD_Swap32(uint32_t x);
extern int MSD_IsLilEndian(void);

#define MSD_SwapLE16(X)	(MSD_IsLilEndian() ? (X) : MSD_Swap16(X))
#define MSD_SwapLE32(X)	(MSD_IsLilEndian() ? (X) : MSD_Swap32(X))
#define MSD_SwapBE16(X)	(MSD_IsLilEndian() ? MSD_Swap16(X) : (X))
#define MSD_SwapBE32(X)	(MSD_IsLilEndian() ? MSD_Swap32(X) : (X))

/*fileprivate*/

typedef struct MSD_RWops {
	int (*seek)(struct MSD_RWops *context, int offset, int whence);
	int (*read)(struct MSD_RWops *context, void *ptr, int size, int maxnum);
	int (*write)(struct MSD_RWops *context, const void *ptr, int size, int num);
	int (*close)(struct MSD_RWops *context);

	uint32_t type;
	union {
	    struct {
			int autoclose;
	 		FILE *fp;
	    } stdio;
	    struct {
			uint8_t *base;
	 		uint8_t *here;
			uint8_t *stop;
	    } mem;
	} hidden;
} MSD_RWops;

extern MSD_RWops *MSD_RWFromFile(const char *file, const char *mode);
extern MSD_RWops *MSD_RWFromFP(FILE *fp, int autoclose);
extern MSD_RWops *MSD_RWFromMem(void *mem, int size);
extern MSD_RWops *MSD_RWFromConstMem(const void *mem, int size);
extern MSD_RWops *MSD_AllocRW(void);
extern void MSD_FreeRW(MSD_RWops *area);

#define MSD_RW_SEEK_SET	0
#define MSD_RW_SEEK_CUR	1	
#define MSD_RW_SEEK_END	2	

#define MSD_RWseek(ctx, offset, whence)	(ctx)->seek(ctx, offset, whence)
#define MSD_RWtell(ctx)	(ctx)->seek(ctx, 0, MSD_RW_SEEK_CUR)
#define MSD_RWread(ctx, ptr, size, n) (ctx)->read(ctx, ptr, size, n)
#define MSD_RWwrite(ctx, ptr, size, n) (ctx)->write(ctx, ptr, size, n)
#define MSD_RWclose(ctx) (ctx)->close(ctx)

extern uint16_t MSD_ReadLE16(MSD_RWops *src);
extern uint16_t MSD_ReadBE16(MSD_RWops *src);
extern uint32_t MSD_ReadLE32(MSD_RWops *src);
extern uint32_t MSD_ReadBE32(MSD_RWops *src);

extern int MSD_WriteLE16(MSD_RWops *dst, uint16_t value);
extern int MSD_WriteBE16(MSD_RWops *dst, uint16_t value);
extern int MSD_WriteLE32(MSD_RWops *dst, uint32_t value);
extern int MSD_WriteBE32(MSD_RWops *dst, uint32_t value);


/*======*/


#define MSD_ALPHA_OPAQUE 255
#define MSD_ALPHA_TRANSPARENT 0

typedef struct MSD_Rect {
	int16_t x, y;
	uint16_t w, h;
} MSD_Rect;

typedef struct MSD_Color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t unused;
} MSD_Color;
#define MSD_Colour MSD_Color

typedef struct MSD_Palette {
	int       ncolors;
	MSD_Color *colors;
} MSD_Palette;

typedef struct MSD_PixelFormat {
	MSD_Palette *palette;
	uint8_t BitsPerPixel;
	uint8_t BytesPerPixel;
	uint8_t Rloss;
	uint8_t Gloss;
	uint8_t Bloss;
	uint8_t Aloss;
	uint8_t Rshift;
	uint8_t Gshift;
	uint8_t Bshift;
	uint8_t Ashift;
	uint32_t Rmask;
	uint32_t Gmask;
	uint32_t Bmask;
	uint32_t Amask;

	uint32_t colorkey;
	uint8_t alpha;
} MSD_PixelFormat;

typedef struct MSD_Surface {
	uint32_t flags; /* Read-only */
	MSD_PixelFormat *format; /* Read-only */
	int w, h; /* Read-only */
	uint16_t pitch; /* Read-only */
	void *pixels; /* Read-write */
	int offset;	/* Private */

	MSD_Rect clip_rect; /* Read-only */

	uint32_t locked; /* Private */
	struct MSD_BlitMap *map; /* Private */
	unsigned int format_version; /* Private */

	int refcount; /* Read-mostly */
} MSD_Surface;

#define MSD_SWSURFACE	0x00000000	/* Surface is in system memory */
#define MSD_HWSURFACE	0x00000001	/* Surface is in video memory */
#define MSD_ASYNCBLIT	0x00000004	/* Use asynchronous blits if possible */

#define MSD_ANYFORMAT	0x10000000	/* Allow any video depth/pixel-format */
#define MSD_HWPALETTE	0x20000000	/* Surface has exclusive palette */
#define MSD_DOUBLEBUF	0x40000000	/* Set up double-buffered video mode */
#define MSD_FULLSCREEN	0x80000000	/* Surface is a full screen display */
#define MSD_OPENGL      0x00000002      /* Create an OpenGL rendering context */
#define MSD_OPENGLBLIT	0x0000000A	/* Create an OpenGL rendering context and use it for blitting */
#define MSD_RESIZABLE	0x00000010	/* This video mode may be resized */
#define MSD_NOFRAME	0x00000020	/* No window caption or edge frame */

#define MSD_HWACCEL	0x00000100	/* Blit uses hardware acceleration */
#define MSD_SRCCOLORKEY	0x00001000	/* Blit uses a source color key */
#define MSD_RLEACCELOK	0x00002000	/* Private flag */
#define MSD_RLEACCEL	0x00004000	/* Surface is RLE encoded */
#define MSD_SRCALPHA	0x00010000	/* Blit uses source alpha blending */
#define MSD_PREALLOC	0x01000000	/* Surface uses preallocated memory */

#define MSD_MUSTLOCK(surface)	(surface->offset || ((surface->flags & (MSD_HWSURFACE|MSD_ASYNCBLIT|MSD_RLEACCEL)) != 0))

typedef int (*MSD_blit)(struct MSD_Surface *src, MSD_Rect *srcrect, struct MSD_Surface *dst, MSD_Rect *dstrect);

extern uint32_t MSD_MapRGB(const MSD_PixelFormat * const format, const uint8_t r, const uint8_t g, const uint8_t b);
extern uint32_t MSD_MapRGBA(const MSD_PixelFormat * const format, const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a);
extern void MSD_GetRGB(uint32_t pixel, MSD_PixelFormat *fmt, uint8_t *r, uint8_t *g, uint8_t *b);
extern void MSD_GetRGBA(uint32_t pixel, MSD_PixelFormat *fmt, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a);

#define MSD_AllocSurface    MSD_CreateRGBSurface
extern MSD_Surface * MSD_CreateRGBSurface(uint32_t flags, int width, int height, int depth, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask);
extern MSD_Surface * MSD_CreateRGBSurfaceFrom(void *pixels, int width, int height, int depth, int pitch, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask);
extern void MSD_FreeSurface(MSD_Surface *surface);
extern int MSD_LockSurface(MSD_Surface *surface);
extern void MSD_UnlockSurface(MSD_Surface *surface);

extern MSD_Surface *MSD_LoadBMP_RW(MSD_RWops *src, int freesrc);
#define MSD_LoadBMP(file) MSD_LoadBMP_RW(MSD_RWFromFile(file, "rb"), 1)
extern int MSD_SaveBMP_RW(MSD_Surface *surface, MSD_RWops *dst, int freedst);
#define MSD_SaveBMP(surface, file) MSD_SaveBMP_RW(surface, MSD_RWFromFile(file, "wb"), 1)

extern int MSD_SetColorKey(MSD_Surface *surface, uint32_t flag, uint32_t key);
extern int MSD_SetAlpha(MSD_Surface *surface, uint32_t flag, uint8_t alpha);
extern MSD_bool MSD_SetClipRect(MSD_Surface *surface, const MSD_Rect *rect);
extern void MSD_GetClipRect(MSD_Surface *surface, MSD_Rect *rect);
extern MSD_Surface * MSD_ConvertSurface(MSD_Surface *src, MSD_PixelFormat *fmt, uint32_t flags);
#define MSD_BlitSurface MSD_UpperBlit
extern int MSD_UpperBlit(MSD_Surface *src, MSD_Rect *srcrect, MSD_Surface *dst, MSD_Rect *dstrect);
extern int MSD_LowerBlit(MSD_Surface *src, MSD_Rect *srcrect, MSD_Surface *dst, MSD_Rect *dstrect);
extern int MSD_FillRect(MSD_Surface *dst, MSD_Rect *dstrect, uint32_t color);
extern int MSD_SoftStretch(MSD_Surface *src, MSD_Rect *srcrect, MSD_Surface *dst, MSD_Rect *dstrect);

/*private*/

typedef struct {
	uint8_t *s_pixels;
	int s_width;
	int s_height;
	int s_skip;
	uint8_t *d_pixels;
	int d_width;
	int d_height;
	int d_skip;
	void *aux_data;
	MSD_PixelFormat *src;
	uint8_t *table;
	MSD_PixelFormat *dst;
} MSD_BlitInfo;

typedef void (*MSD_loblit)(MSD_BlitInfo *info);

struct MSD_private_swaccel {
	MSD_loblit blit;
	void *aux_data;
};

typedef struct MSD_BlitMap {
	MSD_Surface *dst;
	int identity;
	uint8_t *table;
	MSD_blit hw_blit;
	MSD_blit sw_blit;
	struct MSD_private_swaccel *sw_data;

    unsigned int format_version;
} MSD_BlitMap;


extern MSD_PixelFormat *MSD_AllocFormat(int bpp, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask);
extern MSD_PixelFormat *MSD_ReallocFormat(MSD_Surface *surface, int bpp, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask);
extern void MSD_FormatChanged(MSD_Surface *surface);
extern void MSD_FreeFormat(MSD_PixelFormat *format);

extern MSD_BlitMap *MSD_AllocBlitMap(void);
extern void MSD_InvalidateMap(MSD_BlitMap *map);
extern int MSD_MapSurface(MSD_Surface *src, MSD_Surface *dst);
extern void MSD_FreeBlitMap(MSD_BlitMap *map);

extern uint16_t MSD_CalculatePitch(MSD_Surface *surface);
extern void MSD_DitherColors(MSD_Color *colors, int bpp);
extern uint8_t MSD_FindColor(MSD_Palette *pal, uint8_t r, uint8_t g, uint8_t b);
extern void MSD_ApplyGamma(uint16_t *gamma, MSD_Color *colors, MSD_Color *output, int ncolors);

extern int MSD_CalculateBlit(MSD_Surface *surface);
extern MSD_loblit MSD_CalculateBlit0(MSD_Surface *surface, int complex);
extern MSD_loblit MSD_CalculateBlit1(MSD_Surface *surface, int complex);
extern MSD_loblit MSD_CalculateBlitN(MSD_Surface *surface, int complex);
extern MSD_loblit MSD_CalculateAlphaBlit(MSD_Surface *surface, int complex);

