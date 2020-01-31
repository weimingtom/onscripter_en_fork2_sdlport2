#include <windows.h>
#include <stdio.h>
#include <stddef.h>
#include <SDL.h>
#include <assert.h>

static SDL_VideoInfo vinfo;

extern SDL_Surface *__sdl_vsurf;
extern HWND __sdl_hWnd;

enum {
	SURFTYPE_MEMORY	= 0,
	SURFTYPE_BITMAP	= 1
};

typedef struct {
	UINT	type;
	HBITMAP	hbmp;
} _SURFINF, *SURFINF;

#pragma pack(push, 1)
typedef struct {
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD	bmiColors[256];
} BMPINFO;
#pragma pack(pop)

static BYTE mask2sft(DWORD mask) {
	BYTE ret;

	ret = 0;
	if (mask) {
		while(!(mask & (1 << ret))) {
			ret++;
		}
	}
	return(ret);
}

static SDL_Surface *__cresurf_sw(DWORD flags, int width, int height, int depth, DWORD Rmask, DWORD Gmask, DWORD Bmask, DWORD Amask) {
	int	size;
	int	xalign;
	SDL_Surface	*ret;
	SURFINF inf;
	SDL_PixelFormat	*fmt;
#if 0
	SDL_Palette *pal;
#endif
	
	if ((depth != 8) && (depth != 16) && (depth != 24) && (depth != 32)) {
		return NULL;
	}
	xalign = depth / 8;
	size = width * height * xalign;
	size += sizeof(SDL_Surface) + sizeof(_SURFINF) + sizeof(SDL_PixelFormat);
#if 0
	if (depth == 8) {
		size += sizeof(SDL_Palette) + (sizeof(SDL_Color) << depth);
	}
#endif
	ret = (SDL_Surface *)malloc(size);
	if (ret != NULL) {
		memset(ret, 0, size);
		inf = (SURFINF)(ret + 1);
		fmt = (SDL_PixelFormat *)(inf + 1);
		SDL_Surface_set_format(ret, fmt);
		SDL_Surface_set_w(ret, width);
		SDL_Surface_set_h(ret, height);
		SDL_Surface_set_pitch(ret, (WORD)(width * xalign));
#if 0
		if (depth == 8) {
			pal = (SDL_Palette *)(fmt + 1);
			col = (SDL_Color *)(pal + 1);
			fmt->palette = pal;
			pal->ncolors = 1 << depth;
			pal->colors = col;
			ret->pixels = (void *)(col + (1 << depth));
		}
		else 
#endif
		{
			SDL_Surface_set_pixels(ret, (void *)(fmt + 1));
			fmt->Rmask = Rmask;
			fmt->Gmask = Gmask;
			fmt->Bmask = Bmask;
			fmt->Amask = Amask;
			fmt->Rshift = mask2sft(Rmask);
			fmt->Gshift = mask2sft(Gmask);
			fmt->Bshift = mask2sft(Bmask);
			fmt->Ashift = mask2sft(Amask);
		}
		inf->type = SURFTYPE_MEMORY;
		fmt->BitsPerPixel = depth;
	}
	return ret;
}

