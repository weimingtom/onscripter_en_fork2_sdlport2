//#include	"compiler.h"
#include	<windows.h>
#include	<stdio.h>
#include	<stddef.h>
#include	<SDL.h>

#ifndef __GNUC__
typedef	signed int			SINT;
typedef	signed char			SINT8;
typedef	unsigned char		UINT8;
typedef	signed short		SINT16;
typedef	unsigned short		UINT16;
typedef	signed int			SINT32;
typedef	unsigned int		UINT32;
#else
#include	<stdlib.h>
typedef	signed char			SINT8;
typedef	unsigned char		UINT8;
typedef	signed short		SINT16;
typedef	unsigned short		UINT16;
typedef	signed int			SINT32;
#endif

enum {
	SUCCESS		= 0,
	FAILURE		= 1
};


DWORD SDL_GetTicks(void) {

	return(GetTickCount());
}


static	char	__sdl_error[256];


int SDL_InitSubSystem(DWORD flags) {

	return(0);
}

void SDL_Quit(void) {
}

const char *SDL_GetError(void) {

	return(__sdl_error);
}

void __sdl_seterror(const char *error) {

	strncpy(__sdl_error, error, sizeof(__sdl_error));
}


// ----

static	const char	szClassName[] = "Windebug-SDL";
static	const char	szCaptionName[] = "SDL-Window";

BOOL		__sdl_avail = FALSE;
HWND		__sdl_hWnd = NULL;
SDL_Surface	*__sdl_vsurf = NULL;
int			__sdl_eventw = 0;
int			__sdl_mousex = 0;
int			__sdl_mousey = 0;
BYTE		__sdl_mouseb = 0;

static	SINT16		winkeytbl[256];
extern	void		__sdl_videoinit(void);
extern	void		__sdl_videopaint(HWND hWnd, SDL_Surface *screen);

typedef struct {
	UINT16	wincode;
	SINT16	sdlcode;
} SDLKEYTBL;

#define	VK_SEMICOLON	0xBA
#define	VK_EQUALS		0xBB
#define	VK_COMMA		0xBC
#define	VK_MINUS		0xBD
#define	VK_PERIOD		0xBE
#define	VK_SLASH		0xBF
#define	VK_GRAVE		0xC0
#define	VK_LBRACKET		0xDB
#define	VK_BACKSLASH	0xDC
#define	VK_RBRACKET		0xDD
#define	VK_APOSTROPHE	0xDE
#define	VK_BACKTICK		0xDF

