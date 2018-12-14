#pragma once

#include	"SDL_common.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
	SDL_RELEASED	= 0,
	SDL_PRESSED		= 1
};

enum {
	SDL_NOEVENT = 0,
	SDL_ACTIVEEVENT, //FIXME:not implemented
	SDL_KEYDOWN,
	SDL_KEYUP,
	SDL_MOUSEMOTION,
	SDL_MOUSEBUTTONDOWN,
	SDL_MOUSEBUTTONUP,
	SDL_JOYAXISMOTION, //FIXME:not implemented
	SDL_JOYBALLMOTION, //FIXME:not implemented
	SDL_JOYHATMOTION, //FIXME:not implemented
	SDL_JOYBUTTONDOWN, //FIXME:not implemented
	SDL_JOYBUTTONUP, //FIXME:not implemented
	SDL_QUIT,
//	SDL_SYSWMEVENT,
//	SDL_EVENT_RESERVEDA,
//	SDL_EVENT_RESERVEDB,
//	SDL_VIDEORESIZE,
	SDL_VIDEOEXPOSE, //FIXME:not implemented
//	SDL_EVENT_RESERVED2,
//	SDL_EVENT_RESERVED3,
//	SDL_EVENT_RESERVED4,
//	SDL_EVENT_RESERVED5,
//	SDL_EVENT_RESERVED6,
//	SDL_EVENT_RESERVED7,
	SDL_USEREVENT = 24,
	SDL_NUMEVENTS = 32
};

typedef enum {
	SDLK_UNKNOWN		= 0,
	SDLK_FIRST			= 0,
	SDLK_BACKSPACE		= 8,
	SDLK_TAB			= 9,
	SDLK_CLEAR			= 12,
	SDLK_RETURN			= 13,
	SDLK_PAUSE			= 19,
	SDLK_ESCAPE			= 27,
	SDLK_SPACE			= 32,
	SDLK_EXCLAIM		= 33,
	SDLK_QUOTEDBL		= 34,
	SDLK_HASH			= 35,
	SDLK_DOLLAR			= 36,
	SDLK_AMPERSAND		= 38,
	SDLK_QUOTE			= 39,
	SDLK_LEFTPAREN		= 40,
	SDLK_RIGHTPAREN		= 41,
	SDLK_ASTERISK		= 42,
	SDLK_PLUS			= 43,
	SDLK_COMMA			= 44,
	SDLK_MINUS			= 45,
	SDLK_PERIOD			= 46,
	SDLK_SLASH			= 47,
	SDLK_0				= 48,
	SDLK_1				= 49,
	SDLK_2				= 50,
	SDLK_3				= 51,
	SDLK_4				= 52,
	SDLK_5				= 53,
	SDLK_6				= 54,
	SDLK_7				= 55,
	SDLK_8				= 56,
	SDLK_9				= 57,
	SDLK_COLON			= 58,
	SDLK_SEMICOLON		= 59,
	SDLK_LESS			= 60,
	SDLK_EQUALS			= 61,
	SDLK_GREATER		= 62,
	SDLK_QUESTION		= 63,
	SDLK_AT				= 64,
	// Skip uppercase letters
	SDLK_LEFTBRACKET	= 91,
	SDLK_BACKSLASH		= 92,
	SDLK_RIGHTBRACKET	= 93,
	SDLK_CARET			= 94,
	SDLK_UNDERSCORE		= 95,
	SDLK_BACKQUOTE		= 96,
	SDLK_a				= 97,
	SDLK_b				= 98,
	SDLK_c				= 99,
	SDLK_d				= 100,
	SDLK_e				= 101,
	SDLK_f				= 102,
	SDLK_g				= 103,
	SDLK_h				= 104,
	SDLK_i				= 105,
	SDLK_j				= 106,
	SDLK_k				= 107,
	SDLK_l				= 108,
	SDLK_m				= 109,
	SDLK_n				= 110,
	SDLK_o				= 111,
	SDLK_p				= 112,
	SDLK_q				= 113,
	SDLK_r				= 114,
	SDLK_s				= 115,
	SDLK_t				= 116,
	SDLK_u				= 117,
	SDLK_v				= 118,
	SDLK_w				= 119,
	SDLK_x				= 120,
	SDLK_y				= 121,
	SDLK_z				= 122,
	SDLK_DELETE			= 127,

	// Numeric keypad
	SDLK_KP0			= 256,
	SDLK_KP1			= 257,
	SDLK_KP2			= 258,
	SDLK_KP3			= 259,
	SDLK_KP4			= 260,
	SDLK_KP5			= 261,
	SDLK_KP6			= 262,
	SDLK_KP7			= 263,
	SDLK_KP8			= 264,
	SDLK_KP9			= 265,
	SDLK_KP_PERIOD		= 266,
	SDLK_KP_DIVIDE		= 267,
	SDLK_KP_MULTIPLY	= 268,
	SDLK_KP_MINUS		= 269,
	SDLK_KP_PLUS		= 270,
	SDLK_KP_ENTER		= 271,
	SDLK_KP_EQUALS		= 272,

	// Arrows + Home/End pad
	SDLK_UP				= 273,
	SDLK_DOWN			= 274,
	SDLK_RIGHT			= 275,
	SDLK_LEFT			= 276,
	SDLK_INSERT			= 277,
	SDLK_HOME			= 278,
	SDLK_END			= 279,
	SDLK_PAGEUP			= 280,
	SDLK_PAGEDOWN		= 281,

	// Function keys
	SDLK_F1				= 282,
	SDLK_F2				= 283,
	SDLK_F3				= 284,
	SDLK_F4				= 285,
	SDLK_F5				= 286,
	SDLK_F6				= 287,
	SDLK_F7				= 288,
	SDLK_F8				= 289,
	SDLK_F9				= 290,
	SDLK_F10			= 291,
	SDLK_F11			= 292,
	SDLK_F12			= 293,
	SDLK_F13			= 294,
	SDLK_F14			= 295,
	SDLK_F15			= 296,

	// Key state modifier keys
	SDLK_NUMLOCK		= 300,
	SDLK_CAPSLOCK		= 301,
	SDLK_SCROLLOCK		= 302,
	SDLK_RSHIFT			= 303,
	SDLK_LSHIFT			= 304,
	SDLK_RCTRL			= 305,
	SDLK_LCTRL			= 306,
	SDLK_RALT			= 307,
	SDLK_LALT			= 308,
	SDLK_RMETA			= 309,
	SDLK_LMETA			= 310,
	SDLK_LSUPER			= 311,
	SDLK_RSUPER			= 312,
	SDLK_MODE			= 313,
	SDLK_COMPOSE		= 314,

	// Miscellaneous function keys
	SDLK_HELP			= 315,
	SDLK_PRINT			= 316,
	SDLK_SYSREQ			= 317,
	SDLK_BREAK			= 318,
	SDLK_MENU			= 319,
	SDLK_POWER			= 320,
	SDLK_EURO			= 321,
	SDLK_UNDO			= 322,
	SDLK_LAST
} SDLKey;

