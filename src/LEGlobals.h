
#ifndef LE_GLOBALS_H
#define LE_GLOBALS_H

#include "src/iupdef.h"
#include "src/luadef.h"
#include "src/lthreads.h"
#include <stdatomic.h>

// Global variables of LuaEngine
//////////////////////// VARIABLE INITIALIZATION \\\\\\\\\\\\\\\\\\\\\\

char key; int down;
Ihandle *zbox, *cnv, *cnv2, *dg, *upd; // Iup handles setup
const void* EmptyFunc;
char exet = 0; // variable to close Main Loop
lua_State *l, *ll; 
lua_State  *nl;// main, thread and loading stack pointers
/* Frametime and fps*/ 
Ihandle *fpst;
int fpslmt = 60, ftlim; 
short frames = 1;
double ft = 0.01, fps = 60;

const struct {
  char* title;
  char* size;
}LEfault = {(char*)"LuaEngine", (char*)"640x320"};

struct {
  char* title;
  char* size;
  int width, height;
  int sceneindex;
}LE;


      /*main stack positions*/
const char numArgs = 6;

enum stackValue{
  ScEnum = 1,
  CurScene,
  Controls,
  Keys,
  Thread,
  Default
};

Pthread Loading; 
enum LoadState{
  NEW_LOAD = 0,
  LOADING_SCENE = 2,
  END_LOAD = 3,
  RESUME_LOAD = -1,
  NO_LOAD = -3
};

_Atomic int loadScene = NO_LOAD;



#endif