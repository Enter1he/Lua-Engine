#include "luadef.c"

#include "lthreads.h"

#ifdef WIN32
  #include <windows.h>
#endif


#include "glew.h"
#include "iupdef.h"
#include <time.h>
#include <setjmp.h>
#include <signal.h>
#include <stdatomic.h>


//////////////////////// VARIABLE INITIALIZATION \\\\\\\\\\\\\\\\\\\\\\

char key; int down;
Ihandle *zbox, *cnv, *cnv2, *dg, *upd; // Iup handles setup
const void* EmptyFunc;
char exet = 0; // variable to close Main Loop
lua_State *l, *nl, *ll, *tl; // main, thread and loading stacks pointers

/* Frametime and fps*/ 
Ihandle *fpst;
int fpslmt = 60, ftlim; 
short frames;
double ft = 0.01, fps = 0;

jmp_buf errB; // jump buffer for errors
#define error_jump() longjmp(errB, 1);

#define error_message(sl,from) {\
  printf("%s %s %d %s\n",lua_tostring(sl, -1), __FILE__, __LINE__, from); \
  system("pause");\
  error_jump();\
}\



void LE_Signal(int code){
  switch(code){
    case SIGSEGV:{
      printf("SIGSEGV!\n");
      break;
    }
    case SIGABRT:{
      printf("SIGABRT!\n");
      break;
    }
    case SIGFPE:{
      printf("SIGFPE!\n");
      break;
    }
    case SIGILL:{
      printf("SIGILL!\n");
      break;
    }
    case SIGINT:{
      printf("SIGINT!\n");
      break;
    }
    case SIGTERM:{
      printf("SIGTERM!\n");
      break;
    }
  }
  
  IupDestroy(dg);
  IupClose();
  // lua_close(l);
}

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
volatile int loadScene = NO_LOAD;

int LE_idle_action(Ihandle * self);

//////////////////////// FUNCTION INITIALIZATION \\\\\\\\\\\\\\\\\\\\\\

int SetFpsLimit(lua_L){
  fpslmt = lua_tointeger(L,1);
  ftlim = 1000000/fpslmt;
  return 0;
}


int Lalloc(lua_L)
{
  lua_newuserdata(L, luaL_checkinteger(L, 1));
  return 1;
}

int toInt(lua_L){
  if (!lua_isinteger(L,1)){
    if(!lua_isnumber(L,1)) return 0;
    lua_pushinteger(L, (lua_Integer)lua_tonumber(L,1));
  }
  return 1;
}

int Close(lua_L)
{
  
  return exet = 1;
}

int ChangeScene(lua_L){
  
  int scene = luaL_checkinteger(L, 1);
  LE.sceneindex = (LE.sceneindex != scene) ? scene : (printf("Can't load same scene two times!\n"),LE.sceneindex);

  loadScene = NEW_LOAD;
  IupSetFunction("IDLE_ACTION", (Icallback)LE_idle_action);
  return 0;
}

int ResumeLoading(lua_L){
  int nres = 0;
  // loadScene = LOADING_SCENE;
  IupSetFunction("IDLE_ACTION", (Icallback)LE_idle_action);
  return 0;
}

int GetActualScreenSize(lua_L){
  lua_pushinteger(L, LE.width);
  lua_pushinteger(L, LE.height);
  return 2;
}

int toWH(lua_L){
  lua_settop(L,1);
  const char* str = luaL_checkstring(L,1);
  int w = 0;
  int x = 0;
  for (char* c = (char*)str; *c; c++){
    if (isdigit(*c)){
      x = x*10 + ((*c)-'0');
    }
    if (*c == 'x' || *c == 'X'){
      w = x;
      x = 0;
    }
  }
  lua_pushinteger(L, w);
  lua_pushinteger(L, x);
  return 2;
}



