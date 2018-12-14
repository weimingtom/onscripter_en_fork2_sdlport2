#pragma once

#include    "SDL_common.h"
#include	"SDLvideo.h"

#ifdef __cplusplus
extern "C" {
#endif

extern SDL_Surface * IMG_Load_RW(SDL_RWops *src, int freesrc);
extern const char * IMG_GetError();
extern SDL_Surface * IMG_Load(const char *file);



//------------------
//internal api
extern SDL_Surface * IMG_LoadTyped_RW(SDL_RWops *src, int freesrc, char *type);
//extern int IMG_isBMP(SDL_RWops *src);
extern SDL_Surface * IMG_LoadBMP_RW(SDL_RWops *src);
extern void IMG_SetError(const char *str);

//#define IMG_SetError	SDL_SetError
//#define IMG_GetError	SDL_GetError
//------------------

#ifdef __cplusplus
}
#endif
