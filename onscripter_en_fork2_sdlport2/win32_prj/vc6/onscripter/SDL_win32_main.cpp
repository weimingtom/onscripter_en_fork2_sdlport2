#include <stdio.h>
#include <stdlib.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "SDL.h"
#include "SDL_main.h"
#ifdef main
#undef main
#endif

#if defined(_MSC_VER) && !defined(_WIN32_WCE)
#define console_main main
#endif

int console_main(int argc, char *argv[])
{
	SDL_main(argc, argv);
	return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int sw)
{
	console_main(__argc, __argv);
	return 0;
}
