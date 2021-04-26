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


#define T_args void* args

#define CreatePthread(t, tfunc)\
    pthread_create(&t.thread, NULL, tfunc, NULL);\
    pthread_cond_init(&t.on, NULL);\
    pthread_mutex_init(&t.mute, NULL)

#define DestroyPthread(t)\
    pthread_detach(t.thread);\
    pthread_cond_destroy(&t.on);\
    pthread_mutex_destroy(&t.mute)


typedef struct _pthread {
  pthread_t thread; 
  pthread_cond_t on; 
  pthread_mutex_t mute;
}Pthread;

typedef void* TCallback;




char key;
Ihandle *zbox, *cnv, *cnv2, *dg, *upd, *fpst;
lua_State *l, *nl, *ll;
int sceneindex;
const void* EmptyFunc;


int loadScene = NEW_LOAD;
int fpslmt = 68;

      /* Frametime and fps */ 
unsigned long ptime = 0, ctime = 0;
unsigned long  ptime2 = 0, ctime2 = 0, frames = 0;
double ft = 0, fps = 0;


Pthread Loading;

TCallback loading_Scene(T_args)
{
    while(1){
      
      pthread_mutex_lock(&Loading.mute);
      pthread_cond_wait(&Loading.on, &Loading.mute);
      pthread_mutex_unlock(&Loading.mute);
      IupGLMakeCurrent(cnv2);
      if ( loadScene == LOADING_SCENE ){
        lua_cleanargs(nl, 1);
        ll = lua_newthread(nl);

        lua_getglobal(nl, "require");
        lua_getidx(nl, ScEnum, sceneindex);
        lua_pushstring(nl, ".root");
        
        lua_concat(nl, 2);
        
        if (lua_pcall(nl, 1, 1, 0)){
          printf("%s %s %d\n",lua_tostring(nl, -1), __FILE__, __LINE__);
          
          getchar();
        }
      }
      
      
      lua_getvalue(nl, -1, "Load");
      if (lua_topointer(nl, -1) != EmptyFunc){
        lua_pushvalue(nl, -2);
        lua_xmove(nl, ll, 2);
        
        lua_resume(ll, NULL, 1);
        
        if ( loadScene == LOADING_SCENE ){
          lua_pushvalue(nl, -1);
          loadScene = END_LOAD;
        }
      }
      
      wglMakeCurrent(NULL, NULL);
    }
    
  return 0;
}

int ChangeScene(lua_L){
  
  sceneindex = lua_tointeger(l, 1);

  loadScene = NEW_LOAD;

  

  return 0;
}

int ResumeLoading(lua_L){
  pthread_cond_signal(&Loading.on); // calling loading thread
  return 0;
}



int map(Ihandle* self)
{
  IupGLMakeCurrent(cnv);
  
  int err;

  lua_require(l, "modules.Engine");
  lua_getglobal(l, "EmptyFunc");
  EmptyFunc = lua_topointer(l, -1);
  lua_pop(l, 2);
  
  lua_require(l, "Scenes.SceneEnum");
  
  
  lua_require(l, "Scenes.default");

  

  lua_getglobal(l, "Controls");
  
  lua_createtable(l, 11, 11);
  lua_nameAtable(l, Keys, "Keys");

  nl = lua_newthread(l);

  lua_pushvalue(l, ScEnum);
  lua_xmove(l, nl, 1);
  
  lua_getvalue(l, CurScene, "Load");
  
  if (lua_topointer(l, -1) != EmptyFunc){
    lua_pushvalue(l, CurScene);
    if (lua_pcall(l, 1, 0, 0)){
      printf("%s %s %d\n",lua_tostring(l, -1), __FILE__, __LINE__);
      getchar();
    }

    lua_pushvalue(l, CurScene);
  }
  lua_cleanargs(l, numArgs);
  
  return IUP_DEFAULT;
}


int resize(Ihandle* self, int width, int height)
{

  glViewport(0, 0, width, height);
  
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  lua_getglobal(l, "_en");
  lua_getvalue(l, -1, "screen");
  lua_getvalue(l, -1, "w");
  lua_getvalue(l, -2, "h");
  
  
  glOrtho(0, lua_tointeger(l, -2), lua_tointeger(l, -1), 0, 1,0);
  lua_cleanargs(l, numArgs);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
}

int down;

int Key_press(Ihandle* self, int c, int press)
{
  int err;
  
  (c == 0x1000FFE1 || c == 0x1000FFE2)? c -= 0x10000000: c;
  down = press;
  if (c >= ' ' && c <= '~'){
    (c >= 'A' && c <= 'Z') ? c +=32: c;
    (press)? lua_pushinteger(l,press) : lua_pushboolean(l,press);
    lua_seti(l, Keys, c);
    
  }

  if (press){
    lua_getvalue(l, Controls, "Command");
    
    if (lua_topointer(l, -1) != EmptyFunc){
      lua_pushinteger(l, c);
      if (lua_pcall(l, 1, 0, 0)){
        printf("%s %s %d\n",lua_tostring(l, -1), __FILE__, __LINE__);
        getchar();
      }
    }  
    
  }else{
    lua_pushboolean(l, 0);
    lua_seti(l, Keys, c);
    lua_getvalue(l, CurScene, "KeyRelease");
    if (lua_topointer(l, -1) != EmptyFunc){
      lua_pushvalue(l, CurScene);
      lua_pushinteger(l, c);
      if (lua_pcall(l, 2, 0, 0)){
        printf("%s %s %d\n",lua_tostring(l, -1), __FILE__, __LINE__);
        getchar();
      }
    }  
    
  }
  
  lua_cleanargs(l, numArgs);
  return IUP_DEFAULT;
}

