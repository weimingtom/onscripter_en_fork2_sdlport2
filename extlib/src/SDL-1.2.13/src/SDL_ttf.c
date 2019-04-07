#include <stddef.h>
#include <windows.h>
#include "SDL_ttf.h"

#pragma pack(push, 1)
typedef struct {
	BITMAPINFOHEADER	bmiHeader;
	RGBQUAD				bmiColors[256];
} BMPINFO;
#pragma pack(pop)

typedef struct {
	int		ptsize;
	int		bmpalign;
	HDC		hdcimage;
	HBITMAP	hBitmap;
	BYTE	*image;
	HFONT	hfont;
	RECT	rect;
} _TTFFNT, *TTFFNT;

int TTF_Init(void)
{
	//assert(0);
	return 0;
}

static const TCHAR deffontface[] = "SimSun";//"MS Gothic"; //FIXME: for TextOutA gb2312 
TTF_Font *TTF_OpenFont(const char *file, int ptsize)
{
	TTFFNT	ret;
	BMPINFO	bi;
	int		i;
	HDC		hdc;

	if (ptsize < 0) {
		ptsize *= -1;
	}
	if (ptsize < 6) {
		ptsize = 6;
	}
	else if (ptsize > 128) {
		ptsize = 128;
	}
	ret = (TTFFNT)malloc(sizeof(_TTFFNT));
	if (ret == NULL) {
		return(NULL);
	}
	memset(ret, 0, sizeof(_TTFFNT));
	ret->ptsize = ptsize;
	ret->bmpalign = ((ptsize + 31) / 8) & (~3);

	memset(&bi, 0, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = ptsize;
	bi.bmiHeader.biHeight = 0 - ptsize;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 1;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biSizeImage = ret->bmpalign * ptsize;
	bi.bmiHeader.biClrUsed = 2;
	bi.bmiHeader.biClrImportant = 2;
	for (i=0; i<2; i++) {
		bi.bmiColors[i].rgbRed = (i ^ 1) - 1;
		bi.bmiColors[i].rgbGreen = (i ^ 1) - 1;
		bi.bmiColors[i].rgbBlue = (i ^ 1) - 1;
		bi.bmiColors[i].rgbReserved = PC_RESERVED;
	}

    hdc = GetDC(NULL);
	ret->hBitmap = CreateDIBSection(hdc, (BITMAPINFO *)&bi, DIB_RGB_COLORS,
											(void **)&ret->image, NULL, 0);
	ret->hdcimage = CreateCompatibleDC(hdc);
	ReleaseDC(NULL, hdc);
	ret->hBitmap = (HBITMAP)SelectObject(ret->hdcimage, ret->hBitmap);
	SetDIBColorTable(ret->hdcimage, 0, 2, bi.bmiColors);
	ret->hfont = CreateFont(ptsize, 0,
						FW_DONTCARE, FW_DONTCARE, FW_REGULAR,
						FALSE, FALSE, FALSE, /*SHIFTJIS_CHARSET,*/ GB2312_CHARSET, //FIXME: TextOutA cp_acp 
						OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
						NONANTIALIASED_QUALITY, FIXED_PITCH, deffontface); //FIXME: *file is useless
	ret->hfont = (HFONT)SelectObject(ret->hdcimage, ret->hfont);
	SetTextColor(ret->hdcimage, RGB(255, 255, 255));
	SetBkColor(ret->hdcimage, RGB(0, 0, 0));
	SetRect(&ret->rect, 0, 0, ptsize, ptsize);
	return (TTF_Font *)ret;
}

SDL_Surface *TTF_RenderGlyph_Shaded(TTF_Font *font, Uint16 ch, SDL_Color fg, SDL_Color bg)
{
	SDL_Surface	*ret;
	char		sjis[4];
	int			leng;
	TTFFNT		fhdl;
	SIZE		fntsize;
#if 0
	SDL_Palette	*palette;
#endif
	const BYTE		*src;
	BYTE		*dst;
	int			x;
	WCHAR  ch2[] = {ch, 0};

	ret = NULL;
	if (font == NULL) {
		goto trus_exit;
	}
	leng = WideCharToMultiByte(CP_ACP, 0, ch2, -1,
										sjis, sizeof(sjis), NULL, NULL) - 1;
	fhdl = (TTFFNT)font;
	FillRect(fhdl->hdcimage, &fhdl->rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
	TextOutA(fhdl->hdcimage, 0, 0, sjis, leng);
	if ((!GetTextExtentPoint32(fhdl->hdcimage, sjis, leng, &fntsize)) ||
		(fntsize.cx == 0) || (fntsize.cy == 0)) {
		goto trus_exit;
	}
	fntsize.cx = min(fntsize.cx, fhdl->ptsize);
	fntsize.cy = min(fntsize.cy, fhdl->ptsize);
	ret = SDL_CreateRGBSurface(SDL_SWSURFACE, fntsize.cx, fntsize.cy, 8,
																0, 0, 0, 0);
	if (ret == NULL) {
		goto trus_exit;
	}

#if 0
	palette = ret->format->palette;
	palette->colors[0].r = bg.r; //bg
	palette->colors[0].g = bg.g;
	palette->colors[0].b = bg.b;
	palette->colors[1].r = fg.r;
	palette->colors[1].g = fg.g;
	palette->colors[1].b = fg.b;
#endif

	src = (BYTE *)fhdl->image;
	dst = (BYTE *)SDL_Surface_get_pixels(ret);
	do {
		for (x=0; x<fntsize.cx; x++) {
			dst[x] = ((src[x / 8] >> ((x ^ 7) & 7)) & 1) ? 0xff : 0;
		} //0xff;// FIXME: I don't know why (... &1) is wrong, but ((... &1) ? 0xff : 0) is right
		src += fhdl->bmpalign;
		dst += SDL_Surface_get_pitch(ret);
	} while(--fntsize.cy);

trus_exit:
	return(ret);
}

int TTF_GlyphMetrics(TTF_Font *font, Uint16 ch, int *minx, int *maxx, int *miny, int *maxy, int *advance)
{
	int ret;
	char		sjis[4];
	int			leng;
	TTFFNT		fhdl;
	SIZE		fntsize;
	WCHAR  ch2[] = {ch, 0};

	ret = 0;
	if ((font == NULL)) {
		goto trus_exit;
	}
	leng = WideCharToMultiByte(CP_ACP, 0, ch2, -1,
										sjis, sizeof(sjis), NULL, NULL) - 1;
	fhdl = (TTFFNT)font;
	if ((!GetTextExtentPoint32(fhdl->hdcimage, sjis, leng, &fntsize)) ||
		(fntsize.cx == 0) || (fntsize.cy == 0)) {
		goto trus_exit;
	}

	if (minx) *minx = 0;
	if (miny) *miny = 0;
	if (maxx) *maxx = fntsize.cx;
	if (maxy) *maxy = fntsize.cy;
	if (advance) *advance = 0;
trus_exit:
	return(ret);
}

int TTF_FontAscent(const TTF_Font *font)
{
	return ((TTFFNT)font)->ptsize;
}