static SDL_Surface *__cresurf_hw(DWORD flags, int width, int height, int depth, DWORD Rmask, DWORD Gmask, DWORD Bmask, DWORD Amask) {
	BMPINFO	bi;
	HDC	hdc;
	HBITMAP	hbmp;
	void *image;
	SDL_Surface	*ret;
	int	size;
	SURFINF inf;
	SDL_PixelFormat	*fmt;

	memset(&bi, 0, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = width;
	bi.bmiHeader.biHeight = 0 - height;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = depth;
	if (depth == 16) {
		bi.bmiHeader.biCompression = BI_BITFIELDS;
		*(DWORD *)(bi.bmiColors + 0) = 0xf800;
		*(DWORD *)(bi.bmiColors + 1) = 0x07e0;
		*(DWORD *)(bi.bmiColors + 2) = 0x001f;
	}
	else if (depth == 24 || depth == 32) {
		bi.bmiHeader.biCompression = BI_RGB;
	}
	else {
		goto ssc_err1;
	}
	hdc = GetDC(NULL);
	hbmp = CreateDIBSection(hdc, (BITMAPINFO *)&bi, DIB_RGB_COLORS,
														&image, NULL, 0);
	ReleaseDC(NULL, hdc);
	if (hbmp == NULL) {
		goto ssc_err1;
	}
	size = sizeof(SDL_Surface) + sizeof(_SURFINF) + sizeof(SDL_PixelFormat);
	ret = (SDL_Surface *)malloc(size);
	if (ret == NULL) {
		goto ssc_err2;
	}
	memset(ret, 0, size);
	inf = (SURFINF)(ret + 1);
	fmt = (SDL_PixelFormat *)(inf + 1);
	SDL_Surface_set_format(ret, fmt);
	SDL_Surface_set_w(ret, width);
	SDL_Surface_set_h(ret, height);
	SDL_Surface_set_pitch(ret, (WORD)(((width * depth / 8) + 3) & (~3)));
	SDL_Surface_set_pixels(ret, image);
	inf->type = SURFTYPE_BITMAP;
	inf->hbmp = hbmp;
	fmt->BitsPerPixel = depth;
	if (depth == 16) {
		fmt->Rshift = 11;
		fmt->Gshift = 5;
		fmt->Bshift = 0;
		fmt->Rmask = 0xf800;
		fmt->Gmask = 0x07e0;
		fmt->Bmask = 0x001f;
	}
	else if (depth == 24 || depth == 32) {
		fmt->Rshift = 16;
		fmt->Gshift = 8;
		fmt->Bshift = 0;
		fmt->Rmask = 0xff0000;
		fmt->Gmask = 0x00ff00;
		fmt->Bmask = 0x0000ff;
	}
	return(ret);

ssc_err2:
	DeleteObject(hbmp);

ssc_err1:
	return(NULL);
}

#define TEST_DRAW_DIR 0

SDL_Surface *SDL_CreateRGBSurface(DWORD flags, int width, int height, int depth, DWORD Rmask, DWORD Gmask, DWORD Bmask, DWORD Amask) {
	/*
	if ((width <= 0) || (height <= 0)) {
		return(NULL);
	}
	if (flags & SDL_HWSURFACE) {
		return cresurf_hw(flags, width, height, depth, Rmask, Gmask, Bmask, Amask);
	} else {
		return cresurf_sw(flags, width, height, depth, Rmask, Gmask, Bmask, Amask);
	}*/
	
	MSD_Surface *msd_surf;
	SDL_Surface *ret;

	if ((width <= 0) || (height <= 0)) {
		return(NULL);
	}
	if (flags & SDL_HWSURFACE) {
		//because on windows, SDL_DrawMemoryBitmap only support 24 bit color
		depth = 24; 
	}
	msd_surf = MSD_CreateRGBSurface(flags, width, height, depth, Rmask, Gmask, Bmask, Amask);
	ret = (SDL_Surface *)malloc(sizeof(SDL_Surface));
	ret->_surf = msd_surf;
	
	if (flags & SDL_HWSURFACE) {
#if TEST_DRAW_DIR		
		MSD_Surface *surf_test;
		MSD_Rect rect = {0, 0, 640, 480};

		surf_test = MSD_LoadBMP("bs.bmp"); 
		MSD_FillRect(msd_surf, NULL, 0xFFFF0000);
		MSD_SetAlpha(msd_surf, 0, 0xFF);
		MSD_BlitSurface(surf_test, &rect, msd_surf, NULL);
#endif

		return ret;
	} else {
		return ret;
	}
}

void SDL_FreeSurface(SDL_Surface *surface) {
#if 0
	SURFINF	inf;
#endif

	if (surface) {
#if 0
		inf = (SURFINF)(surface + 1);
		if (inf->type == SURFTYPE_BITMAP) {
			DeleteObject(inf->hbmp);
		}
#endif
		if (surface->_surf) {
			MSD_FreeSurface(surface->_surf);
		}
		free(surface);
	}
}

// ---- video

void __sdl_videoinit(void) {
	memset(&vinfo, 0, sizeof(vinfo));
	vinfo.current_w = 640;
	vinfo.current_w = 480;
}

void __sdl_videopaint(HWND hWnd, SDL_Surface *screen) {
	/*
	SURFINF	inf;
	HDC		hdc;
	HDC		hmemdc;
	HBITMAP	hbitmap;

	if (screen) {
		inf = (SURFINF)(screen + 1);
		if (inf->type == SURFTYPE_BITMAP) {
			hdc = GetDC(hWnd);
			hmemdc = CreateCompatibleDC(hdc);
			hbitmap = (HBITMAP)SelectObject(hmemdc, inf->hbmp);
			BitBlt(hdc, 0, 0, SDL_Surface_get_w(screen), SDL_Surface_get_h(screen), hmemdc, 0, 0, SRCCOPY);
			SelectObject(hmemdc, hbitmap);
			DeleteDC(hmemdc);
			ReleaseDC(hWnd, hdc);
		}
	}*/

	if (screen != NULL) {
		MSD_Surface *surf = screen->_surf;
		if (surf != NULL) {
			SDL_DrawMemoryBitmap(hWnd, surf->w, surf->h, surf->pixels);
			//MSD_SaveBMP(surf, "snapshot.bmp");
		}
	}
}

static void setclientsize(HWND hwnd, LONG width, LONG height) {
	int		scx, scy;
	int		x, y, w, h;
	RECT	rectWindow, rectClient, rectDisktop;

	GetWindowRect(hwnd, &rectWindow);
	GetClientRect(hwnd, &rectClient);
	w = width + (rectWindow.right - rectWindow.left)
				- (rectClient.right - rectClient.left);
	h = height + (rectWindow.bottom - rectWindow.top)
				- (rectClient.bottom - rectClient.top);

	scx = GetSystemMetrics(SM_CXSCREEN);
	scy = GetSystemMetrics(SM_CYSCREEN);
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rectDisktop, 0);

	x = rectWindow.left;
	y = rectWindow.top;

	if (scx < w) {
		x = (scx - w) / 2;
	}
	else {
		if ((x + w) > rectDisktop.right) {
			x = rectDisktop.right - w;
		}
		if (x < rectDisktop.left) {
			x = rectDisktop.left;
		}
	}
	if (scy < h) {
		y = (scy - h) / 2;
	}
	else {
		if ((y + h) > rectDisktop.bottom) {
			y = rectDisktop.bottom - h;
		}
		if (y < rectDisktop.top) {
			y = rectDisktop.top;
		}
	}
	MoveWindow(hwnd, x, y, w, h, TRUE);
	vinfo.current_w = width;
	vinfo.current_w = height;
}


