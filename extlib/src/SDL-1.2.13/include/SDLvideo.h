#pragma once

#include	"SDL_common.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	SDL_SWSURFACE	= 0x00000000,
	SDL_HWSURFACE	= 0x00000001,
//	SDL_OPENGL      = 0x00000002,
//	SDL_ASYNCBLIT	= 0x00000004,
	SDL_ANYFORMAT	= 0x10000000,
//	SDL_HWPALETTE	= 0x20000000,
	SDL_DOUBLEBUF	= 0x40000000,
	SDL_FULLSCREEN	= 0x80000000
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
//	int		offset;

	// Hardware-specific surface info
//	struct private_hwdata *hwdata;

	// clipping information
//	SDL_Rect	clip_rect;
//	DWORD		unused1;

	// Allow recursive locks
//	DWORD		locked;

	// info for fast blit mapping to other surfaces
//	struct SDL_BlitMap *map;

	// format version, bumped at every change to invalidate blit maps
//	unsigned int format_version;

	// Reference count -- used when freeing surface
//	int		refcount;
} SDL_Surface;

typedef struct {
//	Uint32 hw_available :1;	/* Flag: Can you create hardware surfaces? */
//	Uint32 wm_available :1;	/* Flag: Can you talk to a window manager? */
//	Uint32 UnusedBits1  :6;
//	Uint32 UnusedBits2  :1;
//	Uint32 blit_hw      :1;	/* Flag: Accelerated blits HW --> HW */
//	Uint32 blit_hw_CC   :1;	/* Flag: Accelerated blits with Colorkey */
//	Uint32 blit_hw_A    :1;	/* Flag: Accelerated blits with Alpha */
//	Uint32 blit_sw      :1;	/* Flag: Accelerated blits SW --> HW */
//	Uint32 blit_sw_CC   :1;	/* Flag: Accelerated blits with Colorkey */
//	Uint32 blit_sw_A    :1;	/* Flag: Accelerated blits with Alpha */
//	Uint32 blit_fill    :1;	/* Flag: Accelerated color fill */
//	Uint32 UnusedBits3  :16;
//	Uint32 video_mem;	/* The total amount of video memory (in K) */
	SDL_PixelFormat *vfmt;	/* Value: The format of the video surface */
	int    current_w;	//FIXME:not implemented
	int    current_h;	//FIXME:not implemented
} SDL_VideoInfo;




SDL_Surface *SDL_CreateRGBSurface(DWORD flags, int width, int height,
			int depth, DWORD Rmask, DWORD Gmask, DWORD Bmask, DWORD Amask);
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

typedef enum {
	CD_TRAYEMPTY,
	CD_STOPPED,
	CD_PLAYING,
	CD_PAUSED,
	CD_ERROR = -1
} CDstatus;

typedef struct SDL_CDtrack {
	Uint8 id;
	Uint8 type;	
	Uint16 unused;
	Uint32 length;
	Uint32 offset;
} SDL_CDtrack;

typedef struct SDL_CD {
	SDL_CDtrack track[SDL_MAX_TRACKS+1];
} SDL_CD;

extern int SDL_CDPlayTracks(SDL_CD *cdrom, int start_track, int start_frame, int ntracks, int nframes);
extern CDstatus SDL_CDStatus(SDL_CD *cdrom);
extern int SDL_CDStop(SDL_CD *cdrom);

typedef struct SDL_AudioCVT {
	Uint8 *buf;
} SDL_AudioCVT;

extern void SDL_WarpMouse(Uint16 x, Uint16 y);

//typedef struct SDL_Surface {
//	Uint32	flags; //FIXME:not implemented
//typedef struct SDL_PixelFormat {
//	BYTE		Rloss; //FIXME:not implemented
//	BYTE		Gloss; //FIXME:not implemented
//	BYTE		Bloss; //FIXME:not implemented
//	BYTE		Aloss; //FIXME:not implemented
//	DWORD		colorkey; //FIXME:not implemented

#define SDL_SRCCOLORKEY	0x00001000	/* Blit uses a source color key */

extern Uint32 SDL_MapRGBA(const SDL_PixelFormat * const format, const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a);

extern SDL_RWops * SDL_RWFromMem(void *mem, int size);

extern int SDL_SetAlpha(SDL_Surface *surface, Uint32 flag, Uint8 alpha);

#define SDL_ALPHA_OPAQUE 255

extern int SDL_SaveBMP(SDL_Surface *surface, const char *file);

#define SDL_DISABLE	 0
#define SDL_ENABLE	 1
extern int SDL_ShowCursor(int toggle);

extern int SDL_WM_IconifyWindow(void);


#define SDL_MAJOR_VERSION	1
#define SDL_MINOR_VERSION	2
#define SDL_PATCHLEVEL		13

typedef struct SDL_version {
	Uint8 major;
	Uint8 minor;
	Uint8 patch;
} SDL_version;

typedef struct SDL_SysWMinfo {
	SDL_version version;
	HWND window;
} SDL_SysWMinfo;

#define SDL_VERSION(X)							\
{									\
	(X)->major = SDL_MAJOR_VERSION;					\
	(X)->minor = SDL_MINOR_VERSION;					\
	(X)->patch = SDL_PATCHLEVEL;					\
}




extern int SDL_GetWMInfo(SDL_SysWMinfo *info);

extern void SDL_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h);

extern void SDL_WM_SetIcon(SDL_Surface *icon, Uint8 *mask);

//	int    current_w;	//FIXME:not implemented
//	int    current_h;	//FIXME:not implemented
//} SDL_VideoInfo;

extern int SDL_CDNumDrives(void);

extern SDL_CD * SDL_CDOpen(int drive);

extern void SDL_CDClose(SDL_CD *cdrom);

#define CD_INDRIVE(status)	((int)(status) > 0)

extern void SDL_UpdateRects(SDL_Surface *screen, int numrects, SDL_Rect *rects);



#define SDL_VERSIONNUM(X, Y, Z)						\
	((X)*1000 + (Y)*100 + (Z))

/* This is the version number macro for the current SDL version */
#define SDL_COMPILEDVERSION \
	SDL_VERSIONNUM(SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL)

#define SDL_VERSION_ATLEAST(X, Y, Z) \
	(SDL_COMPILEDVERSION >= SDL_VERSIONNUM(X, Y, Z))


extern char * SDL_strrchr(const char *string, int c);

extern size_t SDL_strlen(const char *string);

#define SDL_stack_alloc(type, count)    (type*)malloc(sizeof(type)*(count))
#define SDL_stack_free(data)            free(data)

extern size_t SDL_strlcpy(char *dst, const char *src, size_t maxlen);

extern size_t SDL_strlcat(char *dst, const char *src, size_t maxlen);

#define SDL_arraysize(array)	(sizeof(array)/sizeof(array[0]))

extern void SDL_SetModuleHandle(void *hInst);

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

