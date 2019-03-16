#pragma once

#include "SDL_common.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	SDL_SWSURFACE	= 0x00000000,
	SDL_HWSURFACE	= 0x00000001
};

typedef struct {
	BYTE	r;
	BYTE	g;
	BYTE	b;
	BYTE	unused;
} SDL_Color;

typedef struct {
	int			ncolors;
	SDL_Color	*colors;
} SDL_Palette;

typedef struct SDL_PixelFormat {
	SDL_Palette	*palette;
	BYTE		BitsPerPixel;
	BYTE		BytesPerPixel;
	BYTE		Rloss; //FIXME:not implemented
	BYTE		Gloss; //FIXME:not implemented
	BYTE		Bloss; //FIXME:not implemented
	BYTE		Aloss; //FIXME:not implemented
	BYTE		Rshift;
	BYTE		Gshift;
	BYTE		Bshift;
	BYTE		Ashift;
	DWORD		Rmask;
	DWORD		Gmask;
	DWORD		Bmask;
	DWORD		Amask;

	// RGB color key information
	DWORD		colorkey; //FIXME:not implemented

	// Alpha value information (per-surface alpha)
//	BYTE		alpha;
} SDL_PixelFormat;

typedef struct SDL_Surface {
	Uint32	flags; //FIXME:not implemented
	SDL_PixelFormat *format;
	int		w;
	int		h;
	WORD	pitch;
	void	*pixels;
} SDL_Surface;

typedef struct {
	SDL_PixelFormat *vfmt;	/* Value: The format of the video surface */
	int    current_w;	//FIXME:not implemented
	int    current_h;	//FIXME:not implemented
} SDL_VideoInfo;

SDL_Surface *SDL_CreateRGBSurface(DWORD flags, int width, int height, int depth, DWORD Rmask, DWORD Gmask, DWORD Bmask, DWORD Amask);
void SDL_FreeSurface(SDL_Surface *surface);

void SDL_WM_SetCaption(const char *title, const char *icon);
const SDL_VideoInfo *SDL_GetVideoInfo(void);
char *SDL_VideoDriverName(char *namebuf, int maxlen);
SDL_Surface *SDL_SetVideoMode(int width, int height, int bpp, DWORD flags);
SDL_Surface *SDL_GetVideoSurface(void);
int SDL_LockSurface(SDL_Surface *surface);
void SDL_UnlockSurface(SDL_Surface *surface);
int SDL_Flip(SDL_Surface *screen);

//FIXME:dummy
typedef struct {
	int x;
	int y;
	int w;
	int h;
} SDL_Rect;

extern int SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color);
extern Uint32 SDL_MapRGB(const SDL_PixelFormat * const format, const Uint8 r, const Uint8 g, const Uint8 b);
extern int SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect);

//dummy
typedef struct {
	void *mem;
	int size;
	int pos;
} SDL_RWops;

extern void SDL_RWclose(SDL_RWops *ctx);

extern SDL_Surface * SDL_ConvertSurface(SDL_Surface *src, SDL_PixelFormat *fmt, Uint32 flags);

typedef struct SDL_AudioSpec {
	int __;
} SDL_AudioSpec;

#define SDL_MAX_TRACKS	99

typedef struct SDL_AudioCVT {
	Uint8 *buf;
} SDL_AudioCVT;

extern void SDL_WarpMouse(Uint16 x, Uint16 y);

#define SDL_SRCCOLORKEY	0x00001000	/* Blit uses a source color key */

extern Uint32 SDL_MapRGBA(const SDL_PixelFormat * const format, const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a);

extern SDL_RWops * SDL_RWFromMem(void *mem, int size);

extern int SDL_SetAlpha(SDL_Surface *surface, Uint32 flag, Uint8 alpha);

#define SDL_ALPHA_OPAQUE 255

extern int SDL_SaveBMP(SDL_Surface *surface, const char *file);

typedef struct SDL_SysWMinfo {
	HWND window;
} SDL_SysWMinfo;
extern int SDL_GetWMInfo(SDL_SysWMinfo *info);

extern void SDL_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h);

extern void SDL_UpdateRects(SDL_Surface *screen, int numrects, SDL_Rect *rects);

extern SDL_RWops *SDL_RWFromFile(const char *file, const char *mode);
extern int SDL_RWseek(SDL_RWops *stream, long offset, int origin);
extern long SDL_RWtell(SDL_RWops *stream);
extern size_t SDL_RWread(void *buffer, size_t size, size_t count, SDL_RWops *stream);
extern Uint32 SDL_Swap32(Uint32 x);
extern Uint16 SDL_Swap16(Uint16 x);
extern Uint16 SDL_ReadLE16(SDL_RWops *src);
extern Uint32 SDL_ReadLE32(SDL_RWops *src);

#ifdef __cplusplus
}
#endif

