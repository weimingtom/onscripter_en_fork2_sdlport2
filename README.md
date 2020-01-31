# onscripter_en_fork2_sdlport2

## Ref  
* http://onscripter.unclemion.com/onscripter/  
* http://onscripter.osdn.jp/onscripter.html  
* https://github.com/weimingtom/onscripter_en_fork2  
* (x) https://github.com/weimingtom/sdlport2  
* https://gitee.com/weimingtom/sdlport2  
* https://github.com/rururutan/np2s/tree/master/np2/sdl/win32s  

## Status  
* onscripter_en_fork2/  
	Mod from https://github.com/weimingtom/onscripter_en_fork2, sync to onscripter_en_fork2_sdlport2.  
	Use BeyondCompare to compare it with onscripter_en_fork2_sdlport2/  
	
* onscripter_en_fork2_sdlport2/  
	Use sdlport2 (My replacement of SDL 1.2) from https://gitee.com/weimingtom/sdlport2,  
	which is from https://github.com/rururutan/np2s/tree/master/np2/sdl/win32s,    
	Able to run normally now, with some problems (flashing black screen sometimes, maybe it is because USE_FULL_FLASH in function flushDirect).      
	see debug.txt, (1) SDL_savebmp, (2) MSD_SetAlpha  

## Plan  
* Check onscripter_en_fork2_sdlport2 running success completely.    
* Merge into https://github.com/weimingtom/onscripter_en_fork2 branches.  
* Remove timer in sdl.c of onscripter_en_fork2_sdlport2/, if possible. Maybe it is because USE_FULL_FLASH in function flushDirect.   

