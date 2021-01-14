#include "luadef.h"
#ifdef WIN32
  #include <windows.h>
#endif

#include <GL/gl.h>


#include "include/iup.h"
#include "include/iupgl.h"

#include <pthread.h>

 
#define SPEED 100
#define keynum 94
#define DT 0.016666666666667

#define Default 6
#define Thread 5
#define Keys 4
#define Controls 3
#define CurScene 2
#define ScEnum 1
#define numArgs 6

#define NEW_LOAD 0
#define LOADING_SCENE 2
#define END_LOAD 3
#define RESUME_LOAD -1

#define Th_args void* args

#define ThreadVars(t, m, c) pthread_t t; pthread_cond_t c; pthread_mutex_t m 

#define NewPThread(t, tfunc, m, c)\
    pthread_create(&t, NULL, tfunc, NULL);\
    pthread_cond_init(&c, NULL);\
    pthread_mutex_init(&m,NULL)

#define DestroyPThread(t, m, c)\
    pthread_detach(t);\
    pthread_cond_destroy(&c);\
    pthread_mutex_destroy(&m)


typedef int (*_func)();

typedef void* TCallback;


_func 
    lock = &pthread_mutex_lock, 
    unlock = &pthread_mutex_unlock,
    wait = &pthread_cond_wait, 
    sygnal = &pthread_cond_signal;

char key;
Ihandle *zbox, *cnv, *cnv2, *dg, *upd, *load;
lua_State *l, *nl, *ll;
int sceneindex;

lua_CFunction co_yield;

int loadScene = NEW_LOAD;

DWORD ngt = 0, sleep_time = 0, frames = 0;
double ft = 0, fps = 0;

ThreadVars(Loading, mute, on);

TCallback loading_Scene(Th_args)
{
    while(1){
      
      lock(&mute);
      wait(&on, &mute);
      
      IupGLMakeCurrent(cnv2);
      if ( loadScene == LOADING_SCENE ){
        lua_cleanargs(nl, 1);
        ll = lua_newthread(nl);

        lua_getglobal(nl, "require");
        lua_geti(nl, ScEnum, sceneindex);
        lua_pushstring(nl, ".root");
        
        lua_concat(nl, 2);
        
        if (lua_pcall(nl, 1, 1, 0)){
          printf("%s",lua_tostring(nl, -1));
          getchar();
        }
      }
      
      
      
      lua_getfield(nl, -1, "Load");
      lua_pushvalue(nl, -2);
      lua_xmove(nl, ll, 2);
      
      lua_resume(ll, NULL, 1);
      
      if ( loadScene == LOADING_SCENE ){
        lua_pushvalue(nl, -1);
        loadScene = END_LOAD;
      }
      
      unlock(&mute);
      
    }
    
  return 0;
}

int ChangeScene(lua_L){
  
  sceneindex = lua_tointeger(l, 1);

  loadScene = NEW_LOAD;

  
  
  return 0;
}

int ResumeLoading(lua_L){
  sygnal(&on); // calling loading thread
  return 0;
}



int map(Ihandle* self)
{
  IupGLMakeCurrent(cnv);
  
  int err;
  
  
  lua_require(l, "Scenes.SceneEnum");
  
  
  lua_require(l, "Scenes.default");

  lua_getglobal(l, "Controls");
  
  lua_createtable(l, 11, 11);
  lua_nameAtable(l, Keys, "Keys");

  nl = lua_newthread(l);

  lua_pushvalue(l, ScEnum);
  lua_xmove(l, nl, 1);
  
  lua_getfield(l, CurScene, "Load");
  lua_pushvalue(l, CurScene);
  
  if (lua_pcall(l, 1, 0, 0)){
    printf("%s",lua_tostring(l, -1));
    getchar();
  }

  lua_pushvalue(l, CurScene);

  lua_cleanargs(l, numArgs);
  
  IupGLSwapBuffers(cnv);
  return IUP_DEFAULT;
}


int resize(Ihandle* self, int width, int height)
{
  
  IupGLMakeCurrent(cnv);
  
  glViewport(0, 0, width, height);
  
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  lua_getglobal(l, "_en");
  lua_getfield(l, -1, "screen");
  lua_getfield(l, -1, "w");
  lua_getfield(l, -2, "h");
  
  
  glOrtho(0, lua_tointeger(l, -2), lua_tointeger(l, -1), 0, 1,0);
  lua_cleanargs(l, numArgs);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
}



int Key_press( int c, int press)
{
  int err;
  if (c >= ' ' && c <= '~'){
    lua_pushboolean(l, press);
    lua_seti(l, Keys, c);
  }
  

  if (press){
    lua_getfield(l, Controls, "Command");
    lua_pushinteger(l, c);
    if (lua_pcall(l, 1, 0, 0)){
      printf("%s",lua_tostring(l, -1));
      getchar();
    }
    lua_cleanargs(l, numArgs);
  }
  
  
  return IUP_DEFAULT;
}

int Motion_cb(int x, int y, char* status)
{
  int err;
  lua_getfield(l, CurScene, "Motion");
  lua_pushinteger(l, x);
  lua_pushinteger(l, y);
  lua_pushstring(l, status);
  if (lua_pcall(l, 3, 0, 0)){
    printf("%s",lua_tostring(l, -1));
    getchar();
  }
  return IUP_DEFAULT;
}


