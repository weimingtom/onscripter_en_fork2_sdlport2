#pragma once

#include    "SDL_common.h"
#include	"SDLvideo.h"

#ifdef __cplusplus
extern "C" {
#endif

extern SDL_Surface * IMG_Load_RW(SDL_RWops *src, int freesrc);
extern char * IMG_GetError(void);
extern SDL_Surface * IMG_Load(const char *file);

#ifdef __cplusplus
}
#endif