// ----

void SDL_WM_SetCaption(const char *title, const char *icon) {
	char caption[256];

	strncpy(caption, "SDL", sizeof(caption));
	if (title) {
		strncat(caption, " - ", sizeof(caption));
		strncat(caption, title, sizeof(caption));
	}
	SetWindowText(__sdl_hWnd, caption);
}

const SDL_VideoInfo *SDL_GetVideoInfo(void) {
	return &vinfo;
}

//paint method: __sdl_videopaint(__sdl_hWnd, screen);
SDL_Surface *SDL_SetVideoMode(int width, int height, int bpp, DWORD flags) {
	SDL_FreeSurface(__sdl_vsurf);
	__sdl_vsurf = SDL_CreateRGBSurface(SDL_HWSURFACE, width, height, bpp, 0, 0, 0, 0);
	setclientsize(__sdl_hWnd, width, height);
	return __sdl_vsurf;
}

int SDL_LockSurface(SDL_Surface *surface) {
	return MSD_LockSurface(surface->_surf);
}

void SDL_UnlockSurface(SDL_Surface *surface) {
	MSD_UnlockSurface(surface->_surf);
}

int SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color)
{
	//assert(0);
	MSD_Rect _dstrect;
	MSD_Rect *p_dstrect = NULL;
	if (dstrect) {
		p_dstrect = &_dstrect;
		p_dstrect->x = dstrect->x;
		p_dstrect->y = dstrect->y;
		p_dstrect->w = dstrect->w;
		p_dstrect->h = dstrect->h;
	}
	return MSD_FillRect(dst->_surf, p_dstrect, color);
}

Uint32 SDL_MapRGB(const SDL_PixelFormat * const format, const Uint8 r, const Uint8 g, const Uint8 b)
{
	assert(0);
	return 0;
}

int SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect)
{
	//TODO:
	//assert(0);
	MSD_Surface *src_surf = NULL;
	MSD_Surface *dst_surf = NULL;
	MSD_Rect *p_srcrect = NULL, *p_dstrect = NULL, _srcrect, _dstrect;

	
	if (dst == __sdl_vsurf) {
		OutputDebugString("SDL_BlitSurface to __sdl_vsurf\n");
	}	
	if (src && dst) {
		src_surf = src->_surf;
		dst_surf = dst->_surf;
	}
	if (srcrect) {
		p_srcrect = &_srcrect;
		p_srcrect->x = srcrect->x;
		p_srcrect->y = srcrect->y;
		p_srcrect->w = srcrect->w;
		p_srcrect->h = srcrect->h;
	}
	if (dstrect) {
		p_dstrect = &_dstrect;
		p_dstrect->x = dstrect->x;
		p_dstrect->y = dstrect->y;
		p_dstrect->w = dstrect->w;
		p_dstrect->h = dstrect->h;
	}
	//MSD_FillRect(dst_surf, NULL, 0xFF000000);
	MSD_SetAlpha(dst_surf, 0, 0xFF);
	MSD_BlitSurface(src_surf, p_srcrect, dst_surf, p_dstrect);
	if (dst == __sdl_vsurf) {
		SDL_savebmp(dst_surf, "__sdl_vsurf");
	}
	return 0;
}

