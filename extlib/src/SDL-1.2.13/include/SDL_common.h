#pragma once

//TODO:
//0:Test; 1:Normal embed
#define IS_LIB 1

#include <stdlib.h>
#include <windows.h>
#include <stdio.h>
#include <stddef.h>
#include <assert.h>
#include <MSD_video.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef	signed char	Sint8;
typedef	unsigned char Uint8;
typedef	signed short Sint16;
typedef	unsigned short Uint16;
typedef	signed int Sint32;
typedef	unsigned int Uint32;

typedef enum SDL_bool {
	SDL_FALSE = 0,
	SDL_TRUE  = 1
} SDL_bool;

typedef enum {
	SDL_ENOMEM,
	SDL_EFREAD,
	SDL_EFWRITE,
	SDL_EFSEEK,
	SDL_UNSUPPORTED,
	SDL_LASTERROR
} SDL_errorcode;

#ifdef __cplusplus
}
#endif