enum Mouse{
  M_left = 1,
  M_middle,
  M_right,
  M_button1 = 1,
  M_button2,
  M_button3,
  M_button4,
  M_button5,
  M_double,
  M_alt,
  M_shift,
  M_control
};

#define MouseCase(c) \
case M_##c:{\
      lua_pushboolean(L, iup_is##c (res));\
      break;\
    };

int isMouse(lua_L){
  lua_settop(L, 2);
  const char* res = luaL_checkstring(L,1);
  int s = luaL_checkinteger(L,2);
  switch(s){
    MouseCase(button1)
    MouseCase(button2)
    MouseCase(button3)
    MouseCase(button4)
    MouseCase(button5)
    MouseCase(double)
    MouseCase(alt)
    MouseCase(shift)
    MouseCase(control)
  }

  return 1;
}
#undef MouseCase


lua_Table LE_util[] ={
  
  {"Close", &Close},
  { "ChangeScene", &ChangeScene},
  { "GetActualScreenSize", &GetActualScreenSize},
  {"Lalloc", &Lalloc},
  { "ResumeLoading", &ResumeLoading},
  { "int", &toInt},
  { "SetFpsLimit", &SetFpsLimit},
  { "WxHtoints", &toWH},
  { "isMouse", &isMouse},
  lua_eoT
};


//////////////////////// CALLBACK INITIALIZATION \\\\\\\\\\\\\\\\\\\\\\

TCallback LE_loading_Thread(T_args)
{
  int nres;
    do{
      
      pthread_mutex_lock(&Loading.mute);
      pthread_cond_wait(&Loading.on, &Loading.mute);
      
      
      IupGLMakeCurrent(cnv2);
      
      IupGLMakeCurrent(cnv);
      pthread_mutex_unlock(&Loading.mute);
    }while(1);
    
  return 0;
}

int LE_idle_action(Ihandle * self){
  int nres = 0;
  switch(loadScene){ 

    case NEW_LOAD: {
      lua_pushstring(l, "default");
      lua_getvalue(l, CurScene, "name");
      if(!lua_compare(l, -2, -1, LUA_OPEQ)){
        lua_pushvalue(l, Default);
        lua_replace(l, CurScene);
      }
      lua_settop(nl, 1);
      ll = lua_newthread(nl);
      stackDump(nl);
      lua_getglobal(nl, "require");
      lua_getidx(nl, ScEnum, LE.sceneindex);
      lua_pushstring(nl, ".root");
      
      lua_concat(nl, 2);
      stackDump(nl);
      if (lua_pcall(nl, 1, 1, 0)){
        error_message(nl, "Scene Loader callback");
      }
      loadScene = LOADING_SCENE;
      
      lua_pop(l, 2);
    }
  case LOADING_SCENE: {
    
    lua_getvalue(nl, -1, "Load");
    if (lua_topointer(nl, -1) != EmptyFunc){
      lua_pushvalue(nl, -2);
      lua_xmove(nl, ll, 2);
      
      int err = lua_resume(ll, nl, 1, &nres);
      if (err != LUA_OK && err != LUA_YIELD){
        error_message(ll, "Error in parallel load!");
      }
      if (nres > 0){
        int isint = 0;
        lua_Integer res = lua_tointegerx(ll, -1, &isint);
        if (isint){
          if (res == 3){
            break;
          }
        }
      }
      
      lua_pushvalue(nl, -1);
      lua_xmove(nl, l, 1);
      lua_replace(l, CurScene);
      IupSetFunction("IDLE_ACTION", NULL);
    }
    break;
    }
  }
  return 0;
}