int Pause(lua_L)
{
  
  char * check = IupGetAttribute(upd, "RUN");
  IupSetAttribute(upd, "RUN", (check[0]=='Y')? "NO" : "YES");
  return 0;
}




int update_cb(Ihandle * self)
{
  
  lua_getfield(l, CurScene, "KeyPress");
  lua_pushvalue(l, CurScene);
  lua_pushvalue(l, Keys);
  if (lua_pcall(l, 2, 0, 0)){
    printf("%s",lua_tostring(l, -1));
    getchar();
  }
  

  lua_getfield(l, CurScene, "Update");
  lua_pushvalue(l, CurScene);
  lua_pushnumber(l, DT);
  if (lua_pcall(l, 2, 0, 0)){
    printf("%s",lua_tostring(l, -1));
    getchar();
  }
  
  switch(loadScene){ 

    case NEW_LOAD: {
      lua_pushstring(l, "default");
      lua_getfield(l, CurScene, "name");
      if(!lua_compare(l, -2, -1, LUA_OPEQ)){
        lua_pushvalue(l, Default);
        lua_replace(l, CurScene);
      }
      loadScene = LOADING_SCENE;
      sygnal(&on); // calling loading_Scene
      lua_pop(l, 2);
      break;
    }

    case END_LOAD:{
      lua_xmove(nl, l, 1);
      lua_replace(l, CurScene);
      
      loadScene = RESUME_LOAD;
      
      break;
    }

  }
  lua_cleanargs(l, numArgs);
  
  IupUpdate(cnv);
  return IUP_DEFAULT;
}





int redraw(Ihandle *self, float x, float y)
{
   
  int err;
  
  
  IupGLMakeCurrent(cnv);
  
  glClearColor(0, 0, 0, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3d(1.0,1.0,1.0);

  lua_getfield(l, CurScene, "Draw");
  lua_pushvalue(l, CurScene);
  if (lua_pcall(l, 1, 0, 0)){
    printf("%s \n",lua_tostring(l, -1));
    getchar();
  }
  
  
  lua_cleanargs(l, numArgs);

  frames++;
  sleep_time = GetTickCount();
  if (sleep_time - ngt >= 1000)
  {
    ft = 1000.0/frames;
    fps = (double)frames;
    
    frames = 0;
    ngt = sleep_time;
  }
  
  lua_pushnumber(l, fps);
  lua_setglobal(l,"fps");
  lua_pushnumber(l, ft);
  lua_setglobal(l,"ft");
  IupGLSwapBuffers(cnv); 
  
  return IUP_DEFAULT;
}

int destroy(Ihandle* self)
{
  
}

int EngineBoot(int *argc, char ***argv)
{
  int ret; 
  
  
  ret = IupOpen(argc, argv);

  ngt = GetTickCount();

  IupGLCanvasOpen();
  
  IupSetGlobal("INPUTCALLBACKS", "Yes");
  
  
  cnv = IupGLCanvas(NULL);
  IupSetHandle("cnvmain", cnv);
  
  IupSetAttribute(cnv, IUP_BUFFER, IUP_DOUBLE);
  IupSetAttribute(cnv, "RASTERSIZE", "640x480");
  IupSetFunction("GLOBALKEYPRESS_CB", (Icallback) Key_press);
  IupSetFunction("GLOBALMOTION_CB", (Icallback) Motion_cb);
  IupSetCallback(cnv, "ACTION", (Icallback) redraw);
  IupSetCallback(cnv, "CLOSE_CB", (Icallback) destroy);
  IupSetCallback(cnv, "MAP_CB", (Icallback) map);
  IupSetCallback(cnv, "RESIZE_CB", (Icallback) resize);
  IupMap(cnv);
  
  cnv2 = IupGLCanvas(NULL);
  IupSetHandle("cnvthread", cnv2);
  IupSetAttribute(cnv2, "VISIBLE", "NO");
  IupSetAttribute(cnv2, "RASTERSIZE", "640x480");
  IupSetAttribute(cnv2, "SHAREDCONTEXT", "cnvmain");
  
  zbox = IupZbox(cnv, cnv2);

  dg = IupDialog(zbox);
  IupSetAttribute(dg, "TITLE", "LuaEngine");
  IupSetAttribute(dg, "SIZE", "640x320");
  
  

  upd = IupTimer();
  IupSetAttribute(upd, "TIME", "10");
  IupSetAttribute(upd, "RUN", "YES");
  IupSetCallback(upd, "ACTION_CB", (Icallback) update_cb);

  NewPThread(Loading, loading_Scene, mute, on);
  
  
  IupShow(dg);
  IupLoopStep();
  if (IupMainLoopLevel() == 0){
    IupMainLoop();
    IupClose();
  }

  DestroyPThread(Loading, mute, on);
  
  return 0;
}





int Close(lua_L)
{
  IupClose();
  return 0;
}

int main(int argc, char **argv)
{
  
  l = luaL_newstate();
  luaL_openlibs(l);

  
  lua_register(l, "Pause", Pause);
  lua_register(l, "Close", Close);
  lua_register(l, "ChangeScene", ChangeScene);
  lua_register(l, "ResumeLoading", ResumeLoading);

  lua_require(l, "config");
  lua_getglobal(l, "startscene");
  sceneindex = lua_tointeger(l, -1);
  lua_pop(l,2);

  lua_require(l, "modules.Engine");
  lua_pop(l, 1);
  
  EngineBoot(&argc, &argv);

  
  return 1;
}