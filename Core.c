#include "luadef.h"
#ifdef WIN32
  #include <windows.h>
#endif

#include <GL/gl.h>


#include "iupdef.h"


#include <pthread.h>

#include <time.h>


#define T_args void* args

typedef void* TCallback;


typedef struct _pthread {
  pthread_t thread; 
  pthread_cond_t on; 
  pthread_mutex_t mute;
}Pthread;

void CreatePthread(Pthread *t, TCallback tfunc){
    
  pthread_create(&t->thread, NULL, tfunc, NULL);
  pthread_cond_init(&t->on, NULL);
  pthread_mutex_init(&t->mute, NULL);
}

void DestroyPthread(Pthread *t){
    
  pthread_detach(t->thread);
  pthread_cond_destroy(&t->on);
  pthread_mutex_destroy(&t->mute);
}




char key; int down;
Ihandle *zbox, *cnv, *cnv2, *dg, *upd; // Iup handles setup
const void* EmptyFunc;
char exet = 1; // variable to close Main Loop

/* Frametime and fps*/ 
Ihandle *fpst;
int fpslmt = 61, ftlim; 
short frames;
double ft = 0.01, fps = 0;


lua_State *l, *nl, *ll; // main, thread and loading stacks pointers
int sceneindex;

      /*main stack positions*/
enum stackValue{
  ScEnum = 1,
  CurScene,
  Controls,
  Keys,
  Thread,
  Default
};


const char numArgs = 6;



Pthread Loading; 
enum LoadState{
  NEW_LOAD = 0,
  LOADING_SCENE = 2,
  END_LOAD = 3,
  RESUME_LOAD = -1
};
int loadScene = NEW_LOAD;

TCallback loading_Scene(T_args)
{
    do{
      
      pthread_mutex_lock(&Loading.mute);
      pthread_cond_wait(&Loading.on, &Loading.mute);
      pthread_mutex_unlock(&Loading.mute);
      IupGLMakeCurrent(cnv2);
      if ( loadScene == LOADING_SCENE ){
        lua_settop(nl, 1);
        ll = lua_newthread(nl);

        lua_getglobal(nl, "require");
        lua_getidx(nl, ScEnum, sceneindex);
        lua_pushstring(nl, ".root");
        
        lua_concat(nl, 2);
        
        if (lua_pcall(nl, 1, 1, 0)){
          printf("%s %s %d %s\n",lua_tostring(nl, -1), __FILE__, __LINE__, "require");
          
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
    }while(1);
    
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
      printf("%s %s %d %s\n",lua_tostring(l, -1), __FILE__, __LINE__, "Load");
      getchar();
    }

    lua_pushvalue(l, CurScene);
  }
  lua_settop(l, numArgs);
  
  return 0;
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
  lua_settop(l, numArgs);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
}



int Key_press(Ihandle* self, int c, int press)
{
  
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
        printf("%s %s %d %s\n",lua_tostring(l, -1), __FILE__, __LINE__, "Command");
        getchar();
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
      printf("%s %s %d %s\n",lua_tostring(l, -1), __FILE__, __LINE__, "KeyRelease");
      getchar();
    }
  }  
  
  fin:
  lua_settop(l, numArgs);
  return 0;
}

int Motion_cb(Ihandle* self, int x, int y, char* status)
{
  
  lua_getvalue(l, CurScene, "Motion");
  if (lua_topointer(l, -1) != EmptyFunc){
    lua_pushinteger(l, x);
    lua_pushinteger(l, y);
    lua_pushstring(l, status);
    if (lua_pcall(l, 3, 0, 0)){
      printf("%s %s %d %s\n",lua_tostring(l, -1), __FILE__, __LINE__, "Motion");
      getchar();
    }
  }
  lua_settop(l, numArgs);
  return 0;
}

int Button_cb(Ihandle* ih, int button, int pressed, int x, int y, char* status)
{
  lua_getvalue(l, CurScene, "Button");
  if (lua_topointer(l, -1) != EmptyFunc){
    lua_pushinteger(l, x);
    lua_pushinteger(l, y);
    lua_pushstring(l, status);
    if (lua_pcall(l, 3, 0, 0)){
      printf("%s %s %d in function %s\n",lua_tostring(l, -1), __FILE__, __LINE__, "Button");
      getchar();
    }
  }
  lua_settop(l, numArgs);
  return 0;
}


int update_cb()
{   
    
    if (lua_getfield(l, CurScene, "KeyPress"))
    if (lua_topointer(l,-1) != EmptyFunc){
      lua_pushvalue(l, CurScene);
      lua_pushvalue(l, Keys);
      if (lua_pcall(l, 2, 0, 0)){
        printf("%s %s %d\n",lua_tostring(l, -1), __FILE__, __LINE__);
        getchar();
      }
    }

    if (lua_getfield(l, CurScene, "Update"))
    if (lua_topointer(l, -1) != EmptyFunc){
      lua_pushvalue(l, CurScene);
      if (lua_pcall(l, 1, 0, 0)){
        printf("%s %s %d\n",lua_tostring(l, -1), __FILE__, __LINE__);
        
        getchar();
      }
    }
    
  lua_settop(l, numArgs);
  return 0;
}