int LE_mapping_canvas(Ihandle* self)
{
  IupGLMakeCurrent(cnv);
  
  lua_require(l, "modules.Engine");
  lua_getglobal(l, "EmptyFunc");
  EmptyFunc = lua_topointer(l, -1);
  lua_pop(l, 2);

  
  
  lua_getglobal(l, "SceneEnum");
  
  if(lua_getglobal(l, "defaultScene")==LUA_TSTRING){
    lua_require(l, lua_tostring(l,-2));
    lua_remove(l, -2);
  }else{
    lua_pop(l,1);
    lua_getglobal(l, "_default");
  }


  lua_getglobal(l, "Controls");
  
  lua_createtable(l, 11, 11);
  lua_nameAtable(l, Keys, "Keys");
  
  nl = lua_newthread(l);
  lua_pushvalue(l, ScEnum);
  lua_xmove(l, nl, 1);
  loadScene = NEW_LOAD;
  IupSetFunction("IDLE_ACTION", (Icallback)LE_idle_action);
  

  lua_getvalue(l, CurScene, "Load");
  
  if (lua_topointer(l, -1) != EmptyFunc){
    lua_pushvalue(l, CurScene);
    
    if (lua_pcall(l, 1, 0, 0)){
      error_message(l,"scene Mapper callback");
    }
    lua_pushvalue(l, CurScene);
  }
  
  lua_settop(l, numArgs);
  
  return 0;
}

int LE_resizing_Canvas(Ihandle* self, int width, int height)
{
  LE.width = width, LE.height = height;
  
  lua_getglobal(l, "_en");
  lua_getvalue(l, -1, "screen");
  lua_getglobal(l, "WxHtoints");
  lua_getvalue(l, -2, "size");
  lua_pcall(l, 1, 2, 0);

  glViewport(0, 0, width, height);
  
  if(lua_getfield(l, CurScene, "Resize") == LUA_TFUNCTION)
    if (lua_topointer(l,-1) != EmptyFunc){
      lua_pushvalue(l, CurScene);
      lua_pushinteger(l, width);
      lua_pushinteger(l, height);
      lua_pcall(l, 3, 0, 0);
    }

  lua_settop(l, numArgs);
  
  
}

int LE_pressing_Keyboard(Ihandle* self, int c, int press)
{
  
  (c == 0x1000FFE1 || c == 0x1000FFE2)? c -= 0x10000000: c;
  down = press;
  if (c >= ' ' && c <= '~'){
    (c >= 'A' && c <= 'Z') ? c +=32: c;
    (press)? lua_pushinteger(l,press) : lua_pushboolean(l,press);
    lua_seti(l, Keys, c);
  }

  if (press){
    if(lua_getfield(l, Controls, "Command") == LUA_TFUNCTION)
    if (lua_topointer(l, -1) != EmptyFunc){
      lua_pushinteger(l, c);
      if (lua_pcall(l, 1, 0, 0)){
        error_message(l,"Scene Command callback");
      }
    }
    goto fin;
  }

  lua_pushboolean(l, 0);
  lua_seti(l, Keys, c);
  lua_getvalue(l, CurScene, "KeyRelease");
  if (lua_topointer(l, -1) != EmptyFunc){
    lua_pushvalue(l, CurScene);
    lua_pushinteger(l, c);
    if (lua_pcall(l, 2, 0, 0)){
      error_message(l,"Key Release callback");
    }
  }  
  fin:;
  lua_settop(l, numArgs);
  return 0;
}

int LE_moving_Mouse(Ihandle* self, int x, int y, char* status)
{
  
  lua_getvalue(l, CurScene, "Motion");
  if (lua_topointer(l, -1) != EmptyFunc){
    lua_pushinteger(l, x);
    lua_pushinteger(l, y);
    lua_pushstring(l, status);
    if (lua_pcall(l, 3, 0, 0)){
      error_message(l, "Scene Motion callback")
    }
  }
  lua_settop(l, numArgs);
  return 0;
}

