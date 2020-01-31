#pragma once

#include "SDL_common.h"

#ifdef __cplusplus
extern "C" {
#endif

//dummy
typedef struct {
	void *mem;
	int size;
	int pos;
} SDL_RWops;
extern void SDL_RWclose(SDL_RWops *ctx);
extern SDL_RWops * SDL_RWFromMem(void *mem, int size);
extern SDL_RWops *SDL_RWFromFile(const char *file, const char *mode);
extern int SDL_RWseek(SDL_RWops *stream, long offset, int origin);
extern long SDL_RWtell(SDL_RWops *stream);
extern size_t SDL_RWread(SDL_RWops *stream, void *buffer, size_t size, size_t count);
extern Uint32 SDL_Swap32(Uint32 x);
extern Uint16 SDL_Swap16(Uint16 x);
extern Uint16 SDL_ReadLE16(SDL_RWops *src);
extern Uint32 SDL_ReadLE32(SDL_RWops *src);

enum {
	SDL_SWSURFACE = 0x00000000,
	SDL_HWSURFACE = 0x00000001
};

typedef struct {
	BYTE r;
	BYTE g;
	BYTE b;
	BYTE unused;
} SDL_Color;

typedef struct SDL_PixelFormat {
	BYTE BitsPerPixel; //depth
	BYTE Rloss; 
	BYTE Gloss; 
	BYTE Bloss; 
	BYTE Aloss; 
	BYTE Rshift;
	BYTE Gshift;
	BYTE Bshift;
	BYTE Ashift;
	DWORD Rmask;
	DWORD Gmask;
	DWORD Bmask;
	DWORD Amask;
	DWORD colorkey;
} SDL_PixelFormat;

//FIXME:dummy
typedef struct {
	int x;
	int y;
	int w;
	int h;
} SDL_Rect;

typedef struct {
	int current_w;
	int current_h;
} SDL_VideoInfo;

typedef struct SDL_Surface {
	//SDL_PixelFormat *_format;
	//int	_w;
	//int	_h;
	//WORD _pitch;
	//void *_pixels;

	SDL_PixelFormat temp_format;
	MSD_Surface *_surf;
} SDL_Surface;

extern SDL_PixelFormat *SDL_Surface_get_format(SDL_Surface *surface);
extern int SDL_Surface_get_w(SDL_Surface *surface);
extern int SDL_Surface_get_h(SDL_Surface *surface);
extern WORD SDL_Surface_get_pitch(SDL_Surface *surface);
extern void *SDL_Surface_get_pixels(SDL_Surface *surface);

extern void SDL_Surface_set_format(SDL_Surface *surface, SDL_PixelFormat *format);
extern void SDL_Surface_set_w(SDL_Surface *surface, int w);
extern void SDL_Surface_set_h(SDL_Surface *surface, int h);
extern void SDL_Surface_set_pitch(SDL_Surface *surface, WORD pitch);
extern void SDL_Surface_set_pixels(SDL_Surface *surface, void *pixels);

//hardware surface
extern void SDL_WM_SetCaption(const char *title, const char *icon);
extern const SDL_VideoInfo *SDL_GetVideoInfo(void);
extern SDL_Surface *SDL_SetVideoMode(int width, int height, int bpp, DWORD flags);
extern void SDL_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h);
extern void SDL_UpdateRects(SDL_Surface *screen, int numrects, SDL_Rect *rects);

//software surface
extern SDL_Surface *SDL_CreateRGBSurface(DWORD flags, int width, int height, int depth, DWORD Rmask, DWORD Gmask, DWORD Bmask, DWORD Amask);
extern SDL_Surface * SDL_ConvertSurface(SDL_Surface *src, SDL_PixelFormat *fmt, Uint32 flags);
extern int SDL_SaveBMP(SDL_Surface *surface, const char *file);
extern void SDL_FreeSurface(SDL_Surface *surface);
extern int SDL_LockSurface(SDL_Surface *surface);
extern void SDL_UnlockSurface(SDL_Surface *surface);
extern int SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color);
extern Uint32 SDL_MapRGB(const SDL_PixelFormat * const format, const Uint8 r, const Uint8 g, const Uint8 b);
extern Uint32 SDL_MapRGBA(const SDL_PixelFormat * const format, const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a);
extern int SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect);
#define SDL_ALPHA_OPAQUE 255
extern int SDL_SetAlpha(SDL_Surface *surface, Uint32 flag, Uint8 alpha);

typedef struct SDL_SysWMinfo {
	HWND window;
} SDL_SysWMinfo;
extern int SDL_GetWMInfo(SDL_SysWMinfo *info);

#ifdef __cplusplus
}
#endif