static const SDLKEYTBL sdlkeytbl[] = {
		{VK_BACK,		SDLK_BACKSPACE},	{VK_TAB,		SDLK_TAB},
		{VK_CLEAR,		SDLK_CLEAR},		{VK_RETURN,		SDLK_RETURN},
		{VK_PAUSE,		SDLK_PAUSE},		{VK_ESCAPE,		SDLK_ESCAPE},
		{VK_SPACE,		SDLK_SPACE},		{VK_APOSTROPHE,	SDLK_QUOTE},
		{VK_COMMA,		SDLK_COMMA},		{VK_MINUS,		SDLK_MINUS},
		{VK_PERIOD,		SDLK_PERIOD},		{VK_SLASH,		SDLK_SLASH},

		{'0', SDLK_0}, {'1', SDLK_1}, {'2', SDLK_2}, {'3', SDLK_3},
		{'4', SDLK_4}, {'5', SDLK_5}, {'6', SDLK_6}, {'7', SDLK_7},
		{'8', SDLK_8}, {'9', SDLK_9},

		{VK_SEMICOLON,	SDLK_SEMICOLON},	{VK_EQUALS,		SDLK_EQUALS},

		{VK_LBRACKET,	SDLK_LEFTBRACKET},	{VK_BACKSLASH,	SDLK_BACKSLASH},
		{VK_RBRACKET,	SDLK_RIGHTBRACKET},	{VK_GRAVE,		SDLK_BACKQUOTE},
		{VK_BACKTICK,	SDLK_BACKQUOTE},

		{'A', SDLK_a}, {'B', SDLK_b}, {'C', SDLK_c}, {'D', SDLK_d},
		{'E', SDLK_e}, {'F', SDLK_f}, {'G', SDLK_g}, {'H', SDLK_h},
		{'I', SDLK_i}, {'J', SDLK_j}, {'K', SDLK_k}, {'L', SDLK_l},
		{'M', SDLK_m}, {'N', SDLK_n}, {'O', SDLK_o}, {'P', SDLK_p},
		{'Q', SDLK_q}, {'R', SDLK_r}, {'S', SDLK_s}, {'T', SDLK_t},
		{'U', SDLK_u}, {'V', SDLK_v}, {'W', SDLK_w}, {'X', SDLK_x},
		{'Y', SDLK_y}, {'Z', SDLK_z},

		{VK_DELETE,		SDLK_DELETE},

		{VK_NUMPAD0,	SDLK_KP0},			{VK_NUMPAD1,	SDLK_KP1},
		{VK_NUMPAD2,	SDLK_KP2},			{VK_NUMPAD3,	SDLK_KP3},
		{VK_NUMPAD4,	SDLK_KP4},			{VK_NUMPAD5,	SDLK_KP5},
		{VK_NUMPAD6,	SDLK_KP6},			{VK_NUMPAD7,	SDLK_KP7},
		{VK_NUMPAD8,	SDLK_KP8},			{VK_NUMPAD9,	SDLK_KP9},
		{VK_DECIMAL,	SDLK_KP_PERIOD},	{VK_DIVIDE,		SDLK_KP_DIVIDE},
		{VK_MULTIPLY,	SDLK_KP_MULTIPLY},	{VK_SUBTRACT,	SDLK_KP_MINUS},
		{VK_ADD,		SDLK_KP_PLUS},

		{VK_UP,			SDLK_UP},			{VK_DOWN,		SDLK_DOWN},
		{VK_RIGHT,		SDLK_RIGHT},		{VK_LEFT,		SDLK_LEFT},
		{VK_INSERT,		SDLK_INSERT},		{VK_HOME,		SDLK_HOME},
		{VK_END,		SDLK_END},			{VK_PRIOR,		SDLK_PAGEUP},
		{VK_NEXT,		SDLK_PAGEDOWN},

		{VK_F1,			SDLK_F1},			{VK_F2,			SDLK_F2},
		{VK_F3,			SDLK_F3},			{VK_F4,			SDLK_F4},
		{VK_F5,			SDLK_F5},			{VK_F6,			SDLK_F6},
		{VK_F7,			SDLK_F7},			{VK_F8,			SDLK_F8},
		{VK_F9,			SDLK_F9},			{VK_F10,		SDLK_F10},
		{VK_F11,		SDLK_F11},			{VK_F12,		SDLK_F12},

		{VK_NUMLOCK,	SDLK_NUMLOCK},		{VK_CAPITAL,	SDLK_CAPSLOCK},
		{VK_SCROLL,		SDLK_SCROLLOCK},	{VK_RSHIFT,		SDLK_RSHIFT},
		{VK_LSHIFT,		SDLK_LSHIFT},		{VK_RCONTROL,	SDLK_RCTRL},
		{VK_LCONTROL,	SDLK_LCTRL},		{VK_RMENU,		SDLK_RALT},
		{VK_LMENU,		SDLK_LALT},			{VK_RWIN,		SDLK_RSUPER},
		{VK_LWIN,		SDLK_LSUPER},		{VK_HELP,		SDLK_HELP},
		{VK_PRINT,		SDLK_PRINT},		{VK_SNAPSHOT,	SDLK_PRINT},
		{VK_CANCEL,		SDLK_BREAK},		{VK_APPS,		SDLK_MENU}};


static void makekeytbl(void) {
	int	i;
	const SDLKEYTBL	*key;
	const SDLKEYTBL	*keyterm;

	for (i = 0; i < 256; i++) {
		winkeytbl[i] = SDLK_UNKNOWN;
	}
	key = sdlkeytbl;
	keyterm = key + (sizeof(sdlkeytbl)/sizeof(SDLKEYTBL));
	while (key < keyterm) {
		winkeytbl[key->wincode & 0xff] = key->sdlcode;
		key++;
	}
}

static short cnvsdlkey(WPARAM wp, LPARAM lp) {
	switch (wp) {
	case VK_CONTROL:
		return (lp & (1 << 24)) ? SDLK_RCTRL : SDLK_LCTRL;

	case VK_SHIFT:
		return SDLK_LSHIFT;

	case VK_MENU:
		return((lp & (1 << 24))?SDLK_RALT:SDLK_LALT);
	}
	return winkeytbl[wp & 0xff];
}

#define IDT_TIMER1 1001

