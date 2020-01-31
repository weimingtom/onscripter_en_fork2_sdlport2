#pragma once

#include	"SDL_common.h"
#include    "SDLvideo.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int __;
} TTF_Font;

extern int TTF_Init(void);
extern TTF_Font *TTF_OpenFont(const char *file, int ptsize);
extern SDL_Surface *TTF_RenderGlyph_Shaded(TTF_Font *font, Uint16 ch, SDL_Color fg, SDL_Color bg);
extern int TTF_GlyphMetrics(TTF_Font *font, Uint16 ch, int *minx, int *maxx, int *miny, int *maxy, int *advance);
extern int TTF_FontAscent(const TTF_Font *font);

#ifdef __cplusplus
}
#endif