int LE_clicking_Mouse(Ihandle* ih, int button, int pressed, int x, int y, char* status)
{
  if (pressed)
    if (lua_getfield(l, CurScene, "Button") == LUA_TFUNCTION)
      if (lua_topointer(l, -1) != EmptyFunc){
        lua_pushinteger(l, x);
        lua_pushinteger(l, y);
        lua_pushstring(l, status);
        if (lua_pcall(l, 3, 0, 0)){
          error_message(l, "Scene Button callback")
        }
      }
  lua_settop(l, numArgs);
  return 0;
}

int LE_updating_canvas()
{   
    
    if (lua_getfield(l, CurScene, "KeyPress"))
    if (lua_topointer(l,-1) != EmptyFunc){
      lua_pushvalue(l, CurScene);
      lua_pushvalue(l, Keys);
      if (lua_pcall(l, 2, 0, 0)){
        error_message(l, "Scene KeyPress callback")
      }
    }

    if (lua_getfield(l, CurScene, "Update"))
    if (lua_topointer(l, -1) != EmptyFunc){
      lua_pushvalue(l, CurScene);
      if (lua_pcall(l, 1, 0, 0)){
        error_message(l, "Scene Update callback")
      }
    }
    
  lua_settop(l, numArgs);
  return 0;
}



int LE_drawing_Canvas(Ihandle *self, float x, float y)
{
   
  int err;
  IupGLMakeCurrent(cnv);
  
  glClearColor(0, 0, 0, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  lua_getvalue(l, CurScene, "Draw");
  if (lua_topointer(l, -1) != EmptyFunc){
    lua_pushvalue(l, CurScene);
    if (lua_pcall(l, 1, 0, 0)){
      error_message(l, "Scene Draw callback");
    }
  }
  
  frames++;
  IupGLSwapBuffers(cnv); 
  
  lua_settop(l, numArgs);
  return 0;
}

int LE_counting_Fps(Ihandle* self){
  
  if (frames == 0) return 0;
  ft = 1000.0/frames;
  fps = (double)frames;
  frames = 0;

  lua_pushnumber(l, fps);
  lua_setglobal(l, "fps");
  lua_pushnumber(l, ft);
  lua_setglobal(l, "ft");

  lua_settop(l, numArgs);
  return 0;
}

int LE_returning_Focus(Ihandle *self){
  IupSetFocus(cnv);
  return 0;
}

int LE_closing_Window(Ihandle *self){
  
  printf("NOOOOOOOOOOOOOOO!");
  return exet = 1;
}

#ifdef WIN32
LONGLONG CpuMs(){
    LARGE_INTEGER t,f;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&t);

    return (t.QuadPart*1000000)/f.QuadPart;
}
#else
  typedef clock_t LONGLONG;
  LONGLONG (*CpuMs)() = &clock;
#endif

