//This implementation is from (with some modifications):
//https://github.com/rururutan/np2s/blob/master/np2/sdl/win32s/SDL_ttf.h
#pragma once

#include <SDL.h>

#define	TTF_Font	void

#ifdef __cplusplus
extern "C" {
#endif

int TTF_Init(void);
TTF_Font *TTF_OpenFont(const char *file, int ptsize);
void TTF_CloseFont(TTF_Font *font);
SDL_Surface *TTF_RenderUNICODE_Solid(TTF_Font *font, const Uint16 *text,
															SDL_Color fg);

extern int TTF_GlyphMetrics(TTF_Font *font, Uint16 ch, int *minx, int *maxx, int *miny, int *maxy, int *advance);
extern int TTF_FontAscent(const TTF_Font *font);
extern SDL_Surface *TTF_RenderGlyph_Shaded(TTF_Font *font, Uint16 ch, SDL_Color fg, SDL_Color bg);

#ifdef __cplusplus
}
#endif

