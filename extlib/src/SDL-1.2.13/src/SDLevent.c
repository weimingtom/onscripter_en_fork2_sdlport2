//#include	"compiler.h"
#include	<windows.h>
#include	<stdio.h>
#include	<stddef.h>
#include	<SDL.h>

#define	EVENT_MAX	128

extern	BOOL	__sdl_avail;
extern	int		__sdl_mousex;
extern	int		__sdl_mousey;

static	SDL_Event	events[EVENT_MAX];
static	UINT		eventp = 0;
static	UINT		eventc = 0;


int SDL_PushEvent(SDL_Event *event) {
	if (eventc < (sizeof(events)/sizeof(SDL_Event))) {
		events[(eventp + eventc) % EVENT_MAX] = *event;
		eventc++;
		return(1);
	}
	else {
		return(0);
	}
}

static int __SDL_getEvent(SDL_Event *event, int isRemoved, int notRemovedIndex) {
	MSG		msg;

	if (__sdl_avail) {
		while(PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE)) {
			if (!GetMessage(&msg, NULL, 0, 0)) {
				SDL_Event quitevent;
				memset(&quitevent, 0, sizeof(quitevent));
				quitevent.type = SDL_QUIT;
				SDL_PushEvent(&quitevent);
				__sdl_avail = FALSE;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	if (eventc) {
		if (isRemoved) {
			eventc--;
			if (event) {
				*event = events[eventp];
			}
			eventp = (eventp + 1) % EVENT_MAX;
			return(1);
		} else {
			if (notRemovedIndex >= 0 && notRemovedIndex < (int)eventc) {
				*event = events[(eventp + notRemovedIndex) % EVENT_MAX];
				return(1);
			} else {
				//overflow
				//assert(0);
				return 0;
			}
		}
	}
	else {
		return(0);
	}
}

int SDL_PollEvent(SDL_Event *event) {
	return __SDL_getEvent(event, 1, 0);
}

//https://wiki.libsdl.org/SDL_PeepEvents
//FIXME:Returns the number of events actually stored or a negative error code on failure
int SDL_PeepEvents(SDL_Event *events, int numevents, SDL_eventaction action, Uint32 mask)
{
	//assert(0);

	if (action == SDL_PEEKEVENT) {
		//will not be removed from the queue
		if (mask == SDL_QUITMASK) {
			//TODO: not implemented
			return 0;
		} else {
			int used = 0;
			int i;
			for (i = 0; i < numevents; ++i) {
				int ret = __SDL_getEvent(&events[i], 0, i);
				if (ret == 0) break;
				used++;
			}
			return 0;
		}
	} else if (action == SDL_GETEVENT) {
		if (mask == SDL_ALLEVENTS) {
			//will be removed from the queue
			int used = 0;
			int i;
			for (i = 0; i < numevents; ++i) {
				int ret = SDL_PollEvent(&events[i]);
				if (ret == 0) break;
				used++;
			}
			return used;
		} else {
			assert(0);
			return 0;
		}
	}
	assert(0);
	return 0;
}

//???FIXME:
//if get event, return 1
int SDL_WaitEvent(SDL_Event *event)
{
	//assert(0);
	//return SDL_PollEvent(event);

	while ( 1 ) {
		SDL_PumpEvents();
		switch(SDL_PeepEvents(event, 1, SDL_GETEVENT, SDL_ALLEVENTS)) {
		    case -1: return 0;
		    case 1: return 1;
		    case 0: SDL_Delay(10);
		}
	}
	return 0;
}

//see sdl_peekevents
void SDL_PumpEvents(void)
{
	//assert(0);
}




BYTE SDL_GetMouseState(int *x, int *y) {

	if (x) {
		*x = __sdl_mousex;
	}
	if (y) {
		*y = __sdl_mousey;
	}
	return(0);
}

//
//http://wiki.libsdl.org/SDL_AddTimer
//
//SDL_TimerID SDL_AddTimer(Uint32            interval,
//                         SDL_TimerCallback callback,
//                         void*             param)
//
//interval: the timer delay (ms) passed to callback
//callback: the function to call when the specified interval elapses; see Remarks for details
//param: a pointer that is passed to callback
//
//Returns a timer ID or 0 if an error occurs; call SDL_GetError() for more information.
//
#define TIMER_MAX 125
static SDL_TimerID timers[TIMER_MAX] = {NULL};
void _SDL_Timer_init(void)
{
	int i;
	for (i = 0; i < TIMER_MAX; ++i) {
		timers[i] = NULL;
	}
}
void CALLBACK SDL_AddTimer_callback(HWND hWnd,UINT nMsg,UINT nTimerid,DWORD dwTime)
{
	int i;
	int idx;
	SDL_TimerID id = NULL;

	for (i = 0; i < TIMER_MAX; ++i) {
		if (timers[i] != NULL && timers[i]->idTimer == nTimerid) {
			idx = i;
			id = timers[i];
			break;
		}
	}

	if (id != NULL) {
		if (id->callback != NULL) {
			OutputDebugString("-------SDL_AddTimer_callback\n");
			(id->callback)(id->interval, id->param);
		} else {
			assert(0);
		}
	} else {
		assert(0);
	}
}
SDL_TimerID SDL_AddTimer(Uint32 interval, SDL_NewTimerCallback callback, void *param)
{
	//TODO:
	//assert(0);
	
	int idx = -1;
	int i;
	SDL_TimerID id = NULL;

	for (i = 0; i < TIMER_MAX; ++i) {
		if (timers[i] == NULL) {
			timers[i] = (SDL_TimerID)malloc(sizeof(struct _SDL_TimerID));
			id = timers[i];
			idx = i;
			break;
		}
	}
	if (id != NULL) {
		id->idTimer = SetTimer(NULL, 0, interval, SDL_AddTimer_callback);
		id->idx = idx;
		id->callback = callback;
		id->param = param;
		id->interval = interval;
	} else {
		assert(0); //timers size not enough
	}
	return id;
}

//https://wiki.libsdl.org/SDL_RemoveTimer
//FIXME:return value not used
SDL_bool SDL_RemoveTimer(SDL_TimerID t)
{
	//assert(0);
	if (t != NULL) {
		if (t->idx >= 0 && t->idx < TIMER_MAX) {
			OutputDebugString("-------SDL_RemoveTimer\n");
			KillTimer(NULL, timers[t->idx]->idTimer);
			timers[t->idx] = NULL;
			free(t);
			return SDL_TRUE;
		}
	}
	assert(0); //impossible
	return SDL_FALSE;
}

void SDL_Delay(Uint32 ms)
{
	//assert(0);
	Sleep(ms);
}

int SDL_EnableUNICODE(int enable)
{
	//assert(0);
	return 0;
}