typedef struct {
//	BYTE	scancode;
	SDLKey	sym;
//	SDLMod	mod;
	Uint16	unicode; //FIXME: not implemented
} SDL_keysym;

typedef struct {
	BYTE		type;
//	BYTE		which;
//	BYTE		state;
	SDL_keysym	keysym;
} SDL_KeyboardEvent;

enum {
	SDL_BUTTON_LEFT			= 1,
//	SDL_BUTTON_MIDDLE		= 2,
	SDL_BUTTON_RIGHT		= 3,
//	SDL_BUTTON_WHEELUP		= 4,
//	SDL_BUTTON_WHEELDOWN	= 5,
};

typedef struct {
	BYTE	type;
//	BYTE	which;
	BYTE	state;
	WORD	x;
	WORD	y;
	short	xrel;
	short	yrel;
} SDL_MouseMotionEvent;

typedef struct {
	BYTE	type;
//	BYTE	which;
	BYTE	button;
	BYTE	state;
	WORD	x;
	WORD	y;
} SDL_MouseButtonEvent;

//FIXME:not implemented
typedef struct SDL_UserEvent {
	Uint8 type;	/* SDL_USEREVENT through SDL_NUMEVENTS-1 */
	int code;	/* User defined event code */
	void *data1;	/* User defined data pointer */
	void *data2;	/* User defined data pointer */
} SDL_UserEvent;

//FIXME:not implemented
typedef struct SDL_JoyAxisEvent {
	Uint8 type;	/* SDL_JOYAXISMOTION */
	Uint8 which;	/* The joystick device index */
	Uint8 axis;	/* The joystick axis index */
	Sint16 value;	/* The axis value (range: -32768 to 32767) */
} SDL_JoyAxisEvent;

