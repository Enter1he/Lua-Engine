// Core.c written with glfw3 for emscripten

#include <emscripten.h>

#define GLFW_INCLUDE_ES3 
#include <GLFW/glfw3.h>

#include <ctype.h>

#include "src/Graphics/Graphics.h"
#include "src/Audio.h"
#include "src/Collision.h"


#include <setjmp.h>


#include "src/luadef.h"

 //                      GLOBALS
char key; int down;

const void* EmptyFunc;
char exet = 0; // variable to close Main Loop
lua_State *l, *nl, *ll; // main, thread and loading stacks pointers

GLFWwindow *w; // window handle

/* Frametime and fps*/ 
#define FPS_CAP 60
int fpslmt = FPS_CAP, ftlim = FPS_CAP; 
unsigned int frames = 0;
double ft = 0.01;
unsigned int fps = FPS_CAP;


jmp_buf errB; // jump buffer for errors
#define error_jump() longjmp(errB, 1);
#define error_message(sl,from) {\
  printf("%s %s %d %s\n",lua_tostring(sl, -1), __FILE__, __LINE__, from); \
  system("pause");\
  error_jump();\
}\

const struct {
  char* title;
  char* size;
  int width, height;
}LEfault = {(char*)"LuaEngine", (char*)"640x320", 800, 600};

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


enum LoadState{
  NEW_LOAD = 0,
  RELOAD_SCENE = 1,
  LOADING_SCENE = 2,
  END_LOAD = 3,
  RESUME_LOAD = -1,
  NO_LOAD = -3
};
volatile int loadScene = NO_LOAD;

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
  if (LE.sceneindex != scene) {
    LE.sceneindex = scene;
  }else{
    printf("Can't load same scene two times! old Scene %d New Scene %d\n", LE.sceneindex, scene);
    return 0;
  }
  loadScene = NEW_LOAD;
  return 0;
}

int ResumeLoading(lua_L){
  int nres = 0;
  loadScene = LOADING_SCENE;
  return 0;
}

int GetActualScreenSize(lua_L){
  lua_pushinteger(L, LE.width);
  lua_pushinteger(L, LE.height);
  return 2;
}


int setCursor(lua_L){
  const char *str = luaL_checkstring(L, 1);
  

  return 0;
}


int setCursorPos(lua_L){
  int x = (int)lua_tointeger(L,1), y = (int)lua_tointeger(L, 2);
  // IupSetStrf(NULL, "CURSORPOS", "%dx%d", x, y);
  return 0;
} 


int getCursorPos(lua_L){
  int x,y, cx,cy;
  
  lua_pushinteger(L, x-cx);
  lua_pushinteger(L, y-cy);
  return 2;
}