#define TEST_SHOWMEMBMP 0 /*test SDL_DrawMemoryBitmap*/
void test_init();
void test_load();
static LRESULT CALLBACK SdlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC	hdc;
	SDL_Event event;

	switch (msg) {
	case WM_CREATE:
		__sdl_hWnd = hWnd;
		makekeytbl();
#if TEST_SHOWMEMBMP
		test_init();
#endif
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		__sdl_videopaint(hWnd, __sdl_vsurf);
#if TEST_SHOWMEMBMP		
		test_load();
#endif
		EndPaint(hWnd, &ps);
		break;

	case WM_MOUSEMOVE:
		__sdl_mousex = LOWORD(lParam);
		__sdl_mousey = HIWORD(lParam);
		memset(&event, 0, sizeof(event));
		event.motion.type = SDL_MOUSEMOTION;
		event.motion.state = __sdl_mouseb;
		event.motion.x = LOWORD(lParam);
		event.motion.y = HIWORD(lParam);
		SDL_PushEvent(&event);
		break;

	case WM_LBUTTONDOWN:
		__sdl_mousex = LOWORD(lParam);
		__sdl_mousey = HIWORD(lParam);
		__sdl_mouseb |= 1 << (SDL_BUTTON_LEFT - 1);
		memset(&event, 0, sizeof(event));
		event.button.type = SDL_MOUSEBUTTONDOWN;
		event.button.button = SDL_BUTTON_LEFT;
		event.button.state = SDL_PRESSED;
		event.button.x = LOWORD(lParam);
		event.button.y = HIWORD(lParam);
		SDL_PushEvent(&event);
		break;

	case WM_LBUTTONUP:
		__sdl_mousex = LOWORD(lParam);
		__sdl_mousey = HIWORD(lParam);
		__sdl_mouseb &= ~(1 << (SDL_BUTTON_LEFT - 1));
		memset(&event, 0, sizeof(event));
		event.button.type = SDL_MOUSEBUTTONUP;
		event.button.button = SDL_BUTTON_LEFT;
		event.button.state = SDL_RELEASED;
		event.button.x = LOWORD(lParam);
		event.button.y = HIWORD(lParam);
		SDL_PushEvent(&event);
		break;

	case WM_RBUTTONDOWN:
		__sdl_mousex = LOWORD(lParam);
		__sdl_mousey = HIWORD(lParam);
		__sdl_mouseb |= 1 << (SDL_BUTTON_RIGHT - 1);
		memset(&event, 0, sizeof(event));
		event.button.type = SDL_MOUSEBUTTONDOWN;
		event.button.button = SDL_BUTTON_RIGHT;
		event.button.state = SDL_PRESSED;
		event.button.x = LOWORD(lParam);
		event.button.y = HIWORD(lParam);
		SDL_PushEvent(&event);
		break;

	case WM_RBUTTONUP:
		__sdl_mousex = LOWORD(lParam);
		__sdl_mousey = HIWORD(lParam);
		__sdl_mouseb &= ~(1 << (SDL_BUTTON_RIGHT - 1));
		memset(&event, 0, sizeof(event));
		event.button.type = SDL_MOUSEBUTTONUP;
		event.button.button = SDL_BUTTON_RIGHT;
		event.button.state = SDL_RELEASED;
		event.button.x = LOWORD(lParam);
		event.button.y = HIWORD(lParam);
		SDL_PushEvent(&event);
		break;

	case WM_KEYDOWN:
		memset(&event, 0, sizeof(event));
		event.key.type = SDL_KEYDOWN;
		event.key.keysym.sym = cnvsdlkey(wParam, lParam);
		if (event.key.keysym.sym != SDLK_UNKNOWN) {
			SDL_PushEvent(&event);
		}
		break;

	case WM_KEYUP:
		memset(&event, 0, sizeof(event));
		event.key.type = SDL_KEYUP;
		event.key.keysym.sym = cnvsdlkey(wParam, lParam);
		if (event.key.keysym.sym != SDLK_UNKNOWN) {
			SDL_PushEvent(&event);
		}
		break;

	case WM_CLOSE:
		memset(&event, 0, sizeof(event));
		event.type = SDL_QUIT;
		SDL_PushEvent(&event);
		__sdl_avail = FALSE;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_TIMER: 
		switch (wParam) { 
		case IDT_TIMER1: 
			//UpdateWindow(__sdl_hWnd);
			__sdl_videopaint(hWnd, __sdl_vsurf);
			return 0; 
		} 
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0L;
}

static BOOL sdlinit(void) {
	HINSTANCE	hInstance;
	WNDCLASS	sdlcls;

	hInstance = (HINSTANCE)GetWindowLong(HWND_DESKTOP, GWL_HINSTANCE);
	sdlcls.style = CS_BYTEALIGNCLIENT | CS_HREDRAW | CS_VREDRAW;
	sdlcls.lpfnWndProc = SdlProc;
	sdlcls.cbClsExtra = 0;
	sdlcls.cbWndExtra = 0;
	sdlcls.hInstance = hInstance;
	sdlcls.hIcon = NULL;
	sdlcls.hCursor = LoadCursor(NULL, IDC_ARROW);
	sdlcls.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	sdlcls.lpszMenuName = NULL;
	sdlcls.lpszClassName = szClassName;
	if (!RegisterClass(&sdlcls)) {
		return(FAILURE);
	}
	__sdl_hWnd = CreateWindowEx(0,
						szClassName, szCaptionName,
						WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
						WS_MINIMIZEBOX,
						CW_USEDEFAULT, CW_USEDEFAULT, 320, 240,
						NULL, NULL, hInstance, NULL);
	if (__sdl_hWnd == NULL) {
		return(FAILURE);
	}
	ShowWindow(__sdl_hWnd, SW_SHOW);
	UpdateWindow(__sdl_hWnd);
	__sdl_avail = TRUE;
	_SDL_Timer_init();
	
	SetTimer(__sdl_hWnd, IDT_TIMER1, 50, (TIMERPROC) NULL); /*500ms*/
	return(SUCCESS);
}

