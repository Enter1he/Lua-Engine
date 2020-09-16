#include<stdio.h>
#include"include/lua.h"
#include"include/lualib.h"
#include"include/lauxlib.h"

int set(lua_State* L)
{
    if (lua_isinteger(L, 1))
    {
        LUA_INTEGER n = lua_tointeger(L, 1);
        lua_createtable(L, n, n);
        int i = 1;
        for (i>1; i<=n; i++){
            char c[1] ;
            sprintf(c, "%d", i);
            lua_pushstring(L,c);
            lua_pushvalue(L, i+1);
            lua_settable(L, n+2);
        };
        return 1;
    };
        
    return 0;
}

int luaopen_Set(lua_State* L)
{
    lua_pushcfunction(L,&set);
    return 1;
}