int LE_engine_Booting(int *argc, char ***argv)
{
  int ret = IupOpen(argc, argv);
  
  

  IupGLCanvasOpen();
  
  cnv = IupGLCanvas(NULL);

  if (!cnv) printf("MayDAY!/n");
  
  IupSetHandle("cnvmain", cnv);
  IupSetAttribute(cnv, "BUFFER", "DOUBLE");
  IupSetAttribute(cnv, "RASTERSIZE", LE.size);
  IupSetCallback(cnv, "KILLFOCUS_CB", (Icallback)LE_returning_Focus); 
  IupSetCallback(cnv,  "KEYPRESS_CB", (Icallback) LE_pressing_Keyboard);
  IupSetCallback(cnv,  "BUTTON_CB", (Icallback) LE_clicking_Mouse);
  IupSetCallback(cnv,  "MOTION_CB", (Icallback) LE_moving_Mouse);
  IupSetCallback(cnv,  "ACTION", (Icallback) LE_drawing_Canvas);
  IupSetCallback(cnv,  "MAP_CB", (Icallback) LE_mapping_canvas);
  IupSetCallback(cnv,  "RESIZE_CB", (Icallback) LE_resizing_Canvas);
  cnv2 = IupGLCanvas(NULL);
  IupSetHandle("cnvthread", cnv2);
  IupSetAttribute(cnv2, "SHAREDCONTEXT", "cnvmain");
  
  zbox = IupZbox(cnv, cnv2);
  
  dg = IupDialog(zbox);
  IupSetAttribute(dg, "TITLE", LE.title);
  IupSetAttribute(dg, "SIZE", LE.size);
  IupSetCallback(dg, "CLOSE_CB", (Icallback) LE_closing_Window);
  IupSetAttribute(dg, "RESIZE", "NO");
  if (lua_getglobal(l, "fullscreen"))
    IupSetAttribute(dg, "FULLSCREEN", "YES");
  lua_pop(l, 1);
  
  
  lua_pushnumber(l, fps);
  lua_setglobal(l, "fps");
  lua_pushnumber(l, ft);
  lua_setglobal(l, "ft");

  fpst = IupTimer();
  IupSetInt(fpst, "TIME", 1000);
  IupSetCallback(fpst, "ACTION_CB", (Icallback) LE_counting_Fps);
  IupSetAttribute(fpst, "RUN", "Yes");
  
  CreatePthread(&Loading, LE_loading_Thread);
  
  ftlim = 1000000/fpslmt;
  
  IupShow(dg);
  IupSetFocus(cnv);
  LONGLONG fpst_c = 1, dt = 1;

  exet = setjmp(errB);
  if (exet) { goto end;}


  fpst_c = CpuMs();
  
  if (lua_getglobal(l, "fixedframed")){
    while (exet == 0){
      dt = CpuMs()-fpst_c;
      if (dt >= ftlim){
        
        LE_updating_canvas();
        IupRedraw(cnv, 0);
        
        fpst_c = CpuMs();
      }
      
      IupLoopStep();
      lua_pushnumber(l, dt * 1e-06);
      lua_setglobal(l, "dt");
    }
    goto end;
  }
  while (exet == 0){
    fpst_c = CpuMs();
    
    LE_updating_canvas();
    IupRedraw(cnv, 0);
    
    dt = CpuMs()-fpst_c;
    
    
    lua_pushnumber(l, dt * 1e-06);
    lua_setglobal(l, "dt");
    IupLoopStep();
  }
  
<<<<<<< HEAD:src/Core.c
  end:;
  DestroyPthread(&Loading);
=======
>>>>>>> cfe6983909db0ad775bb5d75db05d8711523c0dc:Core.c
  IupDestroy(dg);
  IupClose();
  
  return 0;
}

//////////////////////// MAIN INITIALIZATION \\\\\\\\\\\\\\\\\\\\\\

int main(int argc, char **argv)
{
  // freopen("log.txt", "w", stdout);
  signal(SIGSEGV, LE_Signal);
  signal(SIGILL, LE_Signal);
  signal(SIGINT, LE_Signal);
  signal(SIGABRT, LE_Signal);
  signal(SIGTERM, LE_Signal);
  signal(SIGFPE, LE_Signal);

  l = luaL_newstate();
  luaL_openlibs(l);

  luaL_newlib(l, LE_util);
  lua_setglobal(l, "LE");
  stackDump(l);
  
  lua_require(l, "config");
  lua_getglobal(l, "startscene");
  LE.sceneindex = luaL_checkinteger(l, -1);
  lua_getglobal(l, "_en");
  lua_getfield(l, -1, "screen");
  
  if(lua_getfield(l, -1, "size")){
    LE.size = (char*)lua_tostring(l,-1);
  }else{
    LE.size = LEfault.size;
  }

  lua_pushcfunction(l, GetActualScreenSize);
  lua_setfield(l, -3, "asize");

  if (lua_getglobal(l, "luatitle")){
    LE.title = (char*)lua_tostring(l, -1);
  }else{
    LE.title = LEfault.title;
  }

  lua_settop(l,0);
  
  LE_engine_Booting(&argc, &argv);
  
  lua_close(l);
  

  return 1;
}