//FIXME:not implemented
typedef struct SDL_JoyButtonEvent {
	Uint8 type;	/* SDL_JOYBUTTONDOWN or SDL_JOYBUTTONUP */
	Uint8 which;	/* The joystick device index */
	Uint8 button;	/* The joystick button index */
	Uint8 state;	/* SDL_PRESSED or SDL_RELEASED */
} SDL_JoyButtonEvent;

//FIXME:not implemented
typedef struct SDL_ActiveEvent {
	Uint8 type;	/* SDL_ACTIVEEVENT */
	Uint8 gain;	/* Whether given states were gained or lost (1/0) */
	Uint8 state;	/* A mask of the focus states */
} SDL_ActiveEvent;

typedef union {
	BYTE					type;
	SDL_KeyboardEvent		key;
	SDL_MouseMotionEvent	motion;
	SDL_MouseButtonEvent	button;
	SDL_UserEvent user; //FIXME:not implemented
	SDL_JoyButtonEvent jbutton; //FIXME:not implemented
	SDL_JoyAxisEvent jaxis;  //FIXME:not implemented
	SDL_ActiveEvent active; //FIXME:not implemented
} SDL_Event;

int SDL_PushEvent(SDL_Event *event);
int SDL_PollEvent(SDL_Event *event);
BYTE SDL_GetMouseState(int *x, int *y);



















//dummy
typedef struct _SDL_TimerID {
	int __;
}*SDL_TimerID;

typedef Uint32 (*SDL_NewTimerCallback)(Uint32 interval, void *param);

extern SDL_TimerID SDL_AddTimer(Uint32 interval, SDL_NewTimerCallback callback, void *param);
extern SDL_bool SDL_RemoveTimer(SDL_TimerID t);

//	SDL_UserEvent user; //FIXME:not implemented
//	SDL_JoyButtonEvent jbutton; //FIXME:not implemented
//	SDL_JoyAxisEvent jaxis;  //FIXME:not implemented
//	SDL_ActiveEvent active; //FIXME:not implemented
//} SDL_Event;
//	Uint16	unicode; //FIXME: not implemented
//} SDL_keysym;

extern void SDL_Delay(Uint32 ms);
extern int SDL_EnableUNICODE(int enable);

















/* Predefined event masks */
#define SDL_EVENTMASK(X)	(1<<(X))
typedef enum {
	SDL_QUITMASK		= SDL_EVENTMASK(SDL_QUIT),
} SDL_EventMask ;
#define SDL_ALLEVENTS		0xFFFFFFFF

typedef enum {
	SDL_ADDEVENT,
	SDL_PEEKEVENT,
	SDL_GETEVENT
} SDL_eventaction;
extern int SDL_PeepEvents(SDL_Event *events, int numevents, SDL_eventaction action, Uint32 mask);
extern void SDL_PumpEvents(void);


#define SDL_BUTTON(X)		(1 << ((X)-1))
#define SDL_BUTTON_LEFT		1
#define SDL_BUTTON_MIDDLE	2
#define SDL_BUTTON_RIGHT	3
#define SDL_BUTTON_WHEELUP	4
#define SDL_BUTTON_WHEELDOWN	5
#define SDL_BUTTON_X1		6
#define SDL_BUTTON_X2		7
#define SDL_BUTTON_LMASK	SDL_BUTTON(SDL_BUTTON_LEFT)
#define SDL_BUTTON_MMASK	SDL_BUTTON(SDL_BUTTON_MIDDLE)
#define SDL_BUTTON_RMASK	SDL_BUTTON(SDL_BUTTON_RIGHT)
#define SDL_BUTTON_X1MASK	SDL_BUTTON(SDL_BUTTON_X1)
#define SDL_BUTTON_X2MASK	SDL_BUTTON(SDL_BUTTON_X2)



extern int SDL_WaitEvent(SDL_Event *event);

//enum {
//	SDL_ACTIVEEVENT, //FIXME:not implemented
//	SDL_JOYAXISMOTION, //FIXME:not implemented
//  SDL_JOYBALLMOTION, //FIXME:not implemented
//	SDL_JOYHATMOTION, //FIXME:not implemented
//	SDL_JOYBUTTONDOWN, //FIXME:not implemented
//	SDL_JOYBUTTONUP, //FIXME:not implemented
//	SDL_VIDEOEXPOSE, //FIXME:not implemented


#ifdef __cplusplus
}
#endif

