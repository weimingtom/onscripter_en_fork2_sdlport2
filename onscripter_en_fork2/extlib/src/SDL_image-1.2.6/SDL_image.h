#pragma once

#include "SDL.h"

#ifdef __cplusplus
extern "C" {
#endif

extern SDL_Surface * IMG_LoadTyped_RW(SDL_RWops *src, int freesrc, char *type);
extern SDL_Surface * IMG_Load(const char *file);
extern SDL_Surface * IMG_Load_RW(SDL_RWops *src, int freesrc);
extern int IMG_isBMP(SDL_RWops *src);
extern SDL_Surface * IMG_LoadBMP_RW(SDL_RWops *src);

#define IMG_SetError	SDL_SetError
#define IMG_GetError	SDL_GetError

#ifdef __cplusplus
}
#endif