int isCursor(lua_L){
  
  return 0;
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

int StringCall(lua_L){
  lua_settop(L, 2);
  lua_pushlstring(L, lua_tostring(L, 1)+lua_tointeger(L, 2), 1);
  return 1;
}


lua_Table LE_util[] ={
  
  {"Close", &Close},
  { "ChangeScene", &ChangeScene},
  { "GetActualScreenSize", &GetActualScreenSize},
  {"Lalloc", &Lalloc},
  { "ResumeLoading", &ResumeLoading},
  { "int", &toInt},
  {"setCursor", &setCursor},
  {"isCursor", &isCursor},
  {"getCursorPos", &getCursorPos},
  {"setCursorPos", &setCursorPos},
  { "SetFpsLimit", &SetFpsLimit},
  { "WxHtoints", &toWH},
  // { "isMouse", &isMouse},
  lua_eoT
};


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



int LE_idle_action(){
  int nres = 0;
  int scene = 0;
  switch(loadScene){ 

    case NEW_LOAD: {
      lua_pushstring(l, "default");
      lua_getvalue(l, CurScene, "name");
      if(!lua_compare(l, -2, -1, LUA_OPEQ)){
        lua_pushvalue(l, Default);
        lua_replace(l, CurScene);
      }
      lua_settop(nl, 1);
      
      lua_getidx(nl, ScEnum, LE.sceneindex);
      lua_pushstring(nl, ".root");
      lua_concat(nl, 2);
      lua_getglobal(nl, "package");
      lua_getvalue(nl, -1, "loaded");
      lua_pushvalue(nl, -3);
      
      scene = lua_gettable(nl, -2);
      if (scene == LUA_TNIL){
        lua_pop(nl, 1);
        ll = lua_newthread(nl);

        lua_getglobal(nl, "require");
        lua_pushvalue(nl, 2);
        
        if (lua_pcall(nl, 1, 1, 0)){
          error_message(nl, "Scene Loader callback");
        }
      }
      else{
        
        lua_getfield(nl, -1, "__thread");
        
        ll = lua_tothread(nl, -1);
        lua_rotate(nl, -1, 2);
        
      }
      
      if (scene == LUA_TNIL){
        lua_pushvalue(nl, -2);
        lua_setfield(nl, -2, "__thread");
        loadScene = LOADING_SCENE;
      }else{
        loadScene = RELOAD_SCENE;
      }
      
      lua_settop(l, numArgs);
      break;
    }
  case RELOAD_SCENE:{
    lua_xmove(nl, l, 1);  
    lua_replace(l, CurScene);
    loadScene = NO_LOAD;
    break;
  }
  case LOADING_SCENE: {
    lua_getvalue(nl, -1, "Load");
    if (lua_topointer(nl, -1) != EmptyFunc){
      lua_pushvalue(nl, 2);

      lua_xmove(nl, ll, 2);
      int err = lua_resume(ll, nl, 1, &nres);
      if (err != LUA_OK && err != LUA_YIELD){
        error_message(ll, "Error in parallel load!");
      }
      if (nres > 0){
        int isint = 0;
        lua_Integer res = lua_tointegerx(ll, -1, &isint);
        if (isint)
        if (res == 3){
          break;
        }
      }
      lua_xmove(nl, l, 1);
      
      lua_replace(l, CurScene);
      loadScene = NO_LOAD;
      
    }
    break;
    }
  }
  return 0;
}

int LE_mapping_canvas()
{
  
  glfwMakeContextCurrent(w);
  // luaopen_Audio(l);
  luaopen_Graphics(l);
  luaopen_Collision(l);

  lua_settop(l, 0);

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

void LE_resizing_Canvas(GLFWwindow* w, int width, int height)
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

void LE_pressing_Keyboard(GLFWwindow *w, int key, int scancode, int action, int mods)
{
  int press = action;
  int c = key;
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
  
}

void LE_moving_Mouse(GLFWwindow * w, int x, int y )
{
  const char* status = "none";
  lua_getvalue(l, CurScene, "Motion");
  if (lua_topointer(l, -1) != EmptyFunc){
    lua_pushinteger(l, x);
    lua_pushinteger(l, y);
    lua_pushstring(l, status);
    if (lua_pcall(l, 3, 0, 0)){
      error_message(l, "Scene Motion callback");
    }
  }
  lua_settop(l, numArgs);
  
}

void LE_clicking_Mouse(GLFWwindow *w, int button, int pressed, int mods)
{
  int x, y;
  char* status;
  if (lua_getfield(l, CurScene, "Button") == LUA_TFUNCTION)
    if (lua_topointer(l, -1) != EmptyFunc){
      lua_pushboolean(l, pressed);
      lua_pushinteger(l, x);
      lua_pushinteger(l, y);
      lua_pushstring(l, status);
      if (lua_pcall(l, 4, 0, 0)){
        error_message(l, "Scene Button callback");
      }
    }
  lua_settop(l, numArgs);
  
}

int LE_updating_canvas()
{   
    
    if (lua_getfield(l, CurScene, "KeyPress"))
    if (lua_topointer(l,-1) != EmptyFunc){
      lua_pushvalue(l, CurScene);
      lua_pushvalue(l, Keys);
      if (lua_pcall(l, 2, 0, 0)){
        error_message(l, "Scene KeyPress callback");
      }
    }

    if (lua_getfield(l, CurScene, "Update"))
    if (lua_topointer(l, -1) != EmptyFunc){
      lua_pushvalue(l, CurScene);
      if (lua_pcall(l, 1, 0, 0)){
        error_message(l, "Scene Update callback");
      }
    }
    
  lua_settop(l, numArgs);
  return 0;
}



int LE_drawing_Canvas()
{
   
  int err;
  
  glClearColor(0, 0, 0, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  
  lua_getvalue(l, CurScene, "Draw");
  if (lua_topointer(l, -1) != EmptyFunc){
    lua_pushvalue(l, CurScene);
    if (lua_pcall(l, 1, 0, 0)){
      error_message(l, "Scene Draw callback");
    }
  }
  
  // frames++;
  
  glfwSwapBuffers(w);
  lua_settop(l, numArgs);
  return 0;
}

int LE_returning_Focus(GLFWwindow *w, int focus){
  // IupSetFocus(cnv);
  return 0;
}

int LE_closing_Window(){
  
  printf("NOOOOOOOOOOOOOOO!");
  return exet = 1;
}




double cur_t, prev_t, prevFpsU;
double fps_t;
double dt;
long fpst_c;
void LE_mainLoop(void){
  
  if (lua_getglobal(l, "fixedframed")){
    glfwPollEvents();
    
      
      prev_t = cur_t;
      cur_t = glfwGetTime();
      dt = cur_t - prevFpsU;
      
      
      double time_since = cur_t - prevFpsU;
      
      lua_pushnumber(l, dt);
      lua_setglobal(l, "dt");
      frames++;
      if (time_since >= 1.0/ftlim){
        
        
        
        LE_updating_canvas();
        LE_drawing_Canvas();
        
        
        ft = glfwGetTime() - cur_t;
        
        prevFpsU = cur_t;
      }
      fps_t += dt;
      if (fps_t >= 1.0){
        
        lua_pushnumber(l, frames);
        lua_setglobal(l, "fps");
        frames = 0;
        fps_t = 0;
      }
       
      
      if (loadScene == NEW_LOAD || loadScene == LOADING_SCENE || loadScene == RELOAD_SCENE)
        LE_idle_action();
    }
}

int LE_engine_Booting()
{
    if (lua_getglobal(l, "fullscreen"));
        
    lua_pop(l, 1);
  
    if (glfwInit() == GLFW_FALSE){
        printf("Something went wrong with GLFW!\n");
        glfwTerminate(); 
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, 0);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    w = glfwCreateWindow(LE.width, LE.height, LE.title, NULL, NULL);
    

    glfwSetWindowSizeCallback(w, (GLFWwindowsizefun)(LE_resizing_Canvas));
    glfwSetKeyCallback(w, (GLFWkeyfun)(LE_pressing_Keyboard));
    glfwSetMouseButtonCallback(w, (GLFWmousebuttonfun)(LE_clicking_Mouse));
    // glfwSetCursorPosCallback(w, (GLFWcursorposfun)(LE_moving_Mouse));
    glfwSetWindowFocusCallback(w, (GLFWwindowfocusfun)(LE_returning_Focus));
  
  
    
  
    LE_mapping_canvas();
  
  
    ftlim = 60;
    
    
    fpst_c = 1;
    dt = 1.0/ftlim;

    exet = setjmp(errB);
    if (exet) { goto end;}

    

    cur_t = glfwGetTime(), prev_t = 0, prevFpsU = cur_t;
    fps_t = 0;
    
    
    end:;

    glfwDestroyWindow(w);
    
    glfwTerminate();
  
  return 0;
}

int main(){
    l = luaL_newstate();
    luaL_openlibs(l);

    luaL_newlib(l, LE_util);
    lua_setglobal(l, "LE");
    
    lua_pushnumber(l, 1.0/ftlim);
    lua_setglobal(l, "dt");
    lua_pushnumber(l, ftlim);
    lua_setglobal(l, "fps");
    lua_pushnumber(l, ft);
    lua_setglobal(l, "ft");

    lua_require(l, "config");

    lua_getglobal(l, "startscene");
    LE.sceneindex = luaL_checkinteger(l, -1);
    lua_getglobal(l, "_en");
    lua_getvalue(l, -1, "screen");
    
    lua_getvalue(l, -1, "w");
    LE.width = lua_tointeger(l,-1);
    lua_getvalue(l, -2, "h");
    LE.height = lua_tointeger(l,-1);
    lua_pop(l,2);



    lua_pushcfunction(l, GetActualScreenSize);
    lua_setfield(l, -2, "asize");

    if (lua_getglobal(l, "luatitle")){
      LE.title = (char*)lua_tostring(l, -1);
    }else{
      LE.title = LEfault.title;
    }



    lua_pushstring(l, "");
    lua_getmetatable(l, -1);
    lua_pushcfunction(l, StringCall);
    lua_setfield(l, -2, "__call");

    lua_settop(l,0);

    
    LE_engine_Booting();

    lua_close(l);
    return 0;
}