int Motion_cb(Ihandle* self, int x, int y, char* status)
{
  
  lua_getvalue(l, CurScene, "Motion");
  if (lua_topointer(l, -1) != EmptyFunc){
    lua_pushinteger(l, x);
    lua_pushinteger(l, y);
    lua_pushstring(l, status);
    if (lua_pcall(l, 3, 0, 0)){
      printf("%s %s %d\n",lua_tostring(l, -1), __FILE__, __LINE__);
      getchar();
    }
  }
  return IUP_DEFAULT;
}

int Button_cb(Ihandle* ih, int button, int pressed, int x, int y, char* status)
{

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
  switch(loadScene){ 

    case NEW_LOAD: {
      lua_pushstring(l, "default");
      lua_getvalue(l, CurScene, "name");
      if(!lua_compare(l, -2, -1, LUA_OPEQ)){
        lua_pushvalue(l, Default);
        lua_replace(l, CurScene);
      }
      loadScene = LOADING_SCENE;
      pthread_cond_signal(&Loading.on); // calling loading_Scene
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

  
  ctime = GetTickCount();
  
  if (ctime - ptime >= 1000/fpslmt){
    lua_getvalue(l, CurScene, "KeyPress");
    if (lua_topointer(l,-1) != EmptyFunc){
      lua_pushvalue(l, CurScene);
      lua_pushvalue(l, Keys);
      if (lua_pcall(l, 2, 0, 0)){
        printf("%s %s %d\n",lua_tostring(l, -1), __FILE__, __LINE__);
        getchar();
      }
    }

    lua_getvalue(l, CurScene, "Update");
    if (lua_topointer(l, -1) != EmptyFunc){
      lua_pushvalue(l, CurScene);
      lua_pushnumber(l, DT);
      if (lua_pcall(l, 2, 0, 0)){
        printf("%s %s %d\n",lua_tostring(l, -1), __FILE__, __LINE__);
        
        getchar();
      }
    }
    
    ptime = ctime;
    lua_cleanargs(l, numArgs);
  }
  
  
  
  IupUpdate(cnv);
  
  
  return IUP_DEFAULT;
}



int redraw(Ihandle *self, float x, float y)
{
   
  int err;
  
  
  IupGLMakeCurrent(cnv);
  
  glClearColor(0, 0, 0, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  frames++;
  lua_getvalue(l, CurScene, "Draw");
  if (lua_topointer(l, -1) != EmptyFunc){
    lua_pushvalue(l, CurScene);
    if (lua_pcall(l, 1, 0, 0)){
      printf("%s %s %d\n",lua_tostring(l, -1), __FILE__, __LINE__);
      getchar();
    }
  }
  IupGLWait(1);
  
  

  ctime2 = GetTickCount();
  if (ctime2 - ptime2 >= 1000){
    ft = 1000.0/frames;
    fps = (double)frames;
    
    frames = 0;

    lua_pushnumber(l, fps);
    lua_setglobal(l, "fps");
    lua_pushnumber(l, ft);
    lua_setglobal(l, "ft");
    ptime2 = ctime2;
  }

  lua_cleanargs(l, numArgs);
  IupGLSwapBuffers(cnv); 
  
  
  return IUP_DEFAULT;
}


int close_cb(Ihandle *self){
  DestroyPthread(Loading);
  return IUP_DEFAULT;
}



int EngineBoot(int *argc, char ***argv)
{
  int ret; 
  
  
  ret = IupOpen(argc, argv);

  ptime = GetTickCount();
  ptime2 = GetTickCount();

  IupGLCanvasOpen();
  
  
  
  cnv = IupGLCanvas(NULL);
  
  IupSetHandle("cnvmain", cnv);
  
  IupSetAttribute(cnv, IUP_BUFFER, IUP_DOUBLE);
  IupSetAttribute(cnv, "RASTERSIZE", "640x480");
  
  
  IupSetCallback(cnv, "KEYPRESS_CB", (Icallback) Key_press);
  IupSetCallback(cnv, "BUTTON_CB", (Icallback) Button_cb);
  IupSetCallback(cnv, "MOTION_CB", (Icallback) Motion_cb);
  IupSetCallback(cnv, "ACTION", (Icallback) redraw);
  IupSetCallback(cnv, "MAP_CB", (Icallback) map);
  IupSetCallback(cnv, "RESIZE_CB", (Icallback) resize);
  
  
  cnv2 = IupGLCanvas(NULL);
  IupSetHandle("cnvthread", cnv2);
  // IupSetAttribute(cnv2, "VISIBLE", "NO");
  IupSetAttribute(cnv2, "SHAREDCONTEXT", "cnvmain");
  
  zbox = IupZbox(cnv, cnv2);

  dg = IupDialog(zbox);
  IupSetAttribute(dg, "TITLE", "LuaEngine");
  IupSetAttribute(dg, "SIZE", "640x320");
  IupSetCallback(dg, "DESTROY_CB", (Icallback) close_cb);
  
  lua_pushnumber(l, fps);
  lua_setglobal(l, "fps");
  lua_pushnumber(l, ft);
  lua_setglobal(l, "ft");

  
  
  IupSetCallback(cnv, "SWAPBUFFERS_CB", (Icallback) update_cb);

  CreatePthread(Loading, loading_Scene);
  
  IupSetFocus(cnv);
  IupShow(dg);
  
  if (IupMainLoopLevel() == 0){
    IupMainLoop();
    IupClose();
  }
  
  
  return 0;
}





int Close(lua_L)
{
  IupExitLoop();
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
  
  
  
  EngineBoot(&argc, &argv);

  
  return 1;
}