SDL_RWops * SDL_RWFromMem(void *mem, int size)
{
	SDL_RWops * ret = (SDL_RWops *)malloc(sizeof(SDL_RWops));
	ret->mem = mem;
	ret->size = size;
	ret->pos = 0;
	return ret;
}

SDL_Surface * SDL_ConvertSurface(SDL_Surface *src, SDL_PixelFormat *fmt, Uint32 flags)
{
	//assert(0);
	MSD_Surface *result;
	SDL_Surface *result_;
	MSD_PixelFormat temp_format;
	
	temp_format.palette = NULL; //FIXME:???
	temp_format.alpha = 255;

	temp_format.BitsPerPixel = fmt->BitsPerPixel;
	temp_format.Rloss = fmt->Rloss; 
	temp_format.Gloss = fmt->Gloss; 
	temp_format.Bloss = fmt->Bloss; 
	temp_format.Aloss = fmt->Aloss; 
	temp_format.Rshift = fmt->Rshift;
	temp_format.Gshift = fmt->Gshift;
	temp_format.Bshift = fmt->Bshift;
	temp_format.Ashift = fmt->Ashift;
	temp_format.Rmask = fmt->Rmask;
	temp_format.Gmask = fmt->Gmask;
	temp_format.Bmask = fmt->Bmask;
	temp_format.Amask = fmt->Amask;
	temp_format.colorkey = fmt->colorkey;

	result = MSD_ConvertSurface(src->_surf, &temp_format, flags);
	result_ = (SDL_Surface *)malloc(sizeof(SDL_Surface));
	result_->_surf = result;
	return result_;
}

Uint32 SDL_MapRGBA(const SDL_PixelFormat * const format, const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a)
{
	assert(0);
	return 0;
}

void SDL_RWclose(SDL_RWops *ctx)
{
	//assert(0);
	free(ctx);
}

int SDL_SetAlpha(SDL_Surface *surface, Uint32 flag, Uint8 alpha)
{
	//assert(0);
	return 0;
}

int SDL_SaveBMP(SDL_Surface *surface, const char *file)
{
	assert(0);
	return 0;
}

int SDL_GetWMInfo(SDL_SysWMinfo *info)
{
	info->window = NULL;
	return 0;
}

void SDL_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h)
{
	//TODO:
	//assert(0);
	//see WM_TIMER
}

void SDL_UpdateRects(SDL_Surface *screen, int numrects, SDL_Rect *rects)
{
	assert(0);
}

SDL_RWops *SDL_RWFromFile(const char *file, const char *mode) 
{
	FILE *f = NULL;
	SDL_RWops *src = NULL;

	char *mem = NULL;
	long size;

	f = fopen(file, "rb");
	if (f != NULL)
	{
		long pos;
		pos = ftell(f);
		fseek(f, 0, SEEK_END);
		size = ftell(f);
		fseek(f, pos, SEEK_SET);

		mem = malloc(size);
		fread(mem, size, 1, f);

		fclose(f);
	}
	if (mem != NULL)
	{
		src = (SDL_RWops *)malloc(sizeof(SDL_RWops));
		src->mem = mem;
		src->size = (int)size;
		src->pos = 0;
	}
	return src;
}

extern int SDL_RWseek(SDL_RWops *stream, long offset, int origin)
{
	if (origin == SEEK_SET)
	{
		if (offset >= 0 && offset < stream->size)
		{
			stream->pos = offset;
			return 0;
		}
	}
	else if (origin == SEEK_CUR)
	{
		if (stream->pos + offset >= 0 && stream->pos + offset < stream->size)
		{
			stream->pos += offset;
			return 0;
		}
	}
	else if (origin == SEEK_END)
	{
		if (stream->size + offset >= 0 && stream->size + offset < stream->size)
		{
			stream->pos = stream->size + offset;
		}
	}
	assert(0);
	return -1;
}

long SDL_RWtell(SDL_RWops *stream)
{
	return stream->pos;
}

