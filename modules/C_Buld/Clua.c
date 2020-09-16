#include<windows.h>
#include"include/lua.h"
#include"include/lualib.h"
#include"include/lauxlib.h"

int main()
{
    lua_State* L = luaL_newstate();
    lua_getglobal(L, "require");
    lua_pushfstring(L, "modules/Engine");
    lua_call(L, 1, 1);
    lua_getfield(L, 1, "Load");
    lua_call(L, 1, 0);
    lua_getfield(L, 1, "Loop");
    lua_call(L, 1, 0);
    lua_close(L);
}