int Idle(Ihandle * self){
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
  return 0;
}

int draw(Ihandle *self, float x, float y)
{
   
  int err;

  IupGLMakeCurrent(cnv);
  
  glClearColor(0, 0, 0, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  lua_getvalue(l, CurScene, "Draw");
  if (lua_topointer(l, -1) != EmptyFunc){
    lua_pushvalue(l, CurScene);
    if (lua_pcall(l, 1, 0, 0)){
      printf("%s %s %d\n",lua_tostring(l, -1), __FILE__, __LINE__);
      getchar();
    }
  }
  
  
  frames++;
  IupGLSwapBuffers(cnv); 
  
  lua_settop(l, numArgs);
  return 0;
}

int Fps_time(Ihandle* self){
  
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

int Refocus(Ihandle *self){
  IupSetFocus(cnv);
  return 0;
}

int close_cb(Ihandle *self){
  DestroyPthread(&Loading);
  
  exet = 0;
  return 0;
}

LONGLONG CpuMs(){
    LARGE_INTEGER t,f;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&t);

    return (t.QuadPart*1000)/f.QuadPart;
}

int EngineBoot(int *argc, char ***argv)
{
  int ret; 
  
  
  ret = IupOpen(argc, argv);

  

  IupGLCanvasOpen();
  
  
  
  cnv = IupGLCanvas(NULL);
  
  IupSetFunction("IDLE_ACTION", (Icallback)Idle);
  IupSetHandle("cnvmain", cnv);
  
  IupSetAttribute(cnv, "BUFFER", "DOUBLE");
  IupSetAttribute(cnv, "RASTERSIZE", "640x480");

  IupSetCallbacks(cnv, "KILLFOCUS_CB", (Icallback)Refocus, 
                       "KEYPRESS_CB", (Icallback) Key_press,
                       "BUTTON_CB", (Icallback) Button_cb,
                       "MOTION_CB", (Icallback) Motion_cb,
                       "ACTION", (Icallback) draw,
                       "MAP_CB", (Icallback) map,
                       "RESIZE_CB", (Icallback) resize);
  
  cnv2 = IupGLCanvas(NULL);
  IupSetHandle("cnvthread", cnv2);
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

  // upd = IupTimer();
  
  // IupSetInt(upd, "TIME", 1000/fpslmt - 2 );
  // IupSetCallback(upd, "ACTION_CB", (Icallback) update_cb);
  // IupSetAttribute(upd, "RUN", "Yes");


  fpst = IupTimer();
  
  IupSetInt(fpst, "TIME", 1000);
  IupSetCallback(fpst, "ACTION_CB", (Icallback) Fps_time);
  IupSetAttribute(fpst, "RUN", "Yes");

  CreatePthread(&Loading, loading_Scene);
  
  
  ftlim = 1000/fpslmt;
  IupShow(dg);
  IupSetFocus(cnv);
  LONGLONG fpst_c = CpuMs();
  while (exet){
    
    
    if (CpuMs()-fpst_c >= ftlim){
      
      update_cb();
      IupRedraw(cnv, 0);
      fpst_c = CpuMs();
    }
    IupLoopStep();
  };
  
  
  IupDestroy(dg);
  IupClose();
  
  return 0;
}

int SetFpsLimit(lua_L){
  fpslmt = lua_tointeger(L,1);
  ftlim = 1000/fpslmt;
  IupSetInt(upd, "TIME", ftlim);
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
  
  return exet = 0;
}

int ChangeScene(lua_L){
  
  sceneindex = luaL_checkinteger(L, 1);

  loadScene = NEW_LOAD;
  return 0;
}

int ResumeLoading(lua_L){
  pthread_cond_signal(&Loading.on); // calling loading thread
  return 0;
}

int main(int argc, char **argv)
{
  
  l = luaL_newstate();
  luaL_openlibs(l);

  lua_register(l, "Lalloc", Lalloc);
  lua_register(l, "Close", Close);
  lua_register(l, "ChangeScene", ChangeScene);
  lua_register(l, "ResumeLoading", ResumeLoading);
  lua_register(l, "int", toInt);
  lua_register(l, "SetFpsLimit", SetFpsLimit);
  

  lua_require(l, "config");
  lua_getglobal(l, "startscene");
  sceneindex = luaL_checkinteger(l, -1);
  lua_pop(l,2);
  
  
  
  EngineBoot(&argc, &argv);

  lua_close(l);
  
  return 1;
}