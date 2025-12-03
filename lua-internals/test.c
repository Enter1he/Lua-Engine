// test.c

#define LUA_CORE
#include "src/lua.h"
#include "src/lauxlib.h"
#include "src/luaconf.h"
#include "src/luacore.h"

#include <stdlib.h>

#define lua_require(L, modname){\
  lua_getglobal(L, "require");\
  lua_pushstring(L, modname);\
  if(lua_pcall(L, 1, 1, 0)){\
	  printf("%s",lua_tostring(L, -1));\
    system("pause");\
  }\
}

int inc(lua_State *L){
    TValue * v = index2value(L, 1);
    v->value_.i += 1;
    return 0;
}
LUALIB_API void (luaL_openlibs) (lua_State *L);

int main(){
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    lua_register(L,"inc", inc);
    lua_require(L, "test");
    lua_close(L);
    return 0;
}