static void sdlterm(void) {

}


#if !IS_LIB

int main(int argc, char **argv) {
	int	r;

	__sdl_videoinit();
	if (sdlinit() != SUCCESS) {
		return(0);
	}
	r = SDL_main(argc, argv);
	sdlterm();
	return(r);
}

int SDL_main(int argc, char **argv) {
	SDL_Event e;
	int task_avail = 1;

	while (task_avail) {
		if ((!task_avail) || (!SDL_PollEvent(&e))) {
			continue;
		}
		switch(e.type) {
		case SDL_MOUSEMOTION:
			break;

		case SDL_MOUSEBUTTONUP:
			break;

		case SDL_MOUSEBUTTONDOWN:
			break;

		case SDL_KEYDOWN:
			break;

		case SDL_KEYUP:
			break;

		case SDL_QUIT:
			task_avail = FALSE;
			break;
		}
	}
	return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	main(__argc, __argv); 
}

#endif

int SDL_Init(Uint32 flags)
{
	sdlinit();
	return 0;
}

//https://blog.csdn.net/blognkliming/article/details/8331948 
//void SaveToBmpAndBlend(HWND m_hWnd, BYTE *buffer)
void SDL_DrawMemoryBitmap(HWND m_hWnd, int w, int h, BYTE *buffer)
{
	const int bytesPerPixel = 3;
	HDC hdc;
	HDC hdcMem;
	HBITMAP hBmp;
	BITMAPINFO bmpinfo;
	BYTE* pDibs = NULL;
    HGDIOBJ hOldSel;
	//int i;

	if (__sdl_hWnd == NULL) {
		return;
	}

	OutputDebugString(">>>SDL_DrawMemoryBitmap\n");

	hdc = GetDC(m_hWnd);
	hdcMem = CreateCompatibleDC(hdc);
	
	bmpinfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpinfo.bmiHeader.biWidth = w;
	bmpinfo.bmiHeader.biHeight = h;
	bmpinfo.bmiHeader.biPlanes = 1;
	bmpinfo.bmiHeader.biBitCount = 8 * bytesPerPixel;
	bmpinfo.bmiHeader.biCompression = BI_RGB;
	bmpinfo.bmiHeader.biSizeImage = w * h * bytesPerPixel;
	bmpinfo.bmiHeader.biXPelsPerMeter = 0;
	bmpinfo.bmiHeader.biClrImportant = 0;
	bmpinfo.bmiHeader.biClrUsed = 0;
	
	hBmp = CreateDIBSection(hdcMem, &bmpinfo, DIB_RGB_COLORS, (void**)&pDibs, NULL, 0);
	//for (i = 0; i < w * h * bytesPerPixel; i++) {
	//	pDibs[i] = buffer[i];
	//}
	memcpy(pDibs, buffer, w * h * bytesPerPixel);
	hOldSel = SelectObject(hdcMem,hBmp);

	//see TEST_DRAW_DIR
	//BitBlt(hdc, 0, 0, w, h, hdcMem, 0, 0, SRCCOPY);
	StretchBlt(hdc, 0, 0, w, h, hdcMem, 0, h, w, -h, SRCCOPY); //up side down
	
	SelectObject(hdcMem, hOldSel);    
	DeleteDC(hdcMem); 
	ReleaseDC(m_hWnd, hdc);
}

MSD_Surface *surf_test = NULL;
void test_init()
{
	surf_test = MSD_LoadBMP("bs.bmp");
	//MSD_SaveBMP(surf, "tmp.bmp");
	//MSD_FreeSurface(surf);
}

void test_load()
{
	if (surf_test != NULL) {
		SDL_DrawMemoryBitmap(__sdl_hWnd, surf_test->w, surf_test->h, surf_test->pixels);
	}
}

void SDL_XXX()
{
	InvalidateRect(__sdl_hWnd, NULL, TRUE);
}

static int save_time = 0;
void SDL_savebmp(MSD_Surface *surf, const char *name)
{
#if 0
	char filename[256] = {0};
	sprintf(filename, "%s-%d.bmp", name, save_time++);
	MSD_SaveBMP(surf, filename);
#endif
}
