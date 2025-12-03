// luadef.c
#include "luadef.h"

void lua_tableContets(lua_L, int idx){
	lua_pushnil(L);
	while (lua_next(L, idx) != 0) { 
		printf("%s - %s\n",
			lua_isstring(L, -2) ? lua_tostring(L, -2) : lua_typename(L, lua_type(L, -2)),
			lua_typename(L, lua_type(L, -1)));
		lua_pop(L, 1);
	}
}

void stackDumpA(lua_L, int line) {
	printf("stackDump on line: %d\n", line);
	int i;
	int top = lua_gettop(L);
	for(i = 1; i <= top; i++) {
		int t = lua_type(L, i);
		printf("%d:", -(top-i)-1);
		switch(t) {
			case LUA_TNIL:
				printf("nil");
				break;
			case LUA_TBOOLEAN:
				printf(lua_toboolean(L, i) ? "true" : "false");
				break;
			case LUA_TNUMBER:
				printf("%g", lua_tonumber(L, i));
				break;
			case LUA_TSTRING:
				printf("%s", lua_tostring(L, i));
				break;
			case LUA_TFUNCTION:
				printf("%s", lua_typename(L, t));
				break;
			case LUA_TTABLE: {
				lua_getfield(L, i, "name");
				const char* q = lua_tolstring(L, top+1, NULL);
				lua_pop(L, 1);
				printf("%s", q ? q : "table");
				break;
			}
			default:{
				printf("%s", lua_typename(L, t));
				break;
			}
		}
		printf(" ");
	}
	printf("\n");
	
};

void lua_arrtofloat(lua_State *L, Table *a, float *arr, int len){
  lua_Number n = 0;
  for (int i = 0; i < len; i++){
    TValue *v = (TValue*)(lua_fastgetint(a, i+1));
    tonumber(v, &n); 
    arr[i] = (float)(n);
  }
}

TValue * lua_tablevalue(lua_State *L, Table* t, const char* key){
  TString *str = luaS_new(L, key);
  TValue *v = (TValue*)(luaH_getstr(t, str));
  
  return v;
}


void *touserdata (const TValue *o) {
  switch (ttype(o)) {
    case LUA_TUSERDATA: return getudatamem(uvalue(o));
    case LUA_TLIGHTUSERDATA: return pvalue(o);
    default: return NULL;
  }
}

