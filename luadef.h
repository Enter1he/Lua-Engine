#ifndef LUADEF_H
#define LUADEF_H

#include "include/lua.h"
#include "include/lauxlib.h"

LUALIB_API void (luaL_openlibs) (lua_State *L);

#define LUA_DLL __declspec(dllexport)
#define lua_L lua_State *L
#define lua_Table static const struct luaL_Reg  // making big thing small
#define lua_eoT {NULL, NULL} 				 	//

#define lua_require(L, modname){\
  lua_getglobal(L, "require");\
  lua_pushstring(L, modname);\
  if(lua_pcall(L, 1, 1, 0)){\
	  printf("%s",lua_tostring(L, -1));\
    getchar();\
  }\
}

#define lua_getvalue(L, idx, name) if(!lua_getfield(L, idx, name)) {fprintf(stderr, "%s %s line %d: no %s defined in table\n",  __FILE__ , __func__, __LINE__, name); getchar();}

#define lua_getidx(L, idx, n) if(!lua_geti(L, idx, n)) {fprintf(stderr, "%s %s line %d: no %d defined in table\n", __FILE__, __func__, __LINE__, n); getchar();}

#define lua_nameAtable(L, idx, name){\
	lua_pushstring(L, name);\
	lua_setfield(L, (idx) ? idx : idx-1, "name");\
}

#define lua_require0(L, modname){\
  lua_getglobal(L, "require");\
  lua_pushstring(L, modname);\
  lua_call(L, 1, 0);\
}

#define lua_requireAdd(L, modname, add){\
  lua_getglobal(L, "require");\
  lua_pushstring(L, modname);\
  lua_pushstring(L, add);\
  lua_concat(L, 2);\
  lua_call(L, 1, 0);\
}

void lua_tableContets(lua_L, int idx);

#define lua_cleanargs(L, num) {int top = lua_gettop(L); if (top > num) lua_pop(L, top-num);}

#define Error_print(str,...) printf(str,##__VA_ARGS__); printf("\n"); getchar();

#define lua_getA3(L){\
	lua_getidx(L, -1, 1);\
	lua_getidx(L, -2, 2);\
	lua_getidx(L, -3, 3);\
}

#define lua_getA4(L){\
	lua_getidx(L, -1, 1);\
	lua_getidx(L, -2, 2);\
	lua_getidx(L, -3, 3);\
	lua_getidx(L, -4, 4);\
}

#define lua_getA2(L){\
	lua_getidx(L, -1, 1);\
	lua_getidx(L, -2, 2);\
}

void stackDump(lua_L);

#endif