size_t SDL_RWread(SDL_RWops *stream, void *buffer, size_t size, size_t count)
{
	size_t len = size * count;
	if ((int)len + stream->pos > stream->size)
	{
		len = stream->size - stream->pos;
	}
	memcpy(buffer, (void *)((char *)stream->mem + stream->pos), len);
	stream->pos += len;
	return len;
}

Uint32 SDL_Swap32(Uint32 x) 
{
	return((x<<24)|((x<<8)&0x00FF0000)|((x>>8)&0x0000FF00)|(x>>24));
}

Uint16 SDL_Swap16(Uint16 x) 
{
	return((x<<8)|(x>>8));
}

static int is_little_endian(void)
{
  unsigned short flag=0x4321;
  if (*(unsigned char*)&flag==0x21)
    return 1;
  else
    return 0;
}

Uint16 SDL_ReadLE16(SDL_RWops *src)
{
	Uint16 value;

	SDL_RWread(src, &value, (sizeof value), 1);
	//return(SDL_SwapLE16(value));
	return is_little_endian() ? value : SDL_Swap16(value);
}

Uint32 SDL_ReadLE32(SDL_RWops *src)
{
	Uint32 value;

	SDL_RWread(src, &value, (sizeof value), 1);
	//return(SDL_SwapLE32(value));
	return is_little_endian() ? value : SDL_Swap32(value);
}


SDL_PixelFormat *SDL_Surface_get_format(SDL_Surface *surface)
{
	surface->temp_format.BitsPerPixel = surface->_surf->format->BitsPerPixel;
	surface->temp_format.Rloss = surface->_surf->format->Rloss; 
	surface->temp_format.Gloss = surface->_surf->format->Gloss; 
	surface->temp_format.Bloss = surface->_surf->format->Bloss; 
	surface->temp_format.Aloss = surface->_surf->format->Aloss; 
	surface->temp_format.Rshift = surface->_surf->format->Rshift;
	surface->temp_format.Gshift = surface->_surf->format->Gshift;
	surface->temp_format.Bshift = surface->_surf->format->Bshift;
	surface->temp_format.Ashift = surface->_surf->format->Ashift;
	surface->temp_format.Rmask = surface->_surf->format->Rmask;
	surface->temp_format.Gmask = surface->_surf->format->Gmask;
	surface->temp_format.Bmask = surface->_surf->format->Bmask;
	surface->temp_format.Amask = surface->_surf->format->Amask;
	surface->temp_format.colorkey = surface->_surf->format->colorkey;
	return &surface->temp_format;
}

int SDL_Surface_get_w(SDL_Surface *surface)
{
	//return surface->_w;
	return surface->_surf->w;
}

int SDL_Surface_get_h(SDL_Surface *surface)
{
	//return surface->_h;
	return surface->_surf->h;
}

WORD SDL_Surface_get_pitch(SDL_Surface *surface)
{
	//return surface->_pitch;
	return surface->_surf->pitch;
}

void *SDL_Surface_get_pixels(SDL_Surface *surface)
{
	//return surface->_pixels;
	return surface->_surf->pixels;
}

void SDL_Surface_set_format(SDL_Surface *surface, SDL_PixelFormat *format)
{
	surface->_surf->format->BitsPerPixel = format->BitsPerPixel;
	surface->_surf->format->Rloss = format->Rloss; 
	surface->_surf->format->Gloss = format->Gloss; 
	surface->_surf->format->Bloss = format->Bloss; 
	surface->_surf->format->Aloss = format->Aloss; 
	surface->_surf->format->Rshift = format->Rshift;
	surface->_surf->format->Gshift = format->Gshift;
	surface->_surf->format->Bshift = format->Bshift;
	surface->_surf->format->Ashift = format->Ashift;
	surface->_surf->format->Rmask = format->Rmask;
	surface->_surf->format->Gmask = format->Gmask;
	surface->_surf->format->Bmask = format->Bmask;
	surface->_surf->format->Amask = format->Amask;
	surface->_surf->format->colorkey = format->colorkey;
}

void SDL_Surface_set_w(SDL_Surface *surface, int w)
{
	//surface->_w = w;
	surface->_surf->w = w;
}

void SDL_Surface_set_h(SDL_Surface *surface, int h)
{
	//surface->_h = h;
	surface->_surf->h = h;
}

void SDL_Surface_set_pitch(SDL_Surface *surface, WORD pitch)
{
	//surface->_pitch = pitch;
	surface->_surf->pitch = pitch;
}

void SDL_Surface_set_pixels(SDL_Surface *surface, void *pixels)
{
	//surface->_pixels = pixels;
	surface->_surf->pixels = pixels;
}


