#pragma once

#include	"SDL_common.h"
#include	"SDLevent.h"
#include	"SDLvideo.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	SDL_INIT_TIMER			= 0x00000001,
	SDL_INIT_AUDIO			= 0x00000010,
	SDL_INIT_VIDEO			= 0x00000020,
	SDL_INIT_EVERYTHING		= 0x0000FFFF
};

DWORD SDL_GetTicks(void);

extern int SDL_main(int argc, char **argv);

#if IS_LIB
#define main SDL_main
#endif

int SDL_InitSubSystem(DWORD flags);
void SDL_Quit(void);
const char *SDL_GetError(void);

//dummy
#define	SDL_INIT_TIMER 0x00000001
#define SDL_INIT_AUDIO 0x00000010
#define SDL_INIT_VIDEO 0x00000020
#define SDL_INIT_CDROM 0x00000100
#define SDL_INIT_JOYSTICK 0x00000200
#define SDL_INIT_NOPARACHUTE 0x00100000	/* Don't catch fatal signals */
#define SDL_INIT_EVENTTHREAD 0x01000000	/* Not supported on all OS's */
#define SDL_INIT_EVERYTHING	0x0000FFFF

extern int SDL_Init(Uint32 flags);

extern void SDL_DrawMemoryBitmap(HWND m_hWnd, int w, int h, BYTE *buffer);

extern void SDL_XXX();
extern void SDL_savebmp(MSD_Surface *surf, const char *name);

#ifdef __cplusplus
}
#endif

